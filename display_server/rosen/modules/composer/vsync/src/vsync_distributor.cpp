/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "vsync_distributor.h"
#include <chrono>
#include <condition_variable>
#include <algorithm>
#include <sched.h>
#include <sys/resource.h>
#include <scoped_bytrace.h>
#include "vsync_log.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr int32_t SOFT_VSYNC_PERIOD = 16;
constexpr int32_t ERRNO_EAGAIN = -1;
constexpr int32_t ERRNO_OTHER = -2;
#ifndef FT_BUILD
constexpr int32_t THREAD_PRIORTY = -6;
constexpr int32_t SCHED_PRIORITY = 2;
#endif
constexpr uint32_t SOCKET_CHANNEL_SIZE = 1024;
}
VSyncConnection::VSyncConnection(const sptr<VSyncDistributor>& distributor, std::string name)
    : rate_(-1), info_(name), distributor_(distributor)
{
    socketPair_ = new LocalSocketPair();
    int32_t err = socketPair_->CreateChannel(SOCKET_CHANNEL_SIZE, SOCKET_CHANNEL_SIZE);
    if (err != 0) {
        ScopedBytrace func("Create socket channel failed, errno = " + std::to_string(errno));
    }
}

VSyncConnection::~VSyncConnection()
{
}

VsyncError VSyncConnection::RequestNextVSync()
{
    if (distributor_ == nullptr) {
        return VSYNC_ERROR_NULLPTR;
    }
    const sptr<VSyncDistributor> distributor = distributor_.promote();
    if (distributor == nullptr) {
        return VSYNC_ERROR_NULLPTR;
    }
    ScopedBytrace func(info_.name_ + "RequestNextVSync");
    return distributor->RequestNextVSync(this);
}

VsyncError VSyncConnection::GetReceiveFd(int32_t &fd)
{
    fd = socketPair_->GetReceiveDataFd();
    return VSYNC_ERROR_OK;
}

int32_t VSyncConnection::PostEvent(int64_t now)
{
    ScopedBytrace func("SendVsyncTo conn: " + info_.name_ + ", now:" + std::to_string(now));
    int32_t ret = socketPair_->SendData(&now, sizeof(int64_t));
    if (ret > -1) {
        ScopedBytrace successful("successful");
        info_.postVSyncCount_++;
    } else {
        ScopedBytrace failed("failed");
    }
    return ret;
}

VsyncError VSyncConnection::SetVSyncRate(int32_t rate)
{
    if (distributor_ == nullptr) {
        return VSYNC_ERROR_NULLPTR;
    }
    const sptr<VSyncDistributor> distributor = distributor_.promote();
    if (distributor == nullptr) {
        return VSYNC_ERROR_NULLPTR;
    }
    return distributor->SetVSyncRate(rate, this);
}

VSyncDistributor::VSyncDistributor(sptr<VSyncController> controller, std::string name)
    : controller_(controller), mutex_(), con_(), connections_(),
    vsyncEnabled_(false), name_(name)
{
    event_.timestamp = 0;
    event_.vsyncCount = 0;
    vsyncThreadRunning_ = true;
    threadLoop_ = std::thread(std::bind(&VSyncDistributor::ThreadMain, this));
}

VSyncDistributor::~VSyncDistributor()
{
    {
        std::unique_lock<std::mutex> locker(mutex_);
        vsyncThreadRunning_ = false;
    }
    if (threadLoop_.joinable()) {
        con_.notify_all();
        threadLoop_.join();
    }
}

VsyncError VSyncDistributor::AddConnection(const sptr<VSyncConnection>& connection)
{
    if (connection == nullptr) {
        return VSYNC_ERROR_NULLPTR;
    }
    std::lock_guard<std::mutex> locker(mutex_);
    auto it = find(connections_.begin(), connections_.end(), connection);
    if (it != connections_.end()) {
        return VSYNC_ERROR_INVALID_ARGUMENTS;
    }
    ScopedBytrace func("Add VSyncConnection: " + connection->info_.name_);
    connections_.push_back(connection);
    return VSYNC_ERROR_OK;
}

VsyncError VSyncDistributor::RemoveConnection(const sptr<VSyncConnection>& connection)
{
    if (connection == nullptr) {
        return VSYNC_ERROR_NULLPTR;
    }
    std::lock_guard<std::mutex> locker(mutex_);
    auto it = find(connections_.begin(), connections_.end(), connection);
    if (it == connections_.end()) {
        return VSYNC_ERROR_INVALID_ARGUMENTS;
    }
    ScopedBytrace func("Remove VSyncConnection: " + connection->info_.name_);
    connections_.erase(it);
    return VSYNC_ERROR_OK;
}

void VSyncDistributor::ThreadMain()
{
    // set thread priorty
#ifdef FT_BUILD
    pthread_attr_t thread_attr;
    struct sched_param thread_param;
    pthread_attr_init(&thread_attr);
    if (pthread_attr_setschedpolicy(&thread_attr, SCHED_FIFO) == 0) {
        thread_param.sched_priority = sched_get_priority_max(SCHED_FIFO);
        if (pthread_attr_setschedparam(&thread_attr, &thread_param) == 0) {
            VLOGI("set thread priorty: SCHED_FIFO/%{public}d", thread_param.sched_priority);
        }
    }
#else
    setpriority(PRIO_PROCESS, 0, THREAD_PRIORTY);
    struct sched_param param = {0};
    param.sched_priority = SCHED_PRIORITY;
    sched_setscheduler(0, SCHED_FIFO, &param);
#endif


    int64_t timestamp;
    int64_t vsyncCount;
    while (vsyncThreadRunning_ == true) {
        std::vector<sptr<VSyncConnection>> conns;
        {
            bool waitForVSync = false;
            std::unique_lock<std::mutex> locker(mutex_);
            timestamp = event_.timestamp;
            event_.timestamp = 0;
            vsyncCount = event_.vsyncCount;
            CollectConnections(waitForVSync, timestamp, conns, vsyncCount);
            // no vsync signal
            if (timestamp == 0) {
                // there is some connections request next vsync, enable vsync if vsync disable and
                // and start the software vsync with wait_for function
                if (waitForVSync == true && vsyncEnabled_ == false) {
                    EnableVSync();
                    if (con_.wait_for(locker, std::chrono::milliseconds(SOFT_VSYNC_PERIOD)) ==
                        std::cv_status::timeout) {
                        const auto &now = std::chrono::steady_clock::now().time_since_epoch();
                        timestamp = std::chrono::duration_cast<std::chrono::nanoseconds>(now).count();
                        event_.timestamp = timestamp;
                        event_.vsyncCount++;
                    }
                } else {
                    // just wait request or vsync signal
                    if (vsyncThreadRunning_ == true) {
                        con_.wait(locker);
                    }
                }
                continue;
            } else if ((timestamp > 0) && (waitForVSync == false)) {
                // if there is a vsync signal but no vaild connections, we should disable vsync
                DisableVSync();
                continue;
            }
        }

        ScopedBytrace func(name_ + "_SendVsync");
        VLOGI("VSyncDistributor::ThreadMain: Conns size: %{public}zu, timestamp: %{public}ld", conns.size(), timestamp);

        for (uint32_t i = 0; i < conns.size(); i++) {
            int32_t ret = conns[i]->PostEvent(timestamp);
            VLOGD("Distributor name:%{public}s, connection name:%{public}s, ret:%{public}d",
                name_.c_str(), conns[i]->info_.name_.c_str(), ret);
            if (ret == 0 || ret == ERRNO_OTHER) {
                RemoveConnection(conns[i]);
            } else if (ret == ERRNO_EAGAIN) {
                std::unique_lock<std::mutex> locker(mutex_);
                VLOGI("VSyncDistributor::ThreadMain: EAGAIN");
                // Exclude SetVSyncRate
                if (conns[i]->rate_ < 0) {
                    conns[i]->rate_ = 0;
                }
            }
        }
    }
#ifdef FT_BUILD
    pthread_attr_destroy(&thread_attr);
#endif
}

void VSyncDistributor::EnableVSync()
{
    if (controller_ != nullptr && vsyncEnabled_ == false) {
        vsyncEnabled_ = true;
        controller_->SetCallback(this);
        controller_->SetEnable(true);
        VLOGI("Disable VSync.");
    }
}

void VSyncDistributor::DisableVSync()
{
    if (controller_ != nullptr && vsyncEnabled_ == true) {
        vsyncEnabled_ = false;
        controller_->SetEnable(false);
    }
}

void VSyncDistributor::OnVSyncEvent(int64_t now)
{
    std::lock_guard<std::mutex> locker(mutex_);
    event_.timestamp = now;
    event_.vsyncCount++;
    con_.notify_all();
}

void VSyncDistributor::CollectConnections(bool &waitForVSync, int64_t timestamp,
                                          std::vector<sptr<VSyncConnection>> &conns, int64_t vsyncCount)
{
    for (uint32_t i = 0; i < connections_.size(); i++) {
        int32_t rate = connections_[i]->highPriorityState_ ? connections_[i]->highPriorityRate_ :
                                                             connections_[i]->rate_;
        if (rate == 0) {  // for RequestNextVSync
            waitForVSync = true;
            if (timestamp > 0) {
                connections_[i]->rate_ = -1;
                conns.push_back(connections_[i]);
            }
        } else if (rate > 0) {
            if (connections_[i]->rate_ == 0) {  // for SetHighPriorityVSyncRate with RequestNextVSync
                waitForVSync = true;
                if (timestamp > 0 && (vsyncCount % rate == 0)) {
                    connections_[i]->rate_ = -1;
                    conns.push_back(connections_[i]);
                }
            } else if (connections_[i]->rate_ > 0) {  // for SetVSyncRate
                waitForVSync = true;
                if (timestamp > 0 && (vsyncCount % rate == 0)) {
                    conns.push_back(connections_[i]);
                }
            }
        }
    }
}

VsyncError VSyncDistributor::RequestNextVSync(const sptr<VSyncConnection>& connection)
{
    if (connection == nullptr) {
        VLOGE("connection is nullptr");
        return VSYNC_ERROR_NULLPTR;
    }
    ScopedBytrace func(connection->info_.name_ + "_RequestNextVSync");
    std::lock_guard<std::mutex> locker(mutex_);
    auto it = find(connections_.begin(), connections_.end(), connection);
    if (it == connections_.end()) {
        VLOGE("connection is invalid arguments");
        return VSYNC_ERROR_INVALID_ARGUMENTS;
    }
    if (connection->rate_ < 0) {
        connection->rate_ = 0;
        con_.notify_all();
    }
    VLOGD("conn name:%{public}s, rate:%{public}d", connection->info_.name_.c_str(), connection->rate_);
    return VSYNC_ERROR_OK;
}

VsyncError VSyncDistributor::SetVSyncRate(int32_t rate, const sptr<VSyncConnection>& connection)
{
    if (rate <= 0 || connection == nullptr) {
        return VSYNC_ERROR_INVALID_ARGUMENTS;
    }
    std::lock_guard<std::mutex> locker(mutex_);
    auto it = find(connections_.begin(), connections_.end(), connection);
    if (it == connections_.end()) {
        return VSYNC_ERROR_INVALID_ARGUMENTS;
    }
    if (connection->rate_ == rate) {
        return VSYNC_ERROR_INVALID_ARGUMENTS;
    }
    connection->rate_ = rate;
    VLOGD("conn name:%{public}s", connection->info_.name_.c_str());
    con_.notify_all();
    return VSYNC_ERROR_OK;
}

VsyncError VSyncDistributor::SetHighPriorityVSyncRate(int32_t highPriorityRate, const sptr<VSyncConnection>& connection)
{
    if (highPriorityRate <= 0 || connection == nullptr) {
        return VSYNC_ERROR_INVALID_ARGUMENTS;
    }

    std::lock_guard<std::mutex> locker(mutex_);
    auto it = find(connections_.begin(), connections_.end(), connection);
    if (it == connections_.end()) {
        return VSYNC_ERROR_INVALID_ARGUMENTS;
    }
    if (connection->highPriorityRate_ == highPriorityRate) {
        return VSYNC_ERROR_INVALID_ARGUMENTS;
    }
    connection->highPriorityRate_ = highPriorityRate;
    connection->highPriorityState_ = true;
    VLOGD("in, conn name:%{public}s, highPriorityRate:%{public}d", connection->info_.name_.c_str(),
          connection->highPriorityRate_);
    con_.notify_all();
    return VSYNC_ERROR_OK;
}

VsyncError VSyncDistributor::GetVSyncConnectionInfos(std::vector<ConnectionInfo>& infos)
{
    infos.clear();
    for (auto &connection : connections_) {
        infos.push_back(connection->info_);
    }
    return VSYNC_ERROR_OK;
}

VsyncError VSyncDistributor::QosGetPidByName(const std::string& name, uint32_t& pid)
{
    if (name.find("WM") == std::string::npos) {
        return VSYNC_ERROR_INVALID_ARGUMENTS;
    }
    std::string::size_type pos = name.find("_");
    if (pos == std::string::npos) {
        return VSYNC_ERROR_INVALID_ARGUMENTS;
    }
    pid = (uint32_t)stoi(name.substr(pos + 1));
    return VSYNC_ERROR_OK;
}

VsyncError VSyncDistributor::SetQosVSyncRate(uint32_t pid, int32_t rate)
{
    std::lock_guard<std::mutex> locker(mutex_);
    for (auto connection : connections_) {
        uint32_t tmpPid;
        if (QosGetPidByName(connection->info_.name_, tmpPid) != VSYNC_ERROR_OK) {
            continue;
        }

        if (tmpPid == pid) {
            if (connection->highPriorityRate_ != rate) {
                connection->highPriorityRate_ = rate;
                connection->highPriorityState_ = true;
                VLOGD("in, conn name:%{public}s, highPriorityRate:%{public}d", connection->info_.name_.c_str(),
                    connection->highPriorityRate_);
                con_.notify_all();
            }
            break;
        }
    }
    return VSYNC_ERROR_OK;
}

VsyncError VSyncDistributor::GetQosVSyncRateInfos(std::vector<std::pair<uint32_t, int32_t>>& vsyncRateInfos)
{
    vsyncRateInfos.clear();

    std::lock_guard<std::mutex> locker(mutex_);
    for (auto &connection : connections_) {
        uint32_t tmpPid;
        if (QosGetPidByName(connection->info_.name_, tmpPid) != VSYNC_ERROR_OK) {
            continue;
        }
        int32_t tmpRate = connection->highPriorityState_ ? connection->highPriorityRate_ : connection->rate_;
        vsyncRateInfos.push_back(std::make_pair(tmpPid, tmpRate));
    }
    return VSYNC_ERROR_OK;
}
}
}

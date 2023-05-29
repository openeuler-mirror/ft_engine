/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "perform_reporter.h"
#include "window_manager_hilog.h"

#include <hisysevent.h>

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_DISPLAY, "PerformReporter"};
}

/**
 * @brief Construct a new Perform Reporter:: Perform Reporter object
 *
 * @param tag A tag that in report string
 * @param timeSpiltsMs The time-interval that data statistic, details look up the comments in function body
 * @param reportInterval Report data after reportInterval round start-end
 */
PerformReporter::PerformReporter(const std::string& tag,
    const std::vector<int64_t>& timeSpiltsMs, uint32_t reportInterval)
    : tag_(tag), reportInterval_(reportInterval)
{
    // re-organ data struct
    // a, b, c, d -->
    // (0, a] : cnt=0, (a, b] : cnt=0, (b, c] : cnt=0, (c, d] : cnt=0
    for (auto split : timeSpiltsMs) {
        timeSplitCount_[split] = 0;
    }
    // (d, +limit] : cnt=0
    timeSplitCount_[BARRIER] = 0;
    totalCount_ = 0;
}

void PerformReporter::start()
{
    startTime_ = std::chrono::steady_clock::now();
}

void PerformReporter::end()
{
    auto currentTime = std::chrono::steady_clock::now();
    int64_t costTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - startTime_).count();

    count(costTime);

    bool repSucc = report();
    if (repSucc) {
        clear();
    }
}

bool PerformReporter::report()
{
    if (totalCount_ < reportInterval_) {
        return false;
    }

    std::ostringstream oss;
    oss << tag_ << ": ";
    auto maxSplit = 0;
    for (const auto& iter: timeSplitCount_) {
        if (iter.first != BARRIER) {
            oss << "BELLOW" << iter.first << "(ms): " << iter.second << ", ";
            maxSplit = iter.first;
        }
    }
    oss << "ABOVE" << maxSplit << "(ms): " << timeSplitCount_[BARRIER];

    int32_t ret = OHOS::HiviewDFX::HiSysEvent::Write(
        OHOS::HiviewDFX::HiSysEvent::Domain::WINDOW_MANAGER, tag_,
        OHOS::HiviewDFX::HiSysEvent::EventType::STATISTIC, "MSG", oss.str());
    WLOGFI("Write HiSysEvent ret:%{public}d", ret);
    return ret == 0;
}

void PerformReporter::count(int64_t costTime)
{
    totalCount_++;
    for (auto& iter: timeSplitCount_) {
        if (costTime <= iter.first) {
            iter.second++;
            break;
        }
    }

    std::ostringstream oss;
    oss << tag_ << " cost " << costTime << "ms, total count " << totalCount_;
    WLOGFI("%{public}s", oss.str().c_str());
}

void PerformReporter::clear()
{
    totalCount_ = 0;
    for (auto& iter: timeSplitCount_) {
        iter.second = 0;
    }
}
}
}
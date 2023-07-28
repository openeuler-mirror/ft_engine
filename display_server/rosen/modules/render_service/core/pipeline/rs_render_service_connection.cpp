/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "rs_render_service_connection.h"

#include "pipeline/rs_render_node_map.h"
#include "pipeline/rs_render_service_listener.h"
#include "pipeline/rs_surface_capture_task.h"
#include "pipeline/rs_surface_render_node.h"
#include "pipeline/rs_uni_render_judgement.h"
#include "platform/common/rs_log.h"
#include "rs_main_thread.h"
#include "rs_trace.h"

namespace OHOS {
namespace Rosen {
// we guarantee that when constructing this object,
// all these pointers are valid, so will not check them.
RSRenderServiceConnection::RSRenderServiceConnection(
    pid_t remotePid,
    wptr<RSRenderService> renderService,
    RSMainThread* mainThread,
    sptr<RSScreenManager> screenManager,
    sptr<IRemoteObject> token,
    sptr<VSyncDistributor> distributor)
    : remotePid_(remotePid),
      renderService_(renderService),
      mainThread_(mainThread),
      screenManager_(screenManager),
      token_(token),
      connDeathRecipient_(new RSConnectionDeathRecipient(this)),
      ApplicationDeathRecipient_(new RSApplicationRenderThreadDeathRecipient(this)),
      appVSyncDistributor_(distributor)
{
    if (!token_->AddDeathRecipient(connDeathRecipient_)) {
        RS_LOGW("RSRenderServiceConnection: Failed to set death recipient.");
    }
}

RSRenderServiceConnection::~RSRenderServiceConnection() noexcept
{
    CleanAll();
}

void RSRenderServiceConnection::CleanVirtualScreens() noexcept
{
    std::lock_guard<std::mutex> lock(mutex_);

    for (const auto id : virtualScreenIds_) {
        screenManager_->RemoveVirtualScreen(id);
    }
    virtualScreenIds_.clear();

    if (screenChangeCallback_ != nullptr) {
        screenManager_->RemoveScreenChangeCallback(screenChangeCallback_);
        screenChangeCallback_ = nullptr;
    }
}

void RSRenderServiceConnection::CleanRenderNodes() noexcept
{
    auto& context = mainThread_->GetContext();
    auto& nodeMap = context.GetMutableNodeMap();

    nodeMap.FilterNodeByPid(remotePid_);
}

void RSRenderServiceConnection::CleanAll(bool toDelete) noexcept
{
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (cleanDone_) {
            return;
        }
    }
    RS_LOGD("RSRenderServiceConnection::CleanAll() start.");
    mainThread_->ScheduleTask([this]() {
        CleanVirtualScreens();
        CleanRenderNodes();
        mainThread_->ClearTransactionDataPidInfo(remotePid_);
    }).wait();

    for (auto& conn : vsyncConnections_) {
        appVSyncDistributor_->RemoveConnection(conn);
    }

    {
        std::lock_guard<std::mutex> lock(mutex_);
        vsyncConnections_.clear();
        cleanDone_ = true;
    }

    if (toDelete) {
        auto renderService = renderService_.promote();
        if (renderService == nullptr) {
            RS_LOGW("RSRenderServiceConnection::CleanAll() RenderService is dead.");
        } else {
            renderService->RemoveConnection(GetToken());
        }
    }

    RS_LOGD("RSRenderServiceConnection::CleanAll() end.");
}

RSRenderServiceConnection::RSConnectionDeathRecipient::RSConnectionDeathRecipient(
    wptr<RSRenderServiceConnection> conn) : conn_(conn)
{
}

void RSRenderServiceConnection::RSConnectionDeathRecipient::OnRemoteDied(const wptr<IRemoteObject>& token)
{
    auto tokenSptr = token.promote();
    if (tokenSptr == nullptr) {
        RS_LOGW("RSConnectionDeathRecipient::OnRemoteDied: can't promote remote object.");
        return;
    }

    auto rsConn = conn_.promote();
    if (rsConn == nullptr) {
        RS_LOGW("RSConnectionDeathRecipient::OnRemoteDied: RSRenderServiceConnection was dead, do nothing.");
        return;
    }

    if (rsConn->GetToken() != tokenSptr) {
        RS_LOGI("RSConnectionDeathRecipient::OnRemoteDied: token doesn't match, ignore it.");
        return;
    }

    RS_LOGI("RSConnectionDeathRecipient::OnRemoteDied: do the clean work.");
    rsConn->CleanAll(true);
}

RSRenderServiceConnection::RSApplicationRenderThreadDeathRecipient::RSApplicationRenderThreadDeathRecipient(
    wptr<RSRenderServiceConnection> conn) : conn_(conn)
{}

void RSRenderServiceConnection::RSApplicationRenderThreadDeathRecipient::OnRemoteDied(const wptr<IRemoteObject>& token)
{
    auto tokenSptr = token.promote();
    if (tokenSptr == nullptr) {
        RS_LOGW("RSApplicationRenderThreadDeathRecipient::OnRemoteDied: can't promote remote object.");
        return;
    }

    auto rsConn = conn_.promote();
    if (rsConn == nullptr) {
        RS_LOGW("RSApplicationRenderThreadDeathRecipient::OnRemoteDied: RSRenderServiceConnection was dead, do nothing.");
        return;
    }

    RS_LOGI("RSApplicationRenderThreadDeathRecipient::OnRemoteDied: Unregister.");
    auto app = iface_cast<IApplicationAgent>(tokenSptr);
    rsConn->UnRegisterApplicationAgent(app);
}

void RSRenderServiceConnection::CommitTransaction(std::unique_ptr<RSTransactionData>& transactionData)
{
    mainThread_->RecvRSTransactionData(transactionData);
}

void RSRenderServiceConnection::ExecuteSynchronousTask(const std::shared_ptr<RSSyncTask>& task)
{
    if (task == nullptr) {
        return;
    }

    auto& context = mainThread_->GetContext();
    mainThread_->ScheduleTask([task, &context]() {
        task->Process(context);
    }).wait_for(std::chrono::nanoseconds(task->GetTimeout()));
}

int32_t RSRenderServiceConnection::SetRenderModeChangeCallback(sptr<RSIRenderModeChangeCallback> callback)
{
    if (!callback) {
        RS_LOGD("RSRenderServiceConnection::SetRenderModeChangeCallback: callback is nullptr");
        return INVALID_ARGUMENTS;
    }
    mainThread_->SetRenderModeChangeCallback(callback);
    return SUCCESS;
}

void RSRenderServiceConnection::UpdateRenderMode(bool isUniRender)
{
    mainThread_->NotifyRenderModeChanged(isUniRender);
}

bool RSRenderServiceConnection::GetUniRenderEnabled()
{
    return RSUniRenderJudgement::IsUniRender();
}

bool RSRenderServiceConnection::QueryIfRTNeedRender()
{
    return !mainThread_->QueryIfUseUniVisitor();
}

bool RSRenderServiceConnection::CreateNode(const RSSurfaceRenderNodeConfig& config)
{
    std::shared_ptr<RSSurfaceRenderNode> node =
        std::make_shared<RSSurfaceRenderNode>(config, mainThread_->GetContext().weak_from_this());
    if (node == nullptr) {
        RS_LOGE("RSRenderService::CreateNode fail");
        return false;
    }
    std::function<void()> registerNode = [node, this]() -> void {
        this->mainThread_->GetContext().GetMutableNodeMap().RegisterRenderNode(node);
    };
    mainThread_->PostTask(registerNode);
    return true;
}

sptr<Surface> RSRenderServiceConnection::CreateNodeAndSurface(const RSSurfaceRenderNodeConfig& config)
{
    std::shared_ptr<RSSurfaceRenderNode> node =
        std::make_shared<RSSurfaceRenderNode>(config, mainThread_->GetContext().weak_from_this());
    if (node == nullptr) {
        RS_LOGE("RSRenderService::CreateNodeAndSurface CreateNode fail");
        return nullptr;
    }
    sptr<Surface> surface = Surface::CreateSurfaceAsConsumer(config.name);
    if (surface == nullptr) {
        RS_LOGE("RSRenderService::CreateNodeAndSurface get consumer surface fail");
        return nullptr;
    }
    const std::string& surfaceName = surface->GetName();
    RS_LOGI("RsDebug RSRenderService::CreateNodeAndSurface node id:%" PRIu64 " name:%s surface id:%" PRIu64 " name:%s",
        node->GetId(), node->GetName().c_str(), surface->GetUniqueId(), surfaceName.c_str());
    node->SetConsumer(surface);
    std::function<void()> registerNode = [node, this]() -> void {
        this->mainThread_->GetContext().GetMutableNodeMap().RegisterRenderNode(node);
    };
    mainThread_->PostTask(registerNode);
    std::weak_ptr<RSSurfaceRenderNode> surfaceRenderNode(node);
    sptr<IBufferConsumerListener> listener = new RSRenderServiceListener(surfaceRenderNode);
    SurfaceError ret = surface->RegisterConsumerListener(listener);
    if (ret != SURFACE_ERROR_OK) {
        RS_LOGE("RSRenderService::CreateNodeAndSurface Register Consumer Listener fail");
        return nullptr;
    }
    return surface;
}


sptr<IVSyncConnection> RSRenderServiceConnection::CreateVSyncConnection(const std::string& name)
{
    sptr<VSyncConnection> conn = new VSyncConnection(appVSyncDistributor_, name);
    appVSyncDistributor_->AddConnection(conn);
    {
        std::lock_guard<std::mutex> lock(mutex_);
        vsyncConnections_.push_back(conn);
    }
    return conn;
}

int32_t RSRenderServiceConnection::SetFocusAppInfo(
    int32_t pid, int32_t uid, const std::string &bundleName, const std::string &abilityName)
{
    mainThread_->SetFocusAppInfo(pid, uid, bundleName, abilityName);
    return SUCCESS;
}

ScreenId RSRenderServiceConnection::GetDefaultScreenId()
{
    std::lock_guard<std::mutex> lock(mutex_);
    return screenManager_->GetDefaultScreenId();
}

std::vector<ScreenId> RSRenderServiceConnection::GetAllScreenIds()
{
    std::lock_guard<std::mutex> lock(mutex_);
    return screenManager_->GetAllScreenIds();
}

ScreenId RSRenderServiceConnection::CreateVirtualScreen(
    const std::string &name,
    uint32_t width,
    uint32_t height,
    sptr<Surface> surface,
    ScreenId mirrorId,
    int32_t flags)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto newVirtualScreenId = screenManager_->CreateVirtualScreen(name, width, height, surface, mirrorId, flags);
    virtualScreenIds_.insert(newVirtualScreenId);
    return newVirtualScreenId;
}

int32_t RSRenderServiceConnection::SetVirtualScreenSurface(ScreenId id, sptr<Surface> surface)
{
    std::lock_guard<std::mutex> lock(mutex_);
    return screenManager_->SetVirtualScreenSurface(id, surface);
}

void RSRenderServiceConnection::RemoveVirtualScreen(ScreenId id)
{
    std::lock_guard<std::mutex> lock(mutex_);
    screenManager_->RemoveVirtualScreen(id);
    virtualScreenIds_.erase(id);
}

int32_t RSRenderServiceConnection::SetScreenChangeCallback(sptr<RSIScreenChangeCallback> callback)
{
    std::unique_lock<std::mutex> lock(mutex_);
    if (screenChangeCallback_ == callback) {
        return INVALID_ARGUMENTS;
    }

    if (screenChangeCallback_ != nullptr) {
        // remove the old callback
        screenManager_->RemoveScreenChangeCallback(screenChangeCallback_);
    }

    // update
    int32_t status = screenManager_->AddScreenChangeCallback(callback);
    auto tmp = screenChangeCallback_;
    screenChangeCallback_ = callback;
    lock.unlock();
    return status;
}

void RSRenderServiceConnection::SetScreenActiveMode(ScreenId id, uint32_t modeId)
{
    mainThread_->ScheduleTask([=]() {
        screenManager_->SetScreenActiveMode(id, modeId);
    }).wait();
}

int32_t RSRenderServiceConnection::SetVirtualScreenResolution(ScreenId id, uint32_t width, uint32_t height)
{
    return mainThread_->ScheduleTask([=]() {
        return screenManager_->SetVirtualScreenResolution(id, width, height);
    }).get();
}

void RSRenderServiceConnection::SetScreenPowerStatus(ScreenId id, ScreenPowerStatus status)
{
    mainThread_->ScheduleTask([=]() {
        screenManager_->SetScreenPowerStatus(id, status);
    }).wait();
}

void RSRenderServiceConnection::TakeSurfaceCapture(NodeId id, sptr<RSISurfaceCaptureCallback> callback,
    float scaleX, float scaleY)
{
    std::function<void()> captureTask = [scaleY, scaleX, callback, id]() -> void {
        RS_LOGD("RSRenderService::TakeSurfaceCapture callback->OnSurfaceCapture nodeId:[%" PRIu64 "]", id);
        ROSEN_TRACE_BEGIN(HITRACE_TAG_GRAPHIC_AGP, "RSRenderService::TakeSurfaceCapture");
        RSSurfaceCaptureTask task(id, scaleX, scaleY);
        std::unique_ptr<Media::PixelMap> pixelmap = task.Run();
        callback->OnSurfaceCapture(id, pixelmap.release());
        ROSEN_TRACE_END(HITRACE_TAG_GRAPHIC_AGP);
    };
    mainThread_->PostTask(captureTask);
}

void RSRenderServiceConnection::RegisterApplicationAgent(uint32_t pid, sptr<IApplicationAgent> app)
{
    auto captureTask = [=]() -> void {
        mainThread_->RegisterApplicationAgent(pid, app);
    };
    mainThread_->PostTask(captureTask);

    app->AsObject()->AddDeathRecipient(ApplicationDeathRecipient_);
}

void RSRenderServiceConnection::UnRegisterApplicationAgent(sptr<IApplicationAgent> app)
{
    auto captureTask = [=]() -> void {
        RSMainThread::Instance()->UnRegisterApplicationAgent(app);
    };
    RSMainThread::Instance()->ScheduleTask(captureTask).wait();
}

RSVirtualScreenResolution RSRenderServiceConnection::GetVirtualScreenResolution(ScreenId id)
{
    RSVirtualScreenResolution virtualScreenResolution;
    screenManager_->GetVirtualScreenResolution(id, virtualScreenResolution);
    return virtualScreenResolution;
}

RSScreenModeInfo RSRenderServiceConnection::GetScreenActiveMode(ScreenId id)
{
    RSScreenModeInfo screenModeInfo;
    mainThread_->ScheduleTask([=, &screenModeInfo]() {
        return screenManager_->GetScreenActiveMode(id, screenModeInfo);
    }).wait();
    return screenModeInfo;
}

std::vector<RSScreenModeInfo> RSRenderServiceConnection::GetScreenSupportedModes(ScreenId id)
{
    return mainThread_->ScheduleTask([=]() {
        return screenManager_->GetScreenSupportedModes(id);
    }).get();
}

RSScreenCapability RSRenderServiceConnection::GetScreenCapability(ScreenId id)
{
    RSScreenCapability screenCapability;
    return mainThread_->ScheduleTask([=]() {
        return screenManager_->GetScreenCapability(id);
    }).get();
}

ScreenPowerStatus RSRenderServiceConnection::GetScreenPowerStatus(ScreenId id)
{
    return mainThread_->ScheduleTask([=]() {
        return screenManager_->GetScreenPowerStatus(id);
    }).get();
}

RSScreenData RSRenderServiceConnection::GetScreenData(ScreenId id)
{
    RSScreenData screenData;
    return mainThread_->ScheduleTask([=]() {
        return screenManager_->GetScreenData(id);
    }).get();
}

int32_t RSRenderServiceConnection::GetScreenBacklight(ScreenId id)
{
    return mainThread_->ScheduleTask([=]() {
        return screenManager_->GetScreenBacklight(id);
    }).get();
}

void RSRenderServiceConnection::SetScreenBacklight(ScreenId id, uint32_t level)
{
    mainThread_->ScheduleTask([=]() {
        screenManager_->SetScreenBacklight(id, level);
    }).wait();
}

void RSRenderServiceConnection::RegisterBufferAvailableListener(
    NodeId id, sptr<RSIBufferAvailableCallback> callback, bool isFromRenderThread)
{
    auto registerBufferAvailableListener = [id, callback, isFromRenderThread, this]() -> bool {
        if (auto node = this->mainThread_->GetContext().GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(id)) {
            node->RegisterBufferAvailableListener(callback, isFromRenderThread);
            return true;
        }
        return false;
    };
    if (!registerBufferAvailableListener()) {
        RS_LOGI("RegisterBufferAvailableListener: node not found, post task to retry");
        mainThread_->PostTask(registerBufferAvailableListener);
    }
}

int32_t RSRenderServiceConnection::GetScreenSupportedColorGamuts(ScreenId id, std::vector<ScreenColorGamut>& mode)
{
    return mainThread_->ScheduleTask([=, &mode]() {
        return screenManager_->GetScreenSupportedColorGamuts(id, mode);
    }).get();
}

int32_t RSRenderServiceConnection::GetScreenSupportedMetaDataKeys(ScreenId id, std::vector<ScreenHDRMetadataKey>& keys)
{
    return mainThread_->ScheduleTask([=, &keys]() {
        return screenManager_->GetScreenSupportedMetaDataKeys(id, keys);
    }).get();
}

int32_t RSRenderServiceConnection::GetScreenColorGamut(ScreenId id, ScreenColorGamut& mode)
{
    return mainThread_->ScheduleTask([=, &mode]() {
        return screenManager_->GetScreenColorGamut(id, mode);
    }).get();
}

int32_t RSRenderServiceConnection::SetScreenColorGamut(ScreenId id, int32_t modeIdx)
{
    return mainThread_->ScheduleTask([=]() {
        return screenManager_->SetScreenColorGamut(id, modeIdx);
    }).get();
}

int32_t RSRenderServiceConnection::SetScreenGamutMap(ScreenId id, ScreenGamutMap mode)
{
    return mainThread_->ScheduleTask([=]() {
        return screenManager_->SetScreenGamutMap(id, mode);
    }).get();
}

int32_t RSRenderServiceConnection::GetScreenGamutMap(ScreenId id, ScreenGamutMap& mode)
{
    return mainThread_->ScheduleTask([=, &mode]() {
        return screenManager_->GetScreenGamutMap(id, mode);
    }).get();
}

int32_t RSRenderServiceConnection::GetScreenHDRCapability(ScreenId id, RSScreenHDRCapability& screenHdrCapability)
{
    std::lock_guard<std::mutex> lock(mutex_);
    return screenManager_->GetScreenHDRCapability(id, screenHdrCapability);
}

int32_t RSRenderServiceConnection::GetScreenType(ScreenId id, RSScreenType& screenType)
{
    std::lock_guard<std::mutex> lock(mutex_);
    return screenManager_->GetScreenType(id, screenType);
}

int32_t RSRenderServiceConnection::SetScreenSkipFrameInterval(ScreenId id, uint32_t skipFrameInterval)
{
    return mainThread_->ScheduleTask([=]() {
        return screenManager_->SetScreenSkipFrameInterval(id, skipFrameInterval);
    }).get();
}

int32_t RSRenderServiceConnection::RegisterOcclusionChangeCallback(sptr<RSIOcclusionChangeCallback> callback)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (!callback) {
        RS_LOGD("RSRenderServiceConnection::RegisterOcclusionChangeCallback: callback is nullptr");
        return StatusCode::INVALID_ARGUMENTS;
    }
    mainThread_->RegisterOcclusionChangeCallback(callback);
    return StatusCode::SUCCESS;
}

int32_t RSRenderServiceConnection::UnRegisterOcclusionChangeCallback(sptr<RSIOcclusionChangeCallback> callback)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (!callback) {
        RS_LOGD("RSRenderServiceConnection::UnRegisterOcclusionChangeCallback: callback is nullptr");
        return StatusCode::INVALID_ARGUMENTS;
    }
    mainThread_->UnRegisterOcclusionChangeCallback(callback);
    return StatusCode::SUCCESS;
}

void RSRenderServiceConnection::SetAppWindowNum(uint32_t num)
{
    auto task = [this, num]() -> void {
        mainThread_->SetAppWindowNum(num);
    };
    mainThread_->PostTask(task);
}
} // namespace Rosen
} // namespace OHOS

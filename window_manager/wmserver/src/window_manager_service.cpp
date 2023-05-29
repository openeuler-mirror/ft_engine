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

#include "window_manager_service.h"

#include <thread>

#include <ability_manager_client.h>
#include <cinttypes>
#include <chrono>
#include <hisysevent.h>
#include <hitrace_meter.h>
#include <ipc_skeleton.h>
#include <parameters.h>
#include <rs_iwindow_animation_controller.h>
#include <system_ability_definition.h>
#include <sstream>
#include "xcollie/watchdog.h"

#include "color_parser.h"
#include "display_manager_service_inner.h"
#include "dm_common.h"
#include "drag_controller.h"
#include "memory_guard.h"
#include "minimize_app.h"
#include "permission.h"
#include "remote_animation.h"
#include "singleton_container.h"
#include "starting_window.h"
#include "ui/rs_ui_director.h"
#include "window_helper.h"
#include "window_inner_manager.h"
#include "window_layout_policy.h"
#include "window_manager_agent_controller.h"
#include "window_manager_hilog.h"
#include "wm_common.h"
#include "wm_math.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "WindowManagerService"};
}
WM_IMPLEMENT_SINGLE_INSTANCE(WindowManagerService)

const bool REGISTER_RESULT = SystemAbility::MakeAndRegisterAbility(&SingletonContainer::Get<WindowManagerService>());

WindowManagerService::WindowManagerService() : SystemAbility(WINDOW_MANAGER_SERVICE_ID, true),
    rsInterface_(RSInterfaces::GetInstance()),
    windowShowPerformReport_(new PerformReporter("SHOW_WINDOW_TIME", {20, 35, 50}))
{
    windowRoot_ = new WindowRoot(
        [this](Event event, const sptr<IRemoteObject>& remoteObject) { OnWindowEvent(event, remoteObject); });
    inputWindowMonitor_ = new InputWindowMonitor(windowRoot_);
    windowController_ = new WindowController(windowRoot_, inputWindowMonitor_);
    dragController_ = new DragController(windowRoot_);
    windowDumper_ = new WindowDumper(windowRoot_);
    freezeDisplayController_ = new FreezeController();
    windowCommonEvent_ = std::make_shared<WindowCommonEvent>();
    startingOpen_ = system::GetParameter("persist.window.sw.enabled", "1") == "1"; // startingWin default enabled
    runner_ = AppExecFwk::EventRunner::Create(name_);
    handler_ = std::make_shared<AppExecFwk::EventHandler>(runner_);
    snapshotController_ = new SnapshotController(windowRoot_, handler_);
    int ret = HiviewDFX::Watchdog::GetInstance().AddThread(name_, handler_);
    if (ret != 0) {
        WLOGFE("Add watchdog thread failed");
    }
    handler_->PostTask([]() { MemoryGuard cacheGuard; }, AppExecFwk::EventQueue::Priority::IMMEDIATE);
    // init RSUIDirector, it will handle animation callback
    rsUiDirector_ = RSUIDirector::Create();
    rsUiDirector_->SetUITaskRunner([this](const std::function<void()>& task) { PostAsyncTask(task); });
    rsUiDirector_->Init(false);
}

void WindowManagerService::OnStart()
{
    WLOGFI("start");
    if (!Init()) {
        WLOGFE("Init failed");
        return;
    }
    WindowInnerManager::GetInstance().Start(system::GetParameter("persist.window.holder.enable", "0") == "1");
    sptr<IDisplayChangeListener> listener = new DisplayChangeListener();
    DisplayManagerServiceInner::GetInstance().RegisterDisplayChangeListener(listener);

    sptr<IWindowInfoQueriedListener> windowInfoQueriedListener = new WindowInfoQueriedListener();
    DisplayManagerServiceInner::GetInstance().RegisterWindowInfoQueriedListener(windowInfoQueriedListener);

    PostAsyncTask([this]() {
        sptr<IRSScreenChangeListener> rSScreenChangeListener = new IRSScreenChangeListener();
        rSScreenChangeListener->onConnected_
            = std::bind(&WindowManagerService::OnRSScreenConnected, this);
        rSScreenChangeListener->onDisconnected_
            = std::bind(&WindowManagerService::OnRSScreenDisconnected, this);
        WLOGFI("RegisterRSScreenChangeListener");
        DisplayManagerServiceInner::GetInstance().RegisterRSScreenChangeListener(rSScreenChangeListener);
    });

    AddSystemAbilityListener(RENDER_SERVICE);
    AddSystemAbilityListener(ABILITY_MGR_SERVICE_ID);
    AddSystemAbilityListener(COMMON_EVENT_SERVICE_ID);

    if (!Publish(this)) {
        WLOGFE("Publish failed");
    }
    WLOGFI("end");
}

void WindowManagerService::PostAsyncTask(Task task)
{
    if (handler_) {
        bool ret = handler_->PostTask(task, AppExecFwk::EventQueue::Priority::IMMEDIATE);
        if (!ret) {
            WLOGFE("EventHandler PostTask Failed");
        }
    }
}

void WindowManagerService::PostVoidSyncTask(Task task)
{
    if (handler_) {
        bool ret = handler_->PostSyncTask(task, AppExecFwk::EventQueue::Priority::IMMEDIATE);
        if (!ret) {
            WLOGFE("EventHandler PostVoidSyncTask Failed");
        }
    }
}

void WindowManagerService::OnAddSystemAbility(int32_t systemAbilityId, const std::string &deviceId)
{
    WLOGFI("systemAbilityId: %{public}d, start", systemAbilityId);
    switch (systemAbilityId) {
        case RENDER_SERVICE:
            WLOGFI("RENDER_SERVICE");
            InitWithRanderServiceAdded();
            break;
        case ABILITY_MGR_SERVICE_ID:
            WLOGFI("ABILITY_MGR_SERVICE_ID");
            InitWithAbilityManagerServiceAdded();
            break;
        case COMMON_EVENT_SERVICE_ID:
            WLOGFI("COMMON_EVENT_SERVICE_ID");
            windowCommonEvent_->SubscriberEvent();
            break;
        default:
            WLOGFW("unhandled sysabilityId: %{public}d", systemAbilityId);
            break;
    }
    WLOGFI("systemAbilityId: %{public}d, end", systemAbilityId);
}

void WindowManagerService::OnAccountSwitched(int accountId)
{
    PostAsyncTask([this, accountId]() {
        windowRoot_->RemoveSingleUserWindowNodes(accountId);
    });
    WLOGFI("called");
}

void WindowManagerService::WindowVisibilityChangeCallback(std::shared_ptr<RSOcclusionData> occlusionData)
{
    WLOGFD("NotifyWindowVisibilityChange: enter");
    std::weak_ptr<RSOcclusionData> weak(occlusionData);
    PostVoidSyncTask([this, weak]() {
        auto weakOcclusionData = weak.lock();
        if (weakOcclusionData == nullptr) {
            WLOGFE("weak occlusionData is nullptr");
            return;
        }
        windowRoot_->NotifyWindowVisibilityChange(weakOcclusionData);
    });
}

void WindowManagerService::InitWithRanderServiceAdded()
{
    auto windowVisibilityChangeCb = std::bind(&WindowManagerService::WindowVisibilityChangeCallback, this,
        std::placeholders::_1);
    WLOGFI("RegisterWindowVisibilityChangeCallback");
    if (rsInterface_.RegisterOcclusionChangeCallback(windowVisibilityChangeCb) != WM_OK) {
        WLOGFE("RegisterWindowVisibilityChangeCallback failed");
    }
    RenderModeChangeCallback renderModeChangeCb
        = std::bind(&WindowManagerService::OnRenderModeChanged, this, std::placeholders::_1);
    WLOGFI("SetRenderModeChangeCallback");
    if (rsInterface_.SetRenderModeChangeCallback(renderModeChangeCb) != WM_OK) {
        WLOGFE("SetRenderModeChangeCallback failed");
    }

    if (windowRoot_->GetMaxUniRenderAppWindowNumber() <= 0) {
        rsInterface_.UpdateRenderMode(false);
    }
}

void WindowManagerService::InitWithAbilityManagerServiceAdded()
{
    if (snapshotController_ == nullptr) {
        snapshotController_ = new SnapshotController(windowRoot_, handler_);
    }
    WLOGFI("RegisterSnapshotHandler");
    if (AAFwk::AbilityManagerClient::GetInstance()->RegisterSnapshotHandler(snapshotController_) != ERR_OK) {
        WLOGFE("RegisterSnapshotHandler failed");
    }

    if (wmsHandler_ == nullptr) {
        wmsHandler_ = new WindowManagerServiceHandler();
    }
    WLOGFI("RegisterWindowManagerServiceHandler");
    if (AAFwk::AbilityManagerClient::GetInstance()->RegisterWindowManagerServiceHandler(wmsHandler_) != ERR_OK) {
        WLOGFE("RegisterWindowManagerServiceHandler failed");
    }
}

void WindowManagerServiceHandler::NotifyWindowTransition(
    sptr<AAFwk::AbilityTransitionInfo> from, sptr<AAFwk::AbilityTransitionInfo> to)
{
    sptr<WindowTransitionInfo> fromInfo = nullptr;
    sptr<WindowTransitionInfo> toInfo = nullptr;
    if (from) { // if exists, transition to window transition info
        fromInfo = new WindowTransitionInfo(from);
    }
    if (to) {
        toInfo = new WindowTransitionInfo(to);
    }
    WindowManagerService::GetInstance().NotifyWindowTransition(fromInfo, toInfo, false);
}

int32_t WindowManagerServiceHandler::GetFocusWindow(sptr<IRemoteObject>& abilityToken)
{
    return static_cast<int32_t>(WindowManagerService::GetInstance().GetFocusWindowInfo(abilityToken));
}

void WindowManagerServiceHandler::StartingWindow(
    sptr<AAFwk::AbilityTransitionInfo> info, std::shared_ptr<Media::PixelMap> pixelMap)
{
    sptr<WindowTransitionInfo> windowInfo = new WindowTransitionInfo(info);
    WLOGFI("hot start is called");
    WindowManagerService::GetInstance().StartingWindow(windowInfo, pixelMap, false);
}

void WindowManagerServiceHandler::StartingWindow(
    sptr<AAFwk::AbilityTransitionInfo> info, std::shared_ptr<Media::PixelMap> pixelMap, uint32_t bgColor)
{
    sptr<WindowTransitionInfo> windowInfo = new WindowTransitionInfo(info);
    WLOGFI("cold start is called");
    WindowManagerService::GetInstance().StartingWindow(windowInfo, pixelMap, true, bgColor);
}

void WindowManagerServiceHandler::CancelStartingWindow(sptr<IRemoteObject> abilityToken)
{
    WLOGFI("WindowManagerServiceHandler CancelStartingWindow!");
    WindowManagerService::GetInstance().CancelStartingWindow(abilityToken);
}

bool WindowManagerService::Init()
{
    WLOGFI("Init start");
    if (WindowManagerConfig::LoadConfigXml()) {
        if (WindowManagerConfig::GetConfig().IsMap()) {
            WindowManagerConfig::DumpConfig(*WindowManagerConfig::GetConfig().mapValue_);
        }
        ConfigureWindowManagerService();
    }
    WLOGFI("Init success");
    return true;
}

int WindowManagerService::Dump(int fd, const std::vector<std::u16string>& args)
{
    if (windowDumper_ == nullptr) {
        windowDumper_ = new WindowDumper(windowRoot_);
    }

    return PostSyncTask([this, fd, &args]() {
        return static_cast<int>(windowDumper_->Dump(fd, args));
    });
}

void WindowManagerService::ConfigureWindowManagerService()
{
    const auto& config = WindowManagerConfig::GetConfig();
    WindowManagerConfig::ConfigItem item = config["decor"].GetProp("enable");
    if (item.IsBool()) {
        systemConfig_.isSystemDecorEnable_ = item.boolValue_;
    }
    item = config["minimizeByOther"].GetProp("enable");
    if (item.IsBool()) {
        MinimizeApp::SetMinimizedByOtherConfig(item.boolValue_);
    }
    item = config["stretchable"].GetProp("enable");
    if (item.IsBool()) {
        systemConfig_.isStretchable_ = item.boolValue_;
    }
    item = config["defaultWindowMode"];
    if (item.IsInts()) {
        auto numbers = *item.intsValue_;
        if (numbers.size() == 1 &&
            (numbers[0] == static_cast<int32_t>(WindowMode::WINDOW_MODE_FULLSCREEN) ||
             numbers[0] == static_cast<int32_t>(WindowMode::WINDOW_MODE_FLOATING))) {
            systemConfig_.defaultWindowMode_ = static_cast<WindowMode>(static_cast<uint32_t>(numbers[0]));
            StartingWindow::SetDefaultWindowMode(systemConfig_.defaultWindowMode_);
        }
    }
    item = config["remoteAnimation"].GetProp("enable");
    if (item.IsBool()) {
        RemoteAnimation::isRemoteAnimationEnable_ = item.boolValue_;
    }
    item = config["maxAppWindowNumber"];
    if (item.IsInts()) {
        auto numbers = *item.intsValue_;
        if (numbers.size() == 1 && numbers[0] > 0) {
            windowRoot_->SetMaxAppWindowNumber(static_cast<uint32_t>(numbers[0]));
        }
    }
    item = config["maxUniRenderAppWindowNumber"];
    if (item.IsInts()) {
        auto numbers = *item.intsValue_;
        if (numbers.size() == 1 && numbers[0] > 0) {
            windowRoot_->SetMaxUniRenderAppWindowNumber(static_cast<uint32_t>(numbers[0]));
        }
    }
    item = config["modeChangeHotZones"];
    if (item.IsInts()) {
        ConfigHotZones(*item.intsValue_);
    }
    item = config["splitRatios"];
    if (item.IsFloats()) {
        windowRoot_->SetSplitRatios(*item.floatsValue_);
    }
    item = config["exitSplitRatios"];
    if (item.IsFloats()) {
        windowRoot_->SetExitSplitRatios(*item.floatsValue_);
    }
    item = config["windowAnimation"];
    if (item.IsMap()) {
        ConfigWindowAnimation(item);
    }
    item = config["keyboardAnimation"];
    if (item.IsMap()) {
        ConfigKeyboardAnimation(item);
    }
    item = config["windowEffect"];
    if (item.IsMap()) {
        ConfigWindowEffect(item);
    }
    item = config["floatingBottomPosY"];
    if (item.IsInts()) {
        auto numbers = *item.intsValue_;
        if (numbers.size() == 1 && numbers[0] > 0) {
            WindowLayoutPolicy::SetCascadeRectBottomPosYLimit(static_cast<uint32_t>(numbers[0]));
        }
    }
}

void WindowManagerService::ConfigHotZones(const std::vector<int>& numbers)
{
    if (numbers.size() == 3) { // 3 hot zones
        hotZonesConfig_.fullscreenRange_ = static_cast<uint32_t>(numbers[0]); // 0 fullscreen
        hotZonesConfig_.primaryRange_ = static_cast<uint32_t>(numbers[1]);    // 1 primary
        hotZonesConfig_.secondaryRange_ = static_cast<uint32_t>(numbers[2]);  // 2 secondary
        hotZonesConfig_.isModeChangeHotZoneConfigured_ = true;
    }
}

void WindowManagerService::ConfigWindowAnimation(const WindowManagerConfig::ConfigItem& animeConfig)
{
    auto& windowAnimationConfig = WindowNodeContainer::GetAnimationConfigRef().windowAnimationConfig_;
    WindowManagerConfig::ConfigItem item = animeConfig["timing"];
    if (item.IsMap() && item.mapValue_->count("curve")) {
        windowAnimationConfig.animationTiming_.timingCurve_ = CreateCurve(item["curve"]);
    }
    item = animeConfig["timing"]["duration"];
    if (item.IsInts()) {
        auto numbers = *item.intsValue_;
        if (numbers.size() == 1) { // duration
            windowAnimationConfig.animationTiming_.timingProtocol_ =
                RSAnimationTimingProtocol(numbers[0]);
        }
    }
    item = animeConfig["scale"];
    if (item.IsFloats()) {
        auto numbers = *item.floatsValue_;
        if (numbers.size() == 1) { // 1 xy scale
            windowAnimationConfig.scale_.x_ =
            windowAnimationConfig.scale_.y_ = numbers[0]; // 0 xy scale
        } else if (numbers.size() == 2) { // 2 x,y sclae
            windowAnimationConfig.scale_.x_ = numbers[0]; // 0 x scale
            windowAnimationConfig.scale_.y_ = numbers[1]; // 1 y scale
        } else if (numbers.size() == 3) { // 3 x,y,z scale
            windowAnimationConfig.scale_ = Vector3f(&numbers[0]);
        }
    }
    item = animeConfig["rotation"];
    if (item.IsFloats() && item.floatsValue_->size() == 4) { // 4 (axix,angle)
        windowAnimationConfig.rotation_ = Vector4f(item.floatsValue_->data());
    }
    item = animeConfig["translate"];
    if (item.IsFloats()) {
        auto numbers = *item.floatsValue_;
        if (numbers.size() == 2) { // 2 translate xy
            windowAnimationConfig.translate_.x_ = numbers[0]; // 0 translate x
            windowAnimationConfig.translate_.y_ = numbers[1]; // 1 translate y
        } else if (numbers.size() == 3) { // 3 translate xyz
            windowAnimationConfig.translate_.x_ = numbers[0]; // 0 translate x
            windowAnimationConfig.translate_.y_ = numbers[1]; // 1 translate y
            windowAnimationConfig.translate_.z_ = numbers[2]; // 2 translate z
        }
    }
    item = animeConfig["opacity"];
    if (item.IsFloats()) {
        auto numbers = *item.floatsValue_;
        numbers.size() == 1 ? (windowAnimationConfig.opacity_ = numbers[0]) : float();
    }
}

void WindowManagerService::ConfigKeyboardAnimation(const WindowManagerConfig::ConfigItem& animeConfig)
{
    auto& animationConfig = WindowNodeContainer::GetAnimationConfigRef();
    WindowManagerConfig::ConfigItem item = animeConfig["timing"];
    if (item.IsMap() && item.mapValue_->count("curve")) {
        animationConfig.keyboardAnimationConfig_.curve_ = CreateCurve(item["curve"]);
    }
    item = animeConfig["timing"]["durationIn"];
    if (item.IsInts()) {
        auto numbers = *item.intsValue_;
        if (numbers.size() == 1) { // duration
            animationConfig.keyboardAnimationConfig_.durationIn_ =
                RSAnimationTimingProtocol(numbers[0]);
        }
    }
    item = animeConfig["timing"]["durationOut"];
    if (item.IsInts()) {
        auto numbers = *item.intsValue_;
        if (numbers.size() == 1) { // duration
            animationConfig.keyboardAnimationConfig_.durationOut_ =
                RSAnimationTimingProtocol(numbers[0]);
        }
    }
}

bool WindowManagerService::ConfigAppWindowCornerRadius(const WindowManagerConfig::ConfigItem& item, float& out)
{
    std::map<std::string, float> stringToCornerRadius = {
        {"off", 0.0f}, {"defaultCornerRadiusXS", 4.0f}, {"defaultCornerRadiusS", 8.0f},
        {"defaultCornerRadiusM", 12.0f}, {"defaultCornerRadiusL", 16.0f}, {"defaultCornerRadiusXL", 24.0f}
    };

    if (item.IsString()) {
        auto value = item.stringValue_;
        if (stringToCornerRadius.find(value) != stringToCornerRadius.end()) {
            out = stringToCornerRadius[value];
            return true;
        }
    }
    return false;
}

bool WindowManagerService::ConfigAppWindowShadow(const WindowManagerConfig::ConfigItem& shadowConfig,
    WindowShadowParameters& outShadow)
{
    WindowManagerConfig::ConfigItem item = shadowConfig["elevation"];
    if (item.IsFloats()) {
        auto elevation = *item.floatsValue_;
        if (elevation.size() != 1 || MathHelper::LessNotEqual(elevation[0], 0.0)) {
            return false;
        }
        outShadow.elevation_ = elevation[0];
    }

    item = shadowConfig["color"];
    if (item.IsString()) {
        auto color = item.stringValue_;
        uint32_t colorValue;
        if (!ColorParser::Parse(color, colorValue)) {
            return false;
        }
        outShadow.color_ = color;
    }

    item = shadowConfig["offsetX"];
    if (item.IsFloats()) {
        auto offsetX = *item.floatsValue_;
        if (offsetX.size() != 1) {
            return false;
        }
        outShadow.offsetX_ = offsetX[0];
    }

    item = shadowConfig["offsetY"];
    if (item.IsFloats()) {
        auto offsetY = *item.floatsValue_;
        if (offsetY.size() != 1) {
            return false;
        }
        outShadow.offsetY_ = offsetY[0];
    }

    item = shadowConfig["alpha"];
    if (item.IsFloats()) {
        auto alpha = *item.floatsValue_;
        if (alpha.size() != 1 || (alpha.size() == 1 &&
            MathHelper::LessNotEqual(alpha[0], 0.0) && MathHelper::GreatNotEqual(alpha[0], 1.0))) {
            return false;
        }
        outShadow.alpha_ = alpha[0];
    }
    return true;
}

void WindowManagerService::ConfigWindowEffect(const WindowManagerConfig::ConfigItem& effectConfig)
{
    AppWindowEffectConfig config;

    // config corner radius
    WindowManagerConfig::ConfigItem item = effectConfig["appWindows"]["cornerRadius"];
    if (item.IsMap()) {
        if (ConfigAppWindowCornerRadius(item["fullScreen"], config.fullScreenCornerRadius_) &&
            ConfigAppWindowCornerRadius(item["split"], config.splitCornerRadius_) &&
            ConfigAppWindowCornerRadius(item["float"], config.floatCornerRadius_)) {
            systemConfig_.effectConfig_ = config;
        }
    }

    // config shadow
    item = effectConfig["appWindows"]["shadow"]["focused"];
    if (item.IsMap()) {
        if (ConfigAppWindowShadow(item, config.focusedShadow_)) {
            systemConfig_.effectConfig_.focusedShadow_ = config.focusedShadow_;
        }
    }

    item = effectConfig["appWindows"]["shadow"]["unfocused"];
    if (item.IsMap()) {
        if (ConfigAppWindowShadow(item, config.unfocusedShadow_)) {
            systemConfig_.effectConfig_.unfocusedShadow_ = config.unfocusedShadow_;
        }
    }
}

RSAnimationTimingCurve WindowManagerService::CreateCurve(const WindowManagerConfig::ConfigItem& curveConfig)
{
    const auto& nameItem = curveConfig.GetProp("name");
    if (nameItem.IsString()) {
        std::string name = nameItem.stringValue_;
        if (name == "easeOut") {
            return RSAnimationTimingCurve::EASE_OUT;
        } else if (name == "ease") {
            return RSAnimationTimingCurve::EASE;
        } else if (name == "easeIn") {
            return RSAnimationTimingCurve::EASE_IN;
        } else if (name == "easeInOut") {
            return RSAnimationTimingCurve::EASE_IN_OUT;
        } else if (name == "default") {
            return RSAnimationTimingCurve::DEFAULT;
        } else if (name == "linear") {
            return RSAnimationTimingCurve::LINEAR;
        } else if (name == "spring") {
            return RSAnimationTimingCurve::SPRING;
        } else if (name == "interactiveSpring") {
            return RSAnimationTimingCurve::INTERACTIVE_SPRING;
        } else if (name == "cubic" && curveConfig.IsFloats() &&
            curveConfig.floatsValue_->size() == 4) { // 4 curve parameter
            const auto& numbers = *curveConfig.floatsValue_;
            return RSAnimationTimingCurve::CreateCubicCurve(numbers[0], // 0 ctrlX1
                numbers[1], // 1 ctrlY1
                numbers[2], // 2 ctrlX2
                numbers[3]); // 3 ctrlY2
        }
    }
    return RSAnimationTimingCurve::EASE_OUT;
}

void WindowManagerService::OnStop()
{
    windowCommonEvent_->UnSubscriberEvent();
    WindowInnerManager::GetInstance().Stop();
    WLOGFI("ready to stop service.");
}

WMError WindowManagerService::NotifyWindowTransition(
    sptr<WindowTransitionInfo>& fromInfo, sptr<WindowTransitionInfo>& toInfo, bool isFromClient)
{
    if (!isFromClient) {
        WLOGFI("NotifyWindowTransition asynchronously.");
        PostAsyncTask([this, fromInfo, toInfo]() mutable {
            return windowController_->NotifyWindowTransition(fromInfo, toInfo);
        });
        return WMError::WM_OK;
    } else {
        WLOGFI("NotifyWindowTransition synchronously.");
        return PostSyncTask([this, &fromInfo, &toInfo]() {
            return windowController_->NotifyWindowTransition(fromInfo, toInfo);
        });
    }
}

WMError WindowManagerService::GetFocusWindowInfo(sptr<IRemoteObject>& abilityToken)
{
    return PostSyncTask([this, &abilityToken]() {
        return windowController_->GetFocusWindowInfo(abilityToken);
    });
}

void WindowManagerService::StartingWindow(sptr<WindowTransitionInfo> info, std::shared_ptr<Media::PixelMap> pixelMap,
    bool isColdStart, uint32_t bkgColor)
{
    if (!startingOpen_) {
        WLOGFI("startingWindow not open!");
        return;
    }
    PostAsyncTask([this, info, pixelMap, isColdStart, bkgColor]() {
        windowController_->StartingWindow(info, pixelMap, bkgColor, isColdStart);
    });
}

void WindowManagerService::CancelStartingWindow(sptr<IRemoteObject> abilityToken)
{
    WLOGFI("begin CancelStartingWindow!");
    if (!startingOpen_) {
        WLOGFI("startingWindow not open!");
        return;
    }
    PostAsyncTask([this, abilityToken]() {
        windowController_->CancelStartingWindow(abilityToken);
    });
}

bool WindowManagerService::CheckAnimationPermission(const sptr<WindowProperty>& property) const
{
    if (property->GetAnimationFlag() == static_cast<uint32_t>(WindowAnimation::CUSTOM)
        && !Permission::IsSystemCalling()) {
        WLOGFD("check animation permission failed");
        return false;
    }
    return true;
}

bool WindowManagerService::CheckSystemWindowPermission(const sptr<WindowProperty>& property) const
{
    WindowType type = property->GetWindowType();
    if (!WindowHelper::IsSystemWindow(type)) {
        // type is not system
        return true;
    }
    if (type == WindowType::WINDOW_TYPE_DRAGGING_EFFECT || type == WindowType::WINDOW_TYPE_SYSTEM_ALARM_WINDOW ||
        type == WindowType::WINDOW_TYPE_TOAST) {
        // some system types counld be created by normal app
        return true;
    }
    if (type == WindowType::WINDOW_TYPE_FLOAT &&
        Permission::CheckCallingPermission("ohos.permission.SYSTEM_FLOAT_WINDOW")) {
        // WINDOW_TYPE_FLOAT counld be created by normal app with the corresponding permission
        WLOGFD("check create permission success, normal app create float window with request permission.");
        return true;
    }
    if (Permission::IsSystemCalling()) {
        WLOGFD("check create permission success, create with system calling.");
        return true;
    }
    WLOGFD("check system window permission failed.");
    return false;
}

WMError WindowManagerService::CreateWindow(sptr<IWindow>& window, sptr<WindowProperty>& property,
    const std::shared_ptr<RSSurfaceNode>& surfaceNode, uint32_t& windowId, sptr<IRemoteObject> token)
{
    if (!window || property == nullptr || surfaceNode == nullptr || !window->AsObject()) {
        WLOGFE("window is invalid");
        return WMError::WM_ERROR_NULLPTR;
    }
    if (!CheckSystemWindowPermission(property)) {
        WLOGFE("create system window permission denied!");
        return WMError::WM_ERROR_INVALID_PERMISSION;
    }
    int pid = IPCSkeleton::GetCallingPid();
    int uid = IPCSkeleton::GetCallingUid();
    WMError ret = PostSyncTask([this, pid, uid, &window, &property, &surfaceNode, &windowId, &token]() {
        HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "wms:CreateWindow(%u)", windowId);
        return windowController_->CreateWindow(window, property, surfaceNode, windowId, token, pid, uid);
    });
    accessTokenIdMaps_.insert(std::pair(windowId, IPCSkeleton::GetCallingTokenID()));
    return ret;
}

WMError WindowManagerService::AddWindow(sptr<WindowProperty>& property)
{
    if (property == nullptr) {
        WLOGFE("property is nullptr");
        return WMError::WM_ERROR_NULLPTR;
    }
    if (!CheckSystemWindowPermission(property) || !CheckAnimationPermission(property)) {
        WLOGFE("add window permission denied!");
        return WMError::WM_ERROR_INVALID_PERMISSION;
    }
    return PostSyncTask([this, &property]() {
        windowShowPerformReport_->start();
        Rect rect = property->GetRequestRect();
        uint32_t windowId = property->GetWindowId();
        WLOGFI("[WMS] Add: %{public}5d %{public}4d %{public}4d %{public}4d [%{public}4d %{public}4d " \
            "%{public}4d %{public}4d]", windowId, property->GetWindowType(), property->GetWindowMode(),
            property->GetWindowFlags(), rect.posX_, rect.posY_, rect.width_, rect.height_);
        HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "wms:AddWindow(%u)", windowId);
        WMError res = windowController_->AddWindowNode(property);
        if (property->GetWindowType() == WindowType::WINDOW_TYPE_DRAGGING_EFFECT) {
            dragController_->StartDrag(windowId);
        }
        if (res == WMError::WM_OK) {
            windowShowPerformReport_->end();
        }
        return res;
    });
}

WMError WindowManagerService::RemoveWindow(uint32_t windowId)
{
    return PostSyncTask([this, windowId]() {
        WLOGFI("[WMS] Remove: %{public}u", windowId);
        HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "wms:RemoveWindow(%u)", windowId);
        WindowInnerManager::GetInstance().NotifyWindowRemovedOrDestroyed(windowId);
        WMError res = windowController_->RecoverInputEventToClient(windowId);
        if (res != WMError::WM_OK) {
            return res;
        }
        return windowController_->RemoveWindowNode(windowId);
    });
}

WMError WindowManagerService::DestroyWindow(uint32_t windowId, bool onlySelf)
{
    if (!accessTokenIdMaps_.isExistAndRemove(windowId, IPCSkeleton::GetCallingTokenID())) {
        WLOGFI("Operation rejected");
        return WMError::WM_ERROR_INVALID_OPERATION;
    }
    return PostSyncTask([this, windowId, onlySelf]() {
        auto node = windowRoot_->GetWindowNode(windowId);
        if (node == nullptr) {
            return WMError::WM_ERROR_NULLPTR;
        }
        node->stateMachine_.SetDestroyTaskParam(onlySelf);
        auto func = [this, windowId]() {
            WLOGFI("[WMS] Destroy: %{public}u", windowId);
            HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "wms:DestroyWindow(%u)", windowId);
            WindowInnerManager::GetInstance().NotifyWindowRemovedOrDestroyed(windowId);
            auto node = windowRoot_->GetWindowNode(windowId);
            if (node == nullptr) {
                return WMError::WM_OK;
            }
            if (node->GetWindowType() == WindowType::WINDOW_TYPE_DRAGGING_EFFECT) {
                dragController_->FinishDrag(windowId);
            }
            return windowController_->DestroyWindow(windowId, node->stateMachine_.GetDestroyTaskParam());
        };
        if (RemoteAnimation::IsRemoteAnimationEnabledAndFirst(node->GetDisplayId()) &&
            node->stateMachine_.IsRemoteAnimationPlaying()) {
            WLOGFI("SetDestroyTask id:%{public}u", node->GetWindowId());
            node->stateMachine_.SetDestroyTask(func);
            return WMError::WM_OK;
        }
        WLOGFI("DestroyWindow windowId: %{public}u, name:%{public}s state: %{public}u",
            node->GetWindowId(), node->GetWindowName().c_str(),
            static_cast<uint32_t>(node->stateMachine_.GetCurrentState()));
        return func();
    });
}

WMError WindowManagerService::RequestFocus(uint32_t windowId)
{
    return PostSyncTask([this, windowId]() {
        WLOGFI("[WMS] RequestFocus: %{public}u", windowId);
        return windowController_->RequestFocus(windowId);
    });
}

AvoidArea WindowManagerService::GetAvoidAreaByType(uint32_t windowId, AvoidAreaType avoidAreaType)
{
    return PostSyncTask([this, windowId, avoidAreaType]() {
        WLOGFI("[WMS] GetAvoidAreaByType: %{public}u, Type: %{public}u", windowId,
            static_cast<uint32_t>(avoidAreaType));
        return windowController_->GetAvoidAreaByType(windowId, avoidAreaType);
    });
}

bool WindowManagerService::RegisterWindowManagerAgent(WindowManagerAgentType type,
    const sptr<IWindowManagerAgent>& windowManagerAgent)
{
    if (!Permission::IsSystemCalling()) {
        WLOGFE("register windowManager agent permission denied!");
        return false;
    }
    if ((windowManagerAgent == nullptr) || (windowManagerAgent->AsObject() == nullptr)) {
        WLOGFE("windowManagerAgent is null");
        return false;
    }
    return PostSyncTask([this, &windowManagerAgent, type]() {
        bool ret = WindowManagerAgentController::GetInstance().RegisterWindowManagerAgent(windowManagerAgent, type);
        if (type == WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_SYSTEM_BAR) { // if system bar, notify once
            windowController_->NotifySystemBarTints();
        }
        return ret;
    });
}

bool WindowManagerService::UnregisterWindowManagerAgent(WindowManagerAgentType type,
    const sptr<IWindowManagerAgent>& windowManagerAgent)
{
    if (!Permission::IsSystemCalling()) {
        WLOGFE("unregister windowManager agent permission denied!");
        return false;
    }
    if ((windowManagerAgent == nullptr) || (windowManagerAgent->AsObject() == nullptr)) {
        WLOGFE("windowManagerAgent is null");
        return false;
    }
    return PostSyncTask([this, &windowManagerAgent, type]() {
        return WindowManagerAgentController::GetInstance().UnregisterWindowManagerAgent(windowManagerAgent, type);
    });
}

WMError WindowManagerService::SetWindowAnimationController(const sptr<RSIWindowAnimationController>& controller)
{
    if (!Permission::IsSystemCalling()) {
        WLOGFE("set window animation controller permission denied!");
        return WMError::WM_ERROR_INVALID_PERMISSION;
    }
    if (controller == nullptr) {
        WLOGFE("RSWindowAnimation: Failed to set window animation controller, controller is null!");
        return WMError::WM_ERROR_NULLPTR;
    }

    sptr<AgentDeathRecipient> deathRecipient = new AgentDeathRecipient(
        [this](sptr<IRemoteObject>& remoteObject) {
            PostVoidSyncTask([&remoteObject]() {
                RemoteAnimation::OnRemoteDie(remoteObject);
            });
        }
    );
    controller->AsObject()->AddDeathRecipient(deathRecipient);
    RemoteAnimation::SetWindowControllerAndRoot(windowController_, windowRoot_);
    RemoteAnimation::SetMainTaskHandler(handler_);
    return PostSyncTask([this, &controller]() {
        WMError ret = windowController_->SetWindowAnimationController(controller);
        RemoteAnimation::SetAnimationFirst(system::GetParameter("persist.window.af.enabled", "1") == "1");
        return ret;
    });
}

void WindowManagerService::OnWindowEvent(Event event, const sptr<IRemoteObject>& remoteObject)
{
    if (event == Event::REMOTE_DIED) {
        PostVoidSyncTask([this, &remoteObject]() {
            uint32_t windowId = windowRoot_->GetWindowIdByObject(remoteObject);
            auto node = windowRoot_->GetWindowNode(windowId);
            if (node == nullptr) {
                WLOGFD("window node is nullptr, REMOTE_DIED no need to destroy");
                return;
            }
            WLOGI("window %{public}u received REMOTE_DIED", windowId);
            node->stateMachine_.SetDestroyTaskParam(true);
            auto func = [this, windowId]() {
                auto node = windowRoot_->GetWindowNode(windowId);
                if (node == nullptr) {
                    WLOGFD("window node is nullptr");
                    return;
                }
                if (node->GetWindowType() == WindowType::WINDOW_TYPE_DRAGGING_EFFECT) {
                    dragController_->FinishDrag(windowId);
                }
                WindowInnerManager::GetInstance().NotifyWindowRemovedOrDestroyed(windowId);
                windowController_->DestroyWindow(windowId, node->stateMachine_.GetDestroyTaskParam());
            };

            if (node->GetWindowType() == WindowType::WINDOW_TYPE_DESKTOP) {
                RemoteAnimation::OnRemoteDie(remoteObject);
            }
            if (RemoteAnimation::IsRemoteAnimationEnabledAndFirst(node->GetDisplayId()) &&
                node->stateMachine_.IsRemoteAnimationPlaying()) {
                WLOGFI("set destroy task windowId:%{public}u", node->GetWindowId());
                node->stateMachine_.SetDestroyTask(func);
                handler_->PostTask(func, "destroyTimeOutTask", 6000); // 6000 is time out 6s
                return;
            }
            func();
        });
    }
}

void WindowManagerService::NotifyDisplayStateChange(DisplayId defaultDisplayId, sptr<DisplayInfo> displayInfo,
    const std::map<DisplayId, sptr<DisplayInfo>>& displayInfoMap, DisplayStateChangeType type)
{
    HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "wms:NotifyDisplayStateChange(%u)", type);
    DisplayId displayId = (displayInfo == nullptr) ? DISPLAY_ID_INVALID : displayInfo->GetDisplayId();
    if (type == DisplayStateChangeType::FREEZE) {
        freezeDisplayController_->FreezeDisplay(displayId);
    } else if (type == DisplayStateChangeType::UNFREEZE) {
        freezeDisplayController_->UnfreezeDisplay(displayId);
        /*
         * Set 'InnerInputManager Listener' to MMI, ensure that the listener
         * for move/drag won't be replaced by freeze-display-window
         */
        WindowInnerManager::GetInstance().SetInputEventConsumer();
    } else {
        PostAsyncTask([this, defaultDisplayId, displayInfo, displayInfoMap, type]() mutable {
            windowController_->NotifyDisplayStateChange(defaultDisplayId, displayInfo, displayInfoMap, type);
        });
    }
}

void DisplayChangeListener::OnDisplayStateChange(DisplayId defaultDisplayId, sptr<DisplayInfo> displayInfo,
    const std::map<DisplayId, sptr<DisplayInfo>>& displayInfoMap, DisplayStateChangeType type)
{
    WindowManagerService::GetInstance().NotifyDisplayStateChange(defaultDisplayId, displayInfo, displayInfoMap, type);
}

void DisplayChangeListener::OnScreenshot(DisplayId displayId)
{
    WindowManagerService::GetInstance().OnScreenshot(displayId);
}

void WindowManagerService::NotifyServerReadyToMoveOrDrag(uint32_t windowId, sptr<WindowProperty>& windowProperty,
    sptr<MoveDragProperty>& moveDragProperty)
{
    if (windowProperty == nullptr || moveDragProperty == nullptr) {
        WLOGFE("windowProperty or moveDragProperty is invalid");
        return;
    }

    PostAsyncTask([this, windowId, windowProperty, moveDragProperty]() mutable {
        if (moveDragProperty->startDragFlag_ || moveDragProperty->startMoveFlag_) {
            bool res = WindowInnerManager::GetInstance().NotifyServerReadyToMoveOrDrag(windowId,
                windowProperty, moveDragProperty);
            if (!res) {
                WLOGFE("invalid operation");
                return;
            }
            windowController_->InterceptInputEventToServer(windowId);
        }
        windowController_->NotifyServerReadyToMoveOrDrag(windowId, moveDragProperty);
    });
}

void WindowManagerService::ProcessPointDown(uint32_t windowId, bool isPointDown)
{
    PostAsyncTask([this, windowId, isPointDown]() {
        windowController_->ProcessPointDown(windowId, isPointDown);
    });
}

void WindowManagerService::ProcessPointUp(uint32_t windowId)
{
    PostAsyncTask([this, windowId]() {
        WindowInnerManager::GetInstance().NotifyWindowEndUpMovingOrDragging(windowId);
        windowController_->RecoverInputEventToClient(windowId);
        windowController_->ProcessPointUp(windowId);
    });
}

void WindowManagerService::NotifyWindowClientPointUp(uint32_t windowId,
    const std::shared_ptr<MMI::PointerEvent>& pointerEvent)
{
    PostAsyncTask([this, windowId, pointerEvent]() mutable {
        windowController_->NotifyWindowClientPointUp(windowId, pointerEvent);
    });
}

void WindowManagerService::MinimizeAllAppWindows(DisplayId displayId)
{
    if (!Permission::IsSystemCalling()) {
        WLOGFE("minimize all appWindows permission denied!");
        return;
    }
    PostAsyncTask([this, displayId]() {
        HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "wms:MinimizeAllAppWindows(%" PRIu64")", displayId);
        WLOGFI("displayId %{public}" PRIu64"", displayId);
        windowController_->MinimizeAllAppWindows(displayId);
    });
}

WMError WindowManagerService::ToggleShownStateForAllAppWindows()
{
    if (!Permission::IsSystemCalling()) {
        WLOGFE("toggle shown state for all appwindows permission denied!");
        return WMError::WM_ERROR_INVALID_PERMISSION;
    }
    PostAsyncTask([this]() {
        HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "wms:ToggleShownStateForAllAppWindows");
        return windowController_->ToggleShownStateForAllAppWindows();
    });
    return WMError::WM_OK;
}

WMError WindowManagerService::GetTopWindowId(uint32_t mainWinId, uint32_t& topWinId)
{
    return PostSyncTask([this, &topWinId, mainWinId]() {
        return windowController_->GetTopWindowId(mainWinId, topWinId);
    });
}

WMError WindowManagerService::SetWindowLayoutMode(WindowLayoutMode mode)
{
    if (!Permission::IsSystemCalling()) {
        WLOGFE("set window layout mode permission denied!");
        return WMError::WM_ERROR_INVALID_PERMISSION;
    }
    return PostSyncTask([this, mode]() {
        WLOGFI("layoutMode: %{public}u", mode);
        HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "wms:SetWindowLayoutMode");
        return windowController_->SetWindowLayoutMode(mode);
    });
}

WMError WindowManagerService::UpdateProperty(sptr<WindowProperty>& windowProperty, PropertyChangeAction action,
    bool isAsyncTask)
{
    if (windowProperty == nullptr) {
        WLOGFE("windowProperty is nullptr");
        return WMError::WM_ERROR_NULLPTR;
    }
    if ((windowProperty->GetWindowFlags() == static_cast<uint32_t>(WindowFlag::WINDOW_FLAG_FORBID_SPLIT_MOVE) ||
        action == PropertyChangeAction::ACTION_UPDATE_TRANSFORM_PROPERTY) &&
        !Permission::IsSystemCalling()) {
        WLOGFE("SetForbidSplitMove or SetShowWhenLocked or SetTranform or SetTurnScreenOn permission denied!");
        return WMError::WM_ERROR_INVALID_PERMISSION;
    }

    if (action == PropertyChangeAction::ACTION_UPDATE_TRANSFORM_PROPERTY) {
        return PostSyncTask([this, windowProperty, action]() mutable {
            windowController_->UpdateProperty(windowProperty, action);
            return WMError::WM_OK;
        });
    }

    if (isAsyncTask || action == PropertyChangeAction::ACTION_UPDATE_RECT) {
        PostAsyncTask([this, windowProperty, action]() mutable {
            HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "wms:UpdateProperty");
            WMError res = windowController_->UpdateProperty(windowProperty, action);
            if (action == PropertyChangeAction::ACTION_UPDATE_RECT && res == WMError::WM_OK &&
                windowProperty->GetWindowSizeChangeReason() == WindowSizeChangeReason::MOVE) {
                dragController_->UpdateDragInfo(windowProperty->GetWindowId());
            }
        });
        return WMError::WM_OK;
    }

    return PostSyncTask([this, &windowProperty, action]() {
        HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "wms:UpdateProperty");
        WMError res = windowController_->UpdateProperty(windowProperty, action);
        if (action == PropertyChangeAction::ACTION_UPDATE_RECT && res == WMError::WM_OK &&
            windowProperty->GetWindowSizeChangeReason() == WindowSizeChangeReason::MOVE) {
            dragController_->UpdateDragInfo(windowProperty->GetWindowId());
        }
        return res;
    });
}

WMError WindowManagerService::GetAccessibilityWindowInfo(std::vector<sptr<AccessibilityWindowInfo>>& infos)
{
    if (!Permission::IsSystemServiceCalling()) {
        WLOGFE("get accessibility window info permission denied!");
        return WMError::WM_ERROR_INVALID_PERMISSION;
    }
    return PostSyncTask([this, &infos]() {
        return windowController_->GetAccessibilityWindowInfo(infos);
    });
}

WMError WindowManagerService::GetVisibilityWindowInfo(std::vector<sptr<WindowVisibilityInfo>>& infos)
{
    return PostSyncTask([this, &infos]() {
        return windowController_->GetVisibilityWindowInfo(infos);
    });
}

std::shared_ptr<Media::PixelMap> WindowManagerService::GetSnapshot(int32_t windowId)
{
    return nullptr;
}

void WindowManagerService::NotifyDumpInfoResult(const std::vector<std::string>& info)
{
    if (windowDumper_) {
        windowDumper_->dumpInfoFuture_.SetValue(info);
    }
}

WMError WindowManagerService::GetSystemConfig(SystemConfig& systemConfig)
{
    systemConfig = systemConfig_;
    return WMError::WM_OK;
}

WMError WindowManagerService::GetModeChangeHotZones(DisplayId displayId, ModeChangeHotZones& hotZones)
{
    if (!hotZonesConfig_.isModeChangeHotZoneConfigured_) {
        return WMError::WM_DO_NOTHING;
    }

    return windowController_->GetModeChangeHotZones(displayId, hotZones, hotZonesConfig_);
}

void WindowManagerService::MinimizeWindowsByLauncher(std::vector<uint32_t> windowIds, bool isAnimated,
    sptr<RSIWindowAnimationFinishedCallback>& finishCallback)
{
    if (!Permission::IsSystemCalling()) {
        WLOGFE("minimize windows by launcher permission denied!");
        return;
    }
    PostVoidSyncTask([this, windowIds, isAnimated, &finishCallback]() mutable {
        windowController_->MinimizeWindowsByLauncher(windowIds, isAnimated, finishCallback);
    });
}

WMError WindowManagerService::UpdateAvoidAreaListener(uint32_t windowId, bool haveAvoidAreaListener)
{
    return PostSyncTask([this, windowId, haveAvoidAreaListener]() {
        sptr<WindowNode> node = windowRoot_->GetWindowNode(windowId);
        if (node == nullptr) {
            WLOGFE("get window node failed. win %{public}u", windowId);
            return WMError::WM_DO_NOTHING;
        }
        sptr<WindowNodeContainer> container = windowRoot_->GetWindowNodeContainer(node->GetDisplayId());
        if (container == nullptr) {
            WLOGFE("get container failed. win %{public}u display %{public}" PRIu64"", windowId, node->GetDisplayId());
            return WMError::WM_DO_NOTHING;
        }
        container->UpdateAvoidAreaListener(node, haveAvoidAreaListener);
        return WMError::WM_OK;
    });
}

void WindowManagerService::SetAnchorAndScale(int32_t x, int32_t y, float scale)
{
    PostAsyncTask([this, x, y, scale]() {
        windowController_->SetAnchorAndScale(x, y, scale);
    });
}

void WindowManagerService::SetAnchorOffset(int32_t deltaX, int32_t deltaY)
{
    PostAsyncTask([this, deltaX, deltaY]() {
        windowController_->SetAnchorOffset(deltaX, deltaY);
    });
}

void WindowManagerService::OffWindowZoom()
{
    PostAsyncTask([this]() {
        windowController_->OffWindowZoom();
    });
}

WMError WindowManagerService::UpdateRsTree(uint32_t windowId, bool isAdd)
{
    return PostSyncTask([this, windowId, isAdd]() {
        return windowRoot_->UpdateRsTree(windowId, isAdd);
    });
}

void WindowManagerService::OnScreenshot(DisplayId displayId)
{
    PostAsyncTask([this, displayId]() {
        windowController_->OnScreenshot(displayId);
    });
}

WMError WindowManagerService::BindDialogTarget(uint32_t& windowId, sptr<IRemoteObject> targetToken)
{
    if (!Permission::IsSystemCalling()) {
        WLOGFE("bind dialog target permission denied!");
        return WMError::WM_ERROR_INVALID_PERMISSION;
    }
    return PostSyncTask([this, &windowId, targetToken]() {
        return windowController_->BindDialogTarget(windowId, targetToken);
    });
}

void WindowManagerService::HasPrivateWindow(DisplayId displayId, bool& hasPrivateWindow)
{
    PostVoidSyncTask([this, displayId, &hasPrivateWindow]() mutable {
        hasPrivateWindow = windowRoot_->HasPrivateWindow(displayId);
    });
    WLOGFI("called %{public}u", hasPrivateWindow);
}

void WindowInfoQueriedListener::HasPrivateWindow(DisplayId displayId, bool& hasPrivateWindow)
{
    WLOGFI("called");
    WindowManagerService::GetInstance().HasPrivateWindow(displayId, hasPrivateWindow);
}

void WindowManagerService::OnRSScreenConnected()
{
    PostAsyncTask([this]() {
        windowRoot_->SwitchRenderModeIfNeeded();
    });
}

void WindowManagerService::OnRSScreenDisconnected()
{
    PostAsyncTask([this]() {
        windowRoot_->SwitchRenderModeIfNeeded();
    });
}

void WindowManagerService::OnRenderModeChanged(bool isUniRender)
{
    PostAsyncTask([this, isUniRender]() {
        windowRoot_->OnRenderModeChanged(isUniRender);
    });
}
} // namespace Rosen
} // namespace OHOS

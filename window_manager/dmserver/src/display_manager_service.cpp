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

#include "display_manager_service.h"

#include <cinttypes>
#include <hitrace_meter.h>
#include <ipc_skeleton.h>
#include <iservice_registry.h>
#include <system_ability_definition.h>

#include "display_manager_agent_controller.h"
#include "display_manager_config.h"
#include "dm_common.h"
#include "parameters.h"
#include "permission.h"
#include "sensor_connector.h"
#include "transaction/rs_interfaces.h"
#include "window_manager_hilog.h"

namespace OHOS::Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_DISPLAY, "DisplayManagerService"};
    const std::string SCREEN_CAPTURE_PERMISSION = "ohos.permission.CAPTURE_SCREEN";
}
WM_IMPLEMENT_SINGLE_INSTANCE(DisplayManagerService)
const bool REGISTER_RESULT = SystemAbility::MakeAndRegisterAbility(&SingletonContainer::Get<DisplayManagerService>());

#define CHECK_SCREEN_AND_RETURN(ret) \
    do { \
        if (screenId == SCREEN_ID_INVALID) { \
            WLOGFE("screenId invalid"); \
            return ret; \
        } \
    } while (false)

DisplayManagerService::DisplayManagerService() : SystemAbility(DISPLAY_MANAGER_SERVICE_SA_ID, true),
    abstractDisplayController_(new AbstractDisplayController(mutex_,
        std::bind(&DisplayManagerService::NotifyDisplayStateChange, this,
            std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4))),
    abstractScreenController_(new AbstractScreenController(mutex_)),
    displayPowerController_(new DisplayPowerController(mutex_,
        std::bind(&DisplayManagerService::NotifyDisplayStateChange, this,
            std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4))),
    displayCutoutController_(new DisplayCutoutController()),
    isAutoRotationOpen_(OHOS::system::GetParameter(
        "persist.display.ar.enabled", "1") == "1") // autoRotation default enabled
{
}

int DisplayManagerService::Dump(int fd, const std::vector<std::u16string>& args)
{
    if (displayDumper_ == nullptr) {
        displayDumper_ = new DisplayDumper(abstractDisplayController_, abstractScreenController_, mutex_);
    }
    return static_cast<int>(displayDumper_->Dump(fd, args));
}

void DisplayManagerService::OnStart()
{
    WLOGFI("start");
    if (!Init()) {
        WLOGFE("Init failed");
        return;
    }

    if (!Publish(this)) {
        WLOGFE("Publish failed");
    }
    WLOGFI("end");
}

bool DisplayManagerService::Init()
{
    WLOGFI("DisplayManagerService::Init start");
    if (DisplayManagerConfig::LoadConfigXml()) {
        DisplayManagerConfig::DumpConfig();
        ConfigureDisplayManagerService();
    }
    abstractScreenController_->Init();
    abstractDisplayController_->Init(abstractScreenController_);
    WLOGFI("DisplayManagerService::Init success");
    return true;
}

void DisplayManagerService::ConfigureDisplayManagerService()
{
    auto numbersConfig = DisplayManagerConfig::GetIntNumbersConfig();
    auto enableConfig = DisplayManagerConfig::GetEnableConfig();
    auto stringConfig = DisplayManagerConfig::GetStringConfig();
    if (numbersConfig.count("defaultDeviceRotationOffset") != 0) {
        uint32_t defaultDeviceRotationOffset = static_cast<uint32_t>(numbersConfig["defaultDeviceRotationOffset"][0]);
        ScreenRotationController::SetDefaultDeviceRotationOffset(defaultDeviceRotationOffset);
    }
    if (enableConfig.count("isWaterfallDisplay") != 0) {
        displayCutoutController_->SetIsWaterfallDisplay(
            static_cast<bool>(enableConfig["isWaterfallDisplay"]));
    }
    if (numbersConfig.count("curvedScreenBoundary") != 0) {
        displayCutoutController_->SetCurvedScreenBoundary(
            static_cast<std::vector<int>>(numbersConfig["curvedScreenBoundary"]));
    }
    if (stringConfig.count("defaultDisplayCutoutPath") != 0) {
        displayCutoutController_->SetBuiltInDisplayCutoutSvgPath(
            static_cast<std::string>(stringConfig["defaultDisplayCutoutPath"]));
    }
    ConfigureWaterfallDisplayCompressionParams();
    if (numbersConfig.count("buildInDefaultOrientation") != 0) {
        Orientation orientation = static_cast<Orientation>(numbersConfig["buildInDefaultOrientation"][0]);
        abstractScreenController_->SetBuildInDefaultOrientation(orientation);
    }
}

void DisplayManagerService::ConfigureWaterfallDisplayCompressionParams()
{
    auto numbersConfig = DisplayManagerConfig::GetIntNumbersConfig();
    auto enableConfig = DisplayManagerConfig::GetEnableConfig();
    if (enableConfig.count("isWaterfallAreaCompressionEnableWhenHorizontal") != 0) {
        DisplayCutoutController::SetWaterfallAreaCompressionEnableWhenHorzontal(
            static_cast<bool>(enableConfig["isWaterfallAreaCompressionEnableWhenHorizontal"]));
    }
    if (numbersConfig.count("waterfallAreaCompressionSizeWhenHorzontal") != 0) {
        DisplayCutoutController::SetWaterfallAreaCompressionSizeWhenHorizontal(
            static_cast<uint32_t>(numbersConfig["waterfallAreaCompressionSizeWhenHorzontal"][0]));
    }
}

void DisplayManagerService::RegisterDisplayChangeListener(sptr<IDisplayChangeListener> listener)
{
    displayChangeListener_ = listener;
    WLOGFI("IDisplayChangeListener registered");
}

void DisplayManagerService::RegisterWindowInfoQueriedListener(const sptr<IWindowInfoQueriedListener>& listener)
{
    windowInfoQueriedListener_ = listener;
}

DMError DisplayManagerService::HasPrivateWindow(DisplayId displayId, bool& hasPrivateWindow)
{
    if (!Permission::IsSystemCalling()) {
        WLOGFE("check has private window permission denied!");
        return DMError::DM_ERROR_INVALID_PERMISSION;
    }
    std::vector<DisplayId> displayIds = GetAllDisplayIds();
    auto iter = std::find(displayIds.begin(), displayIds.end(), displayId);
    if (iter == displayIds.end()) {
        WLOGFE("invalid displayId");
        return DMError::DM_ERROR_INVALID_PARAM;
    }
    if (windowInfoQueriedListener_ != nullptr) {
        windowInfoQueriedListener_->HasPrivateWindow(displayId, hasPrivateWindow);
        return DMError::DM_OK;
    }
    return DMError::DM_ERROR_NULLPTR;
}

void DisplayManagerService::NotifyDisplayStateChange(DisplayId defaultDisplayId, sptr<DisplayInfo> displayInfo,
    const std::map<DisplayId, sptr<DisplayInfo>>& displayInfoMap, DisplayStateChangeType type)
{
    DisplayId id = (displayInfo == nullptr) ? DISPLAY_ID_INVALID : displayInfo->GetDisplayId();
    WLOGFI("DisplayId %{public}" PRIu64"", id);
    if (displayChangeListener_ != nullptr) {
        displayChangeListener_->OnDisplayStateChange(defaultDisplayId, displayInfo, displayInfoMap, type);
    }
}

void DisplayManagerService::NotifyScreenshot(DisplayId displayId)
{
    if (displayChangeListener_ != nullptr) {
        displayChangeListener_->OnScreenshot(displayId);
    }
}

sptr<DisplayInfo> DisplayManagerService::GetDefaultDisplayInfo()
{
    ScreenId dmsScreenId = abstractScreenController_->GetDefaultAbstractScreenId();
    WLOGFD("GetDefaultDisplayInfo %{public}" PRIu64"", dmsScreenId);
    sptr<AbstractDisplay> display = abstractDisplayController_->GetAbstractDisplayByScreen(dmsScreenId);
    if (display == nullptr) {
        WLOGFE("fail to get displayInfo by id: invalid display");
        return nullptr;
    }
    return display->ConvertToDisplayInfo();
}

sptr<DisplayInfo> DisplayManagerService::GetDisplayInfoById(DisplayId displayId)
{
    sptr<AbstractDisplay> display = abstractDisplayController_->GetAbstractDisplay(displayId);
    if (display == nullptr) {
        WLOGFE("fail to get displayInfo by id: invalid display");
        return nullptr;
    }
    return display->ConvertToDisplayInfo();
}

sptr<DisplayInfo> DisplayManagerService::GetDisplayInfoByScreen(ScreenId screenId)
{
    sptr<AbstractDisplay> display = abstractDisplayController_->GetAbstractDisplayByScreen(screenId);
    if (display == nullptr) {
        WLOGFE("fail to get displayInfo by screenId: invalid display");
        return nullptr;
    }
    return display->ConvertToDisplayInfo();
}

ScreenId DisplayManagerService::CreateVirtualScreen(VirtualScreenOption option,
    const sptr<IRemoteObject>& displayManagerAgent)
{
    if (displayManagerAgent == nullptr) {
        WLOGFE("displayManagerAgent invalid");
        return SCREEN_ID_INVALID;
    }
    HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "dms:CreateVirtualScreen(%s)", option.name_.c_str());
    if (option.surface_ != nullptr && !Permission::CheckCallingPermission(SCREEN_CAPTURE_PERMISSION) &&
        !Permission::IsStartByHdcd()) {
        WLOGFE("permission denied");
        return SCREEN_ID_INVALID;
    }
    ScreenId screenId = abstractScreenController_->CreateVirtualScreen(option, displayManagerAgent);
    CHECK_SCREEN_AND_RETURN(SCREEN_ID_INVALID);
    accessTokenIdMaps_.insert(std::pair(screenId, IPCSkeleton::GetCallingTokenID()));
    return screenId;
}

DMError DisplayManagerService::DestroyVirtualScreen(ScreenId screenId)
{
    if (!Permission::IsSystemCalling()) {
        WLOGFE("destory virtual screen permission denied!");
        return DMError::DM_ERROR_INVALID_PERMISSION;
    }
    if (!accessTokenIdMaps_.isExistAndRemove(screenId, IPCSkeleton::GetCallingTokenID())) {
        return DMError::DM_ERROR_INVALID_CALLING;
    }

    WLOGFI("DestroyVirtualScreen::ScreenId: %{public}" PRIu64 "", screenId);
    CHECK_SCREEN_AND_RETURN(DMError::DM_ERROR_INVALID_PARAM);

    HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "dms:DestroyVirtualScreen(%" PRIu64")", screenId);
    return abstractScreenController_->DestroyVirtualScreen(screenId);
}

DMError DisplayManagerService::SetVirtualScreenSurface(ScreenId screenId, sptr<Surface> surface)
{
    WLOGFI("SetVirtualScreenSurface::ScreenId: %{public}" PRIu64 "", screenId);
    CHECK_SCREEN_AND_RETURN(DMError::DM_ERROR_INVALID_PARAM);
    if (Permission::CheckCallingPermission(SCREEN_CAPTURE_PERMISSION) ||
        Permission::IsStartByHdcd()) {
        return abstractScreenController_->SetVirtualScreenSurface(screenId, surface);
    }
    WLOGFE("permission denied");
    return DMError::DM_ERROR_INVALID_CALLING;
}

bool DisplayManagerService::SetOrientation(ScreenId screenId, Orientation orientation)
{
    if (!Permission::IsSystemCalling()) {
        WLOGFE("set orientation permission denied!");
        return false;
    }
    if (orientation < Orientation::UNSPECIFIED || orientation > Orientation::REVERSE_HORIZONTAL) {
        WLOGFE("SetOrientation::orientation: %{public}u", static_cast<uint32_t>(orientation));
        return false;
    }
    HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "dms:SetOrientation(%" PRIu64")", screenId);
    return abstractScreenController_->SetOrientation(screenId, orientation, false);
}

bool DisplayManagerService::SetOrientationFromWindow(ScreenId screenId, Orientation orientation)
{
    HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "dms:SetOrientationFromWindow(%" PRIu64")", screenId);
    return abstractScreenController_->SetOrientation(screenId, orientation, true);
}

bool DisplayManagerService::SetRotationFromWindow(ScreenId screenId, Rotation targetRotation)
{
    HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "dms:SetRotationFromWindow(%" PRIu64")", screenId);
    return abstractScreenController_->SetRotation(screenId, targetRotation, true);
}

std::shared_ptr<Media::PixelMap> DisplayManagerService::GetDisplaySnapshot(DisplayId displayId)
{
    HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "dms:GetDisplaySnapshot(%" PRIu64")", displayId);
    if (Permission::CheckCallingPermission(SCREEN_CAPTURE_PERMISSION) ||
        Permission::IsStartByHdcd()) {
        auto res = abstractDisplayController_->GetScreenSnapshot(displayId);
        if (res != nullptr) {
            NotifyScreenshot(displayId);
        }
        return res;
    }
    return nullptr;
}

uint32_t DisplayManagerService::GetRSScreenNum() const
{
    return abstractScreenController_->GetRSScreenNum();
}

DMError DisplayManagerService::GetScreenSupportedColorGamuts(ScreenId screenId,
    std::vector<ScreenColorGamut>& colorGamuts)
{
    WLOGFI("GetScreenSupportedColorGamuts::ScreenId: %{public}" PRIu64 "", screenId);
    CHECK_SCREEN_AND_RETURN(DMError::DM_ERROR_INVALID_PARAM);
    return abstractScreenController_->GetScreenSupportedColorGamuts(screenId, colorGamuts);
}

DMError DisplayManagerService::GetScreenColorGamut(ScreenId screenId, ScreenColorGamut& colorGamut)
{
    WLOGFI("GetScreenColorGamut::ScreenId: %{public}" PRIu64 "", screenId);
    CHECK_SCREEN_AND_RETURN(DMError::DM_ERROR_INVALID_PARAM);
    return abstractScreenController_->GetScreenColorGamut(screenId, colorGamut);
}

DMError DisplayManagerService::SetScreenColorGamut(ScreenId screenId, int32_t colorGamutIdx)
{
    WLOGFI("SetScreenColorGamut::ScreenId: %{public}" PRIu64 ", colorGamutIdx %{public}d", screenId, colorGamutIdx);
    CHECK_SCREEN_AND_RETURN(DMError::DM_ERROR_INVALID_PARAM);
    return abstractScreenController_->SetScreenColorGamut(screenId, colorGamutIdx);
}

DMError DisplayManagerService::GetScreenGamutMap(ScreenId screenId, ScreenGamutMap& gamutMap)
{
    WLOGFI("GetScreenGamutMap::ScreenId: %{public}" PRIu64 "", screenId);
    CHECK_SCREEN_AND_RETURN(DMError::DM_ERROR_INVALID_PARAM);
    return abstractScreenController_->GetScreenGamutMap(screenId, gamutMap);
}

DMError DisplayManagerService::SetScreenGamutMap(ScreenId screenId, ScreenGamutMap gamutMap)
{
    WLOGFI("SetScreenGamutMap::ScreenId: %{public}" PRIu64 ", ScreenGamutMap %{public}u",
        screenId, static_cast<uint32_t>(gamutMap));
    CHECK_SCREEN_AND_RETURN(DMError::DM_ERROR_INVALID_PARAM);
    return abstractScreenController_->SetScreenGamutMap(screenId, gamutMap);
}

DMError DisplayManagerService::SetScreenColorTransform(ScreenId screenId)
{
    WLOGFI("SetScreenColorTransform::ScreenId: %{public}" PRIu64 "", screenId);
    CHECK_SCREEN_AND_RETURN(DMError::DM_ERROR_INVALID_PARAM);
    return abstractScreenController_->SetScreenColorTransform(screenId);
}

void DisplayManagerService::OnStop()
{
    WLOGFI("ready to stop display service.");
}

bool DisplayManagerService::RegisterDisplayManagerAgent(const sptr<IDisplayManagerAgent>& displayManagerAgent,
    DisplayManagerAgentType type)
{
    if (type == DisplayManagerAgentType::SCREEN_EVENT_LISTENER && !Permission::IsSystemCalling()) {
        WLOGFE("register display manager agent permission denied!");
        return false;
    }
    if ((displayManagerAgent == nullptr) || (displayManagerAgent->AsObject() == nullptr)) {
        WLOGFE("displayManagerAgent invalid");
        return false;
    }
    return DisplayManagerAgentController::GetInstance().RegisterDisplayManagerAgent(displayManagerAgent, type);
}

bool DisplayManagerService::UnregisterDisplayManagerAgent(const sptr<IDisplayManagerAgent>& displayManagerAgent,
    DisplayManagerAgentType type)
{
    if (type == DisplayManagerAgentType::SCREEN_EVENT_LISTENER && !Permission::IsSystemCalling()) {
        WLOGFE("unregister display manager agent permission denied!");
        return false;
    }
    if ((displayManagerAgent == nullptr) || (displayManagerAgent->AsObject() == nullptr)) {
        WLOGFE("displayManagerAgent invalid");
        return false;
    }
    return DisplayManagerAgentController::GetInstance().UnregisterDisplayManagerAgent(displayManagerAgent, type);
}

bool DisplayManagerService::WakeUpBegin(PowerStateChangeReason reason)
{
    HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "dms:WakeUpBegin(%u)", reason);
    if (!Permission::IsSystemServiceCalling()) {
        WLOGFE("wake up begin permission denied!");
        return false;
    }
    return DisplayManagerAgentController::GetInstance().NotifyDisplayPowerEvent(DisplayPowerEvent::WAKE_UP,
        EventStatus::BEGIN);
}

bool DisplayManagerService::WakeUpEnd()
{
    if (!Permission::IsSystemServiceCalling()) {
        WLOGFE("wake up end permission denied!");
        return false;
    }
    return DisplayManagerAgentController::GetInstance().NotifyDisplayPowerEvent(DisplayPowerEvent::WAKE_UP,
        EventStatus::END);
}

bool DisplayManagerService::SuspendBegin(PowerStateChangeReason reason)
{
    HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "dms:SuspendBegin(%u)", reason);
    if (!Permission::IsSystemServiceCalling()) {
        WLOGFE("suspend begin permission denied!");
        return false;
    }
    displayPowerController_->SuspendBegin(reason);
    return DisplayManagerAgentController::GetInstance().NotifyDisplayPowerEvent(DisplayPowerEvent::SLEEP,
        EventStatus::BEGIN);
}

bool DisplayManagerService::SuspendEnd()
{
    if (!Permission::IsSystemServiceCalling()) {
        WLOGFE("suspend end permission denied!");
        return false;
    }
    return DisplayManagerAgentController::GetInstance().NotifyDisplayPowerEvent(DisplayPowerEvent::SLEEP,
        EventStatus::END);
}

bool DisplayManagerService::SetScreenPowerForAll(ScreenPowerState state, PowerStateChangeReason reason)
{
    WLOGFI("SetScreenPowerForAll");
    if (!Permission::IsSystemServiceCalling()) {
        WLOGFE("set screen power for all permission denied!");
        return false;
    }
    return abstractScreenController_->SetScreenPowerForAll(state, reason);
}

ScreenPowerState DisplayManagerService::GetScreenPower(ScreenId dmsScreenId)
{
    return abstractScreenController_->GetScreenPower(dmsScreenId);
}

bool DisplayManagerService::SetDisplayState(DisplayState state)
{
    if (!Permission::IsSystemServiceCalling()) {
        WLOGFE("set display state permission denied!");
        return false;
    }
    ScreenId dmsScreenId = abstractScreenController_->GetDefaultAbstractScreenId();
    sptr<AbstractDisplay> display = abstractDisplayController_->GetAbstractDisplayByScreen(dmsScreenId);
    if (display != nullptr) {
        display->SetDisplayState(state);
    }
    return displayPowerController_->SetDisplayState(state);
}

ScreenId DisplayManagerService::GetScreenIdByDisplayId(DisplayId displayId) const
{
    sptr<AbstractDisplay> abstractDisplay = abstractDisplayController_->GetAbstractDisplay(displayId);
    if (abstractDisplay == nullptr) {
        WLOGFE("GetScreenIdByDisplayId: GetAbstractDisplay failed");
        return SCREEN_ID_INVALID;
    }
    return abstractDisplay->GetAbstractScreenId();
}

DisplayState DisplayManagerService::GetDisplayState(DisplayId displayId)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    return displayPowerController_->GetDisplayState(displayId);
}

void DisplayManagerService::NotifyDisplayEvent(DisplayEvent event)
{
    if (!Permission::IsSystemServiceCalling()) {
        WLOGFE("notify display event permission denied!");
        return;
    }
    displayPowerController_->NotifyDisplayEvent(event);
}

bool DisplayManagerService::SetFreeze(std::vector<DisplayId> displayIds, bool isFreeze)
{
    if (!Permission::IsSystemCalling()) {
        WLOGFE("set freeze permission denied!");
        return false;
    }
    abstractDisplayController_->SetFreeze(displayIds, isFreeze);
    return true;
}

ScreenId DisplayManagerService::MakeMirror(ScreenId mainScreenId, std::vector<ScreenId> mirrorScreenIds)
{
    if (!Permission::IsSystemCalling()) {
        WLOGFE("make mirror permission denied!");
        return SCREEN_ID_INVALID;
    }
    WLOGFI("MakeMirror. mainScreenId :%{public}" PRIu64"", mainScreenId);
    auto allMirrorScreenIds = abstractScreenController_->GetAllValidScreenIds(mirrorScreenIds);
    auto iter = std::find(allMirrorScreenIds.begin(), allMirrorScreenIds.end(), mainScreenId);
    if (iter != allMirrorScreenIds.end()) {
        allMirrorScreenIds.erase(iter);
    }
    auto mainScreen = abstractScreenController_->GetAbstractScreen(mainScreenId);
    if (mainScreen == nullptr || allMirrorScreenIds.empty()) {
        WLOGFI("create mirror fail. main screen :%{public}" PRIu64", screens' size:%{public}u",
            mainScreenId, static_cast<uint32_t>(allMirrorScreenIds.size()));
        return SCREEN_ID_INVALID;
    }
    HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "dms:MakeMirror");
    if (!abstractScreenController_->MakeMirror(mainScreenId, allMirrorScreenIds)) {
        WLOGFE("make mirror failed.");
        return SCREEN_ID_INVALID;
    }
    if (abstractScreenController_->GetAbstractScreenGroup(mainScreen->groupDmsId_) == nullptr) {
        WLOGFE("get screen group failed.");
        return SCREEN_ID_INVALID;
    }
    return mainScreen->groupDmsId_;
}

void DisplayManagerService::RemoveVirtualScreenFromGroup(std::vector<ScreenId> screens)
{
    abstractScreenController_->RemoveVirtualScreenFromGroup(screens);
}

void DisplayManagerService::UpdateRSTree(DisplayId displayId, DisplayId parentDisplayId,
    std::shared_ptr<RSSurfaceNode>& surfaceNode, bool isAdd, bool isMultiDisplay)
{
    WLOGFI("UpdateRSTree, currentDisplayId: %{public}" PRIu64", isAdd: %{public}d, isMultiDisplay: %{public}d, "
        "parentDisplayId: %{public}" PRIu64"", displayId, isAdd, isMultiDisplay, parentDisplayId);
    ScreenId screenId = GetScreenIdByDisplayId(displayId);
    ScreenId parentScreenId = GetScreenIdByDisplayId(parentDisplayId);
    CHECK_SCREEN_AND_RETURN();

    abstractScreenController_->UpdateRSTree(screenId, parentScreenId, surfaceNode, isAdd, isMultiDisplay);
}

sptr<ScreenInfo> DisplayManagerService::GetScreenInfoById(ScreenId screenId)
{
    auto screen = abstractScreenController_->GetAbstractScreen(screenId);
    if (screen == nullptr) {
        WLOGE("cannot find screenInfo: %{public}" PRIu64"", screenId);
        return nullptr;
    }
    return screen->ConvertToScreenInfo();
}

sptr<ScreenGroupInfo> DisplayManagerService::GetScreenGroupInfoById(ScreenId screenId)
{
    auto screenGroup = abstractScreenController_->GetAbstractScreenGroup(screenId);
    if (screenGroup == nullptr) {
        WLOGE("cannot find screenGroupInfo: %{public}" PRIu64"", screenId);
        return nullptr;
    }
    return screenGroup->ConvertToScreenGroupInfo();
}

ScreenId DisplayManagerService::GetScreenGroupIdByScreenId(ScreenId screenId)
{
    auto screen = abstractScreenController_->GetAbstractScreen(screenId);
    if (screen == nullptr) {
        WLOGE("cannot find screenInfo: %{public}" PRIu64"", screenId);
        return SCREEN_ID_INVALID;
    }
    return screen->GetScreenGroupId();
}

std::vector<DisplayId> DisplayManagerService::GetAllDisplayIds()
{
    return abstractDisplayController_->GetAllDisplayIds();
}

std::vector<sptr<ScreenInfo>> DisplayManagerService::GetAllScreenInfos()
{
    if (!Permission::IsSystemCalling()) {
        WLOGFE("get all screen infos permission denied!");
        return std::vector<sptr<ScreenInfo>>();
    }
    std::vector<ScreenId> screenIds = abstractScreenController_->GetAllScreenIds();
    std::vector<sptr<ScreenInfo>> screenInfos;
    for (auto screenId: screenIds) {
        auto screenInfo = GetScreenInfoById(screenId);
        if (screenInfo == nullptr) {
            WLOGE("cannot find screenInfo: %{public}" PRIu64"", screenId);
            continue;
        }
        screenInfos.emplace_back(screenInfo);
    }
    return screenInfos;
}

ScreenId DisplayManagerService::MakeExpand(std::vector<ScreenId> expandScreenIds, std::vector<Point> startPoints)
{
    if (!Permission::IsSystemCalling()) {
        WLOGFE("make expand permission denied!");
        return SCREEN_ID_INVALID;
    }
    if (expandScreenIds.empty() || startPoints.empty() || expandScreenIds.size() != startPoints.size()) {
        WLOGFI("create expand fail, input params is invalid. "
            "screenId vector size :%{public}ud, startPoint vector size :%{public}ud",
            static_cast<uint32_t>(expandScreenIds.size()), static_cast<uint32_t>(startPoints.size()));
        return SCREEN_ID_INVALID;
    }
    std::map<ScreenId, Point> pointsMap;
    uint32_t size = expandScreenIds.size();
    for (uint32_t i = 0; i < size; i++) {
        if (pointsMap.find(expandScreenIds[i]) != pointsMap.end()) {
            continue;
        }
        pointsMap[expandScreenIds[i]] = startPoints[i];
    }
    ScreenId defaultScreenId = abstractScreenController_->GetDefaultAbstractScreenId();
    WLOGFI("MakeExpand, defaultScreenId:%{public}" PRIu64"", defaultScreenId);
    auto allExpandScreenIds = abstractScreenController_->GetAllValidScreenIds(expandScreenIds);
    auto iter = std::find(allExpandScreenIds.begin(), allExpandScreenIds.end(), defaultScreenId);
    if (iter != allExpandScreenIds.end()) {
        allExpandScreenIds.erase(iter);
    }
    if (allExpandScreenIds.empty()) {
        WLOGFE("allExpandScreenIds is empty. make expand failed.");
        return SCREEN_ID_INVALID;
    }
    std::shared_ptr<RSDisplayNode> rsDisplayNode;
    std::vector<Point> points;
    for (uint32_t i = 0; i < allExpandScreenIds.size(); i++) {
        rsDisplayNode = abstractScreenController_->GetRSDisplayNodeByScreenId(allExpandScreenIds[i]);
        points.emplace_back(pointsMap[allExpandScreenIds[i]]);
        if (rsDisplayNode != nullptr) {
            rsDisplayNode->SetDisplayOffset(pointsMap[allExpandScreenIds[i]].posX_,
                pointsMap[allExpandScreenIds[i]].posY_);
        }
    }
    HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "dms:MakeExpand");
    if (!abstractScreenController_->MakeExpand(allExpandScreenIds, points)) {
        WLOGFE("make expand failed.");
        return SCREEN_ID_INVALID;
    }
    auto screen = abstractScreenController_->GetAbstractScreen(allExpandScreenIds[0]);
    if (screen == nullptr || abstractScreenController_->GetAbstractScreenGroup(screen->groupDmsId_) == nullptr) {
        WLOGFE("get screen group failed.");
        return SCREEN_ID_INVALID;
    }
    return screen->groupDmsId_;
}

bool DisplayManagerService::SetScreenActiveMode(ScreenId screenId, uint32_t modeId)
{
    if (!Permission::IsSystemCalling()) {
        WLOGFE("set screen active permission denied!");
        return false;
    }
    HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "dms:SetScreenActiveMode(%" PRIu64", %u)", screenId, modeId);
    return abstractScreenController_->SetScreenActiveMode(screenId, modeId);
}

bool DisplayManagerService::SetVirtualPixelRatio(ScreenId screenId, float virtualPixelRatio)
{
    if (!Permission::IsSystemCalling()) {
        WLOGFE("set virtual pixel permission denied!");
        return false;
    }
    HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "dms:SetVirtualPixelRatio(%" PRIu64", %f)", screenId,
        virtualPixelRatio);
    return abstractScreenController_->SetVirtualPixelRatio(screenId, virtualPixelRatio);
}

bool DisplayManagerService::IsScreenRotationLocked()
{
    if (!Permission::IsSystemCalling()) {
        WLOGFE("is screen rotation locked permission denied!");
        return false;
    }
    return ScreenRotationController::IsScreenRotationLocked();
}

void DisplayManagerService::SetScreenRotationLocked(bool isLocked)
{
    if (!Permission::IsSystemCalling()) {
        WLOGFE("set screen rotation locked permission denied!");
        return;
    }
    ScreenRotationController::SetScreenRotationLocked(isLocked);
}

void DisplayManagerService::SetGravitySensorSubscriptionEnabled()
{
    if (!isAutoRotationOpen_) {
        WLOGFE("autoRotation is not open");
        ScreenRotationController::Init();
        return;
    }
    SensorConnector::SubscribeRotationSensor();
}

sptr<CutoutInfo> DisplayManagerService::GetCutoutInfo(DisplayId displayId)
{
    return displayCutoutController_->GetCutoutInfo(displayId);
}

void DisplayManagerService::RegisterRSScreenChangeListener(const sptr<IRSScreenChangeListener>& listener)
{
    abstractScreenController_->RegisterRSScreenChangeListener(listener);
}
} // namespace OHOS::Rosen
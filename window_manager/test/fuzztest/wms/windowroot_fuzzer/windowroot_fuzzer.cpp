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

#include <refbase.h>
#include <parcel.h>
#include <securec.h>

#include "windowroot_fuzzer.h"

#include "display_info.h"
#include "display_manager.h"
#include "marshalling_helper.h"
#include "window.h"
#include "window_node.h"
#include "window_root.h"
#include "window_manager.h"
#include "window_manager_service.h"
#include "window_scene.h"
#include "window_dumper.h"

using namespace OHOS::Rosen;

namespace OHOS {
namespace {
    constexpr size_t DATA_MIN_SIZE = 2;
    constexpr size_t VECTOR_MAX_LEN = 4;
}

template<class T>
size_t GetObject(T &object, const uint8_t *data, size_t size)
{
    size_t objectSize = sizeof(object);
    if (objectSize > size) {
        return 0;
    }
    return memcpy_s(&object, objectSize, data, objectSize) == EOK ? objectSize : 0;
}

sptr<WindowNode> CreateWindowNode(const std::string& windowName,
    const uint8_t *data, size_t size)
{
    sptr<OHOS::Rosen::WindowProperty> property = nullptr;
    Parcel parcel;
    if (parcel.WriteBuffer(data, size)) {
        property = OHOS::Rosen::WindowProperty::Unmarshalling(parcel);
    }

    sptr<OHOS::Rosen::WindowNode> windowNode = new OHOS::Rosen::WindowNode(property);
    if (windowNode == nullptr) {
        return nullptr;
    }
    return windowNode;
}

sptr<OHOS::Rosen::DisplayInfo> CreateDisplayInfo(const uint8_t *data, size_t size)
{
    sptr<OHOS::Rosen::DisplayInfo> displayInfo = nullptr;
    Parcel parcel;
    if (parcel.WriteBuffer(data, size)) {
        displayInfo = OHOS::Rosen::DisplayInfo::Unmarshalling(parcel);
    }
    return displayInfo;
}

void WindowRootFuzzPart1(sptr<WindowRoot> windowRoot, sptr<WindowNode> windowNode,
    const uint8_t *data, size_t size)
{
    if (windowRoot == nullptr || windowNode == nullptr || data == nullptr || size < DATA_MIN_SIZE) {
        return;
    }
    size_t startPos = 0;
    OHOS::Rosen::DisplayId displayId;
    startPos += GetObject<OHOS::Rosen::DisplayId>(displayId, data + startPos, size - startPos);
    windowRoot->GetOrCreateWindowNodeContainer(displayId);
    windowRoot->GetWindowNodeContainer(displayId);

    auto displayInfo = CreateDisplayInfo(data, size);
    windowRoot->CreateWindowNodeContainer(displayInfo);

    uint32_t windowId;
    startPos += GetObject<uint32_t>(windowId, data + startPos, size - startPos);
    windowRoot->GetWindowNode(windowId);

    std::vector<sptr<OHOS::Rosen::WindowNode>> windowNodes;
    OHOS::Rosen::ScreenId screenGroupId;
    startPos += GetObject<OHOS::Rosen::ScreenId>(screenGroupId, data + startPos, size - startPos);
    windowRoot->GetBackgroundNodesByScreenId(screenGroupId, windowNodes);

    windowRoot->FindWindowNodeWithToken(windowNode->abilityToken_);

    bool fromStartingWin;
    startPos += GetObject<bool>(fromStartingWin, data + startPos, size - startPos);
    windowRoot->AddWindowNode(windowNode->GetParentId(), windowNode, fromStartingWin);

    OHOS::Rosen::WindowUpdateReason reason;
    startPos += GetObject<OHOS::Rosen::WindowUpdateReason>(reason, data + startPos, size - startPos);
    windowRoot->UpdateWindowNode(windowNode->GetWindowId(), reason);

    uint64_t surfaceNodeId;
    GetObject<uint64_t>(surfaceNodeId, data + startPos, size - startPos);
    windowRoot->AddSurfaceNodeIdWindowNodePair(surfaceNodeId, windowNode);

    windowRoot->IsVerticalDisplay(windowNode);

    windowRoot->RemoveWindowNode(windowNode->GetWindowId(), fromStartingWin);
}

void WindowRootFuzzPart2(sptr<WindowRoot> windowRoot, sptr<WindowNode> windowNode,
    const uint8_t *data, size_t size)
{
    if (windowRoot == nullptr || windowNode == nullptr || data == nullptr || size < DATA_MIN_SIZE) {
        return;
    }
    size_t startPos = 0;
    DisplayId displayId;
    startPos += GetObject<DisplayId>(displayId, data + startPos, size - startPos);

    windowRoot->IsForbidDockSliceMove(displayId);
    windowRoot->IsDockSliceInExitSplitModeArea(displayId);
    windowRoot->ExitSplitMode(displayId);

    windowRoot->RequestFocus(windowNode->GetWindowId());
    windowRoot->RequestActiveWindow(windowNode->GetWindowId());

    windowRoot->MinimizeStructuredAppWindowsExceptSelf(windowNode);

    AvoidAreaType avoidAreaType;
    startPos += GetObject<AvoidAreaType>(avoidAreaType, data + startPos, size - startPos);
    windowRoot->GetAvoidAreaByType(windowNode->GetWindowId(), avoidAreaType);

    WindowMode dstMode;
    startPos += GetObject<WindowMode>(dstMode, data + startPos, size - startPos);
    windowRoot->SetWindowMode(windowNode, dstMode);

    uint32_t mainWinId;
    uint32_t topWinId;
    startPos += GetObject<uint32_t>(mainWinId, data + startPos, size - startPos);
    startPos += GetObject<uint32_t>(topWinId, data + startPos, size - startPos);
    windowRoot->GetTopWindowId(mainWinId, topWinId);

    startPos += GetObject<DisplayId>(displayId, data + startPos, size - startPos);
    windowRoot->MinimizeAllAppWindows(displayId);

    windowRoot->ToggleShownStateForAllAppWindows();

    WindowLayoutMode windowLayoutMode;
    startPos += GetObject<WindowLayoutMode>(windowLayoutMode, data + startPos, size - startPos);
    windowRoot->SetWindowLayoutMode(displayId, windowLayoutMode);

    WindowState windowState;
    WindowStateChangeReason windowStateChangeReason;
    startPos += GetObject<WindowState>(windowState, data + startPos, size - startPos);
    GetObject<WindowStateChangeReason>(windowStateChangeReason, data + startPos, size - startPos);
    windowRoot->ProcessWindowStateChange(windowState, windowStateChangeReason);
}

void WindowRootFuzzPart3(sptr<WindowRoot> windowRoot, sptr<WindowNode> windowNode,
    const uint8_t *data, size_t size)
{
    if (windowRoot == nullptr || windowNode == nullptr || data == nullptr || size < DATA_MIN_SIZE) {
        return;
    }
    size_t startPos = 0;
    windowRoot->RaiseZOrderForAppWindow(windowNode);

    DisplayId displayId;
    startPos += GetObject<DisplayId>(displayId, data + startPos, size - startPos);
    windowRoot->GetVirtualPixelRatio(displayId);
    windowRoot->GetDisplayGroupRect(displayId);

    WindowSizeChangeReason windowSizeChangeReason;
    startPos += GetObject<WindowSizeChangeReason>(windowSizeChangeReason, data + startPos, size - startPos);
    windowRoot->UpdateSizeChangeReason(windowNode->GetWindowId(), windowSizeChangeReason);

    float brightNess;
    startPos += GetObject<float>(brightNess, data + startPos, size - startPos);
    windowRoot->SetBrightness(windowNode->GetWindowId(), brightNess);

    windowRoot->UpdateFocusableProperty(windowNode->GetWindowId());
    bool requireLock;
    startPos += GetObject<bool>(requireLock, data + startPos, size - startPos);

    windowRoot->HandleKeepScreenOn(windowNode->GetWindowId(), requireLock);

    uint32_t windowNum;
    startPos += GetObject<uint32_t>(windowNum, data + startPos, size - startPos);
    windowRoot->SetMaxAppWindowNumber(windowNum);

    uint32_t uniAppWindowNum;
    startPos += GetObject<uint32_t>(uniAppWindowNum, data + startPos, size - startPos);
    windowRoot->SetMaxUniRenderAppWindowNumber(uniAppWindowNum);

    ModeChangeHotZones hotZones;
    ModeChangeHotZonesConfig config;
    startPos += GetObject<ModeChangeHotZones>(hotZones, data + startPos, size - startPos);
    startPos += GetObject<ModeChangeHotZonesConfig>(config, data + startPos, size - startPos);
    windowRoot->GetModeChangeHotZones(displayId, hotZones, config);

    std::vector<float> splitRatioNumbers;
    for (size_t i = 0; i < VECTOR_MAX_LEN; i++) {
        float value;
        startPos += GetObject<float>(value, data + startPos, size - startPos);
        splitRatioNumbers.emplace_back(value);
    }
    windowRoot->SetSplitRatios(splitRatioNumbers);
    windowRoot->SetExitSplitRatios(splitRatioNumbers);
}

void WindowRootFuzzPart4(sptr<WindowRoot> windowRoot, sptr<WindowNode> windowNode,
    const uint8_t *data, size_t size)
{
    if (windowRoot == nullptr || windowNode == nullptr || data == nullptr || size < DATA_MIN_SIZE) {
        return;
    }
    size_t startPos = 0;

    DisplayId displayId;
    startPos += GetObject<DisplayId>(displayId, data + startPos, size - startPos);
    windowRoot->HasPrivateWindow(displayId);
    windowRoot->GetDisplayRectWithoutSystemBarAreas(displayId);
    windowRoot->GetSplitScreenWindowNodes(displayId);
    windowRoot->GetAllDisplayIds();
    windowRoot->GetTotalWindowNum();

    bool isUniRender;
    startPos += GetObject<bool>(isUniRender, data + startPos, size - startPos);
    windowRoot->OnRenderModeChanged(isUniRender);

    windowRoot->TakeWindowPairSnapshot(displayId);
    windowRoot->ClearWindowPairSnapshot(displayId);

    std::vector<wptr<WindowNode>> windowNodes;
    windowNodes.emplace_back(windowNode);
    windowRoot->GetAllAnimationPlayingNodes(windowNodes);

    std::vector<sptr<WindowVisibilityInfo>> infos;
    for (size_t i = 0; i < VECTOR_MAX_LEN; i++) {
        sptr<WindowVisibilityInfo> visibilityInfo = nullptr;
        Parcel parcel;
        if (parcel.WriteBuffer(data, size)) {
            visibilityInfo = WindowVisibilityInfo::Unmarshalling(parcel);
        }
        if (visibilityInfo != nullptr) {
            infos.emplace_back(visibilityInfo);
        }
    }
    windowRoot->GetVisibilityWindowInfo(infos);
    infos.clear();

    int accountId;
    GetObject<int>(accountId, data + startPos, size - startPos);
    windowRoot->RemoveSingleUserWindowNodes(accountId);

    windowRoot->NotifySystemBarTints();

    windowRoot->FocusFaultDetection();
    windowRoot->GetMaxUniRenderAppWindowNumber();

    windowRoot->GetWindowForDumpAceHelpInfo();
    windowRoot->DestroyLeakStartingWindow();
}

void WindowRootFuzzPart5(sptr<WindowRoot> windowRoot, sptr<WindowNode> windowNode,
    const uint8_t *data, size_t size)
{
    if (windowRoot == nullptr || windowNode == nullptr || data == nullptr || size < DATA_MIN_SIZE) {
        return;
    }
    size_t startPos = 0;
    std::vector<uint32_t> windowIds;
    for (size_t i = 0; i < VECTOR_MAX_LEN; i++) {
        uint32_t value;
        startPos += GetObject<uint32_t>(value, data + startPos, size - startPos);
        windowIds.emplace_back(value);
    }
    windowRoot->MinimizeTargetWindows(windowIds);
    windowRoot->UpdateRsTree(windowNode->GetWindowId(), false);

    windowRoot->SwitchRenderModeIfNeeded();
    windowRoot->IsUniRender();

    bool afterAnimation;
    GetObject<bool>(afterAnimation, data + startPos, size - startPos);
    windowRoot->LayoutWhenAddWindowNode(windowNode, afterAnimation);
}

void WindowRootFuzzPart6(sptr<WindowRoot> windowRoot, sptr<WindowNode> windowNode,
    const uint8_t *data, size_t size)
{
    if (windowRoot == nullptr || windowNode == nullptr || data == nullptr || size < DATA_MIN_SIZE) {
        return;
    }
    size_t startPos = 0;

    windowRoot->GenAllWindowsLogInfo();
    DisplayId displayId;
    bool isRecordedDisplay;
    startPos += GetObject<DisplayId>(displayId, data + startPos, size - startPos);
    startPos += GetObject<bool>(isRecordedDisplay, data + startPos, size - startPos);

    windowRoot->GetScreenGroupId(displayId, isRecordedDisplay);
    
    std::vector<DisplayId> infos;
    for (size_t i = 0; i < VECTOR_MAX_LEN; i++) {
        DisplayId disId;
        startPos += GetObject<DisplayId>(disId, data + startPos, size - startPos);
        infos.emplace_back(disId);
    }
    windowRoot->GetAllDisplayInfos(infos);

    DisplayId defaultDisplayId;
    startPos += GetObject<DisplayId>(defaultDisplayId, data + startPos, size - startPos);
    windowRoot->MoveNotShowingWindowToDefaultDisplay(defaultDisplayId, displayId);

    bool isToUnified;
    GetObject<bool>(isToUnified, data + startPos, size - startPos);
    windowRoot->ChangeRSRenderModeIfNeeded(isToUnified);
    windowRoot->IsAppWindowExceed();
}

void DoFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return;
    }
    auto windowRoot = OHOS::Rosen::WindowManagerService::GetInstance().windowRoot_;
    auto windowNode = CreateWindowNode("TestWindowNode", data, size);
    if (windowNode == nullptr || windowRoot == nullptr) {
        return;
    }
    windowRoot->SaveWindow(windowNode);

    OHOS::WindowRootFuzzPart1(windowRoot, windowNode, data, size);
    OHOS::WindowRootFuzzPart2(windowRoot, windowNode, data, size);
    OHOS::WindowRootFuzzPart3(windowRoot, windowNode, data, size);
    OHOS::WindowRootFuzzPart4(windowRoot, windowNode, data, size);
    OHOS::WindowRootFuzzPart5(windowRoot, windowNode, data, size);
    OHOS::WindowRootFuzzPart6(windowRoot, windowNode, data, size);

    windowRoot->DestroyWindow(windowNode->GetWindowId(), false);
    windowRoot->surfaceIdWindowNodeMap_.clear();
}
} // namespace.OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::DoFuzzTest(data, size);
    return 0;
}


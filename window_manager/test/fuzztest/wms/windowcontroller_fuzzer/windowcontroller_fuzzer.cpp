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

#include "windowcontroller_fuzzer.h"

#include <securec.h>
#include <parcel.h>

#include "window.h"
#include "window_controller.h"
#include "window_manager_service.h"
#include "window_transition_info.h"

using namespace OHOS::Rosen;

namespace OHOS {
namespace {
    constexpr size_t DATA_MIN_SIZE = 2;
    constexpr char END_CHAR = '\0';
    constexpr size_t LEN = 10;
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

sptr<WindowTransitionInfo> MakeWindowTransitionInfo(const uint8_t* data, size_t size)
{
    sptr<WindowTransitionInfo> info = new (std::nothrow)WindowTransitionInfo;
    if (info == nullptr) {
        return nullptr;
    }
    size_t startPos = 0;
    char name1[LEN + 1];
    char name2[LEN + 1];
    name1[LEN] = END_CHAR;
    name2[LEN] = END_CHAR;
    for (size_t i = 0; i < LEN; i++) {
        startPos += GetObject<char>(name1[i], data + startPos, size - startPos);
        startPos += GetObject<char>(name2[i], data + startPos, size - startPos);
    }
    std::string bundleName(name1);
    std::string abilityName(name2);
    info->SetBundleName(bundleName);
    info->SetAbilityName(abilityName);

    OHOS::Rosen::WindowMode mode;
    startPos += GetObject<WindowMode>(mode, data + startPos, size - startPos);
    info->SetWindowMode(mode);

    OHOS::Rosen::Rect rect;
    startPos += GetObject<OHOS::Rosen::Rect>(rect, data + startPos, size - startPos);
    info->SetWindowRect(rect);

    OHOS::Rosen::DisplayId displayId;
    startPos += GetObject<OHOS::Rosen::DisplayId>(displayId, data + startPos, size - startPos);
    info->SetDisplayId(displayId);

    OHOS::Rosen::WindowType windowType;
    startPos += GetObject<WindowType>(windowType, data + startPos, size - startPos);
    info->SetWindowType(windowType);

    bool boolVal;
    startPos += GetObject<bool>(boolVal, data + startPos, size - startPos);
    info->SetShowFlagWhenLocked(boolVal);
    startPos += GetObject<bool>(boolVal, data + startPos, size - startPos);
    info->SetIsRecent(boolVal);

    OHOS::Rosen::TransitionReason reson;
    startPos += GetObject<OHOS::Rosen::TransitionReason>(reson, data + startPos, size - startPos);
    info->SetTransitionReason(reson);

    startPos += GetObject<OHOS::Rosen::WindowSizeLimits>(info->sizeLimits_, data + startPos, size - startPos);

    int32_t missionId;
    GetObject<int32_t>(missionId, data + startPos, size - startPos);
    info->SetMissionId(missionId);
    return info;
}

void WindowControllerFuzzTestPart1(sptr<WindowController> windowController,
    const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return;
    }
    size_t startPos = 0;
    auto fromInfo = MakeWindowTransitionInfo(data, size);
    auto toInfo = MakeWindowTransitionInfo(data, size);
    windowController->NotifyWindowTransition(fromInfo, toInfo);

    uint32_t windowId;
    startPos += GetObject<uint32_t>(windowId, data + startPos, size - startPos);
    windowController->RequestFocus(windowId);

    AvoidAreaType avoidAreaType;
    startPos += GetObject<AvoidAreaType>(avoidAreaType, data + startPos, size - startPos);
    windowController->GetAvoidAreaByType(windowId, avoidAreaType);

    uint32_t mainWindowId;
    startPos += GetObject<uint32_t>(mainWindowId, data + startPos, size - startPos);
    windowController->GetTopWindowId(mainWindowId, windowId);

    Parcel percel;
    if (percel.WriteBuffer(data, size)) {
        sptr<MoveDragProperty> moveDragProperty = nullptr;
        moveDragProperty = MoveDragProperty::Unmarshalling(percel);
        windowController->NotifyServerReadyToMoveOrDrag(mainWindowId, moveDragProperty);
    }

    startPos += GetObject<uint32_t>(mainWindowId, data + startPos, size - startPos);
    windowController->ProcessPointUp(mainWindowId);
    windowController->ProcessPointDown(mainWindowId, true);

    DisplayId displayId;
    startPos += GetObject<DisplayId>(displayId, data + startPos, size - startPos);
    windowController->MinimizeAllAppWindows(displayId);

    windowController->ToggleShownStateForAllAppWindows();

    WindowLayoutMode mode;
    GetObject<WindowLayoutMode>(mode, data + startPos, size - startPos);
    windowController->SetWindowLayoutMode(mode);
}

void WindowControllerFuzzTestPart2(sptr<WindowController> windowController,
    const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return;
    }
    size_t startPos = 0;
    uint32_t windowId;
    startPos += GetObject<uint32_t>(windowId, data + startPos, size - startPos);
    Parcel percel;
    if (percel.WriteBuffer(data, size)) {
        sptr<MoveDragProperty> moveDragProperty = MoveDragProperty::Unmarshalling(percel);
        windowController->NotifyServerReadyToMoveOrDrag(windowId, moveDragProperty);

        sptr<WindowProperty> property = WindowProperty::Unmarshalling(percel);
        PropertyChangeAction action;
        startPos += GetObject<PropertyChangeAction>(action, data + startPos, size - startPos);
        windowController->UpdateProperty(property, action);
    }

    windowController->NotifySystemBarTints();

    DisplayId displayId;
    ModeChangeHotZones hotZones;
    ModeChangeHotZonesConfig config;
    startPos += GetObject<DisplayId>(displayId, data + startPos, size - startPos);
    startPos += GetObject<ModeChangeHotZones>(hotZones, data + startPos, size - startPos);
    startPos += GetObject<ModeChangeHotZonesConfig>(config, data + startPos, size - startPos);
    windowController->GetModeChangeHotZones(displayId, hotZones, config);

    windowController->OnScreenshot(displayId);

    int32_t x;
    int32_t y;
    int32_t scale;
    startPos += GetObject<int32_t>(x, data + startPos, size - startPos);
    startPos += GetObject<int32_t>(y, data + startPos, size - startPos);
    startPos += GetObject<int32_t>(scale, data + startPos, size - startPos);
    windowController->SetAnchorAndScale(x, y, scale);

    windowController->SetAnchorOffset(x, y);
    windowController->OffWindowZoom();

    GetObject<uint32_t>(windowId, data + startPos, size - startPos);
    windowController->InterceptInputEventToServer(windowId);
    windowController->RecoverInputEventToClient(windowId);
    windowController->RecoverDefaultMouseStyle(windowId);
}

void WindowControllerFuzzTestPart3(sptr<WindowController> windowController,
    const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return;
    }
    size_t startPos = 0;

    uint32_t windowId;
    startPos += GetObject<uint32_t>(windowId, data + startPos, size - startPos);
    windowController->GenWindowId();
    windowController->RecordBootAnimationEvent();

    OHOS::Rosen::WindowType type;
    startPos += GetObject<OHOS::Rosen::WindowType>(type, data + startPos, size - startPos);
    windowController->SetWindowType(windowId, type);

    uint32_t flags;
    startPos += GetObject<uint32_t>(flags, data + startPos, size - startPos);
    windowController->SetWindowFlags(windowId, flags);

    OHOS::Rosen::SystemBarProperty property;
    startPos += GetObject<OHOS::Rosen::SystemBarProperty>(property, data + startPos, size - startPos);
    windowController->SetSystemBarProperty(windowId, type, property);

    OHOS::Rosen::Rect rect;
    OHOS::Rosen::WindowSizeChangeReason reason;
    startPos += GetObject<OHOS::Rosen::Rect>(rect, data + startPos, size - startPos);
    startPos += GetObject<OHOS::Rosen::WindowSizeChangeReason>(reason, data + startPos, size - startPos);
    windowController->ResizeRect(windowId, rect, reason);
    windowController->ResizeRectAndFlush(windowId, rect, reason);

    OHOS::Rosen::WindowMode dstMode;
    startPos += GetObject<OHOS::Rosen::WindowMode>(dstMode, data + startPos, size - startPos);
    windowController->SetWindowMode(windowId, dstMode);
    windowController->RestoreCallingWindowSizeIfNeed();

    GetObject<uint32_t>(windowId, data + startPos, size - startPos);
    windowController->UpdateTransform(windowId);
    windowController->AsyncFlushInputInfo(windowId);
}

bool DoWindowControllerFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }
    auto windowController = WindowManagerService::GetInstance().windowController_;
    if (windowController == nullptr) {
        return false;
    }
    OHOS::WindowControllerFuzzTestPart1(windowController, data, size);
    OHOS::WindowControllerFuzzTestPart2(windowController, data, size);
    OHOS::WindowControllerFuzzTestPart3(windowController, data, size);

    return true;
}
} // namespace.OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::DoWindowControllerFuzzTest(data, size);
    return 0;
}


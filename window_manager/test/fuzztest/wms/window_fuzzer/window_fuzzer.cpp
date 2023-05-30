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

#include "marshalling_helper.h"

#include <securec.h>

#include <want.h>
#include "window.h"
#include "window_impl.h"
#include "window_manager.h"

using namespace OHOS::Rosen;

namespace OHOS {
namespace {
    constexpr size_t DATA_MIN_SIZE = 2;
}
class FocusChangedListener : public IFocusChangedListener {
public:
    virtual void OnFocused(const sptr<FocusChangeInfo>& focusChangeInfo) override
    {
    }

    virtual void OnUnfocused(const sptr<FocusChangeInfo>& focusChangeInfo) override
    {
    }
};

class SystemBarChangedListener : public ISystemBarChangedListener {
public:
    virtual void OnSystemBarPropertyChange(DisplayId displayId, const SystemBarRegionTints& tints) override
    {
    }
};

class VisibilityChangedListener : public IVisibilityChangedListener {
public:
    virtual void OnWindowVisibilityChanged(const std::vector<sptr<WindowVisibilityInfo>>& windowVisibilityInfo) override
    {
    }
};

class WindowUpdateListener : public IWindowUpdateListener {
public:
    virtual void OnWindowUpdate(const std::vector<sptr<AccessibilityWindowInfo>>& infos, WindowUpdateType type) override
    {
    }
};

bool DoSomethingInterestingWithMyAPI(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }
    std::string name = "WindowFuzzTest";
    sptr<WindowOption> option = nullptr;
    sptr<Window> window = Window::Create(name, option);
    if (window == nullptr) {
        return false;
    }
    window->Show(0);
    Orientation orientation = static_cast<Orientation>(data[0]);
    window->SetRequestedOrientation(static_cast<Orientation>(data[0]));
    if (window->GetRequestedOrientation() != orientation) {
        return false;
    }
    window->Hide(0);
    window->Destroy();
    return true;
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

void CheckWindowImplFunctionsPart1(sptr<Window> window, const uint8_t* data, size_t size)
{
    if (window == nullptr || data == nullptr || size < DATA_MIN_SIZE) {
        return;
    }
    size_t startPos = 0;

    bool boolVal = false;
    startPos += GetObject(boolVal, data + startPos, size - startPos);
    window->SetFocusable(boolVal);
    window->IsFocused();
    startPos += GetObject(boolVal, data + startPos, size - startPos);
    window->SetTouchable(boolVal);

    WindowType windowType;
    WindowMode windowMode;
    startPos += GetObject(windowType, data + startPos, size - startPos);
    startPos += GetObject(windowMode, data + startPos, size - startPos);
    window->SetWindowType(windowType);
    window->SetWindowMode(windowMode);

    float alpha;
    startPos += GetObject(alpha, data + startPos, size - startPos);
    window->SetAlpha(alpha);

    Transform transForm;
    startPos += GetObject(transForm, data + startPos, size - startPos);
    window->SetTransform(transForm);

    WindowFlag windowFlag;
    startPos += GetObject(windowFlag, data + startPos, size - startPos);
    window->AddWindowFlag(windowFlag);
    startPos += GetObject(windowFlag, data + startPos, size - startPos);
    window->RemoveWindowFlag(windowFlag);

    SystemBarProperty systemBarProperty;
    startPos += GetObject(windowType, data + startPos, size - startPos);
    startPos += GetObject(systemBarProperty, data + startPos, size - startPos);
    window->SetSystemBarProperty(windowType, systemBarProperty);

    startPos += GetObject(boolVal, data + startPos, size - startPos);
    window->SetLayoutFullScreen(boolVal);

    startPos += GetObject(boolVal, data + startPos, size - startPos);
    window->SetFullScreen(boolVal);

    startPos += GetObject(boolVal, data + startPos, size - startPos);
    window->UpdateSurfaceNodeAfterCustomAnimation(boolVal);
}

void CheckWindowImplFunctionsPart2(sptr<WindowImpl> window, const uint8_t* data, size_t size)
{
    if (window == nullptr || data == nullptr || size < DATA_MIN_SIZE) {
        return;
    }
    size_t startPos = 0;

    int32_t posX;
    int32_t posY;
    startPos += GetObject(posX, data + startPos, size - startPos);
    startPos += GetObject(posY, data + startPos, size - startPos);
    window->MoveTo(posX, posY);

    uint32_t width;
    uint32_t hight;
    startPos += GetObject(width, data + startPos, size - startPos);
    startPos += GetObject(hight, data + startPos, size - startPos);
    window->Resize(width, hight);

    bool boolVal = false;
    startPos += GetObject(boolVal, data + startPos, size - startPos);
    window->SetKeepScreenOn(boolVal);

    startPos += GetObject(boolVal, data + startPos, size - startPos);
    window->SetTurnScreenOn(boolVal);

    startPos += GetObject(boolVal, data + startPos, size - startPos);
    window->SetTransparent(boolVal);

    startPos += GetObject(boolVal, data + startPos, size - startPos);
    window->SetBrightness(boolVal);

    int32_t windowId;
    startPos += GetObject(windowId, data + startPos, size - startPos);
    window->SetCallingWindow(windowId);

    startPos += GetObject(boolVal, data + startPos, size - startPos);
    window->SetPrivacyMode(boolVal);

    startPos += GetObject(boolVal, data + startPos, size - startPos);
    window->SetSystemPrivacyMode(boolVal);

    startPos += GetObject(boolVal, data + startPos, size - startPos);
    window->SetSnapshotSkip(boolVal);
}

void CheckWindowImplFunctionsPart3(sptr<WindowImpl> window, const uint8_t* data, size_t size)
{
    if (window == nullptr || data == nullptr || size < DATA_MIN_SIZE) {
        return;
    }
    size_t startPos = 0;
    float floatVal;
    startPos += GetObject(floatVal, data + startPos, size - startPos);
    window->SetCornerRadius(floatVal);

    startPos += GetObject(floatVal, data + startPos, size - startPos);
    window->SetShadowRadius(floatVal);

    startPos += GetObject(floatVal, data + startPos, size - startPos);
    window->SetShadowOffsetX(floatVal);
    startPos += GetObject(floatVal, data + startPos, size - startPos);
    window->SetShadowOffsetY(floatVal);
    startPos += GetObject(floatVal, data + startPos, size - startPos);
    window->SetBlur(floatVal);
    startPos += GetObject(floatVal, data + startPos, size - startPos);
    window->SetBackdropBlur(floatVal);

    WindowBlurStyle blurStyle;
    startPos += GetObject(blurStyle, data + startPos, size - startPos);
    window->SetBackdropBlurStyle(blurStyle);

    bool boolVal;
    OHOS::Rosen::Rect rect;
    WindowSizeChangeReason reason;
    startPos += GetObject(boolVal, data + startPos, size - startPos);
    startPos += GetObject(rect, data + startPos, size - startPos);
    startPos += GetObject(reason, data + startPos, size - startPos);
    window->UpdateRect(rect, boolVal, reason);

    WindowMode mode;
    startPos += GetObject(mode, data + startPos, size - startPos);
    window->UpdateMode(mode);

    uint32_t modeSupportInfo;
    startPos += GetObject(modeSupportInfo, data + startPos, size - startPos);
    window->UpdateModeSupportInfo(modeSupportInfo);

    WindowState windowState;
    startPos += GetObject(windowState, data + startPos, size - startPos);
    window->UpdateWindowState(windowState);

    PointInfo point;
    DragEvent event;
    startPos += GetObject(point, data + startPos, size - startPos);
    startPos += GetObject(event, data + startPos, size - startPos);
    window->UpdateDragEvent(point, event);

    DisplayId displayId[2];
    startPos += GetObject(displayId[0], data + startPos, size - startPos);
    startPos += GetObject(displayId[1], data + startPos, size - startPos);
    window->UpdateDisplayId(displayId[0], displayId[1]);
}

void CheckWindowImplFunctionsPart4(sptr<WindowImpl> window, const uint8_t* data, size_t size)
{
    if (window == nullptr || data == nullptr || size < DATA_MIN_SIZE) {
        return;
    }
    size_t startPos = 0;

    bool boolVal;
    startPos += GetObject(boolVal, data + startPos, size - startPos);
    window->UpdateActiveStatus(boolVal);

    Transform trans;
    startPos += GetObject(trans, data + startPos, size - startPos);
    startPos += GetObject(boolVal, data + startPos, size - startPos);
    window->UpdateZoomTransform(trans, boolVal);

    startPos += GetObject(boolVal, data + startPos, size - startPos);
    window->SetNeedRemoveWindowInputChannel(boolVal);

    std::vector<OHOS::Rosen::Rect> rectVector;
    OHOS::Rosen::Rect rect;
    startPos += GetObject(rect, data + startPos, size - startPos);
    rectVector.emplace_back(rect);
    window->SetTouchHotAreas(rectVector);
    window->GetRequestedTouchHotAreas(rectVector);
    rectVector.clear();

    ColorSpace colorSpace;
    startPos += GetObject(colorSpace, data + startPos, size - startPos);
    window->SetColorSpace(colorSpace);

    int32_t level;
    startPos += GetObject(level, data + startPos, size - startPos);
    window->NotifyMemoryLevel(level);

    uint32_t mode;
    startPos += GetObject(mode, data + startPos, size - startPos);
    window->RestoreSplitWindowMode(mode);
}

void CheckWindowImplFunctionsPart5(sptr<WindowImpl> window, const uint8_t* data, size_t size)
{
    if (window == nullptr || data == nullptr || size < DATA_MIN_SIZE) {
        return;
    }
    size_t startPos = 0;

    OHOS::Rosen::Rect rect;
    WindowSizeChangeReason reason;
    startPos += GetObject(rect, data + startPos, size - startPos);
    startPos += GetObject(reason, data + startPos, size - startPos);
    window->NotifySizeChange(rect, reason);

    DisplayId displayId[2];
    startPos += GetObject(displayId[0], data + startPos, size - startPos);
    startPos += GetObject(displayId[1], data + startPos, size - startPos);
    window->NotifyDisplayMoveChange(displayId[0], displayId[1]);

    WindowMode mode;
    startPos += GetObject(mode, data + startPos, size - startPos);
    window->NotifyModeChange(mode);

    PointInfo point;
    DragEvent dragEvent;
    startPos += GetObject(point, data + startPos, size - startPos);
    startPos += GetObject(dragEvent, data + startPos, size - startPos);
    window->NotifyDragEvent(point, dragEvent);

    int32_t posX;
    int32_t posY;
    int32_t pointId;
    int32_t sourceType;
    startPos += GetObject(posX, data + startPos, size - startPos);
    startPos += GetObject(posY, data + startPos, size - startPos);
    startPos += GetObject(pointId, data + startPos, size - startPos);
    startPos += GetObject(sourceType, data + startPos, size - startPos);
    window->EndMoveOrDragWindow(posX, posY, pointId, sourceType);
    window->IsPointInDragHotZone(posX, posY);

    bool boolVal;
    startPos += GetObject(boolVal, data + startPos, size - startPos);
    window->AdjustWindowAnimationFlag(boolVal);

    PropertyChangeAction action;
    startPos += GetObject(action, data + startPos, size - startPos);
    window->UpdateProperty(action);

    uint32_t uinte32Val;
    startPos += GetObject(uinte32Val, data + startPos, size - startPos);
    window->SetBackgroundColor(uinte32Val);

    LifeCycleEvent lifeCycleEvent;
    OHOS::Rosen::WMError errCode;
    startPos += GetObject(lifeCycleEvent, data + startPos, size - startPos);
    startPos += GetObject(errCode, data + startPos, size - startPos);
    window->RecordLifeCycleExceptionEvent(lifeCycleEvent, errCode);
    window->TransferLifeCycleEventToString(lifeCycleEvent);

    startPos += GetObject(rect, data + startPos, size - startPos);
    window->GetSystemAlarmWindowDefaultSize(rect);
    window->HandleModeChangeHotZones(posX, posY);
}


void CheckWindowImplFunctionsPart6(sptr<WindowImpl> window, const uint8_t* data, size_t size)
{
    if (window == nullptr || data == nullptr || size < DATA_MIN_SIZE) {
        return;
    }
    size_t startPos = 0;

    TransitionReason reason;
    startPos += GetObject(reason, data + startPos, size - startPos);
    window->NotifyWindowTransition(reason);

    WindowType type;
    startPos += GetObject(type, data + startPos, size - startPos);
    window->CheckCameraFloatingWindowMultiCreated(type);

    uint32_t uint32Val[2];
    startPos += GetObject(uint32Val[0], data + startPos, size - startPos);
    startPos += GetObject(uint32Val[1], data + startPos, size - startPos);
    window->SetModeSupportInfo(uint32Val[0]);

    float floatVal;
    startPos += GetObject(floatVal, data + startPos, size - startPos);
    window->CalculateStartRectExceptHotZone(floatVal);

    Transform transform;
    startPos += GetObject(transform, data + startPos, size - startPos);
    window->TransformSurfaceNode(transform);

    window->WindowCreateCheck(uint32Val[0]);
    window->CalculatePointerDirection(uint32Val[0], uint32Val[1]);
}

void WindowImplFuzzTest(const uint8_t* data, size_t size)
{
    std::string name = "WindowFuzzTest";
    sptr<OHOS::Rosen::WindowOption> option = new OHOS::Rosen::WindowOption();
    sptr<OHOS::Rosen::WindowImpl> window = new(std::nothrow) OHOS::Rosen::WindowImpl(option);
    if (window == nullptr) {
        return;
    }
    OHOS::Rosen::WMError error = window->Create(option->GetParentId(), nullptr);
    if (error != OHOS::Rosen::WMError::WM_OK) {
        return;
    }

    size_t startPos = 0;
    uint32_t reason = 0;
    bool withAnimation = false;
    startPos += GetObject(reason, data + startPos, size - startPos);
    startPos += GetObject(withAnimation, data + startPos, size - startPos);
    window->Show(reason, withAnimation);

    OHOS::CheckWindowImplFunctionsPart1(window, data, size);
    OHOS::CheckWindowImplFunctionsPart2(window, data, size);
    OHOS::CheckWindowImplFunctionsPart3(window, data, size);
    OHOS::CheckWindowImplFunctionsPart4(window, data, size);
    OHOS::CheckWindowImplFunctionsPart5(window, data, size);
    OHOS::CheckWindowImplFunctionsPart6(window, data, size);

    window->Hide(reason, withAnimation);
    window->Destroy();
}
} // namespace.OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::DoSomethingInterestingWithMyAPI(data, size);
    OHOS::WindowImplFuzzTest(data, size);
    return 0;
}
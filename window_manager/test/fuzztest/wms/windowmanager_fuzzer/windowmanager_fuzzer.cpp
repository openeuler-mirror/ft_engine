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

#include <parcel.h>

#include <securec.h>

#include "marshalling_helper.h"
#include "window_manager.h"

using namespace OHOS::Rosen;

namespace OHOS {
namespace {
    constexpr size_t DATA_MIN_SIZE = 2;
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

class CameraFloatWindowChangedListener : public ICameraFloatWindowChangedListener {
public:
    void OnCameraFloatWindowChange(uint32_t accessTokenId, bool isShowing) override
    {
    }
};


bool DoSomethingForWindowManagerImpl(WindowManager& windowManager, const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }
    size_t startPos = 0;
    uint32_t accessTokenId;
    bool isShowing;
    startPos += GetObject<uint32_t>(accessTokenId, data + startPos, size - startPos);
    startPos += GetObject<bool>(isShowing, data + startPos, size - startPos);
    windowManager.UpdateCameraFloatWindowStatus(accessTokenId, isShowing);

    Parcel windowVisibilityInfosParcel;
    if (windowVisibilityInfosParcel.WriteBuffer(data, size)) {
        std::vector<sptr<WindowVisibilityInfo>> infos;
        if (MarshallingHelper::UnmarshallingVectorParcelableObj<WindowVisibilityInfo>(windowVisibilityInfosParcel,
            infos)) {
            windowManager.GetVisibilityWindowInfo(infos);
            windowManager.UpdateWindowVisibilityInfo(infos);
        }
    }

    DisplayId displayId;
    SystemBarRegionTints tints;
    startPos += GetObject<DisplayId>(displayId, data + startPos, size - startPos);
    GetObject<SystemBarRegionTints>(tints, data + startPos, size - startPos);
    windowManager.UpdateSystemBarRegionTints(displayId, tints);

    return true;
}

void CheckAccessibilityWindowInfo(WindowManager& windowManager, const uint8_t* data, size_t size)
{
    Parcel accessibilityWindowInfosParcel;
    if (accessibilityWindowInfosParcel.WriteBuffer(data, size)) {
        std::vector<sptr<AccessibilityWindowInfo>> infos;
        if (MarshallingHelper::UnmarshallingVectorParcelableObj<AccessibilityWindowInfo>(accessibilityWindowInfosParcel,
            infos)) {
            windowManager.GetAccessibilityWindowInfo(infos);
            WindowUpdateType type;
            GetObject<WindowUpdateType>(type, data, size);
            windowManager.NotifyAccessibilityWindowInfo(infos, type);
        }
    }
}

bool DoSomethingInterestingWithMyAPI(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }
    WindowManager& windowManager = WindowManager::GetInstance();
    CheckAccessibilityWindowInfo(windowManager, data, size);
    
    Parcel focusChangeInfoParcel;
    if (focusChangeInfoParcel.WriteBuffer(data, size)) {
        FocusChangeInfo::Unmarshalling(focusChangeInfoParcel);
    }
    Parcel parcel;
    sptr<FocusChangeInfo> focusChangeInfo = new FocusChangeInfo();
    focusChangeInfo->Marshalling(parcel);

    Parcel windowVisibilityInfoParcel;
    if (windowVisibilityInfoParcel.WriteBuffer(data, size)) {
        WindowVisibilityInfo::Unmarshalling(windowVisibilityInfoParcel);
    }
    sptr<WindowVisibilityInfo> windowVisibilityInfo = new WindowVisibilityInfo();
    windowVisibilityInfo->Marshalling(parcel);

    Parcel accessibilityWindowInfoParcel;
    if (accessibilityWindowInfoParcel.WriteBuffer(data, size)) {
        AccessibilityWindowInfo::Unmarshalling(accessibilityWindowInfoParcel);
    }
    sptr<AccessibilityWindowInfo> accessibilityWindowInfo = new AccessibilityWindowInfo();
    accessibilityWindowInfo->Marshalling(parcel);

    windowManager.MinimizeAllAppWindows(static_cast<DisplayId>(data[0]));
    sptr<IFocusChangedListener> focusChangedListener = new FocusChangedListener();
    windowManager.RegisterFocusChangedListener(focusChangedListener);
    sptr<ISystemBarChangedListener> systemBarChangedListener = new SystemBarChangedListener();
    windowManager.RegisterSystemBarChangedListener(systemBarChangedListener);
    sptr<IVisibilityChangedListener> visibilityChangedListener = new VisibilityChangedListener();
    windowManager.RegisterVisibilityChangedListener(visibilityChangedListener);
    sptr<IWindowUpdateListener> windowUpdateListener = new WindowUpdateListener();
    windowManager.RegisterWindowUpdateListener(windowUpdateListener);
    sptr<ICameraFloatWindowChangedListener> cameraFloatWindowChanagedListener = new CameraFloatWindowChangedListener();
    windowManager.RegisterCameraFloatWindowChangedListener(cameraFloatWindowChanagedListener);
    windowManager.SetWindowLayoutMode(static_cast<WindowLayoutMode>(data[0]));
    windowManager.UnregisterFocusChangedListener(focusChangedListener);
    windowManager.UnregisterSystemBarChangedListener(systemBarChangedListener);
    windowManager.UnregisterVisibilityChangedListener(visibilityChangedListener);
    windowManager.UnregisterWindowUpdateListener(windowUpdateListener);
    windowManager.UnregisterCameraFloatWindowChangedListener(cameraFloatWindowChanagedListener);
    return true;
}
} // namespace.OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::DoSomethingInterestingWithMyAPI(data, size);
    return 0;
}


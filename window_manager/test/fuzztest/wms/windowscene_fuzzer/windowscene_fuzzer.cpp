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

#include "windowscene_fuzzer.h"

#include <securec.h>

#include "display_manager.h"
#include "window.h"
#include "window_manager.h"
#include "window_scene.h"

using namespace OHOS::Rosen;

namespace OHOS {
namespace {
    constexpr size_t DATA_MIN_SIZE = 2;
    constexpr char END_CHAR = '\0';
    constexpr size_t LEN = 10;
}
class WindowLifeCycle : public IWindowLifeCycle {
public:
    virtual void AfterForeground() override
    {
    }
    virtual void AfterBackground() override
    {
    }
    virtual void AfterFocused() override
    {
    }
    virtual void AfterUnfocused() override
    {
    }
    virtual void AfterActive() override
    {
    }
    virtual void AfterInactive() override
    {
    }
};

template<class T>
size_t GetObject(T &object, const uint8_t *data, size_t size)
{
    size_t objectSize = sizeof(object);
    if (objectSize > size) {
        return 0;
    }
    return memcpy_s(&object, objectSize, data, objectSize) == EOK ? objectSize : 0;
}

size_t InitWindowOption1(WindowOption &windowOption, const uint8_t *data, size_t size)
{
    size_t startPos = 0;
    Rect windowRect;
    startPos += GetObject<Rect>(windowRect, data + startPos, size - startPos);
    windowOption.SetWindowRect(windowRect);
    uint32_t type;
    startPos += GetObject<uint32_t>(type, data + startPos, size - startPos);
    windowOption.SetWindowType(static_cast<WindowType>(type));
    uint32_t mode;
    startPos += GetObject<uint32_t>(mode, data + startPos, size - startPos);
    windowOption.SetWindowMode(static_cast<WindowMode>(mode));
    bool focusable;
    startPos += GetObject<bool>(focusable, data + startPos, size - startPos);
    windowOption.SetFocusable(focusable);
    bool touchable;
    startPos += GetObject<bool>(touchable, data + startPos, size - startPos);
    windowOption.SetTouchable(touchable);
    DisplayId displayId;
    startPos += GetObject<DisplayId>(displayId, data + startPos, size - startPos);
    windowOption.SetDisplayId(displayId);
    uint32_t parentId;
    startPos += GetObject<uint32_t>(parentId, data + startPos, size - startPos);
    windowOption.SetParentId(parentId);
    char name[LEN + 1];
    name[LEN] = END_CHAR;
    for (int i = 0; i < LEN; i++) {
        startPos += GetObject<char>(name[i], data + startPos, size - startPos);
    }
    std::string windowName(name);
    windowOption.SetWindowName(windowName);
    return startPos;
}

size_t InitWindowOption2(WindowOption &windowOption, const uint8_t *data, size_t size)
{
    size_t startPos = 0;
    uint32_t flags;
    startPos += GetObject<uint32_t>(flags, data + startPos, size - startPos);
    windowOption.SetWindowFlags(flags);
    PointInfo hitOffset;
    startPos += GetObject<PointInfo>(hitOffset, data + startPos, size - startPos);
    windowOption.SetHitOffset(hitOffset.x, hitOffset.y);
    WindowTag windowTag;
    startPos += GetObject<WindowTag>(windowTag, data + startPos, size - startPos);
    windowOption.SetWindowTag(windowTag);
    bool keepScreenOn;
    startPos += GetObject<bool>(keepScreenOn, data + startPos, size - startPos);
    windowOption.SetKeepScreenOn(keepScreenOn);
    bool turnScreenOn;
    startPos += GetObject<bool>(turnScreenOn, data + startPos, size - startPos);
    windowOption.SetTurnScreenOn(turnScreenOn);
    float brightness;
    startPos += GetObject<float>(brightness, data + startPos, size - startPos);
    windowOption.SetBrightness(brightness);
    uint32_t callingWindow;
    startPos += GetObject<uint32_t>(callingWindow, data + startPos, size - startPos);
    windowOption.SetCallingWindow(callingWindow);
    SystemBarProperty statusBarProperty;
    SystemBarProperty navigationBarProperty;
    startPos += GetObject<SystemBarProperty>(statusBarProperty, data + startPos, size - startPos);
    startPos += GetObject<SystemBarProperty>(navigationBarProperty, data + startPos, size - startPos);
    windowOption.SetSystemBarProperty(WindowType::WINDOW_TYPE_STATUS_BAR, statusBarProperty);
    windowOption.SetSystemBarProperty(WindowType::WINDOW_TYPE_NAVIGATION_BAR, navigationBarProperty);
    Orientation requestedOrientation;
    startPos += GetObject<Orientation>(requestedOrientation, data + startPos, size - startPos);
    windowOption.SetRequestedOrientation(requestedOrientation);
    bool isMainHandleAvailable;
    startPos += GetObject<bool>(isMainHandleAvailable, data + startPos, size - startPos);
    windowOption.SetMainHandlerAvailable(isMainHandleAvailable);
    return startPos;
}

size_t InitWindowOption(WindowOption &windowOption, const uint8_t *data, size_t size)
{
    size_t startPos = 0;
    startPos += InitWindowOption1(windowOption, data + startPos, size - startPos);
    startPos += InitWindowOption2(windowOption, data + startPos, size - startPos);
    return startPos;
}

bool DoSomethingInterestingWithMyAPI(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }
    DisplayId displayId = DisplayManager::GetInstance().GetDefaultDisplayId();
    sptr<WindowScene> windowScene = new WindowScene();
    sptr<IWindowLifeCycle> listener = new WindowLifeCycle();
    sptr<WindowOption> option = new WindowOption();
    size_t startPos = 0;
    startPos += InitWindowOption(*option, data, size);
    windowScene->Init(displayId, nullptr, listener, option);
    char name[LEN + 1];
    name[LEN] = END_CHAR;
    for (int i = 0; i < LEN; i++) {
        startPos += GetObject<char>(name[i], data + startPos, size - startPos);
    }
    std::string windowName(name);
    sptr<WindowOption> windowOption = new WindowOption();
    startPos += InitWindowOption(*windowOption, data + startPos, size - startPos);
    sptr<Window> window = windowScene->CreateWindow(windowName, windowOption);
    uint32_t reason;
    startPos += GetObject<uint32_t>(reason, data + startPos, size - startPos);
    windowScene->GoForeground(reason);
    SystemBarProperty systemBarProperty;
    WindowType type;
    startPos += GetObject<SystemBarProperty>(systemBarProperty, data + startPos, size - startPos);
    startPos += GetObject<WindowType>(type, data + startPos, size - startPos);
    windowScene->SetSystemBarProperty(type, systemBarProperty);
    GetObject<uint32_t>(reason, data + startPos, size - startPos);
    windowScene->GoBackground(reason);
    if (window != nullptr) {
        window->Destroy();
    }
    windowScene->GoDestroy();
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


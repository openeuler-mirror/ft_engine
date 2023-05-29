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

#include "displaymanager_fuzzer.h"

#include <securec.h>
#include "display_manager.h"

namespace OHOS ::Rosen {
class DisplayListener : public DisplayManager::IDisplayListener {
public:
    virtual void OnCreate(DisplayId) override
    {
    }
    virtual void OnDestroy(DisplayId) override
    {
    }
    virtual void OnChange(DisplayId) override
    {
    }
};

class DisplayPowerEventListener : public IDisplayPowerEventListener {
public:
    virtual void OnDisplayPowerEvent(DisplayPowerEvent event, EventStatus status) override
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

bool DisplayFuzzTest(const uint8_t* data, size_t size)
{
    DisplayId displayId;
    ScreenId screenId;
    if (data == nullptr || size < sizeof(displayId) + sizeof(screenId)) {
        return false;
    }
    size_t startPos = 0;
    DisplayManager& displayManager = DisplayManager::GetInstance();
    sptr<DisplayListener> displayListener = new DisplayListener();
    displayManager.GetAllDisplays();
    displayManager.GetAllDisplayIds();
    displayManager.GetDefaultDisplayId();
    displayManager.GetDefaultDisplay();

    startPos += GetObject<DisplayId>(displayId, data + startPos, size - startPos);
    displayManager.GetDisplayById(displayId);
    startPos += GetObject<ScreenId>(screenId, data + startPos, size - startPos);
    displayManager.GetDisplayByScreen(screenId);
    displayManager.RegisterDisplayListener(displayListener);
    displayManager.UnregisterDisplayListener(displayListener);
    return true;
}

bool GetScreenshotFuzzTest(const uint8_t* data, size_t size)
{
    DisplayId displayId;
    Media::Rect rect;
    Media::Size mediaSize;
    int rotation;
    if (data == nullptr || size < sizeof(displayId) + sizeof(rect) + sizeof(size) + sizeof(rotation)) {
        return false;
    }
    size_t startPos = 0;
    DisplayManager& displayManager = DisplayManager::GetInstance();
    startPos += GetObject<DisplayId>(displayId, data + startPos, size - startPos);
    displayManager.GetScreenshot(displayId);
    startPos += GetObject<Media::Rect>(rect, data + startPos, size - startPos);
    startPos += GetObject<Media::Size>(mediaSize, data + startPos, size - startPos);
    GetObject<int>(rotation, data + startPos, size - startPos);
    displayManager.GetScreenshot(displayId, rect, mediaSize, rotation);
    return true;
}

bool DisplayPowerFuzzTest(const uint8_t* data, size_t size)
{
    uint32_t reason;
    DisplayId displayId;
    uint32_t state;
    if (data == nullptr || size < sizeof(reason) + sizeof(displayId) + sizeof(state)) {
        return false;
    }
    size_t startPos = 0;
    DisplayManager& displayManager = DisplayManager::GetInstance();
    sptr<IDisplayPowerEventListener> listener = new DisplayPowerEventListener();
    displayManager.RegisterDisplayPowerEventListener(listener);

    startPos += GetObject<uint32_t>(reason, data + startPos, size - startPos);
    displayManager.WakeUpBegin(static_cast<PowerStateChangeReason>(reason));
    displayManager.WakeUpEnd();
    displayManager.SuspendBegin(static_cast<PowerStateChangeReason>(reason));
    displayManager.SuspendEnd();

    startPos += GetObject<uint32_t>(state, data + startPos, size - startPos);
    DisplayStateCallback callback = [](DisplayState state) {
    };
    displayManager.SetDisplayState(static_cast<DisplayState>(state), callback);
    startPos += GetObject<DisplayId>(displayId, data + startPos, size - startPos);
    displayManager.GetDisplayState(displayId);

    displayManager.UnregisterDisplayPowerEventListener(listener);
    return true;
}

bool ScreenBrightnessFuzzTest(const uint8_t* data, size_t size)
{
    uint64_t screenId;
    uint32_t level;
    if (data == nullptr || size < sizeof(screenId) + sizeof(level)) {
        return false;
    }
    size_t startPos = 0;
    DisplayManager& displayManager = DisplayManager::GetInstance();

    startPos += GetObject<uint64_t>(screenId, data + startPos, size - startPos);
    GetObject<uint32_t>(level, data + startPos, size - startPos);
    displayManager.SetScreenBrightness(screenId, level);
    displayManager.GetScreenBrightness(screenId);
    return true;
}

bool FreezeFuzzTest(const uint8_t* data, size_t size)
{
    // 10 displays
    if (data == nullptr || size < sizeof(DisplayId) * 10) {
        return false;
    }
    size_t startPos = 0;
    DisplayManager& displayManager = DisplayManager::GetInstance();
    // 10 displays
    std::vector<DisplayId> displays(10);
    for (DisplayId& id : displays) {
        startPos += GetObject<DisplayId>(id, data + startPos, size - startPos);
    }
    displayManager.Freeze(displays);
    displayManager.Unfreeze(displays);
    return true;
}

bool NotifyDisplayEventFuzzTest(const uint8_t* data, size_t size)
{
    // 10 displays
    if (data == nullptr || size < sizeof(DisplayEvent)) {
        return false;
    }
    size_t startPos = 0;
    DisplayManager& displayManager = DisplayManager::GetInstance();
    uint32_t event;
    GetObject<uint32_t>(event, data + startPos, size - startPos);
    displayManager.NotifyDisplayEvent(static_cast<DisplayEvent>(event));
    return true;
}
} // namespace.OHOS::Rosen

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::DisplayFuzzTest(data, size);
    OHOS::Rosen::GetScreenshotFuzzTest(data, size);
    OHOS::Rosen::DisplayPowerFuzzTest(data, size);
    OHOS::Rosen::ScreenBrightnessFuzzTest(data, size);
    OHOS::Rosen::FreezeFuzzTest(data, size);
    OHOS::Rosen::NotifyDisplayEventFuzzTest(data, size);
    return 0;
}


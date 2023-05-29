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

#include "screenmanager_fuzzer.h"

#include <iostream>
#include <securec.h>

#include "dm_common.h"
#include "screen.h"
#include "screen_manager.h"

namespace OHOS::Rosen {
class ScreenListener : public ScreenManager::IScreenListener {
public:
    virtual void OnConnect(ScreenId screenId) override
    {
    };

    virtual void OnDisconnect(ScreenId screenId) override
    {
    }

    virtual void OnChange(ScreenId screenId) override
    {
    }
};

class ScreenGroupListener : public ScreenManager::IScreenGroupListener {
public:
    virtual void OnChange(const std::vector<ScreenId>& screenIds, ScreenGroupChangeEvent event) override
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

bool ScreenPowerFuzzTest(const uint8_t *data, size_t size)
{
    uint32_t screenPowerState;
    uint32_t powerStateChangeReason;
    if (data == nullptr || size < sizeof(screenPowerState) + sizeof(powerStateChangeReason)) {
        return false;
    }
    size_t startPos = 0;
    ScreenManager &screenManager = ScreenManager::GetInstance();
    sptr<ScreenManager::IScreenListener> screenListener = new ScreenListener();
    screenManager.RegisterScreenListener(screenListener);
    sptr<ScreenManager::IScreenGroupListener> screenGroupListener = new ScreenGroupListener();
    screenManager.RegisterScreenGroupListener(screenGroupListener);

    startPos += GetObject<uint32_t>(screenPowerState, data + startPos, size - startPos);
    startPos += GetObject<uint32_t>(powerStateChangeReason, data + startPos, size - startPos);
    screenManager.SetScreenPowerForAll(static_cast<ScreenPowerState>(screenPowerState),
        static_cast<PowerStateChangeReason>(startPos));
    auto allScreen = screenManager.GetAllScreens();
    for (auto screen: allScreen) {
        screenManager.GetScreenPower(screen->GetId());
    }

    screenManager.UnregisterScreenGroupListener(screenGroupListener);
    screenManager.UnregisterScreenListener(screenListener);
    return true;
}

bool MakeMirrorWithVirtualScreenFuzzTest(const uint8_t *data, size_t size)
{
    if (data == nullptr || size < sizeof(VirtualScreenOption)) {
        return false;
    }
    size_t startPos = 0;
    ScreenManager &screenManager = ScreenManager::GetInstance();
    sptr<ScreenManager::IScreenListener> screenListener = new ScreenListener();
    screenManager.RegisterScreenListener(screenListener);
    sptr<ScreenManager::IScreenGroupListener> screenGroupListener = new ScreenGroupListener();
    screenManager.RegisterScreenGroupListener(screenGroupListener);

    std::string name = "screen";
    VirtualScreenOption option = { name };
    startPos += GetObject<uint32_t>(option.width_, data + startPos, size - startPos);
    startPos += GetObject<uint32_t>(option.height_, data + startPos, size - startPos);
    startPos += GetObject<float>(option.density_, data + startPos, size - startPos);
    startPos += GetObject<int32_t>(option.flags_, data + startPos, size - startPos);
    GetObject<bool>(option.isForShot_, data + startPos, size - startPos);
    ScreenId screenId = screenManager.CreateVirtualScreen(option);
    if (screenId == SCREEN_ID_INVALID) {
        return false;
    }
    screenManager.SetVirtualScreenSurface(screenId, nullptr);

    // make mirror
    ScreenId groupId = screenManager.MakeMirror(0, { screenId });
    if (groupId == SCREEN_ID_INVALID) {
        screenManager.DestroyVirtualScreen(screenId);
        return false;
    }
    sptr<ScreenGroup> group = screenManager.GetScreenGroup(groupId);
    if (group == nullptr) {
        screenManager.DestroyVirtualScreen(screenId);
        return false;
    }
    std::vector<ScreenId> ids = group->GetChildIds();
    screenManager.RemoveVirtualScreenFromGroup(ids);
    screenManager.DestroyVirtualScreen(screenId);
    screenManager.UnregisterScreenGroupListener(screenGroupListener);
    screenManager.UnregisterScreenListener(screenListener);
    return true;
}

bool MakeExpandWithVirtualScreenFuzzTest(const uint8_t *data, size_t size)
{
    if (data == nullptr || size < sizeof(VirtualScreenOption)) {
        return false;
    }
    size_t startPos = 0;
    ScreenManager &screenManager = ScreenManager::GetInstance();
    sptr<ScreenManager::IScreenListener> screenListener = new ScreenListener();
    screenManager.RegisterScreenListener(screenListener);
    sptr<ScreenManager::IScreenGroupListener> screenGroupListener = new ScreenGroupListener();
    screenManager.RegisterScreenGroupListener(screenGroupListener);

    std::string name = "screen";
    VirtualScreenOption option = { name };
    startPos += GetObject<uint32_t>(option.width_, data + startPos, size - startPos);
    startPos += GetObject<uint32_t>(option.height_, data + startPos, size - startPos);
    startPos += GetObject<float>(option.density_, data + startPos, size - startPos);
    startPos += GetObject<int32_t>(option.flags_, data + startPos, size - startPos);
    GetObject<bool>(option.isForShot_, data + startPos, size - startPos);
    ScreenId screenId = screenManager.CreateVirtualScreen(option);
    if (screenId == SCREEN_ID_INVALID) {
        return false;
    }
    screenManager.SetVirtualScreenSurface(screenId, nullptr);
    // make expand
    std::vector<ExpandOption> options = {{0, 0, 0}, {screenId, 0, 0}};
    ScreenId groupId = screenManager.MakeExpand(options);
    if (groupId == SCREEN_ID_INVALID) {
        screenManager.DestroyVirtualScreen(screenId);
        return false;
    }
    sptr<ScreenGroup> group = screenManager.GetScreenGroup(groupId);
    if (group == nullptr) {
        screenManager.DestroyVirtualScreen(screenId);
        return false;
    }
    std::vector<ScreenId> ids = group->GetChildIds();
    screenManager.RemoveVirtualScreenFromGroup(ids);
    screenManager.DestroyVirtualScreen(screenId);
    screenManager.UnregisterScreenGroupListener(screenGroupListener);
    screenManager.UnregisterScreenListener(screenListener);
    return true;
}

bool CreateAndDestroyVirtualScreenFuzzTest(const uint8_t *data, size_t size)
{
    if (data == nullptr || size < sizeof(VirtualScreenOption)) {
        return false;
    }
    size_t startPos = 0;
    ScreenManager &screenManager = ScreenManager::GetInstance();
    sptr<ScreenManager::IScreenListener> screenListener = new ScreenListener();
    screenManager.RegisterScreenListener(screenListener);
    sptr<ScreenManager::IScreenGroupListener> screenGroupListener = new ScreenGroupListener();
    screenManager.RegisterScreenGroupListener(screenGroupListener);

    std::string name = "screen";
    VirtualScreenOption option = { name };
    startPos += GetObject<uint32_t>(option.width_, data + startPos, size - startPos);
    startPos += GetObject<uint32_t>(option.height_, data + startPos, size - startPos);
    startPos += GetObject<float>(option.density_, data + startPos, size - startPos);
    startPos += GetObject<int32_t>(option.flags_, data + startPos, size - startPos);
    startPos += GetObject<bool>(option.isForShot_, data + startPos, size - startPos);
    ScreenId screenId = screenManager.CreateVirtualScreen(option);
    if (screenId == SCREEN_ID_INVALID) {
        return false;
    }
    screenManager.DestroyVirtualScreen(screenId);
    GetObject<ScreenId>(screenId, data + startPos, size - startPos);
    screenManager.DestroyVirtualScreen(screenId);
    screenManager.UnregisterScreenGroupListener(screenGroupListener);
    screenManager.UnregisterScreenListener(screenListener);
    return true;
}

bool SetVirtualScreenSurfaceFuzzTest(const uint8_t *data, size_t size)
{
    ScreenId screenId;
    if (data == nullptr || size < sizeof(screenId)) {
        return false;
    }
    size_t startPos = 0;
    ScreenManager &screenManager = ScreenManager::GetInstance();
    GetObject<ScreenId>(screenId, data + startPos, size - startPos);
    screenManager.SetVirtualScreenSurface(screenId, nullptr);
    return true;
}

bool RemoveVirtualScreenFromGroupFuzzTest(const uint8_t *data, size_t size)
{
    ScreenId screenId;
    if (data == nullptr || size < sizeof(screenId)) {
        return false;
    }
    size_t startPos = 0;
    ScreenManager &screenManager = ScreenManager::GetInstance();
    GetObject<ScreenId>(screenId, data + startPos, size - startPos);
    std::vector<ScreenId> screenIds = {screenId, screenId, screenId};
    screenManager.RemoveVirtualScreenFromGroup(screenIds);
    return true;
}

bool MakeMirrorFuzzTest(const uint8_t *data, size_t size)
{
    ScreenId screenId;
    // 10 screens.
    if (data == nullptr || size < sizeof(ScreenId) * 10) {
        return false;
    }
    size_t startPos = 0;
    ScreenManager &screenManager = ScreenManager::GetInstance();
    sptr<ScreenManager::IScreenListener> screenListener = new ScreenListener();
    screenManager.RegisterScreenListener(screenListener);
    sptr<ScreenManager::IScreenGroupListener> screenGroupListener = new ScreenGroupListener();
    screenManager.RegisterScreenGroupListener(screenGroupListener);

    std::vector<ScreenId> screenIds;
    // 10 screens
    for (size_t i = 0; i < 10; i++) {
        startPos += GetObject<ScreenId>(screenId, data + startPos, size - startPos);
        screenIds.emplace_back(screenId);
    }
    GetObject<ScreenId>(screenId, data + startPos, size - startPos);
    screenManager.MakeMirror(screenId, screenIds);
    screenManager.UnregisterScreenGroupListener(screenGroupListener);
    screenManager.UnregisterScreenListener(screenListener);
    return true;
}

bool MakeExpandFuzzTest(const uint8_t *data, size_t size)
{
    ScreenId screenId;
    // 10 screens.
    if (data == nullptr || size < sizeof(ScreenId) * 10) {
        return false;
    }
    size_t startPos = 0;
    ScreenManager &screenManager = ScreenManager::GetInstance();
    sptr<ScreenManager::IScreenListener> screenListener = new ScreenListener();
    screenManager.RegisterScreenListener(screenListener);
    sptr<ScreenManager::IScreenGroupListener> screenGroupListener = new ScreenGroupListener();
    screenManager.RegisterScreenGroupListener(screenGroupListener);
    std::vector<ExpandOption> options;
    // 10 screens
    for (size_t i = 0; i < 10; i++) {
        startPos += GetObject<ScreenId>(screenId, data + startPos, size - startPos);
        ExpandOption option = {screenId, 0, 0};
        options.emplace_back(option);
    }
    screenManager.MakeExpand(options);
    screenManager.UnregisterScreenGroupListener(screenGroupListener);
    screenManager.UnregisterScreenListener(screenListener);
    return true;
}
} // namespace.OHOS::Rosen

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::ScreenPowerFuzzTest(data, size);
    OHOS::Rosen::MakeMirrorWithVirtualScreenFuzzTest(data, size);
    OHOS::Rosen::MakeMirrorFuzzTest(data, size);
    OHOS::Rosen::MakeExpandWithVirtualScreenFuzzTest(data, size);
    OHOS::Rosen::MakeExpandFuzzTest(data, size);
    OHOS::Rosen::CreateAndDestroyVirtualScreenFuzzTest(data, size);
    OHOS::Rosen::SetVirtualScreenSurfaceFuzzTest(data, size);
    OHOS::Rosen::RemoveVirtualScreenFromGroupFuzzTest(data, size);
    return 0;
}


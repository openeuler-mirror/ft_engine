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

#include "screen_fuzzer.h"

#include <iostream>
#include <securec.h>

#include "display_manager.h"
#include "display.h"
#include "dm_common.h"
#include "screen.h"
#include "screen_info.h"
#include "screen_manager.h"

namespace OHOS::Rosen {

namespace {
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

sptr<ScreenInfo> CreateScreenInfo(const uint8_t *data, size_t size)
{
    sptr<ScreenInfo> info = new(std::nothrow) ScreenInfo();
    if (info == nullptr) {
        return nullptr;
    }
    size_t startPos = 0;
    char name[LEN + 1];
    name[LEN] = END_CHAR;
    for (size_t i = 0; i < LEN; i++) {
        startPos += GetObject<char>(name[i], data + startPos, size - startPos);
    }
    std::string windowName(name);
    info->name_ = windowName;
    startPos += GetObject<ScreenId>(info->id_, data + startPos, size - startPos);
    startPos += GetObject<uint32_t>(info->virtualWidth_, data + startPos, size - startPos);
    startPos += GetObject<uint32_t>(info->virtualHeight_, data + startPos, size - startPos);
    startPos += GetObject<float>(info->virtualPixelRatio_, data + startPos, size - startPos);
    startPos += GetObject<ScreenId>(info->lastParent_, data + startPos, size - startPos);
    startPos += GetObject<ScreenId>(info->parent_, data + startPos, size - startPos);
    startPos += GetObject<bool>(info->isScreenGroup_, data + startPos, size - startPos);
    startPos += GetObject<Rotation>(info->rotation_, data + startPos, size - startPos);
    startPos += GetObject<Orientation>(info->orientation_, data + startPos, size - startPos);
    startPos += GetObject<ScreenType>(info->type_, data + startPos, size - startPos);
    GetObject<uint32_t>(info->modeId_, data + startPos, size - startPos);
    return info;
}
bool ScreenFuzzTest(sptr<Screen> screen, sptr<Display> display, const uint8_t *data, size_t size)
{
    if (screen == nullptr || display == nullptr) {
        return false;
    }
    uint32_t modeId;
    Orientation orientation;
    size_t minSize = sizeof(modeId) + sizeof(orientation);
    if (data == nullptr || size < minSize) {
        return false;
    }
    size_t startPos = 0;
    uint32_t originalDensityDpi = static_cast<uint32_t>(display->GetVirtualPixelRatio() * DOT_PER_INCH);
    uint32_t modifiedDensityDpi = static_cast<uint32_t>(
        originalDensityDpi + 80 >= 640 ? originalDensityDpi - 80 : originalDensityDpi + 80);
    startPos += GetObject<uint32_t>(modeId, data + startPos, size - startPos);
    GetObject<Orientation>(orientation, data + startPos, size - startPos);
    screen->SetScreenActiveMode(modeId);
    screen->SetOrientation(orientation);
    screen->SetDensityDpi(modifiedDensityDpi);
    screen->SetScreenActiveMode(0);
    screen->SetOrientation(Orientation::UNSPECIFIED);
    screen->SetDensityDpi(originalDensityDpi);
    return true;
}

bool ScreenFuzzTestNoDisplay(sptr<Screen> screen, const uint8_t *data, size_t size)
{
    if (screen == nullptr) {
        return false;
    }
    uint32_t modeId;
    Orientation orientation;
    uint32_t originalDensityDpi = 0;
    uint32_t modifiedDensityDpi = 0;
    size_t minSize = sizeof(modeId) + sizeof(orientation) + sizeof(originalDensityDpi) +
        sizeof(modifiedDensityDpi);
    if (data == nullptr || size < minSize) {
        return false;
    }
    size_t startPos = 0;
    GetObject<uint32_t>(originalDensityDpi, data + startPos, size - startPos);
    GetObject<uint32_t>(modifiedDensityDpi, data + startPos, size - startPos);
    startPos += GetObject<uint32_t>(modeId, data + startPos, size - startPos);
    GetObject<Orientation>(orientation, data + startPos, size - startPos);
    screen->SetScreenActiveMode(modeId);
    screen->SetOrientation(orientation);
    screen->SetDensityDpi(modifiedDensityDpi);
    screen->SetScreenActiveMode(0);
    screen->SetOrientation(Orientation::UNSPECIFIED);
    screen->SetDensityDpi(originalDensityDpi);
    return true;
}

bool ColorGamutsFuzzTest(sptr<Screen> screen, const uint8_t *data, size_t size)
{
    if (screen == nullptr) {
        return false;
    }
    int32_t colorGamutIdx;
    uint32_t gamutMap;
    if (data == nullptr || size < sizeof(colorGamutIdx) + sizeof(gamutMap)) {
        return false;
    }
    size_t startPos = 0;
    startPos += GetObject<int32_t>(colorGamutIdx, data + startPos, size - startPos);
    GetObject<uint32_t>(gamutMap, data + startPos, size - startPos);
    std::vector<ScreenColorGamut> colorGamuts;
    screen->GetScreenSupportedColorGamuts(colorGamuts);
    size_t colorGamutsSize = colorGamuts.size();
    if (colorGamutsSize == 0) {
        return false;
    }
    int32_t index = colorGamutIdx % colorGamutsSize;
    screen->SetScreenColorGamut(index);
    ScreenColorGamut colorGamut;
    screen->GetScreenColorGamut(colorGamut);
    // It is necessary to judge whether colorGamuts[index] and colorGamut are equal.
    screen->SetScreenGamutMap(static_cast<ScreenGamutMap>(gamutMap));
    ScreenGamutMap screenGamutMap;
    screen->GetScreenGamutMap(screenGamutMap);
    // It is necessary to judge whether gamutMap and screenGamutMap are equal.
    return true;
}

void DoMyFuzzTest(const uint8_t* data, size_t size)
{
    DisplayManager& displayManager = DisplayManager::GetInstance();
    sptr<Display> display = displayManager.GetDefaultDisplay();

    sptr<Screen> screen = nullptr;
    if (display != nullptr) {
        ScreenId screenId = display->GetScreenId();
        screen = ScreenManager::GetInstance().GetScreenById(screenId);
    } else {
        sptr<ScreenInfo> info = CreateScreenInfo(data, size);
        screen = new (std::nothrow) Screen(info);
    }
    if (display == nullptr) {
        ScreenFuzzTestNoDisplay(screen, data, size);
    } else {
        ScreenFuzzTest(screen, display, data, size);
    }
    ColorGamutsFuzzTest(screen, data, size);
}

} // namespace.OHOS::Rosen

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::DoMyFuzzTest(data, size);
    return 0;
}


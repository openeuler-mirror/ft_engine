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

#ifndef FOUNDATION_DM_SCREEN_H
#define FOUNDATION_DM_SCREEN_H

#include <screen_manager/screen_types.h>
#include <string>
#include <surface.h>
#include <vector>

#include "dm_common.h"
#include "noncopyable.h"

namespace OHOS::Rosen {
class ScreenInfo;

struct Point {
    int32_t posX_;
    int32_t posY_;
    Point() : posX_(0), posY_(0) {};
    Point(int32_t posX, int32_t posY) : posX_(posX), posY_(posY) {};
};

struct SupportedScreenModes : public RefBase {
    uint32_t width_;
    uint32_t height_;
    uint32_t refreshRate_;
};

struct VirtualScreenOption {
    std::string name_;
    uint32_t width_;
    uint32_t height_;
    float density_;
    sptr<Surface> surface_;
    int32_t flags_;
    bool isForShot_ {true};
};

struct ExpandOption {
    ScreenId screenId_;
    uint32_t startX_;
    uint32_t startY_;
};

class Screen : public RefBase {
friend class ScreenManager;
public:
    ~Screen();
    Screen(const Screen&) = delete;
    Screen(Screen&&) = delete;
    Screen& operator=(const Screen&) = delete;
    Screen& operator=(Screen&&) = delete;
    bool IsGroup() const;
    std::string GetName() const;
    ScreenId GetId() const;
    uint32_t GetWidth() const;
    uint32_t GetHeight() const;
    uint32_t GetVirtualWidth() const;
    uint32_t GetVirtualHeight() const;
    float GetVirtualPixelRatio() const;
    Rotation GetRotation() const;
    Orientation GetOrientation() const;
    bool IsReal() const;
    ScreenId GetParentId() const;
    uint32_t GetModeId() const;
    std::vector<sptr<SupportedScreenModes>> GetSupportedModes() const;
    bool SetScreenActiveMode(uint32_t modeId);
    bool SetOrientation(Orientation orientation) const;
    bool SetDensityDpi(uint32_t dpi) const;
    sptr<ScreenInfo> GetScreenInfo() const;

    // colorspace, gamut
    DMError GetScreenSupportedColorGamuts(std::vector<ScreenColorGamut>& colorGamuts) const;
    DMError GetScreenColorGamut(ScreenColorGamut& colorGamut) const;
    DMError SetScreenColorGamut(int32_t colorGamutIdx);
    DMError GetScreenGamutMap(ScreenGamutMap& gamutMap) const;
    DMError SetScreenGamutMap(ScreenGamutMap gamutMap);
    DMError SetScreenColorTransform();
protected:
    // No more methods or variables can be defined here.
    Screen(sptr<ScreenInfo> info);
    void UpdateScreenInfo() const;
    void UpdateScreenInfo(sptr<ScreenInfo> info) const;
private:
    // No more methods or variables can be defined here.
    class Impl;
    sptr<Impl> pImpl_;
};
} // namespace OHOS::Rosen

#endif // FOUNDATION_DM_SCREEN_H
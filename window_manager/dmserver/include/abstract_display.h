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

#ifndef FOUNDATION_DMSERVER_ABSTRACT_DISPLAY_H
#define FOUNDATION_DMSERVER_ABSTRACT_DISPLAY_H

#include <refbase.h>

#include "abstract_screen.h"
#include "display_info.h"

namespace OHOS::Rosen {
enum class FreezeFlag : uint32_t {
    FREEZING,
    UNFREEZING,
};

class AbstractDisplay : public RefBase {
public:
    constexpr static int32_t DEFAULT_WIDTH = 720;
    constexpr static int32_t DEFAULT_HIGHT = 1280;
    constexpr static float DEFAULT_VIRTUAL_PIXEL_RATIO = 1.0;
    constexpr static uint32_t DEFAULT_FRESH_RATE = 60;
    AbstractDisplay(DisplayId id, std::string name, sptr<SupportedScreenModes>& info, sptr<AbstractScreen>& absScreen);
    WM_DISALLOW_COPY_AND_MOVE(AbstractDisplay);
    ~AbstractDisplay() = default;
    static inline bool IsVertical(Rotation rotation)
    {
        return (rotation == Rotation::ROTATION_0 || rotation == Rotation::ROTATION_180);
    }
    DisplayId GetId() const;
    int32_t GetOffsetX() const;
    int32_t GetOffsetY() const;
    int32_t GetWidth() const;
    int32_t GetHeight() const;
    uint32_t GetRefreshRate() const;
    float GetVirtualPixelRatio() const;
    ScreenId GetAbstractScreenId() const;
    ScreenId GetAbstractScreenGroupId() const;
    bool BindAbstractScreen(sptr<AbstractScreen> abstractDisplay);
    sptr<DisplayInfo> ConvertToDisplayInfo() const;
    Rotation GetRotation() const;
    Orientation GetOrientation() const;
    FreezeFlag GetFreezeFlag() const;

    void SetId(DisplayId displayId);
    void SetOffsetX(int32_t offsetX);
    void SetOffsetY(int32_t offsetY);
    void SetWidth(int32_t width);
    void SetHeight(int32_t height);
    void SetOffset(int32_t offsetX, int32_t offsetY);
    void SetRefreshRate(uint32_t refreshRate);
    void SetVirtualPixelRatio(float virtualPixelRatio);
    void SetOrientation(Orientation orientation);
    bool RequestRotation(Rotation rotation);
    void SetFreezeFlag(FreezeFlag);
    DEFINE_VAR_DEFAULT_FUNC_GET_SET(bool, WaterfallDisplayCompressionStatus, waterfallDisplayCompressionStatus, false);
private:
    DisplayId id_ { DISPLAY_ID_INVALID };
    std::string name_ { "" };
    ScreenId screenId_ { SCREEN_ID_INVALID };
    ScreenId screenGroupId_ { SCREEN_ID_INVALID };
    int32_t offsetX_ { 0 };
    int32_t offsetY_ { 0 };
    int32_t width_ { 0 };
    int32_t height_ { 0 };
    uint32_t refreshRate_ { 0 };
    float virtualPixelRatio_ { 1.0 };
    Rotation rotation_ { Rotation::ROTATION_0 };
    Orientation orientation_ { Orientation::UNSPECIFIED };
    FreezeFlag freezeFlag_ { FreezeFlag::UNFREEZING };
    DEFINE_VAR_DEFAULT_FUNC_SET(DisplayState, DisplayState, displayState, DisplayState::UNKNOWN);
};
} // namespace OHOS::Rosen
#endif // FOUNDATION_DMSERVER_ABSTRACT_DISPLAY_H
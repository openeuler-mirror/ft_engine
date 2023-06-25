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

#ifndef FOUNDATION_DM_DISPLAY_H
#define FOUNDATION_DM_DISPLAY_H

#include <string>
#include <parcel.h>

#include "dm_common.h"
#include "class_var_definition.h"
#include "noncopyable.h"

namespace OHOS::Rosen {
class CutoutInfo;

enum class DisplayType : uint32_t {
    DEFAULT = 0,
};

class DisplayInfo : public Parcelable {
friend class AbstractDisplay;
public:
    DisplayInfo() = default;
    ~DisplayInfo() = default;
    WM_DISALLOW_COPY_AND_MOVE(DisplayInfo);

    virtual bool Marshalling(Parcel& parcel) const override;
    static DisplayInfo *Unmarshalling(Parcel& parcel);

    DEFINE_VAR_DEFAULT_FUNC_GET(std::string, Name, name, "");
    DEFINE_VAR_DEFAULT_FUNC_GET_SET(DisplayId, DisplayId, id, DISPLAY_ID_INVALID);
    DEFINE_VAR_DEFAULT_FUNC_GET_SET(DisplayType, DisplayType, type, DisplayType::DEFAULT);
    DEFINE_VAR_DEFAULT_FUNC_GET_SET(int32_t, Width, width, 0);
    DEFINE_VAR_DEFAULT_FUNC_GET_SET(int32_t, Height, height, 0);
    DEFINE_VAR_DEFAULT_FUNC_GET_SET(uint32_t, RefreshRate, refreshRate, 0);
    DEFINE_VAR_DEFAULT_FUNC_GET_SET(ScreenId, ScreenId, screenId, SCREEN_ID_INVALID);
    DEFINE_VAR_DEFAULT_FUNC_GET_SET(ScreenId, ScreenGroupId, screenGroupId, SCREEN_ID_INVALID);
    DEFINE_VAR_DEFAULT_FUNC_GET_SET(float, VirtualPixelRatio, virtualPixelRatio, 1.0f);
    DEFINE_VAR_DEFAULT_FUNC_GET_SET(float, XDpi, xDpi, 0.0f);
    DEFINE_VAR_DEFAULT_FUNC_GET_SET(float, YDpi, yDpi, 0.0f);
    DEFINE_VAR_DEFAULT_FUNC_GET_SET(Rotation, Rotation, rotation, Rotation::ROTATION_0);
    DEFINE_VAR_DEFAULT_FUNC_GET_SET(Orientation, Orientation, orientation, Orientation::UNSPECIFIED);
    DEFINE_VAR_DEFAULT_FUNC_GET_SET(int32_t, OffsetX, offsetX, 0);
    DEFINE_VAR_DEFAULT_FUNC_GET_SET(int32_t, OffsetY, offsetY, 0);
    DEFINE_VAR_DEFAULT_FUNC_GET(DisplayState, DisplayState, displayState, DisplayState::UNKNOWN);
    DEFINE_VAR_DEFAULT_FUNC_GET_SET(bool, WaterfallDisplayCompressionStatus, waterfallDisplayCompressionStatus, false);
};

class Display : public RefBase {
friend class DisplayManager;
public:
    ~Display();
    Display(const Display&) = delete;
    Display(Display&&) = delete;
    Display& operator=(const Display&) = delete;
    Display& operator=(Display&&) = delete;
    DisplayId GetId() const;
    std::string GetName() const;
    int32_t GetWidth() const;
    int32_t GetHeight() const;
    uint32_t GetRefreshRate() const;
    ScreenId GetScreenId() const;
    float GetVirtualPixelRatio() const;
    int GetDpi() const;
    Rotation GetRotation() const;
    Orientation GetOrientation() const;
    sptr<DisplayInfo> GetDisplayInfo() const;
    sptr<CutoutInfo> GetCutoutInfo() const;
protected:
    // No more methods or variables can be defined here.
    Display(const std::string& name, sptr<DisplayInfo> info);
private:
    // No more methods or variables can be defined here.
    void UpdateDisplayInfo(sptr<DisplayInfo>) const;
    void UpdateDisplayInfo() const;
    class Impl;
    sptr<Impl> pImpl_;
};
} // namespace OHOS::Rosen

#endif // FOUNDATION_DM_DISPLAY_H
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
#include "dm_common.h"
#include "noncopyable.h"

namespace OHOS::Rosen {
class DisplayInfo;
class CutoutInfo;

enum class DisplayType : uint32_t {
    DEFAULT = 0,
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
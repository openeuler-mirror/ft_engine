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

#ifndef OHOS_ROSEN_WINDOW_OPTION_H
#define OHOS_ROSEN_WINDOW_OPTION_H
#include <refbase.h>
#include <string>
#include <unordered_map>

#include "../dm/dm_common.h"
#include "wm_common.h"

namespace OHOS {
namespace Rosen {
class WindowOption : public RefBase {
public:
    WindowOption();
    virtual ~WindowOption() = default;

    void SetWindowRect(const struct Rect& rect);
    void SetWindowType(WindowType type);
    void SetWindowMode(WindowMode mode);
    void SetFocusable(bool isFocusable);
    void SetTouchable(bool isTouchable);
    void SetDisplayId(DisplayId displayId);
    void SetParentId(uint32_t parentId);
    void SetWindowName(const std::string& windowName);
    void AddWindowFlag(WindowFlag flag);
    void RemoveWindowFlag(WindowFlag flag);
    void SetWindowFlags(uint32_t flags);
    void SetSystemBarProperty(WindowType type, const SystemBarProperty& property);
    void SetHitOffset(int32_t x, int32_t y);
    void SetWindowTag(WindowTag windowTag);
    void SetKeepScreenOn(bool keepScreenOn);
    bool IsKeepScreenOn() const;
    void SetTurnScreenOn(bool turnScreenOn);
    bool IsTurnScreenOn() const;
    void SetBrightness(float brightness);
    void SetRequestedOrientation(Orientation orientation);
    void SetCallingWindow(uint32_t windowId);
    void SetMainHandlerAvailable(bool isMainHandlerAvailable);
    void SetDragHotZoneNone(bool hotZoneNone);
    void SetWindowLimitSize(uint32_t minWidth, uint32_t minHeight, uint32_t maxWidth, uint32_t maxHeight);

    Rect GetWindowRect() const;
    WindowType GetWindowType() const;
    WindowMode GetWindowMode() const;
    bool GetFocusable() const;
    bool GetTouchable() const;
    DisplayId GetDisplayId() const;
    uint32_t GetParentId() const;
    const std::string& GetWindowName() const;
    uint32_t GetWindowFlags() const;
    const std::unordered_map<WindowType, SystemBarProperty>& GetSystemBarProperty() const;
    const PointInfo& GetHitOffset() const;
    WindowTag GetWindowTag() const;
    float GetBrightness() const;
    Orientation GetRequestedOrientation() const;
    uint32_t GetCallingWindow() const;
    bool GetMainHandlerAvailable() const;
    bool GetDragHotZoneNone() const;
    uint32_t GetWinMinWidth() const;
    uint32_t GetWinMinHeight() const;
    uint32_t GetWinMaxWidth() const;
    uint32_t GetWinMaxHeight() const;
    bool LimitSizeUpdated() const;

private:
    Rect windowRect_ { 0, 0, 0, 0 };
    WindowType type_ { WindowType::WINDOW_TYPE_APP_MAIN_WINDOW };
    WindowMode mode_ { WindowMode::WINDOW_MODE_UNDEFINED };
    bool focusable_ { true };
    bool touchable_ { true };
    DisplayId displayId_ { 0 };
    uint32_t parentId_ = INVALID_WINDOW_ID;
    std::string windowName_ { "" };
    uint32_t flags_ { 0 };
    PointInfo hitOffset_ { 0, 0 };
    WindowTag windowTag_;
    bool keepScreenOn_ = false;
    bool turnScreenOn_ = false;
    bool isMainHandlerAvailable_ = true;
    float brightness_ = UNDEFINED_BRIGHTNESS;
    uint32_t callingWindow_ = INVALID_WINDOW_ID;
    std::unordered_map<WindowType, SystemBarProperty> sysBarPropMap_ {
        { WindowType::WINDOW_TYPE_STATUS_BAR,     SystemBarProperty() },
        { WindowType::WINDOW_TYPE_NAVIGATION_BAR, SystemBarProperty() },
    };
    Orientation requestedOrientation_ { Orientation::UNSPECIFIED };
    bool dragHotZoneNone_ = false;
    bool limitSizeUpdated_ = false;
    uint32_t winMinWidth_ = 320;
    uint32_t winMinHeight_ = 240;
    uint32_t winMaxWidth_ = 1920;
    uint32_t winMaxHeight_ = 1920;
};
} // namespace Rosen
} // namespace OHOS
#endif // OHOS_ROSEN_WINDOW_OPTION_H

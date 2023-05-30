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

#ifndef FRAMEWORKS_WM_TEST_ST_WINDOW_TEST_UTILS_H
#define FRAMEWORKS_WM_TEST_ST_WINDOW_TEST_UTILS_H

#include "pointer_event.h"
#include "display_manager.h"
#include "window.h"
#include "window_layout_policy.h"
#include "window_option.h"
#include "window_scene.h"
#include "wm_common.h"

namespace OHOS {
namespace Rosen {
struct SplitRects {
    Rect primaryRect;
    Rect secondaryRect;
    Rect dividerRect;
};

class WindowTestUtils {
public:
    struct TestWindowInfo {
        std::string name;
        Rect rect;
        WindowType type;
        WindowMode mode;
        bool needAvoid;
        bool parentLimit;
        bool forbidSplitMove {false};
        bool showWhenLocked;
        uint32_t parentId;
        bool focusable_ { true };
        Orientation orientation_ { Orientation::UNSPECIFIED };
    };
    static Rect displayRect_;
    static Rect limitDisplayRect_;
    static Rect statusBarRect_;
    static Rect naviBarRect_;
    static Rect customAppRect_;
    static Rect customAppDecoRect_;
    static Rect dockWindowRect_;
    static SplitRects splitRects_;
    static bool isVerticalDisplay_;
    static Rect singleTileRect_;
    static std::vector<Rect> doubleTileRects_;
    static std::vector<Rect> tripleTileRects_;
    static AvoidArea systemAvoidArea_;

    static void InitByDisplayRect(const Rect& displayRect);
    static sptr<Window> CreateTestWindow(const TestWindowInfo& info);
    static sptr<Window> CreateStatusBarWindow();
    static sptr<Window> CreateNavigationBarWindow();
    static sptr<Window> CreateDockWindow();
    static sptr<WindowScene> CreateWindowScene();
    static bool RectEqualTo(const sptr<Window>& window, const Rect& r);
    static bool InitSplitRects();
    static void UpdateSplitRects(const sptr<Window>& window);
    static bool RectEqualToRect(const Rect& l, const Rect& r);
    static Rect GetDefaultFloatingRect(const sptr<Window>& window);
    static Rect CalcLimitedRect(const Rect& rect, float virtualPixelRatio);
    static Rect GetDecorateRect(const Rect& rect, float virtualPixelRatio);
    static Rect GetFloatingLimitedRect(const Rect& rect, float virtualPixelRatio);
    static void InitTileWindowRects(const sptr<Window>& window);
    static float GetVirtualPixelRatio(DisplayId displayId);
    static std::shared_ptr<MMI::PointerEvent> CreatePointerEvent(int32_t posX, int32_t posY, uint32_t pointerId,
        int32_t pointerAction);
    static uint32_t GetMaxTileWinNum();

private:
    void UpdateLimitDisplayRect(const Rect& avoidRect);
    static void UpdateLimitSplitRects(int32_t divPos);
    static void UpdateLimitSplitRect(Rect& limitSplitRect);
    static AvoidPosType GetAvoidPosType(const Rect& rect);
    AvoidArea avoidArea_;
};
} // namespace ROSEN
} // namespace OHOS
#endif // FRAMEWORKS_WM_TEST_ST_WINDOW_TEST_UTILS_H

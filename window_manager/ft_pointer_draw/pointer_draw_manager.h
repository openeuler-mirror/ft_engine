/*
 * Copyright (c) 2023 Huawei Technologies Co., Ltd.
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

#ifndef POINTER_DRAW_H
#define POINTER_DRAW_H

#include "i_pointer_drawing_manager.h"
#include "hilog/log.h"
#include "nocopyable.h"
#include "ui/rs_display_node.h"
#include "ui/rs_surface_node.h"
#include "pixel_map.h"
#include "event_handler.h"
#ifdef ENABLE_GPU
#include "render_context/render_context.h"
#endif

static constexpr unsigned int HILOG_DOMAIN_WINDOW = 0xD004200;

#define _W_DFUNC HiviewDFX::HiLog::Debug
#define _W_IFUNC HiviewDFX::HiLog::Info
#define _W_WFUNC HiviewDFX::HiLog::Warn
#define _W_EFUNC HiviewDFX::HiLog::Error

#define _W_CPRINTF(func, fmt, ...) func(LABEL, "<%{public}d>" fmt, __LINE__, ##__VA_ARGS__)

#define WLOGD(fmt, ...) _W_CPRINTF(_W_DFUNC, fmt, ##__VA_ARGS__)
#define WLOGI(fmt, ...) _W_CPRINTF(_W_IFUNC, fmt, ##__VA_ARGS__)
#define WLOGW(fmt, ...) _W_CPRINTF(_W_WFUNC, fmt, ##__VA_ARGS__)
#define WLOGE(fmt, ...) _W_CPRINTF(_W_EFUNC, fmt, ##__VA_ARGS__)

#define _W_FUNC __func__

#define WLOGFD(fmt, ...) WLOGD("%{public}s: " fmt, _W_FUNC, ##__VA_ARGS__)
#define WLOGFI(fmt, ...) WLOGI("%{public}s: " fmt, _W_FUNC, ##__VA_ARGS__)
#define WLOGFW(fmt, ...) WLOGW("%{public}s: " fmt, _W_FUNC, ##__VA_ARGS__)
#define WLOGFE(fmt, ...) WLOGE("%{public}s: " fmt, _W_FUNC, ##__VA_ARGS__)

enum MOUSE_ICON {
    DEFAULT = 0,
    EAST = 1,
    WEST = 2,
    SOUTH = 3,
    NORTH = 4,
    WEST_EAST = 5,
    NORTH_SOUTH = 6,
    NORTH_EAST = 7,
    NORTH_WEST = 8,
    SOUTH_EAST = 9,
    SOUTH_WEST = 10,
    NORTH_EAST_SOUTH_WEST = 11,
    NORTH_WEST_SOUTH_EAST = 12,
    CROSS = 13,
    CURSOR_COPY = 14,
    CURSOR_FORBID = 15,
    COLOR_SUCKER = 16,
    HAND_GRABBING = 17,
    HAND_OPEN = 18,
    HAND_POINTING = 19,
    HELP = 20,
    CURSOR_MOVE = 21,
    RESIZE_LEFT_RIGHT = 22,
    RESIZE_UP_DOWN = 23,
    SCREENSHOT_CHOOSE = 24,
    SCREENSHOT_CURSOR = 25,
    TEXT_CURSOR = 26,
    ZOOM_IN = 27,
    ZOOM_OUT = 28,
    MIDDLE_BTN_EAST = 29,
    MIDDLE_BTN_WEST = 30,
    MIDDLE_BTN_SOUTH = 31,
    MIDDLE_BTN_NORTH = 32,
    MIDDLE_BTN_NORTH_SOUTH = 33,
    MIDDLE_BTN_NORTH_EAST = 34,
    MIDDLE_BTN_NORTH_WEST = 35,
    MIDDLE_BTN_SOUTH_EAST = 36,
    MIDDLE_BTN_SOUTH_WEST = 37,
    MIDDLE_BTN_NORTH_SOUTH_WEST_EAST = 38,
};

enum ICON_TYPE {
    ANGLE_E = 0,
    ANGLE_S = 1,
    ANGLE_W = 2,
    ANGLE_N = 3,
    ANGLE_SE = 4,
    ANGLE_NE = 5,
    ANGLE_SW = 6,
    ANGLE_NW = 7,
    ANGLE_CENTER = 8,
};

class PointerDrawingManager : public IPointerDrawingManager {
public:
    DISALLOW_COPY_AND_MOVE(PointerDrawingManager);
    PointerDrawingManager() = default;
    ~PointerDrawingManager() = default;

    bool Init() override;
    void DrawPointer(int32_t displayId, int32_t physicalX, int32_t physicalY, int32_t mouseStyle) override;
    void UpdateDisplayInfo(const ScreenDisplayInfo& displayInfo) override;
    void SetPointerLocation(int32_t pid, int32_t x, int32_t y) override;
    void OnDisplayInfo(const DisplayGroupInfo& displayGroupInfo) override {}
    void OnWindowInfo(int32_t windowPid, int32_t windowId) override {}
    void DeletePointerVisible(int32_t pid) override {}
    void SetMouseDisplayState(bool state) override {}
    void DrawPointerStyle() override {}
    int32_t SetPointerVisible(int32_t pid, bool visible) override
    {
        return OHOS::WMError::WM_OK;
    }
    int32_t SetPointerStyle(int32_t pid, int32_t windowId, int32_t pointerStyle) override
    {
        return OHOS::WMError::WM_OK;
    }
    int32_t GetPointerStyle(int32_t pid, int32_t windowId, int32_t &pointerStyle) override
    {
        return OHOS::WMError::WM_OK;
    }
    bool IsPointerVisible() override
    {
        return true;
    }
    bool GetMouseDisplayState() const override
    {
        return true;
    }

private:
    using Task = std::function<void()>;
    struct IconStyle {
        ICON_TYPE type;
        std::string iconPath;
    };

    OHOS::WMError InitLayerNode(int32_t x, int32_t y);
    OHOS::WMError InitIconPixel();
    OHOS::WMError CheckPixelFile(const std::string &filePath);
    OHOS::WMError MoveTo(int32_t x, int32_t y);
    std::unique_ptr<OHOS::Media::PixelMap> DecodeImageToPixelMap(const std::string &imagePath);
    SkColorType PixelFormatConvert(const OHOS::Media::PixelFormat& pixelFormat);

    bool isDrawing_ = false;
    int32_t displayId_ = 0;
    std::map<MOUSE_ICON, IconStyle> mouseIcons_;
    OHOS::Rosen::RSSurfaceNode::SharedPtr surfaceNode_ = nullptr;
    std::shared_ptr<OHOS::Rosen::RSSurface> rsSurface_ = nullptr;
    std::shared_ptr<OHOS::AppExecFwk::EventRunner> runner_ = nullptr;
    std::shared_ptr<OHOS::AppExecFwk::EventHandler> handler_ = nullptr;
#ifdef ENABLE_GPU
    std::unique_ptr<OHOS::Rosen::RenderContext> renderContext_ = nullptr;
#endif
};
#endif // POINTER_DRAW_H

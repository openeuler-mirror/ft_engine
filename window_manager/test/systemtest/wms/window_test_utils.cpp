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

#include "window_test_utils.h"
#include <ability_context.h>
#include "window_helper.h"
#include "wm_common_inner.h"
#include "wm_common.h"
namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "WindowTestUtils"};
    constexpr uint32_t EDGE_INTERVAL = 48;
    constexpr uint32_t MID_INTERVAL = 24;
}

Rect WindowTestUtils::displayRect_        = {0, 0, 0, 0};
Rect WindowTestUtils::statusBarRect_      = {0, 0, 0, 0};
Rect WindowTestUtils::naviBarRect_        = {0, 0, 0, 0};
Rect WindowTestUtils::customAppRect_      = {0, 0, 0, 0};
Rect WindowTestUtils::limitDisplayRect_   = {0, 0, 0, 0};
Rect WindowTestUtils::dockWindowRect_     = {0, 0, 0, 0};
SplitRects WindowTestUtils::splitRects_   = {
    .primaryRect   = {0, 0, 0, 0},
    .secondaryRect = {0, 0, 0, 0},
    .dividerRect   = {0, 0, 0, 0},
};
Rect WindowTestUtils::singleTileRect_     = {0, 0, 0, 0};
std::vector<Rect> WindowTestUtils::doubleTileRects_ = std::vector<Rect>(2);
std::vector<Rect> WindowTestUtils::tripleTileRects_ = std::vector<Rect>(3);
AvoidArea WindowTestUtils::systemAvoidArea_ = {};

bool WindowTestUtils::isVerticalDisplay_ = false;

sptr<Window> WindowTestUtils::CreateTestWindow(const TestWindowInfo& info)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowRect(info.rect);
    option->SetWindowType(info.type);
    option->SetWindowMode(info.mode);
    option->SetFocusable(info.focusable_);
    option->SetRequestedOrientation(info.orientation_);
    if (info.parentId != INVALID_WINDOW_ID) {
        option->SetParentId(info.parentId);
    }
    if (info.needAvoid) {
        option->AddWindowFlag(WindowFlag::WINDOW_FLAG_NEED_AVOID);
    } else {
        option->RemoveWindowFlag(WindowFlag::WINDOW_FLAG_NEED_AVOID);
    }
    if (info.parentLimit) {
        option->AddWindowFlag(WindowFlag::WINDOW_FLAG_PARENT_LIMIT);
    } else {
        option->RemoveWindowFlag(WindowFlag::WINDOW_FLAG_PARENT_LIMIT);
    }
    if (info.forbidSplitMove) {
        option->AddWindowFlag(WindowFlag::WINDOW_FLAG_FORBID_SPLIT_MOVE);
    } else {
        option->RemoveWindowFlag(WindowFlag::WINDOW_FLAG_FORBID_SPLIT_MOVE);
    }
    if (info.showWhenLocked) {
        option->AddWindowFlag(WindowFlag::WINDOW_FLAG_SHOW_WHEN_LOCKED);
    } else {
        option->RemoveWindowFlag(WindowFlag::WINDOW_FLAG_SHOW_WHEN_LOCKED);
    }
    sptr<Window> window = Window::Create(info.name, option);
    return window;
}

sptr<Window> WindowTestUtils::CreateDockWindow()
{
    TestWindowInfo info = {
        .name = "dockWindow",
        .rect = dockWindowRect_,
        .type = WindowType::WINDOW_TYPE_LAUNCHER_DOCK,
        .mode = WindowMode::WINDOW_MODE_FLOATING,
        .needAvoid = false,
        .parentLimit = false,
        .parentId = INVALID_WINDOW_ID,
    };
    return CreateTestWindow(info);
}

sptr<Window> WindowTestUtils::CreateStatusBarWindow()
{
    TestWindowInfo info = {
        .name = "statusBar",
        .rect = statusBarRect_,
        .type = WindowType::WINDOW_TYPE_STATUS_BAR,
        .mode = WindowMode::WINDOW_MODE_FLOATING,
        .needAvoid = false,
        .parentLimit = false,
        .parentId = INVALID_WINDOW_ID,
    };
    return CreateTestWindow(info);
}

sptr<Window> WindowTestUtils::CreateNavigationBarWindow()
{
    TestWindowInfo info = {
        .name = "naviBar",
        .rect = naviBarRect_,
        .type = WindowType::WINDOW_TYPE_NAVIGATION_BAR,
        .mode = WindowMode::WINDOW_MODE_FLOATING,
        .needAvoid = false,
        .parentLimit = false,
        .parentId = INVALID_WINDOW_ID,
    };
    return CreateTestWindow(info);
}

sptr<WindowScene> WindowTestUtils::CreateWindowScene()
{
    sptr<IWindowLifeCycle> listener = nullptr;
    std::shared_ptr<AbilityRuntime::AbilityContext> abilityContext = nullptr;

    sptr<WindowScene> scene = new WindowScene();
    scene->Init(0, abilityContext, listener);
    return scene;
}

Rect WindowTestUtils::GetDefaultFloatingRect(const sptr<Window>& window)
{
    limitDisplayRect_ = displayRect_;
    UpdateSplitRects(window);
    constexpr uint32_t half = 2;
    constexpr float ratio = DEFAULT_ASPECT_RATIO;  // 0.67: default height/width ratio
    float vpr = GetVirtualPixelRatio(0);

    /*
     * Calculate default width and height, if width or height is
     * smaller than minWidth or minHeight, use the minimum limits
     */
    uint32_t defaultW = std::max(static_cast<uint32_t>(displayRect_.width_ * ratio),
                                 static_cast<uint32_t>(MIN_FLOATING_WIDTH * vpr));
    uint32_t defaultH = std::max(static_cast<uint32_t>(displayRect_.height_ * ratio),
                                 static_cast<uint32_t>(MIN_FLOATING_HEIGHT * vpr));
    // calculate default x and y
    Rect resRect = {0, 0, defaultW, defaultH};
    if (defaultW <= limitDisplayRect_.width_ && defaultH <= limitDisplayRect_.height_) {
        int32_t centerPosX = limitDisplayRect_.posX_ + static_cast<int32_t>(limitDisplayRect_.width_ / half);
        resRect.posX_ = centerPosX - static_cast<int32_t>(defaultW / half);

        int32_t centerPosY = limitDisplayRect_.posY_ + static_cast<int32_t>(limitDisplayRect_.height_ / half);
        resRect.posY_ = centerPosY - static_cast<int32_t>(defaultH / half);
    }

    return resRect;
}

Rect WindowTestUtils::CalcLimitedRect(const Rect& rect, float virtualPixelRatio)
{
    constexpr uint32_t maxLimitLen = 2560;
    constexpr int32_t maxPosRemain = 48;
    uint32_t minFloatingW = static_cast<uint32_t>(MIN_FLOATING_WIDTH * virtualPixelRatio);
    uint32_t minFloatingH = static_cast<uint32_t>(MIN_FLOATING_HEIGHT * virtualPixelRatio);
    Rect resRect = {
        std::min(std::max(rect.posX_, maxPosRemain - static_cast<int32_t>(rect.width_)),
            static_cast<int32_t>(displayRect_.width_) - maxPosRemain),
        std::min(std::max(rect.posY_, maxPosRemain), static_cast<int32_t>(displayRect_.height_) - maxPosRemain),
        std::min(std::max(minFloatingW, rect.width_), maxLimitLen),
        std::min(std::max(minFloatingH, rect.height_), maxLimitLen),
    };
    return resRect;
}

Rect WindowTestUtils::GetFloatingLimitedRect(const Rect& rect, float virtualPixelRatio)
{
    uint32_t minFloatingW = static_cast<uint32_t>(MIN_FLOATING_WIDTH * virtualPixelRatio);
    uint32_t minFloatingH = static_cast<uint32_t>(MIN_FLOATING_HEIGHT * virtualPixelRatio);
    Rect resRect = {
        rect.posX_,
        rect.posY_,
        std::max(minFloatingW, rect.width_),
        std::max(minFloatingH, rect.height_),
    };
    return resRect;
}

Rect WindowTestUtils::GetDecorateRect(const Rect& rect, float virtualPixelRatio)
{
    uint32_t winFrameW = static_cast<uint32_t>(WINDOW_FRAME_WIDTH * virtualPixelRatio);
    uint32_t winTitleBarH = static_cast<uint32_t>(WINDOW_TITLE_BAR_HEIGHT * virtualPixelRatio);

    Rect resRect;
    resRect.posX_ = rect.posX_;
    resRect.posY_ = rect.posY_;
    resRect.width_ = rect.width_ + winFrameW + winFrameW;
    resRect.height_ = rect.height_ + winTitleBarH + winFrameW;
    return resRect;
}

void WindowTestUtils::InitByDisplayRect(const Rect& displayRect)
{
    const float barRatio = 0.07;
    const float spaceRation = 0.125;
    displayRect_ = displayRect;
    limitDisplayRect_ = displayRect;
    if (displayRect_.width_ < displayRect_.height_) {
        isVerticalDisplay_ = true;
    }
    statusBarRect_ = {0, 0, displayRect_.width_, displayRect_.height_ * barRatio};
    naviBarRect_ = {0, displayRect_.height_ * (1 - barRatio), displayRect_.width_, displayRect_.height_ * barRatio};
    dockWindowRect_ = {0, displayRect_.height_ * (1 - barRatio), displayRect_.width_, displayRect_.height_ * barRatio};
    customAppRect_ = {
        displayRect_.width_ * spaceRation,
        displayRect_.height_ * spaceRation,
        displayRect_.width_ * DEFAULT_ASPECT_RATIO,
        displayRect_.height_ * DEFAULT_ASPECT_RATIO
    };
}

std::shared_ptr<MMI::PointerEvent> WindowTestUtils::CreatePointerEvent(int32_t posX, int32_t posY, uint32_t pointerId,
    int32_t pointerAction)
{
    MMI::PointerEvent::PointerItem pointerItem;
    pointerItem.SetPointerId(pointerId);
    pointerItem.SetDisplayX(posX);
    pointerItem.SetDisplayY(posY);

    std::shared_ptr<MMI::PointerEvent> pointerEvent = MMI::PointerEvent::Create();
    pointerEvent->AddPointerItem(pointerItem);
    pointerEvent->SetPointerId(pointerId);
    pointerEvent->SetPointerAction(pointerAction);
    pointerEvent->SetSourceType(MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN);
    return pointerEvent;
}

uint32_t WindowTestUtils::GetMaxTileWinNum()
{
    float virtualPixelRatio = GetVirtualPixelRatio(0);
    constexpr uint32_t half = 2;
    uint32_t edgeIntervalVp = static_cast<uint32_t>(EDGE_INTERVAL * half * virtualPixelRatio);
    uint32_t midIntervalVp = static_cast<uint32_t>(MID_INTERVAL * virtualPixelRatio);
    uint32_t minFloatingW = static_cast<uint32_t>(MIN_FLOATING_WIDTH * virtualPixelRatio);
    uint32_t drawableW = limitDisplayRect_.width_ - edgeIntervalVp + midIntervalVp;
    uint32_t maxNum = static_cast<uint32_t>(drawableW / (minFloatingW + midIntervalVp));
    WLOGFI("maxNum: %{public}d", maxNum);
    return maxNum;
}

void WindowTestUtils::InitTileWindowRects(const sptr<Window>& window)
{
    float virtualPixelRatio = GetVirtualPixelRatio(0);
    uint32_t edgeInterval = static_cast<uint32_t>(EDGE_INTERVAL * virtualPixelRatio); // 48 is edge interval
    uint32_t midInterval = static_cast<uint32_t>(MID_INTERVAL * virtualPixelRatio); // 24 is mid interval
    constexpr float ratio = DEFAULT_ASPECT_RATIO;
    constexpr int half = 2;
    limitDisplayRect_ = displayRect_;
    UpdateSplitRects(window);

    uint32_t minFloatingW = static_cast<uint32_t>(MIN_FLOATING_WIDTH * virtualPixelRatio);
    uint32_t minFloatingH = static_cast<uint32_t>(MIN_FLOATING_HEIGHT * virtualPixelRatio);
    uint32_t w = std::max(static_cast<uint32_t>(displayRect_.width_ * ratio), minFloatingW);
    uint32_t h = std::max(static_cast<uint32_t>(displayRect_.height_ * ratio), minFloatingH);
    w = w > limitDisplayRect_.width_ ? limitDisplayRect_.width_ : w;
    h = h > limitDisplayRect_.height_ ? limitDisplayRect_.height_ : h;
    int x = limitDisplayRect_.posX_ + ((limitDisplayRect_.width_ - w) / half);
    int y = limitDisplayRect_.posY_ + ((limitDisplayRect_.height_ - h) / half);
    singleTileRect_ = { x, y, w, h };
    WLOGFI("singleRect_: %{public}d %{public}d %{public}d %{public}d", x, y, w, h);
    x = edgeInterval;
    w = (limitDisplayRect_.width_ - edgeInterval * half - midInterval) / half;
    // calc doubleRect
    doubleTileRects_[0] = {x, y, w, h};
    doubleTileRects_[1] = {x + w + midInterval, y, w, h};
    WLOGFI("doubleRects_: %{public}d %{public}d %{public}d %{public}d", x, y, w, h);
    // calc tripleRect
    w = (limitDisplayRect_.width_ - edgeInterval * half - midInterval * half) / 3; // 3 is triple rects num
    tripleTileRects_[0] = {x, y, w, h};
    tripleTileRects_[1] = {x + w + midInterval, y, w, h};
    tripleTileRects_[2] = {x + w * half + midInterval * half, y, w, h}; // 2 is third index
    WLOGFI("tripleRects_: %{public}d %{public}d %{public}d %{public}d", x, y, w, h);
}

bool WindowTestUtils::RectEqualTo(const sptr<Window>& window, const Rect& r)
{
    usleep(100000); // 100000us
    Rect l = window->GetRect();
    bool res = ((l.posX_ == r.posX_) && (l.posY_ == r.posY_) && (l.width_ == r.width_) && (l.height_ == r.height_));
    if (!res) {
        WLOGFE("GetLayoutRect: %{public}d %{public}d %{public}d %{public}d, " \
            "Expect: %{public}d %{public}d %{public}d %{public}d", l.posX_, l.posY_, l.width_, l.height_,
            r.posX_, r.posY_, r.width_, r.height_);
    }
    return res;
}

bool WindowTestUtils::RectEqualToRect(const Rect& l, const Rect& r)
{
    bool res = ((l.posX_ == r.posX_) && (l.posY_ == r.posY_) && (l.width_ == r.width_) && (l.height_ == r.height_));
    if (!res) {
        WLOGFE("GetLayoutRect: %{public}d %{public}d %{public}d %{public}d, " \
            "Expect: %{public}d %{public}d %{public}d %{public}d", l.posX_, l.posY_, l.width_, l.height_,
            r.posX_, r.posY_, r.width_, r.height_);
    }
    return res;
}

AvoidPosType WindowTestUtils::GetAvoidPosType(const Rect& rect)
{
    auto display = DisplayManager::GetInstance().GetDisplayById(0);
    if (display == nullptr) {
        WLOGFE("GetAvoidPosType fail. Get display fail. displayId: 0");
        return AvoidPosType::AVOID_POS_UNKNOWN;
    }
    auto displayInfo = display->GetDisplayInfo();
    Rect displayRect = {displayInfo->GetOffsetX(), displayInfo->GetOffsetY(), displayInfo->GetWidth(),
        displayInfo->GetHeight()};
    return WindowHelper::GetAvoidPosType(rect, displayRect);
}

bool WindowTestUtils::InitSplitRects()
{
    auto display = DisplayManager::GetInstance().GetDisplayById(0);
    if (display == nullptr) {
        WLOGFE("GetDefaultDisplay: failed!");
        return false;
    }
    WLOGFI("GetDefaultDisplay: id %{public}" PRIu64", w %{public}d, h %{public}d, fps %{public}u",
        display->GetId(), display->GetWidth(), display->GetHeight(), display->GetRefreshRate());

    Rect displayRect = {0, 0, display->GetWidth(), display->GetHeight()};
    displayRect_ = displayRect;
    limitDisplayRect_ = displayRect;

    float virtualPixelRatio = WindowTestUtils::GetVirtualPixelRatio(0);
    uint32_t dividerWidth = static_cast<uint32_t>(DIVIDER_WIDTH * virtualPixelRatio);

    if (displayRect_.width_ < displayRect_.height_) {
        isVerticalDisplay_ = true;
    }
    if (isVerticalDisplay_) {
        splitRects_.dividerRect = { 0,
                                    static_cast<uint32_t>((displayRect_.height_ - dividerWidth) * DEFAULT_SPLIT_RATIO),
                                    displayRect_.width_,
                                    dividerWidth, };
    } else {
        splitRects_.dividerRect = { static_cast<uint32_t>((displayRect_.width_ - dividerWidth) * DEFAULT_SPLIT_RATIO),
                                    0,
                                    dividerWidth,
                                    displayRect_.height_ };
    }
    return true;
}

void WindowTestUtils::UpdateSplitRects(const sptr<Window>& window)
{
    std::unique_ptr<WindowTestUtils> testUtils = std::make_unique<WindowTestUtils>();
    testUtils->avoidArea_ = systemAvoidArea_;
    testUtils->UpdateLimitDisplayRect(testUtils->avoidArea_.leftRect_);
    testUtils->UpdateLimitDisplayRect(testUtils->avoidArea_.topRect_);
    testUtils->UpdateLimitDisplayRect(testUtils->avoidArea_.rightRect_);
    testUtils->UpdateLimitDisplayRect(testUtils->avoidArea_.bottomRect_);

    if (isVerticalDisplay_) {
        splitRects_.dividerRect.posY_ = limitDisplayRect_.posY_ +
            static_cast<uint32_t>((limitDisplayRect_.height_ - splitRects_.dividerRect.height_) * DEFAULT_SPLIT_RATIO);
        testUtils->UpdateLimitSplitRects(splitRects_.dividerRect.posY_);
    } else {
        splitRects_.dividerRect.posX_ = limitDisplayRect_.posX_ +
            static_cast<uint32_t>((limitDisplayRect_.width_ - splitRects_.dividerRect.width_) * DEFAULT_SPLIT_RATIO);
        testUtils->UpdateLimitSplitRects(splitRects_.dividerRect.posX_);
    }
}

void WindowTestUtils::UpdateLimitDisplayRect(const Rect& avoidRect)
{
    if (((avoidRect.posX_ == 0) && (avoidRect.posY_ == 0) &&
        (avoidRect.width_ == 0) && (avoidRect.height_ == 0))) {
        return;
    }
    auto avoidPosType = GetAvoidPosType(avoidRect);
    int32_t offsetH = 0;
    int32_t offsetW = 0;
    switch (avoidPosType) {
        case AvoidPosType::AVOID_POS_TOP:
            offsetH = avoidRect.posY_ + avoidRect.height_ - limitDisplayRect_.posY_;
            limitDisplayRect_.posY_ += offsetH;
            limitDisplayRect_.height_ -= offsetH;
            break;
        case AvoidPosType::AVOID_POS_BOTTOM:
            offsetH = limitDisplayRect_.posY_ + limitDisplayRect_.height_ - avoidRect.posY_;
            limitDisplayRect_.height_ -= offsetH;
            break;
        case AvoidPosType::AVOID_POS_LEFT:
            offsetW = avoidRect.posX_ + avoidRect.width_ - limitDisplayRect_.posX_;
            limitDisplayRect_.posX_ += offsetW;
            limitDisplayRect_.width_ -= offsetW;
            break;
        case AvoidPosType::AVOID_POS_RIGHT:
            offsetW = limitDisplayRect_.posX_ + limitDisplayRect_.width_ - avoidRect.posX_;
            limitDisplayRect_.width_ -= offsetW;
            break;
        default:
            WLOGFE("invalid avoidPosType: %{public}d", avoidPosType);
    }
}

void WindowTestUtils::UpdateLimitSplitRects(int32_t divPos)
{
    std::unique_ptr<WindowTestUtils> testUtils = std::make_unique<WindowTestUtils>();
    if (isVerticalDisplay_) {
        splitRects_.dividerRect.posY_ = divPos;

        splitRects_.primaryRect.posX_ = displayRect_.posX_;
        splitRects_.primaryRect.posY_ = displayRect_.posY_;
        splitRects_.primaryRect.height_ = divPos;
        splitRects_.primaryRect.width_ = displayRect_.width_;

        splitRects_.secondaryRect.posX_ = displayRect_.posX_;
        splitRects_.secondaryRect.posY_ = splitRects_.dividerRect.posY_ + splitRects_.dividerRect.height_;
        splitRects_.secondaryRect.height_ = displayRect_.height_ - splitRects_.secondaryRect.posY_;
        splitRects_.secondaryRect.width_ = displayRect_.width_;
    } else {
        splitRects_.dividerRect.posX_ = divPos;

        splitRects_.primaryRect.posX_ = displayRect_.posX_;
        splitRects_.primaryRect.posY_ = displayRect_.posY_;
        splitRects_.primaryRect.width_ = divPos;
        splitRects_.primaryRect.height_ = displayRect_.height_;

        splitRects_.secondaryRect.posX_ = splitRects_.dividerRect.posX_ + splitRects_.dividerRect.width_;
        splitRects_.secondaryRect.posY_ = displayRect_.posY_;
        splitRects_.secondaryRect.width_ = displayRect_.width_ - splitRects_.secondaryRect.posX_;
        splitRects_.secondaryRect.height_ = displayRect_.height_;
    }

    testUtils->UpdateLimitSplitRect(splitRects_.primaryRect);
    testUtils->UpdateLimitSplitRect(splitRects_.secondaryRect);
}

void WindowTestUtils::UpdateLimitSplitRect(Rect& limitSplitRect)
{
    Rect curLimitRect = limitSplitRect;
    limitSplitRect.posX_ = std::max(limitDisplayRect_.posX_, curLimitRect.posX_);
    limitSplitRect.posY_ = std::max(limitDisplayRect_.posY_, curLimitRect.posY_);
    limitSplitRect.width_ = std::min(limitDisplayRect_.posX_ + limitDisplayRect_.width_,
                                     curLimitRect.posX_ + curLimitRect.width_) -
                                     limitSplitRect.posX_;
    limitSplitRect.height_ = std::min(limitDisplayRect_.posY_ + limitDisplayRect_.height_,
                                      curLimitRect.posY_ + curLimitRect.height_) -
                                      limitSplitRect.posY_;
}

float WindowTestUtils::GetVirtualPixelRatio(DisplayId displayId)
{
    auto display = DisplayManager::GetInstance().GetDisplayById(displayId);
    if (display == nullptr) {
        WLOGFE("GetVirtualPixel fail. Get display fail. displayId:%{public}" PRIu64", use Default vpr:1.0", displayId);
        return 1.0;  // Use DefaultVPR 1.0
    }

    float virtualPixelRatio = display->GetVirtualPixelRatio();
    if (virtualPixelRatio == 0.0) {
        WLOGFE("GetVirtualPixel fail. vpr is 0.0. displayId:%{public}" PRIu64", use Default vpr:1.0", displayId);
        return 1.0;  // Use DefaultVPR 1.0
    }

    WLOGFI("GetVirtualPixel success. displayId:%{public}" PRIu64", vpr:%{public}f", displayId, virtualPixelRatio);
    return virtualPixelRatio;
}
} // namespace ROSEN
} // namespace OHOS

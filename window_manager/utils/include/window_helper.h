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

#ifndef OHOS_WM_INCLUDE_WM_HELPER_H
#define OHOS_WM_INCLUDE_WM_HELPER_H

#include <unistd.h>
#include <vector>
#include "ability_info.h"
#include "window_transition_info.h"
#include "wm_common.h"
#include "wm_common_inner.h"
#include "wm_math.h"

namespace OHOS {
namespace Rosen {
namespace {
    const char DISABLE_WINDOW_ANIMATION_PATH[] = "/etc/disable_window_animation";
}
class WindowHelper {
public:
    static inline bool IsMainWindow(WindowType type)
    {
        return (type >= WindowType::APP_MAIN_WINDOW_BASE && type < WindowType::APP_MAIN_WINDOW_END);
    }

    static inline bool IsSubWindow(WindowType type)
    {
        return (type >= WindowType::APP_SUB_WINDOW_BASE && type < WindowType::APP_SUB_WINDOW_END);
    }

    static inline bool IsAppWindow(WindowType type)
    {
        return (IsMainWindow(type) || IsSubWindow(type));
    }

    static inline bool IsAppFloatingWindow(WindowType type)
    {
        return (type == WindowType::WINDOW_TYPE_FLOAT) || (type == WindowType::WINDOW_TYPE_FLOAT_CAMERA);
    }

    static inline bool IsBelowSystemWindow(WindowType type)
    {
        return (type >= WindowType::BELOW_APP_SYSTEM_WINDOW_BASE && type < WindowType::BELOW_APP_SYSTEM_WINDOW_END);
    }

    static inline bool IsAboveSystemWindow(WindowType type)
    {
        return (type >= WindowType::ABOVE_APP_SYSTEM_WINDOW_BASE && type < WindowType::ABOVE_APP_SYSTEM_WINDOW_END);
    }

    static inline bool IsSystemSubWindow(WindowType type)
    {
        return (type >= WindowType::SYSTEM_SUB_WINDOW_BASE && type < WindowType::SYSTEM_SUB_WINDOW_END);
    }

    static inline bool IsSystemWindow(WindowType type)
    {
        return (IsBelowSystemWindow(type) || IsAboveSystemWindow(type) || IsSystemSubWindow(type));
    }

    static inline bool IsMainFloatingWindow(WindowType type, WindowMode mode)
    {
        return ((IsMainWindow(type)) && (mode == WindowMode::WINDOW_MODE_FLOATING));
    }

    static inline bool IsMainFullScreenWindow(WindowType type, WindowMode mode)
    {
        return ((IsMainWindow(type)) && (mode == WindowMode::WINDOW_MODE_FULLSCREEN));
    }

    static inline bool IsFloatingWindow(WindowMode mode)
    {
        return mode == WindowMode::WINDOW_MODE_FLOATING;
    }

    static inline bool IsSystemBarWindow(WindowType type)
    {
        return (type == WindowType::WINDOW_TYPE_STATUS_BAR || type == WindowType::WINDOW_TYPE_NAVIGATION_BAR);
    }

    static inline bool IsOverlayWindow(WindowType type)
    {
        return (type == WindowType::WINDOW_TYPE_STATUS_BAR
            || type == WindowType::WINDOW_TYPE_NAVIGATION_BAR
            || type == WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT);
    }

    static inline bool IsRotatableWindow(WindowType type, WindowMode mode)
    {
        return WindowHelper::IsMainFullScreenWindow(type, mode) || type == WindowType::WINDOW_TYPE_KEYGUARD ||
            type == WindowType::WINDOW_TYPE_DESKTOP ||
            ((type == WindowType::WINDOW_TYPE_LAUNCHER_RECENT) && (mode == WindowMode::WINDOW_MODE_FULLSCREEN));
    }

    static inline bool IsFullScreenWindow(WindowMode mode)
    {
        return mode == WindowMode::WINDOW_MODE_FULLSCREEN;
    }

    static inline bool IsSplitWindowMode(WindowMode mode)
    {
        return mode == WindowMode::WINDOW_MODE_SPLIT_PRIMARY || mode == WindowMode::WINDOW_MODE_SPLIT_SECONDARY;
    }

    static inline bool IsValidWindowMode(WindowMode mode)
    {
        return mode == WindowMode::WINDOW_MODE_FULLSCREEN || mode == WindowMode::WINDOW_MODE_SPLIT_PRIMARY ||
            mode == WindowMode::WINDOW_MODE_SPLIT_SECONDARY || mode == WindowMode::WINDOW_MODE_FLOATING ||
            mode == WindowMode::WINDOW_MODE_PIP;
    }

    static inline bool IsEmptyRect(const Rect& r)
    {
        return (r.posX_ == 0 && r.posY_ == 0 && r.width_ == 0 && r.height_ == 0);
    }

    static inline bool IsLandscapeRect(const Rect& r)
    {
        return r.width_ > r.height_;
    }

    static inline bool IsShowWhenLocked(uint32_t flags)
    {
        return flags & static_cast<uint32_t>(WindowFlag::WINDOW_FLAG_SHOW_WHEN_LOCKED);
    }

    static Rect GetOverlap(const Rect& rect1, const Rect& rect2, const int offsetX, const int offsetY)
    {
        int32_t x_begin = std::max(rect1.posX_, rect2.posX_);
        int32_t x_end = std::min(rect1.posX_ + static_cast<int32_t>(rect1.width_),
            rect2.posX_ + static_cast<int32_t>(rect2.width_));
        int32_t y_begin = std::max(rect1.posY_, rect2.posY_);
        int32_t y_end = std::min(rect1.posY_ + static_cast<int32_t>(rect1.height_),
            rect2.posY_ + static_cast<int32_t>(rect2.height_));
        if (y_begin >= y_end || x_begin >= x_end) {
            return { 0, 0, 0, 0 };
        }
        return { x_begin - offsetX, y_begin - offsetY,
            static_cast<uint32_t>(x_end - x_begin), static_cast<uint32_t>(y_end - y_begin) };
    }

    static bool IsWindowModeSupported(uint32_t modeSupportInfo, WindowMode mode)
    {
        switch (mode) {
            case WindowMode::WINDOW_MODE_FULLSCREEN:
                return WindowModeSupport::WINDOW_MODE_SUPPORT_FULLSCREEN & modeSupportInfo;
            case WindowMode::WINDOW_MODE_FLOATING:
                return WindowModeSupport::WINDOW_MODE_SUPPORT_FLOATING & modeSupportInfo;
            case WindowMode::WINDOW_MODE_SPLIT_PRIMARY:
                return WindowModeSupport::WINDOW_MODE_SUPPORT_SPLIT_PRIMARY & modeSupportInfo;
            case WindowMode::WINDOW_MODE_SPLIT_SECONDARY:
                return WindowModeSupport::WINDOW_MODE_SUPPORT_SPLIT_SECONDARY & modeSupportInfo;
            case WindowMode::WINDOW_MODE_PIP:
                return WindowModeSupport::WINDOW_MODE_SUPPORT_PIP & modeSupportInfo;
            default:
                return true;
        }
    }

    static WindowMode GetWindowModeFromModeSupportInfo(uint32_t modeSupportInfo)
    {
        // get the binary number consists of the last 1 and 0 behind it
        uint32_t windowModeSupport = modeSupportInfo & (~modeSupportInfo + 1);

        switch (windowModeSupport) {
            case WindowModeSupport::WINDOW_MODE_SUPPORT_FULLSCREEN:
                return WindowMode::WINDOW_MODE_FULLSCREEN;
            case WindowModeSupport::WINDOW_MODE_SUPPORT_FLOATING:
                return WindowMode::WINDOW_MODE_FLOATING;
            case WindowModeSupport::WINDOW_MODE_SUPPORT_SPLIT_PRIMARY:
                return WindowMode::WINDOW_MODE_SPLIT_PRIMARY;
            case WindowModeSupport::WINDOW_MODE_SUPPORT_SPLIT_SECONDARY:
                return WindowMode::WINDOW_MODE_SPLIT_SECONDARY;
            case WindowModeSupport::WINDOW_MODE_SUPPORT_PIP:
                return WindowMode::WINDOW_MODE_PIP;
            default:
                return WindowMode::WINDOW_MODE_UNDEFINED;
        }
    }

    static uint32_t ConvertSupportModesToSupportInfo(const std::vector<AppExecFwk::SupportWindowMode>& supportModes)
    {
        uint32_t modeSupportInfo = 0;
        for (auto& mode : supportModes) {
            if (mode == AppExecFwk::SupportWindowMode::FULLSCREEN) {
                modeSupportInfo |= WindowModeSupport::WINDOW_MODE_SUPPORT_FULLSCREEN;
            } else if (mode == AppExecFwk::SupportWindowMode::SPLIT) {
                modeSupportInfo |= (WindowModeSupport::WINDOW_MODE_SUPPORT_SPLIT_PRIMARY |
                                    WindowModeSupport::WINDOW_MODE_SUPPORT_SPLIT_SECONDARY);
            } else if (mode == AppExecFwk::SupportWindowMode::FLOATING) {
                modeSupportInfo |= WindowModeSupport::WINDOW_MODE_SUPPORT_FLOATING;
            }
        }
        return modeSupportInfo;
    }

    static Rect GetFixedWindowRectByLimitSize(const Rect& oriDstRect, const Rect& lastRect, bool isVertical,
        float virtualPixelRatio)
    {
        uint32_t minFloatingW = static_cast<uint32_t>(MIN_FLOATING_WIDTH * virtualPixelRatio);
        uint32_t minFloatingH = static_cast<uint32_t>(MIN_FLOATING_HEIGHT * virtualPixelRatio);
        Rect dstRect = oriDstRect;
        // fix minimum size
        dstRect.width_ = std::max(minFloatingW, oriDstRect.width_);
        dstRect.height_ = std::max(minFloatingH, oriDstRect.height_);

        // fix maximum size
        dstRect.width_ = std::min(static_cast<uint32_t>(MAX_FLOATING_SIZE * virtualPixelRatio), dstRect.width_);
        dstRect.height_ = std::min(static_cast<uint32_t>(MAX_FLOATING_SIZE * virtualPixelRatio), dstRect.height_);

        // limit position by fixed width or height
        if (oriDstRect.posX_ != lastRect.posX_) {
            dstRect.posX_ = oriDstRect.posX_ + static_cast<int32_t>(oriDstRect.width_) -
                static_cast<int32_t>(dstRect.width_);
        }
        if (oriDstRect.posY_ != lastRect.posY_) {
            dstRect.posY_ = oriDstRect.posY_ + static_cast<int32_t>(oriDstRect.height_) -
                static_cast<int32_t>(dstRect.height_);
        }
        return dstRect;
    }

    static bool IsPointInTargetRect(int32_t pointPosX, int32_t pointPosY, const Rect& targetRect)
    {
        if ((pointPosX > targetRect.posX_) &&
            (pointPosX < (targetRect.posX_ + static_cast<int32_t>(targetRect.width_)) - 1) &&
            (pointPosY > targetRect.posY_) &&
            (pointPosY < (targetRect.posY_ + static_cast<int32_t>(targetRect.height_)) - 1)) {
            return true;
        }
        return false;
    }

    static bool IsPointInTargetRectWithBound(int32_t pointPosX, int32_t pointPosY, const Rect& targetRect)
    {
        if ((pointPosX >= targetRect.posX_) &&
            (pointPosX < (targetRect.posX_ + static_cast<int32_t>(targetRect.width_))) &&
            (pointPosY >= targetRect.posY_) &&
            (pointPosY < (targetRect.posY_ + static_cast<int32_t>(targetRect.height_)))) {
            return true;
        }
        return false;
    }

    static bool IsPointInWindowExceptCorner(int32_t pointPosX, int32_t pointPosY, const Rect& rectExceptCorner)
    {
        if ((pointPosX > rectExceptCorner.posX_ &&
            pointPosX < (rectExceptCorner.posX_ + static_cast<int32_t>(rectExceptCorner.width_)) - 1) ||
            (pointPosY > rectExceptCorner.posY_ &&
            pointPosY < (rectExceptCorner.posY_ + static_cast<int32_t>(rectExceptCorner.height_)) - 1)) {
            return true;
        }
        return false;
    }

    static inline bool IsSwitchCascadeReason(WindowUpdateReason reason)
    {
        return (reason >= WindowUpdateReason::NEED_SWITCH_CASCADE_BASE) &&
            (reason < WindowUpdateReason::NEED_SWITCH_CASCADE_END);
    }

    static AvoidPosType GetAvoidPosType(const Rect& rect, const Rect& displayRect)
    {
        if (rect.width_ ==  displayRect.width_) {
            if (rect.posY_ == displayRect.posY_) {
                return AvoidPosType::AVOID_POS_TOP;
            } else {
                return AvoidPosType::AVOID_POS_BOTTOM;
            }
        } else if (rect.height_ ==  displayRect.height_) {
            if (rect.posX_ == displayRect.posX_) {
                return AvoidPosType::AVOID_POS_LEFT;
            } else {
                return AvoidPosType::AVOID_POS_RIGHT;
            }
        }

        return AvoidPosType::AVOID_POS_UNKNOWN;
    }

    static inline bool IsNumber(std::string str)
    {
        if (str.size() == 0) {
            return false;
        }
        for (int32_t i = 0; i < static_cast<int32_t>(str.size()); i++) {
            if (str.at(i) < '0' || str.at(i) > '9') {
                return false;
            }
        }
        return true;
    }

    static bool IsFloatingNumber(std::string str, bool allowNeg = false)
    {
        if (str.size() == 0) {
            return false;
        }

        int32_t i = 0;
        if (allowNeg && str.at(i) == '-') {
            i++;
        }

        for (; i < static_cast<int32_t>(str.size()); i++) {
            if ((str.at(i) < '0' || str.at(i) > '9') &&
                (str.at(i) != '.' || std::count(str.begin(), str.end(), '.') > 1)) {
                return false;
            }
        }
        return true;
    }

    static std::vector<std::string> Split(std::string str, std::string pattern)
    {
        int32_t position;
        std::vector<std::string> result;
        str += pattern;
        int32_t length = static_cast<int32_t>(str.size());
        for (int32_t i = 0; i < length; i++) {
            position = static_cast<int32_t>(str.find(pattern, i));
            if (position < length) {
                std::string tmp = str.substr(i, position - i);
                result.push_back(tmp);
                i = position + static_cast<int32_t>(pattern.size()) - 1;
            }
        }
        return result;
    }

    static PointInfo CalculateOriginPosition(const Rect& rOrigin, const Rect& rActial, const PointInfo& pos)
    {
        PointInfo ret = pos;
        ret.x += rActial.posX_ - pos.x;
        ret.y += rActial.posY_ - pos.y;
        ret.x += rOrigin.posX_ - rActial.posX_;
        ret.y += rOrigin.posY_ - rActial.posY_;
        ret.x += (pos.x - rActial.posX_) * rOrigin.width_ / rActial.width_;
        ret.y += (pos.y - rActial.posY_) * rOrigin.height_ / rActial.height_;
        return ret;
    }

    // Transform a point at screen to its oringin position in 3D world and project to xy plane
    static PointInfo CalculateOriginPosition(const TransformHelper::Matrix4& transformMat, const PointInfo& pointPos)
    {
        TransformHelper::Vector2 p(static_cast<float>(pointPos.x), static_cast<float>(pointPos.y));
        TransformHelper::Vector2 originPos = TransformHelper::GetOriginScreenPoint(p, transformMat);
        return PointInfo { static_cast<uint32_t>(originPos.x_), static_cast<uint32_t>(originPos.y_) };
    }

    // This method is used to update transform when rect changed, but world transform matrix should not change.
    static void GetTransformFromWorldMat4(const TransformHelper::Matrix4& inWorldMat, const Rect& rect,
        Transform& transform)
    {
        TransformHelper::Vector3 pivotPos = { rect.posX_ + transform.pivotX_ * rect.width_,
            rect.posY_ + transform.pivotY_ * rect.height_, 0 };
        TransformHelper::Matrix4 worldMat = TransformHelper::CreateTranslation(pivotPos) * inWorldMat *
                        TransformHelper::CreateTranslation(-pivotPos);
        auto scale = worldMat.GetScale();
        auto translation = worldMat.GetTranslation();
        transform.scaleX_ = scale.x_;
        transform.scaleY_ = scale.y_;
        transform.scaleZ_ = scale.z_;
        transform.translateX_ = translation.x_;
        transform.translateY_ = translation.y_;
        transform.translateZ_ = translation.z_;
    }

    static TransformHelper::Matrix4 ComputeWorldTransformMat4(const Transform& transform)
    {
        TransformHelper::Matrix4 ret = TransformHelper::Matrix4::Identity;
        // set scale
        if ((transform.scaleX_ - 1) || (transform.scaleY_ - 1) || (transform.scaleY_ - 1)) {
            ret *= TransformHelper::CreateScale(transform.scaleX_, transform.scaleY_, transform.scaleZ_);
        }
        // set rotation
        if (transform.rotationX_) {
            ret *= TransformHelper::CreateRotationX(MathHelper::ToRadians(-transform.rotationX_));
        }
        if (transform.rotationY_) {
            ret *= TransformHelper::CreateRotationY(MathHelper::ToRadians(-transform.rotationY_));
        }
        if (transform.rotationZ_) {
            ret *= TransformHelper::CreateRotationZ(MathHelper::ToRadians(transform.rotationZ_));
        }
        // set translation
        if (transform.translateX_ || transform.translateY_ || transform.translateZ_) {
            ret *= TransformHelper::CreateTranslation(TransformHelper::Vector3(transform.translateX_,
                transform.translateY_, transform.translateZ_));
        }
        return ret;
    }

    // Transform rect by matrix and get the circumscribed rect
    static Rect TransformRect(const TransformHelper::Matrix4& transformMat, const Rect& rect)
    {
        TransformHelper::Vector3 a = TransformHelper::TransformWithPerspDiv(
            TransformHelper::Vector3(rect.posX_, rect.posY_, 0), transformMat);
        TransformHelper::Vector3 b = TransformHelper::TransformWithPerspDiv(
            TransformHelper::Vector3(rect.posX_ + rect.width_, rect.posY_, 0), transformMat);
        TransformHelper::Vector3 c = TransformHelper::TransformWithPerspDiv(
            TransformHelper::Vector3(rect.posX_, rect.posY_ + rect.height_, 0), transformMat);
        TransformHelper::Vector3 d = TransformHelper::TransformWithPerspDiv(
            TransformHelper::Vector3(rect.posX_ + rect.width_, rect.posY_ + rect.height_, 0), transformMat);
        // Return smallest rect involve transformed rect(abcd)
        int32_t xmin = MathHelper::Min(a.x_, b.x_, c.x_, d.x_);
        int32_t ymin = MathHelper::Min(a.y_, b.y_, c.y_, d.y_);
        int32_t xmax = MathHelper::Max(a.x_, b.x_, c.x_, d.x_);
        int32_t ymax = MathHelper::Max(a.y_, b.y_, c.y_, d.y_);
        uint32_t w = static_cast<uint32_t>(xmax - xmin);
        uint32_t h = static_cast<uint32_t>(ymax - ymin);
        return Rect { xmin, ymin, w, h };
    }

    static TransformHelper::Vector2 CalculateHotZoneScale(const TransformHelper::Matrix4& transformMat)
    {
        TransformHelper::Vector2 hotZoneScale;
        TransformHelper::Vector3 a = TransformHelper::TransformWithPerspDiv(TransformHelper::Vector3(0, 0, 0),
            transformMat);
        TransformHelper::Vector3 b = TransformHelper::TransformWithPerspDiv(TransformHelper::Vector3(1, 0, 0),
            transformMat);
        TransformHelper::Vector3 c = TransformHelper::TransformWithPerspDiv(TransformHelper::Vector3(0, 1, 0),
            transformMat);
        TransformHelper::Vector2 axy(a.x_, a.y_);
        TransformHelper::Vector2 bxy(b.x_, b.y_);
        TransformHelper::Vector2 cxy(c.x_, c.y_);
        hotZoneScale.x_ = (axy - bxy).Length();
        hotZoneScale.y_ = (axy - cxy).Length();
        if (std::isnan(hotZoneScale.x_) || std::isnan(hotZoneScale.y_) ||
            MathHelper::NearZero(hotZoneScale.x_) || MathHelper::NearZero(hotZoneScale.y_)) {
            return TransformHelper::Vector2(1, 1);
        } else {
            return hotZoneScale;
        }
    }

    static bool CalculateTouchHotAreas(const Rect& windowRect, const std::vector<Rect>& requestRects,
        std::vector<Rect>& outRects)
    {
        bool isOk = true;
        for (const auto& rect : requestRects) {
            if (rect.posX_ < 0 || rect.posY_ < 0 || rect.width_ == 0 || rect.height_ == 0) {
                return false;
            }
            Rect hotArea;
            if (rect.posX_ >= static_cast<int32_t>(windowRect.width_) ||
                rect.posY_ >= static_cast<int32_t>(windowRect.height_)) {
                isOk = false;
                continue;
            }
            hotArea.posX_ = windowRect.posX_ + rect.posX_;
            hotArea.posY_ = windowRect.posY_ + rect.posY_;
            hotArea.width_ = static_cast<uint32_t>(std::min(hotArea.posX_ + rect.width_,
                windowRect.posX_ + windowRect.width_) - hotArea.posX_);
            hotArea.height_ = static_cast<uint32_t>(std::min(hotArea.posY_ + rect.height_,
                windowRect.posY_ + windowRect.height_) - hotArea.posY_);
            outRects.emplace_back(hotArea);
        }
        return isOk;
    }

    static bool IsRectSatisfiedWithSizeLimits(const Rect& rect, const WindowSizeLimits& sizeLimits)
    {
        if (rect.height_ == 0) {
            return false;
        }
        auto curRatio = static_cast<float>(rect.width_) / static_cast<float>(rect.height_);
        if (sizeLimits.minWidth_ <= rect.width_ && rect.width_ <= sizeLimits.maxWidth_ &&
            sizeLimits.minHeight_ <= rect.height_ && rect.height_ <= sizeLimits.maxHeight_ &&
            sizeLimits.minRatio_ <= curRatio && curRatio <= sizeLimits.maxRatio_) {
            return true;
        }
        return false;
    }

    static bool IsOnlySupportSplitAndShowWhenLocked(bool isShowWhenLocked, uint32_t modeSupportInfo)
    {
        uint32_t splitModeInfo = (WindowModeSupport::WINDOW_MODE_SUPPORT_SPLIT_PRIMARY |
                                  WindowModeSupport::WINDOW_MODE_SUPPORT_SPLIT_SECONDARY);
        if (isShowWhenLocked && (splitModeInfo == modeSupportInfo)) {
            return true;
        }
        return false;
    }

    static bool IsInvalidWindowInTileLayoutMode(uint32_t supportModeInfo, WindowLayoutMode layoutMode)
    {
        if ((!IsWindowModeSupported(supportModeInfo, WindowMode::WINDOW_MODE_FLOATING)) &&
            (layoutMode == WindowLayoutMode::TILE)) {
            return true;
        }
        return false;
    }

    static bool CheckSupportWindowMode(WindowMode winMode, uint32_t modeSupportInfo,
        const sptr<WindowTransitionInfo>& info)
    {
        if (!WindowHelper::IsMainWindow(info->GetWindowType())) {
            return true;
        }

        if ((!IsWindowModeSupported(modeSupportInfo, winMode)) ||
            (IsOnlySupportSplitAndShowWhenLocked(info->GetShowFlagWhenLocked(), modeSupportInfo))) {
            return false;
        }
        return true;
    }

private:
    WindowHelper() = default;
    ~WindowHelper() = default;
};
} // namespace OHOS
} // namespace Rosen
#endif // OHOS_WM_INCLUDE_WM_HELPER_H

/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef OHOS_ROSEN_WINDOW_NODE_H
#define OHOS_ROSEN_WINDOW_NODE_H

#include <ipc_skeleton.h>
#include <refbase.h>
#include <running_lock.h>
#include <ui/rs_surface_node.h>
#include "zidl/window_interface.h"
#include "window_manager_hilog.h"
#include "window_node_state_machine.h"

namespace OHOS {
namespace Rosen {
class WindowNode : public RefBase {
public:
    WindowNode(const sptr<WindowProperty>& property, const sptr<IWindow>& window,
        std::shared_ptr<RSSurfaceNode> surfaceNode)
        : surfaceNode_(surfaceNode), property_(property), windowToken_(window)
    {
        if (property != nullptr) {
            abilityInfo_ = property->GetAbilityInfo();
        }
    }
    WindowNode(const sptr<WindowProperty>& property, const sptr<IWindow>& window,
        std::shared_ptr<RSSurfaceNode> surfaceNode, int32_t pid, int32_t uid)
        : surfaceNode_(surfaceNode), property_(property), windowToken_(window), callingPid_(pid), callingUid_(uid)
    {
        inputCallingPid_ = pid;
        if (property != nullptr) {
            abilityInfo_ = property->GetAbilityInfo();
        }
    }
    WindowNode() : property_(new WindowProperty())
    {
    }
    explicit WindowNode(const sptr<WindowProperty>& property) : property_(property)
    {
    }
    ~WindowNode();

    void SetDisplayId(DisplayId displayId);
    void SetEntireWindowTouchHotArea(const Rect& rect);
    void SetEntireWindowPointerHotArea(const Rect& rect);
    void SetWindowRect(const Rect& rect);
    void SetDecoStatus(bool decoStatus);
    void SetRequestRect(const Rect& rect);
    void SetWindowProperty(const sptr<WindowProperty>& property);
    void SetSystemBarProperty(WindowType type, const SystemBarProperty& property);
    void SetWindowMode(WindowMode mode);
    void SetBrightness(float brightness);
    void SetFocusable(bool focusable);
    void SetTouchable(bool touchable);
    void SetTurnScreenOn(bool turnScreenOn);
    void SetKeepScreenOn(bool keepScreenOn);
    void SetCallingWindow(uint32_t windowId);
    void SetInputEventCallingPid(int32_t pid);
    void SetCallingPid(int32_t pid);
    void SetCallingUid(int32_t uid);
    void SetWindowToken(sptr<IWindow> window);
    uint32_t GetCallingWindow() const;
    void SetWindowSizeChangeReason(WindowSizeChangeReason reason);
    void SetRequestedOrientation(Orientation orientation);
    void SetShowingDisplays(const std::vector<DisplayId>& displayIdVec);
    void SetModeSupportInfo(uint32_t modeSupportInfo);
    void SetDragType(DragType dragType);
    void SetOriginRect(const Rect& rect);
    void SetTouchHotAreas(const std::vector<Rect>& rects);
    void SetPointerHotAreas(const std::vector<Rect>& rects);
    void SetWindowSizeLimits(const WindowSizeLimits& sizeLimits);
    void SetWindowUpdatedSizeLimits(const WindowSizeLimits& sizeLimits);
    void ComputeTransform();
    void SetTransform(const Transform& trans);
    void SetSnapshot(std::shared_ptr<Media::PixelMap> pixelMap);
    std::shared_ptr<Media::PixelMap> GetSnapshot();
    Transform GetZoomTransform() const;
    void UpdateZoomTransform(const Transform& trans, bool isDisplayZoomOn);

    const sptr<IWindow>& GetWindowToken() const;
    uint32_t GetWindowId() const;
    uint32_t GetParentId() const;
    const std::string& GetWindowName() const;
    DisplayId GetDisplayId() const;
    Rect GetEntireWindowTouchHotArea() const;
    Rect GetEntireWindowPointerHotArea() const;
    Rect GetWindowRect() const;
    bool GetDecoStatus() const;
    Rect GetRequestRect() const;
    WindowType GetWindowType() const;
    WindowMode GetWindowMode() const;
    float GetBrightness() const;
    bool IsTurnScreenOn() const;
    bool IsKeepScreenOn() const;
    uint32_t GetWindowFlags() const;
    const sptr<WindowProperty>& GetWindowProperty() const;
    int32_t GetInputEventCallingPid() const;
    int32_t GetCallingPid() const;
    int32_t GetCallingUid() const;
    const std::unordered_map<WindowType, SystemBarProperty>& GetSystemBarProperty() const;
    bool IsSplitMode() const;
    WindowSizeChangeReason GetWindowSizeChangeReason() const;
    Orientation GetRequestedOrientation() const;
    std::vector<DisplayId> GetShowingDisplays() const;
    uint32_t GetModeSupportInfo() const;
    DragType GetDragType() const;
    bool GetStretchable() const;
    const Rect& GetOriginRect() const;
    void ResetWindowSizeChangeReason();
    void GetTouchHotAreas(std::vector<Rect>& rects) const;
    void GetPointerHotAreas(std::vector<Rect>& rects) const;
    uint32_t GetAccessTokenId() const;
    WindowSizeLimits GetWindowSizeLimits() const;
    WindowSizeLimits GetWindowUpdatedSizeLimits() const;

    bool EnableDefaultAnimation(bool animationPlayed);
    sptr<WindowNode> parent_;
    std::vector<sptr<WindowNode>> children_;
    std::shared_ptr<RSSurfaceNode> surfaceNode_;
    std::shared_ptr<RSSurfaceNode> leashWinSurfaceNode_ = nullptr;
    std::shared_ptr<RSSurfaceNode> startingWinSurfaceNode_ = nullptr;
    sptr<IRemoteObject> dialogTargetToken_ = nullptr;
    sptr<IRemoteObject> abilityToken_ = nullptr;
    std::shared_ptr<PowerMgr::RunningLock> keepScreenLock_;
    int32_t priority_ { 0 };
    uint32_t zOrder_ { 0 };
    bool requestedVisibility_ { false };
    bool currentVisibility_ { false };
    bool isVisible_ { false };
    bool isAppCrash_ { false };
    bool isPlayAnimationShow_ { false }; // delete when enable state machine
    bool isPlayAnimationHide_ { false }; // delete when enable state machine
    bool startingWindowShown_ { false };
    bool firstFrameAvaliable_ { false };
    bool isShowingOnMultiDisplays_ { false };
    std::vector<DisplayId> showingDisplays_;
    AbilityInfo abilityInfo_;
    WindowNodeStateMachine stateMachine_;

private:
    sptr<WindowProperty> property_ = nullptr;
    sptr<IWindow> windowToken_ = nullptr;
    Rect entireWindowTouchHotArea_ { 0, 0, 0, 0 };
    Rect entireWindowPointerHotArea_ { 0, 0, 0, 0 };
    std::vector<Rect> touchHotAreas_; // coordinates relative to display.
    std::vector<Rect> pointerHotAreas_; // coordinates relative to display.
    std::shared_ptr<Media::PixelMap> snapshot_;
    int32_t callingPid_ = { 0 };
    int32_t inputCallingPid_ = { 0 };
    int32_t callingUid_ = { 0 };
    WindowSizeChangeReason windowSizeChangeReason_ {WindowSizeChangeReason::UNDEFINED};
};
} // Rosen
} // OHOS
#endif // OHOS_ROSEN_WINDOW_NODE_H

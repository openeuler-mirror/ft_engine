/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef OHOS_ROSEN_REMOTE_ANIMATION_H
#define OHOS_ROSEN_REMOTE_ANIMATION_H

#include <refbase.h>
#include <rs_iwindow_animation_controller.h>
#include <rs_window_animation_finished_callback.h>
#include <rs_window_animation_target.h>

#include "wm_common.h"
#include "window_controller.h"
#include "window_node.h"
#include "window_root.h"
#include "window_transition_info.h"

namespace OHOS {
namespace Rosen {
enum class TransitionEvent : uint32_t {
    APP_TRANSITION,
    HOME,
    MINIMIZE,
    CLOSE,
    UNKNOWN,
    BACK_TRANSITION,
};

class RemoteAnimation : public RefBase {
public:
    RemoteAnimation() = delete;
    ~RemoteAnimation() = default;

    static bool CheckTransition(sptr<WindowTransitionInfo> srcInfo, const sptr<WindowNode>& srcNode,
        sptr<WindowTransitionInfo> dstInfo, const sptr<WindowNode>& dstNode);
    static TransitionEvent GetTransitionEvent(sptr<WindowTransitionInfo> srcInfo,
        sptr<WindowTransitionInfo> dstInfo, const sptr<WindowNode>& srcNode, const sptr<WindowNode>& dstNode);
    static WMError SetWindowAnimationController(const sptr<RSIWindowAnimationController>& controller);
    static WMError NotifyAnimationTransition(sptr<WindowTransitionInfo> srcInfo, sptr<WindowTransitionInfo> dstInfo,
        const sptr<WindowNode>& srcNode, const sptr<WindowNode>& dstNode);
    static WMError NotifyAnimationMinimize(sptr<WindowTransitionInfo> srcInfo, const sptr<WindowNode>& srcNode);
    static WMError NotifyAnimationClose(sptr<WindowTransitionInfo> srcInfo, const sptr<WindowNode>& srcNode,
        TransitionEvent event);
    static WMError NotifyAnimationBackTransition(sptr<WindowTransitionInfo> srcInfo,
        sptr<WindowTransitionInfo> dstInfo, const sptr<WindowNode>& srcNode,
        const sptr<WindowNode>& dstNode);
    static void NotifyAnimationUpdateWallpaper(sptr<WindowNode> node);
    static void OnRemoteDie(const sptr<IRemoteObject>& remoteObject);
    static bool CheckAnimationController();
    static bool CheckRemoteAnimationEnabled(DisplayId displayId);
    static WMError NotifyAnimationByHome();
    static WMError NotifyAnimationScreenUnlock(std::function<void(void)> callback);
    static void SetMainTaskHandler(std::shared_ptr<AppExecFwk::EventHandler> handler);
    static void NotifyAnimationTargetsUpdate(std::vector<uint32_t>& fullScreenWinIds,
        std::vector<uint32_t>& floatWinIds);
    static void SetAnimationFirst(bool animationFirst);
    static void SetWindowControllerAndRoot(const sptr<WindowController>& windowController,
    const sptr<WindowRoot>& windowRoot);
    static bool IsRemoteAnimationEnabledAndFirst(DisplayId displayId = 0);
    static void CallbackTimeOutProcess();
    static sptr<RSWindowAnimationFinishedCallback> CreateAnimationFinishedCallback(
        const std::function<void(void)>& callback);
    static inline bool IsAnimationFirst()
    {
        return animationFirst_;
    }
    static bool isRemoteAnimationEnable_;
private:
    static sptr<RSWindowAnimationTarget> CreateWindowAnimationTarget(sptr<WindowTransitionInfo> info,
        const sptr<WindowNode>& windowNode);
    static WMError NotifyAnimationStartApp(sptr<WindowTransitionInfo> srcInfo,
        const sptr<WindowNode>& srcNode, const sptr<WindowNode>& dstNode,
        sptr<RSWindowAnimationTarget>& dstTarget, sptr<RSWindowAnimationFinishedCallback>& finishedCallback);
    static sptr<RSWindowAnimationFinishedCallback> CreateShowAnimationFinishedCallback(
        const sptr<WindowNode>& srcNode, const sptr<WindowNode>& dstNode, bool needMinimizeSrcNode);
    static sptr<RSWindowAnimationFinishedCallback> CreateHideAnimationFinishedCallback(
        const sptr<WindowNode>& srcNode, TransitionEvent event);
    static void ProcessNodeStateTask(sptr<WindowNode>& node);
    static void GetExpectRect(const sptr<WindowNode>& dstNode, const sptr<RSWindowAnimationTarget>& dstTarget);
    static void PostProcessShowCallback(const sptr<WindowNode>& node);
    static void ExecuteFinalStateTask(sptr<WindowNode>& node);
    static void GetAnimationTargetsForHome(std::vector<sptr<RSWindowAnimationTarget>>& animationTargets,
        std::vector<wptr<WindowNode>> needMinimizeAppNodes);
    static sptr<RSWindowAnimationFinishedCallback> GetTransitionFinishedCallback(
        const sptr<WindowNode>& srcNode, const sptr<WindowNode>& dstNode);

    static sptr<RSIWindowAnimationController> windowAnimationController_;
    static wptr<WindowRoot> windowRoot_;
    static std::weak_ptr<AppExecFwk::EventHandler> wmsTaskHandler_;
    static wptr<WindowController> windowController_;
    static bool animationFirst_;
    static std::atomic<uint32_t> allocationId_;
};
} // Rosen
} // OHOS
#endif // OHOS_ROSEN_REMOTE_ANIMATION_H

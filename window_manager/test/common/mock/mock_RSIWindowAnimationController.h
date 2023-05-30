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
#ifndef FRAMEWORKS_WM_TEST_UT_MOCK_RSIWINDOWANIMATIONCONTROLLER_H
#define FRAMEWORKS_WM_TEST_UT_MOCK_RSIWINDOWANIMATIONCONTROLLER_H

#include <vector>
#include <iremote_broker.h>
#include <gmock/gmock.h>

#include "rs_iwindow_animation_controller.h"
#include "iremote_object_mocker.h"

namespace OHOS {
namespace Rosen {
class RSIWindowAnimationControllerMocker : public RSIWindowAnimationController {
public:
    RSIWindowAnimationControllerMocker()
    {
        remoteObject_ = new IRemoteObjectMocker();
    }

    ~RSIWindowAnimationControllerMocker() {};
    void OnWindowAnimationTargetsUpdate(const sptr<RSWindowAnimationTarget>& fullScreenWindowTarget,
        const std::vector<sptr<RSWindowAnimationTarget>>& floatingWindowTargets) override
    {
        animationTarget_ = fullScreenWindowTarget;
        floatingWindowTargets_ = floatingWindowTargets;
        return;
    }

    void OnWallpaperUpdate(const sptr<RSWindowAnimationTarget>& wallpaperTarget) override
    {
        animationTarget_ = wallpaperTarget;
        return;
    }

    void OnStartApp(StartingAppType type, const sptr<RSWindowAnimationTarget>& startingWindowTarget,
        const sptr<RSIWindowAnimationFinishedCallback>& finishedCallback) override
    {
        finishedCallback_ = finishedCallback;
    }

    void OnAppTransition(const sptr<RSWindowAnimationTarget>& from, const sptr<RSWindowAnimationTarget>& to,
        const sptr<RSIWindowAnimationFinishedCallback>& finishedCallback)
    {
        finishedCallback_ = finishedCallback;
    }

    void OnAppBackTransition(const sptr<RSWindowAnimationTarget>& from, const sptr<RSWindowAnimationTarget>& to,
        const sptr<RSIWindowAnimationFinishedCallback>& finishedCallback)
    {
        finishedCallback_ = finishedCallback;
    }

    void OnMinimizeWindow(const sptr<RSWindowAnimationTarget>& minimizingWindow,
        const sptr<RSIWindowAnimationFinishedCallback>& finishedCallback)
    {
        finishedCallback_ = finishedCallback;
    }

    void OnMinimizeAllWindow(std::vector<sptr<RSWindowAnimationTarget>> minimizingWindows,
        const sptr<RSIWindowAnimationFinishedCallback>& finishedCallback)
    {
        finishedCallback_ = finishedCallback;
    }

    void OnCloseWindow(const sptr<RSWindowAnimationTarget>& closingWindow,
        const sptr<RSIWindowAnimationFinishedCallback>& finishedCallback)
    {
        finishedCallback_ = finishedCallback;
    }

    void OnScreenUnlock(const sptr<RSIWindowAnimationFinishedCallback>& finishedCallback)
    {
        finishedCallback_ = finishedCallback;
    }

    sptr<IRemoteObject> AsObject() override
    {
        return remoteObject_;
    };

    sptr<RSIWindowAnimationFinishedCallback> finishedCallback_ = nullptr;
    sptr<IRemoteObject> remoteObject_ = nullptr;
    sptr<RSWindowAnimationTarget> animationTarget_ = nullptr;
    std::vector<sptr<RSWindowAnimationTarget>> floatingWindowTargets_;
};
} // namespace Rosen
} // namespace OHOS
#endif // FRAMEWORKS_WM_TEST_UT_MOCK_RSIWINDOWANIMATIONCONTROLLER_H
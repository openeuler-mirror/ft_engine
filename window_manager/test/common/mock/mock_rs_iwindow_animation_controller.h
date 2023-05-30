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

#ifndef FRAMEWORKS_WM_TEST_UT_MOCK_RS_IWINDOW_ANIMATION_CONTROLLER_H
#define FRAMEWORKS_WM_TEST_UT_MOCK_RS_IWINDOW_ANIMATION_CONTROLLER_H
#include <gmock/gmock.h>

#include <rs_iwindow_animation_controller.h>
#include <rs_window_animation_finished_callback.h>
#include <rs_window_animation_target.h>

namespace OHOS {
namespace Rosen {
class MockRSIWindowAnimationController : public RSIWindowAnimationController {
public:
    MockRSIWindowAnimationController() {};
    MOCK_METHOD3(OnStartApp, void(StartingAppType type, const sptr<RSWindowAnimationTarget>& startingWindowTarget,
        const sptr<RSIWindowAnimationFinishedCallback>& finishedCallback));
    MOCK_METHOD3(OnAppTransition, void(const sptr<RSWindowAnimationTarget>& from,
        const sptr<RSWindowAnimationTarget>& to, const sptr<RSIWindowAnimationFinishedCallback>& finishedCallback));
    MOCK_METHOD3(OnAppBackTransition, void(const sptr<RSWindowAnimationTarget>& from,
        const sptr<RSWindowAnimationTarget>& to, const sptr<RSIWindowAnimationFinishedCallback>& finishedCallback));

    MOCK_METHOD2(OnMinimizeWindow, void(const sptr<RSWindowAnimationTarget>& minimizingWindow,
        const sptr<RSIWindowAnimationFinishedCallback>& finishedCallback));
    MOCK_METHOD2(OnMinimizeAllWindow, void(std::vector<sptr<RSWindowAnimationTarget>> minimizingWindows,
        const sptr<RSIWindowAnimationFinishedCallback>& finishedCallback));
    MOCK_METHOD2(OnCloseWindow, void(const sptr<RSWindowAnimationTarget>& closingWindow,
        const sptr<RSIWindowAnimationFinishedCallback>& finishedCallback));

    MOCK_METHOD1(OnScreenUnlock, void(const sptr<RSIWindowAnimationFinishedCallback>& finishedCallback));
    MOCK_METHOD2(OnWindowAnimationTargetsUpdate, void(const sptr<RSWindowAnimationTarget>& fullScreenWindowTarget,
        const std::vector<sptr<RSWindowAnimationTarget>>& floatingWindowTargets));
    MOCK_METHOD1(OnWallpaperUpdate, void(const sptr<RSWindowAnimationTarget>& wallpaperTarget));
    MOCK_METHOD0(AsObject, sptr<IRemoteObject>());
};
}
} // namespace OHOS

#endif // FRAMEWORKS_WM_TEST_UT_MOCK_RS_IWINDOW_ANIMATION_CONTROLLER_H

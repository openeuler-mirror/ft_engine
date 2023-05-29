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

// gtest
#include <gtest/gtest.h>
#include "window.h"
#include "wm_common.h"
#include "window_test_utils.h"
using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "WindowAnimationTest"};
}

using Utils = WindowTestUtils;

class TestAnimationTransitionController : public IAnimationTransitionController {
public:
    explicit TestAnimationTransitionController(sptr<Window> window) : window_(window) {}
    void AnimationForShown() override;
    void AnimationForHidden() override;
private:
    sptr<Window> window_ = nullptr;
};

class WindowAnimationTransitionTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    virtual void SetUp() override;
    virtual void TearDown() override;
    sptr<TestAnimationTransitionController> testAnimationTransitionListener_;
    Utils::TestWindowInfo windowInfo_;
    Transform trans_;
    Transform defaultTrans_;
};

void TestAnimationTransitionController::AnimationForShown()
{
    WLOGFI("TestAnimationTransitionController AnimationForShown");
    Transform trans;
    window_->SetTransform(trans);
}

void TestAnimationTransitionController::AnimationForHidden()
{
    WLOGFI("TestAnimationTransitionController AnimationForHidden");
    Transform trans;
    trans.pivotX_ = 1.0f;
    trans.pivotY_ = 0.6f;
    window_->SetTransform(trans);
}

void WindowAnimationTransitionTest::SetUpTestCase()
{
}

void WindowAnimationTransitionTest::TearDownTestCase()
{
}

void WindowAnimationTransitionTest::SetUp()
{
    windowInfo_ = {
            .name = "AnimationTestWindow",
            .rect = {0, 0, 200, 200},
            .type = WindowType::WINDOW_TYPE_FLOAT,
            .mode = WindowMode::WINDOW_MODE_FLOATING,
            .needAvoid = false,
            .parentLimit = false,
            .parentId = INVALID_WINDOW_ID,
    };
    trans_.pivotX_ = 1.0f;
    trans_.pivotY_ = 0.6f;
}

void WindowAnimationTransitionTest::TearDown()
{
}

namespace {
/**
 * @tc.name: AnimationTransitionTest01
 * @tc.desc: Register AnimationController and hide with custom animation
 * @tc.type: FUNC
 * @tc.require: issueI5NDLK
 */
HWTEST_F(WindowAnimationTransitionTest, AnimationTransitionTest01, Function | MediumTest | Level3)
{
    const sptr<Window>& window = Utils::CreateTestWindow(windowInfo_);
    ASSERT_NE(nullptr, window);
    sptr<TestAnimationTransitionController> testAnimationTransitionListener =
        new TestAnimationTransitionController(window);
    window->RegisterAnimationTransitionController(testAnimationTransitionListener);
    ASSERT_EQ(WMError::WM_OK, window->Show());
    usleep(500000); // 500000us = 0.5s
    ASSERT_EQ(WMError::WM_OK, window->Hide(0, true));
    usleep(500000); // 500000us = 0.5s
    ASSERT_TRUE(trans_ == window->GetTransform());
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: AnimationTransitionTest02
 * @tc.desc: Register AnimationController and show without animation
 * @tc.type: FUNC
 * @tc.require: issueI5NDLK
 */
HWTEST_F(WindowAnimationTransitionTest, AnimationTransitionTest02, Function | MediumTest | Level3)
{
    const sptr<Window>& window = Utils::CreateTestWindow(windowInfo_);
    ASSERT_NE(nullptr, window);
    sptr<TestAnimationTransitionController> testAnimationTransitionListener =
        new TestAnimationTransitionController(window);
    window->RegisterAnimationTransitionController(testAnimationTransitionListener);
    ASSERT_EQ(WMError::WM_OK, window->Show());
    usleep(500000); // 500000us = 0.5s
    ASSERT_TRUE(defaultTrans_ == window->GetTransform());
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: AnimationTransitionTest03
 * @tc.desc: hide with default animation without register animationController
 * @tc.type: FUNC
 * @tc.require: issueI5NDLK
 */
HWTEST_F(WindowAnimationTransitionTest, AnimationTransitionTest03, Function | MediumTest | Level3)
{
    const sptr<Window>& window = Utils::CreateTestWindow(windowInfo_);
    ASSERT_NE(nullptr, window);
    ASSERT_EQ(WMError::WM_OK, window->Hide(0, true));
    usleep(500000); // 500000us = 0.5s
    ASSERT_TRUE(defaultTrans_ == window->GetTransform());
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: AnimationTransitionTest04
 * @tc.desc: hide without custom animation and register animationController
 * @tc.type: FUNC
 * @tc.require: issueI5NDLK
 */
HWTEST_F(WindowAnimationTransitionTest, AnimationTransitionTest04, Function | MediumTest | Level3)
{
    const sptr<Window>& window = Utils::CreateTestWindow(windowInfo_);
    ASSERT_NE(nullptr, window);
    sptr<TestAnimationTransitionController> testAnimationTransitionListener =
        new TestAnimationTransitionController(window);
    window->RegisterAnimationTransitionController(testAnimationTransitionListener);
    ASSERT_EQ(WMError::WM_OK, window->Show());
    usleep(500000); // 500000us = 0.5s
    ASSERT_EQ(WMError::WM_OK, window->Hide());
    ASSERT_TRUE(defaultTrans_ == window->GetTransform());
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}
}
} // namespace Rosen
} // namespace OHOS

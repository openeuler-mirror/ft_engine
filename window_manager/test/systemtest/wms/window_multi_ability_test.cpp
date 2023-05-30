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

// gtest
#include <gtest/gtest.h>
#include "window_test_utils.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
using Utils = WindowTestUtils;
class WindowMultiAbilityTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    virtual void SetUp() override;
    virtual void TearDown() override;
};

void WindowMultiAbilityTest::SetUpTestCase()
{
}

void WindowMultiAbilityTest::TearDownTestCase()
{
}

void WindowMultiAbilityTest::SetUp()
{
}

void WindowMultiAbilityTest::TearDown()
{
}

/**
 * @tc.name: MultiAbilityWindow01
 * @tc.desc: Five scene process in one thread
 * @tc.type: FUNC
 */
HWTEST_F(WindowMultiAbilityTest, MultiAbilityWindow01, Function | MediumTest | Level2)
{
    sptr<WindowScene> scene1 = Utils::CreateWindowScene();
    sptr<WindowScene> scene2 = Utils::CreateWindowScene();
    sptr<WindowScene> scene3 = Utils::CreateWindowScene();
    sptr<WindowScene> scene4 = Utils::CreateWindowScene();
    sptr<WindowScene> scene5 = Utils::CreateWindowScene();

    ASSERT_EQ(WMError::WM_OK, scene1->GoForeground());
    ASSERT_EQ(WMError::WM_OK, scene2->GoForeground());
    ASSERT_EQ(WMError::WM_OK, scene3->GoForeground());
    ASSERT_EQ(WMError::WM_OK, scene4->GoForeground());
    ASSERT_EQ(WMError::WM_OK, scene5->GoForeground());
    ASSERT_EQ(WMError::WM_OK, scene5->GoBackground());
    ASSERT_EQ(WMError::WM_OK, scene4->GoBackground());
    ASSERT_EQ(WMError::WM_OK, scene3->GoBackground());
    ASSERT_EQ(WMError::WM_OK, scene2->GoBackground());
    ASSERT_EQ(WMError::WM_OK, scene1->GoBackground());
    ASSERT_EQ(WMError::WM_OK, scene1->GoDestroy());
    ASSERT_EQ(WMError::WM_OK, scene2->GoDestroy());
    ASSERT_EQ(WMError::WM_OK, scene3->GoDestroy());
    ASSERT_EQ(WMError::WM_OK, scene4->GoDestroy());
    ASSERT_EQ(WMError::WM_OK, scene5->GoDestroy());
}

/**
 * @tc.name: MultiAbilityWindow04
 * @tc.desc: Five scene process in one thread, create/show/hide/destroy in order
 * @tc.type: FUNC
 */
HWTEST_F(WindowMultiAbilityTest, MultiAbilityWindow02, Function | MediumTest | Level3)
{
    sptr<WindowScene> scene1 = Utils::CreateWindowScene();
    ASSERT_EQ(WMError::WM_OK, scene1->GoForeground());
    ASSERT_EQ(WMError::WM_OK, scene1->GoBackground());
    ASSERT_EQ(WMError::WM_OK, scene1->GoDestroy());

    sptr<WindowScene> scene2 = Utils::CreateWindowScene();
    ASSERT_EQ(WMError::WM_OK, scene2->GoForeground());
    ASSERT_EQ(WMError::WM_OK, scene2->GoBackground());
    ASSERT_EQ(WMError::WM_OK, scene2->GoDestroy());

    sptr<WindowScene> scene3 = Utils::CreateWindowScene();
    ASSERT_EQ(WMError::WM_OK, scene3->GoForeground());
    ASSERT_EQ(WMError::WM_OK, scene3->GoBackground());
    ASSERT_EQ(WMError::WM_OK, scene3->GoDestroy());

    sptr<WindowScene> scene4 = Utils::CreateWindowScene();
    ASSERT_EQ(WMError::WM_OK, scene4->GoForeground());
    ASSERT_EQ(WMError::WM_OK, scene4->GoBackground());
    ASSERT_EQ(WMError::WM_OK, scene4->GoDestroy());

    sptr<WindowScene> scene5 = Utils::CreateWindowScene();
    ASSERT_EQ(WMError::WM_OK, scene5->GoForeground());
    ASSERT_EQ(WMError::WM_OK, scene5->GoBackground());
    ASSERT_EQ(WMError::WM_OK, scene5->GoDestroy());
}

/**
 * @tc.name: MultiAbilityWindow05
 * @tc.desc: Five scene process in one thread, create/show/hide/destroy out of order
 * @tc.type: FUNC
 */
HWTEST_F(WindowMultiAbilityTest, MultiAbilityWindow03, Function | MediumTest | Level3)
{
    sptr<WindowScene> scene1 = Utils::CreateWindowScene();
    ASSERT_EQ(WMError::WM_OK, scene1->GoForeground());
    sptr<WindowScene> scene2 = Utils::CreateWindowScene();
    sptr<WindowScene> scene3 = Utils::CreateWindowScene();
    ASSERT_EQ(WMError::WM_OK, scene3->GoForeground());
    ASSERT_EQ(WMError::WM_OK, scene1->GoBackground());
    ASSERT_EQ(WMError::WM_OK, scene1->GoDestroy());
    sptr<WindowScene> scene4 = Utils::CreateWindowScene();
    ASSERT_EQ(WMError::WM_OK, scene3->GoBackground());
    ASSERT_EQ(WMError::WM_OK, scene2->GoForeground());
    ASSERT_EQ(WMError::WM_OK, scene4->GoForeground());
    ASSERT_EQ(WMError::WM_OK, scene2->GoBackground());
    sptr<WindowScene> scene5 = Utils::CreateWindowScene();
    ASSERT_EQ(WMError::WM_OK, scene3->GoDestroy());
    ASSERT_EQ(WMError::WM_OK, scene5->GoForeground());
    ASSERT_EQ(WMError::WM_OK, scene5->GoBackground());
    ASSERT_EQ(WMError::WM_OK, scene4->GoBackground());
    ASSERT_EQ(WMError::WM_OK, scene4->GoDestroy());
    ASSERT_EQ(WMError::WM_OK, scene5->GoDestroy());
    ASSERT_EQ(WMError::WM_OK, scene2->GoDestroy());
}
} // namespace Rosen
} // namespace OHOS

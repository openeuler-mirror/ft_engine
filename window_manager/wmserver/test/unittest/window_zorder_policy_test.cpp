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

#include <gtest/gtest.h>
#include "window_zorder_policy.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class WindowZorderPolicyTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    virtual void SetUp() override;
    virtual void TearDown() override;
};

void WindowZorderPolicyTest::SetUpTestCase()
{
}

void WindowZorderPolicyTest::TearDownTestCase()
{
}

void WindowZorderPolicyTest::SetUp()
{
}

void WindowZorderPolicyTest::TearDown()
{
}
namespace {
/**
 * @tc.name: GetWindowPriority01
 * @tc.desc: get wallpaper window zorder
 * @tc.type: FUNC
 */
HWTEST_F(WindowZorderPolicyTest, GetWindowPriority01, Function | SmallTest | Level2)
{
    sptr<WindowZorderPolicy> zorderPolicy = new WindowZorderPolicy();
    int32_t zorder = zorderPolicy->GetWindowPriority(WindowType::WINDOW_TYPE_DESKTOP);
    ASSERT_EQ(zorder, 1);
}

/**
 * @tc.name: GetWindowPriority02
 * @tc.desc: get invalid type window zorder
 * @tc.type: FUNC
 */
HWTEST_F(WindowZorderPolicyTest, GetWindowPriority02, Function | SmallTest | Level2)
{
    sptr<WindowZorderPolicy> zorderPolicy = new WindowZorderPolicy();
    int32_t zorder = zorderPolicy->GetWindowPriority(static_cast<WindowType>(3000));
    ASSERT_EQ(zorder, 0);
}
}
}
}
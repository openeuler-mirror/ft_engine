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
#include "wm_common.h"
#include "common_test_utils.h"
#include "freeze_controller.h"
#include "display_manager.h"

using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace Rosen {
class WindowFreezeControllerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    sptr<FreezeController> controller_;

    void SetAceessTokenPermission(const std::string processName);
};

void WindowFreezeControllerTest::SetUpTestCase()
{
    const char** perms = new const char *[1];
    perms[0] = "ohos.permission.CAPTURE_SCREEN";
    std::string processName = "WindowFreezeControllerTest";
    CommonTestUtils::SetAceessTokenPermission(processName, perms, 1);
}

void WindowFreezeControllerTest::TearDownTestCase()
{
}

void WindowFreezeControllerTest::SetUp()
{
    controller_ = new FreezeController();
}

void WindowFreezeControllerTest::TearDown()
{
}

namespace {
/**
 * @tc.name: FreezeDisplay01
 * @tc.desc: test FreezeDisplay/UnFreezeDisplay
 * @tc.type: FUNC
 */
HWTEST_F(WindowFreezeControllerTest, FreezeDisplay01, Function | SmallTest | Level2)
{
    DisplayId displayId = DISPLAY_ID_INVALID;
    bool res = true;

    res = controller_->UnfreezeDisplay(displayId);
    ASSERT_EQ(res, false);
    res = controller_->FreezeDisplay(displayId);
    ASSERT_EQ(res, false);
    res = controller_->FreezeDisplay(displayId);
    ASSERT_EQ(res, false);
    res = controller_->UnfreezeDisplay(displayId);
    ASSERT_EQ(res, true);

    displayId = DisplayManager::GetInstance().GetDefaultDisplayId();

    res = controller_->FreezeDisplay(displayId);
    ASSERT_EQ(res, false);
    res = controller_->UnfreezeDisplay(displayId);
    ASSERT_EQ(res, true);
}
}
}
}

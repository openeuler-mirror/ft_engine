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

#include "abstract_display.h"
#include "abstract_screen_controller.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class AbstractDisplayTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    DisplayId id = 1;
    std::string name = "abstract_display_test";
    SupportedScreenModes modesInfo;
    std::recursive_mutex mutex;
    sptr<AbstractScreenController> absController;
    sptr<AbstractScreen> absScreen;
    sptr<AbstractDisplay> absDisplay;
    sptr<AbstractDisplay> absDisplay2;
    sptr<AbstractDisplay> absDisplay3;
};

void AbstractDisplayTest::SetUpTestCase()
{
}

void AbstractDisplayTest::TearDownTestCase()
{
}

void AbstractDisplayTest::SetUp()
{
    modesInfo.width_ = 2160;
    modesInfo.height_ = 1600;
    modesInfo.refreshRate_ = 60;
    sptr<SupportedScreenModes> info = new SupportedScreenModes(modesInfo);
    absController = nullptr;
    absScreen = new AbstractScreen(absController, name, 1, 1);
    absDisplay = new AbstractDisplay(id, name, info, absScreen);
    modesInfo.width_ = 800;
    modesInfo.height_ = 2560;
    absDisplay2 = new AbstractDisplay(id, name, info, absScreen);
    modesInfo.width_ = 2560;
    modesInfo.height_ = 2560;
    absDisplay3 = new AbstractDisplay(id, name, info, absScreen);
}

void AbstractDisplayTest::TearDown()
{
}

namespace {
/**
 * @tc.name: BindAbstractScreen
 * @tc.desc: BindAbstractScreen test
 * @tc.type: FUNC
 */
HWTEST_F(AbstractDisplayTest, BindAbstractScreen01, Function | SmallTest | Level3)
{
    sptr<AbstractScreen> abstractScreen = nullptr;
    ASSERT_EQ(false, absDisplay->BindAbstractScreen(abstractScreen));
}
/**
 * @tc.name: BindAbstractScreen
 * @tc.desc: BindAbstractScreen test
 * @tc.type: FUNC
 */
HWTEST_F(AbstractDisplayTest, BindAbstractScreen02, Function | SmallTest | Level3)
{
    sptr<AbstractScreen> abstractScreen = absScreen;
    abstractScreen->activeIdx_ = -1;
    ASSERT_EQ(false, absDisplay->BindAbstractScreen(abstractScreen));
}
}
} // namespace Rosen
} // namespace OHOS

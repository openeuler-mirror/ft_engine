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

#include <gtest/gtest.h>
#include "screen_group.h"
#include "screen_manager.h"
#include "screen_group_info.h"
#include "mock_display_manager_adapter.h"
#include "singleton_mocker.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class ScreenGroupTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    sptr<ScreenGroup> group_;
};


void ScreenGroupTest::SetUpTestCase()
{
}

void ScreenGroupTest::TearDownTestCase()
{
}

void ScreenGroupTest::SetUp()
{
    group_ = ScreenManager::GetInstance().GetScreenGroup(0);
}

void ScreenGroupTest::TearDown()
{
}

namespace {
/**
 * @tc.name: UpdateScreenGroupInfo02
 * @tc.desc: test InterfaceToken check failed
 * @tc.type: FUNC
 */
HWTEST_F(ScreenGroupTest, UpdateScreenGroupInfo02, Function | SmallTest | Level2)
{
    sptr<ScreenGroupInfo> screenGroupInfo = new(std::nothrow) ScreenGroupInfo();
    sptr<ScreenGroup> screenGroup = new ScreenGroup(screenGroupInfo);
    SingletonMocker<ScreenManagerAdapter, MockScreenManagerAdapter> m;
    EXPECT_CALL(m.Mock(), GetScreenGroupInfoById(_)).Times(1).WillOnce(Return(nullptr));
    ScreenCombination comb = screenGroup->GetCombination();
    ASSERT_EQ(ScreenCombination::SCREEN_ALONE, comb);
}

/**
 * @tc.name: UpdateScreenGroupInfo01
 * @tc.desc: test InterfaceToken check success
 * @tc.type: FUNC
 */
HWTEST_F(ScreenGroupTest, UpdateScreenGroupInfo01, Function | SmallTest | Level2)
{
    sptr<ScreenGroupInfo> screenGroupInfo = new(std::nothrow) ScreenGroupInfo();
    sptr<ScreenGroup> screenGroup = new ScreenGroup(screenGroupInfo);
    screenGroupInfo->combination_ = ScreenCombination::SCREEN_EXPAND;
    SingletonMocker<ScreenManagerAdapter, MockScreenManagerAdapter> m;
    EXPECT_CALL(m.Mock(), GetScreenGroupInfoById(_)).Times(1).WillOnce(Return(screenGroupInfo));
    ScreenCombination comb = screenGroup->GetCombination();
    ASSERT_EQ(ScreenCombination::SCREEN_EXPAND, comb);
}

/**
 * @tc.name: UpdateScreenGroupInfo03
 * @tc.desc: test InterfaceToken check success
 * @tc.type: FUNC
 */
HWTEST_F(ScreenGroupTest, UpdateScreenGroupInfo03, Function | SmallTest | Level2)
{
    sptr<ScreenGroupInfo> screenGroupInfo = new(std::nothrow) ScreenGroupInfo();
    sptr<ScreenGroup> screenGroup = new ScreenGroup(screenGroupInfo);
    std::vector<Point> position;
    position.emplace_back(0, 0);
    screenGroupInfo->position_ = position;
    SingletonMocker<ScreenManagerAdapter, MockScreenManagerAdapter> m;
    EXPECT_CALL(m.Mock(), GetScreenGroupInfoById(_)).Times(1).WillOnce(Return(screenGroupInfo));
    std::vector<Point> pos = screenGroup->GetChildPositions();
    ASSERT_EQ(position.size(), pos.size());
}
/**
 * @tc.name: GetChildIds
 * @tc.desc: for interface coverage & check GetChildIds
 * @tc.type: FUNC
 */
HWTEST_F(ScreenGroupTest, GetChildIds, Function | SmallTest | Level2)
{
    sptr<ScreenGroupInfo> screenGroupInfo = new(std::nothrow) ScreenGroupInfo();
    sptr<ScreenGroup> screenGroup = new ScreenGroup(screenGroupInfo);
    auto result = screenGroup->GetChildIds();
    ASSERT_TRUE(result.empty());

    screenGroupInfo->children_.emplace_back(1);
    result = screenGroup->GetChildIds();
    ASSERT_EQ(result.size(), 1);
}
}
}
}
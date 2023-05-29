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

#include "screen_group_info.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class ScreenGroupInfoTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    virtual void SetUp() override;
    virtual void TearDown() override;
};

void ScreenGroupInfoTest::SetUpTestCase()
{
}

void ScreenGroupInfoTest::TearDownTestCase()
{
}

void ScreenGroupInfoTest::SetUp()
{
}

void ScreenGroupInfoTest::TearDown()
{
}

namespace {
/**
 * @tc.name: MarshallingUnmarshalling
 * @tc.desc: Marshalling Unmarshalling test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenGroupInfoTest, MarshallingUnmarshalling, Function | SmallTest | Level1)
{
    std::vector<ScreenId> screenIds;
    screenIds.push_back(1);
    screenIds.push_back(2);
    screenIds.push_back(3);
    ScreenGroupInfo screenGroupInfoSrc;
    screenGroupInfoSrc.SetChildren(screenIds);

    Parcel parcel;
    screenGroupInfoSrc.Marshalling(parcel);
    ScreenGroupInfo* screenGroupInfoDst = screenGroupInfoSrc.Unmarshalling(parcel);

    std::vector<ScreenId> screenIdsDst = screenGroupInfoDst->GetChildren();
    ASSERT_EQ(screenIdsDst.size(), 3);
    ASSERT_EQ(screenIdsDst[0], 1);
    ASSERT_EQ(screenIdsDst[1], 2);
    ASSERT_EQ(screenIdsDst[2], 3);
    delete screenGroupInfoDst;
}
}
} // namespace Rosen
} // namespace OHOS
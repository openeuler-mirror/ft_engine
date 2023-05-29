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

#include "display_info.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class DisplayInfoTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    virtual void SetUp() override;
    virtual void TearDown() override;
};

void DisplayInfoTest::SetUpTestCase()
{
}

void DisplayInfoTest::TearDownTestCase()
{
}

void DisplayInfoTest::SetUp()
{
}

void DisplayInfoTest::TearDown()
{
}

namespace {
/**
 * @tc.name: MarshallingUnmarshalling
 * @tc.desc: Marshalling Unmarshalling test
 * @tc.type: FUNC
 */
HWTEST_F(DisplayInfoTest, MarshallingUnmarshalling, Function | SmallTest | Level2)
{
    DisplayInfo displayInfoSrc;
    displayInfoSrc.SetDisplayId(1);

    Parcel parcel;
    displayInfoSrc.Marshalling(parcel);
    DisplayInfo* displayInfoDst = displayInfoSrc.Unmarshalling(parcel);

    ASSERT_EQ(displayInfoDst->GetDisplayId(), 1);
    delete displayInfoDst;
}
}
} // namespace Rosen
} // namespace OHOS
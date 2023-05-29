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

#include "screen_info.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class ScreenInfoTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    virtual void SetUp() override;
    virtual void TearDown() override;
};

void ScreenInfoTest::SetUpTestCase()
{
}

void ScreenInfoTest::TearDownTestCase()
{
}

void ScreenInfoTest::SetUp()
{
}

void ScreenInfoTest::TearDown()
{
}

namespace {
/**
 * @tc.name: MarshallingUnmarshalling
 * @tc.desc: Marshalling Unmarshalling test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenInfoTest, MarshallingUnmarshalling, Function | SmallTest | Level2)
{
    ScreenInfo screenInfoSrc;
    screenInfoSrc.SetScreenId(1);

    Parcel parcel;
    screenInfoSrc.Marshalling(parcel);
    ScreenInfo* screenInfoDst = screenInfoSrc.Unmarshalling(parcel);

    ASSERT_EQ(screenInfoDst->GetScreenId(), 1);
    delete screenInfoDst;
}
}
} // namespace Rosen
} // namespace OHOS
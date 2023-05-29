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
#include "cutout_info.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class CutoutInfoTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void CutoutInfoTest::SetUpTestCase()
{
}

void CutoutInfoTest::TearDownTestCase()
{
}

void CutoutInfoTest::SetUp()
{
}

void CutoutInfoTest::TearDown()
{
}

namespace {
/**
 * @tc.name: WriteBoundingRectsVector
 * @tc.desc: WriteBoundingRectsVector test
 * @tc.type: FUNC
 */
HWTEST_F(CutoutInfoTest, WriteBoundingRectsVector, Function | SmallTest | Level2)
{
    sptr<CutoutInfo> info = new CutoutInfo();
    DMRect rect = {0, 0, 0, 0};
    std::vector<DMRect> boundingRects;
    boundingRects.emplace_back(rect);
    Parcel parcel;
    bool ret = info->WriteBoundingRectsVector(boundingRects, parcel);
    ASSERT_TRUE(ret);
}
}
} // namespace Rosen
} // namespace OHOS
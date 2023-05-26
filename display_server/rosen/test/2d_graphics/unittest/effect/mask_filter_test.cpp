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

#include "gtest/gtest.h"

#include "effect/mask_filter.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class MaskFilterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void MaskFilterTest::SetUpTestCase() {}
void MaskFilterTest::TearDownTestCase() {}
void MaskFilterTest::SetUp() {}
void MaskFilterTest::TearDown() {}

/**
 * @tc.name: CreateBlurMaskFilter001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(MaskFilterTest, CreateBlurMaskFilter001, TestSize.Level1)
{
    auto maskFilter = MaskFilter::CreateBlurMaskFilter(BlurType::NORMAL, 0.5f);
    EXPECT_TRUE(maskFilter != nullptr);
}

/**
 * @tc.name: CreateBlurMaskFilter002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(MaskFilterTest, CreateBlurMaskFilter002, TestSize.Level1)
{
    auto maskFilter = MaskFilter::CreateBlurMaskFilter(BlurType::OUTER, 0.2f);
    EXPECT_TRUE(maskFilter != nullptr);
}

/**
 * @tc.name: NoAgrsConstructor001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(MaskFilterTest, NoAgrsConstructor001, TestSize.Level1)
{
    auto maskFilter = std::make_unique<MaskFilter>(MaskFilter::FilterType::BLUR, BlurType::NORMAL, 0.2f);
    EXPECT_TRUE(maskFilter != nullptr);
}

/**
 * @tc.name: NoAgrsConstructor002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(MaskFilterTest, NoAgrsConstructor002, TestSize.Level1)
{
    auto maskFilter = std::make_unique<MaskFilter>(MaskFilter::FilterType::NO_TYPE, BlurType::NORMAL, 0.4f);
    EXPECT_TRUE(maskFilter != nullptr);
}

/**
 * @tc.name: AgrsConstructor001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(MaskFilterTest, AgrsConstructor001, TestSize.Level1)
{
    MaskFilter::FilterType filterType = MaskFilter::FilterType::NO_TYPE;
    auto maskFilter = std::make_unique<MaskFilter>(filterType, BlurType::SOLID, 0.5f);
    ASSERT_TRUE(maskFilter != nullptr);
    EXPECT_EQ(filterType, maskFilter->GetType());
}

/**
 * @tc.name: AgrsConstructor002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(MaskFilterTest, AgrsConstructor002, TestSize.Level1)
{
    MaskFilter::FilterType filterType = MaskFilter::FilterType::BLUR;
    auto maskFilter = std::make_unique<MaskFilter>(filterType, BlurType::OUTER, 15.5f);
    ASSERT_TRUE(maskFilter != nullptr);
    EXPECT_EQ(filterType, maskFilter->GetType());
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

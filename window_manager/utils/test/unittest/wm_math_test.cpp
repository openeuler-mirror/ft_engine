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

#include "wm_math.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
using namespace TransformHelper;
class WmMathTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    virtual void SetUp() override;
    virtual void TearDown() override;
};

void WmMathTest::SetUpTestCase()
{
}

void WmMathTest::TearDownTestCase()
{
}

void WmMathTest::SetUp()
{
}

void WmMathTest::TearDown()
{
}

namespace {
/**
 * @tc.name: MathHalper
 * @tc.desc: MathHalper test
 * @tc.type: FUNC
 */
HWTEST_F(WmMathTest, MathHalper, Function | SmallTest | Level2)
{
    {
        const float t = 0.5f;
        ASSERT_EQ(true, MathHelper::NearZero(0));
        ASSERT_EQ(true, MathHelper::NearZero(t * MathHelper::NAG_ZERO));
        ASSERT_EQ(true, MathHelper::NearZero(t * MathHelper::POS_ZERO));
    }
    {
        float radians = MathHelper::PI, degrees = 180.f;
        ASSERT_EQ(true, MathHelper::NearZero(MathHelper::ToDegrees(radians) - degrees));
        ASSERT_EQ(true, MathHelper::NearZero(MathHelper::ToRadians(degrees) - radians));
    }
    {
        int a = 1, b = 2, c = 3, d = 4;
        ASSERT_EQ(true, MathHelper::Max(a, b, c) == c);
        ASSERT_EQ(true, MathHelper::Min(a, b, c) == a);
        ASSERT_EQ(true, MathHelper::Max(a, b, c, d) == d);
        ASSERT_EQ(true, MathHelper::Min(a, b, c, d) == a);
        ASSERT_EQ(true, MathHelper::Clamp(a, b, c) == b);
        ASSERT_EQ(true, MathHelper::Clamp(b, a, c) == b);
        ASSERT_EQ(true, MathHelper::Clamp(c, a, b) == b);
    }
}

/**
 * @tc.name: TransformMatrix
 * @tc.desc: Create transform matrix
 *           Get scale component from transform matrix
 *           Get translation component from transform matrix
 * @tc.type: FUNC
 */
HWTEST_F(WmMathTest, TransformMatrix, Function | SmallTest | Level2)
{
    Vector3 scale(1.5f, 0.7f, 2.2f), translation(100.f, 132.f, 20.f);
    Matrix4 transformMat = CreateScale(scale.x_, scale.y_, scale.z_);
    float theta = 2.34f;
    transformMat *= CreateRotationY(theta);
    transformMat *= CreateTranslation(translation);
    Vector3 scaleComp = transformMat.GetScale();
    Vector3 translationComp = transformMat.GetTranslation();
    ASSERT_EQ(true, MathHelper::NearZero((scale - scaleComp).Length()));
    ASSERT_EQ(true, MathHelper::NearZero((translation - translationComp).Length()));
}
/**
 * @tc.name: TransformWithPerspDiv
 * @tc.desc: Create transform matrix
 *           Get scale component from transform matrix
 *           Get translation component from transform matrix
 * @tc.type: FUNC
 */
HWTEST_F(WmMathTest, TransformWithPerspDiv, Function | SmallTest | Level2)
{
    Vector3 vec(1.0, 1.0, 1.0);
    Matrix4 mat = Matrix4::Identity;
    auto result = TransformWithPerspDiv(vec, mat, 0.5);
    auto expect = vec * 2;
    ASSERT_EQ(expect.x_, result.x_);
    ASSERT_EQ(expect.y_, result.y_);
    ASSERT_EQ(expect.z_, result.z_);

    result = TransformWithPerspDiv(vec, mat, 0);
    ASSERT_EQ(vec.x_, result.x_);
    ASSERT_EQ(vec.y_, result.y_);
    ASSERT_EQ(vec.z_, result.z_);
}

/**
 * @tc.name: Invert
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(WmMathTest, Invert, Function | SmallTest | Level2)
{
    Matrix4 mat;
    mat.mat_[0][0] = 0.f;
    mat.mat_[1][0] = -1.0f;
    mat.Invert();
    ASSERT_EQ(false, MathHelper::NearZero(0.f - mat.mat_[1][0]));
}
}
} // namespace Rosen
} // namespace OHOS
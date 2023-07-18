/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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
#include "ui/rs_surface_node.h"
#include "limit_number.h"
static constexpr uint32_t FIRST_COLOR_VALUE = 0x034123;
static constexpr uint32_t SECOND_COLOR_VALUE = 0x45ba87;
static constexpr uint32_t THIRD_COLOR_VALUE = 0x32aadd;
namespace OHOS::Rosen {
class RSSurfaceNodeTest : public testing::Test {
public:
    constexpr static float floatData[] = {
        0.0f, 485.44f, -34.4f,
        std::numeric_limits<float>::max(), std::numeric_limits<float>::min(),
        };
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSSurfaceNodeTest::SetUpTestCase() {}
void RSSurfaceNodeTest::TearDownTestCase() {}
void RSSurfaceNodeTest::SetUp() {}
void RSSurfaceNodeTest::TearDown() {}

/**
 * @tc.name: Create001
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSSurfaceNodeTest, Create001)
{
    RSSurfaceNodeConfig c;

    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    ASSERT_TRUE(surfaceNode != nullptr);

    auto surface = surfaceNode->GetSurface();
    ASSERT_TRUE(surface != nullptr);
}

/**
 * @tc.name: CreateNodeInRenderThread001
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSSurfaceNodeTest, CreateNodeInRenderThread001)
{
    RSSurfaceNodeConfig c;

    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    ASSERT_TRUE(surfaceNode != nullptr);

    surfaceNode->CreateNodeInRenderThread();
}

/**
 * @tc.name: SetBufferAvailableCallback001
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSSurfaceNodeTest, SetBufferAvailableCallback001)
{
    RSSurfaceNodeConfig c;

    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    ASSERT_TRUE(surfaceNode != nullptr);

    bool isSuccess = surfaceNode->SetBufferAvailableCallback([]() {
        std::cout << "SetBufferAvailableCallback" << std::endl;
    });
    ASSERT_TRUE(isSuccess);
}

/**
 * @tc.name: SetandGetBounds001
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSSurfaceNodeTest, SetandGetBounds001)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetBounds(TestSrc::limitNumber::floatLimit[0], TestSrc::limitNumber::floatLimit[1],
        TestSrc::limitNumber::floatLimit[2], TestSrc::limitNumber::floatLimit[3]);
    auto bounds = surfaceNode->GetStagingProperties().GetBounds();
    EXPECT_TRUE(ROSEN_EQ(bounds.x_, TestSrc::limitNumber::floatLimit[0]));
    EXPECT_TRUE(ROSEN_EQ(bounds.y_, TestSrc::limitNumber::floatLimit[1]));
    EXPECT_TRUE(ROSEN_EQ(bounds.z_, TestSrc::limitNumber::floatLimit[2]));
    EXPECT_TRUE(ROSEN_EQ(bounds.w_, TestSrc::limitNumber::floatLimit[3]));
}

/**
 * @tc.name: SetandGetBounds002
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSSurfaceNodeTest, SetandGetBounds002)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetBounds(TestSrc::limitNumber::floatLimit[3], TestSrc::limitNumber::floatLimit[1],
        TestSrc::limitNumber::floatLimit[2], TestSrc::limitNumber::floatLimit[0]);
    auto bounds = surfaceNode->GetStagingProperties().GetBounds();
    EXPECT_TRUE(ROSEN_EQ(bounds.x_, TestSrc::limitNumber::floatLimit[3]));
    EXPECT_TRUE(ROSEN_EQ(bounds.y_, TestSrc::limitNumber::floatLimit[1]));
    EXPECT_TRUE(ROSEN_EQ(bounds.z_, TestSrc::limitNumber::floatLimit[2]));
    EXPECT_TRUE(ROSEN_EQ(bounds.w_, TestSrc::limitNumber::floatLimit[0]));
}

/**
 * @tc.name: SetandGetBounds003
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSSurfaceNodeTest, SetandGetBounds003)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetBounds(TestSrc::limitNumber::floatLimit[3], TestSrc::limitNumber::floatLimit[2],
        TestSrc::limitNumber::floatLimit[1], TestSrc::limitNumber::floatLimit[0]);
    auto bounds = surfaceNode->GetStagingProperties().GetBounds();
    EXPECT_TRUE(ROSEN_EQ(bounds.x_, TestSrc::limitNumber::floatLimit[3]));
    EXPECT_TRUE(ROSEN_EQ(bounds.y_, TestSrc::limitNumber::floatLimit[2]));
    EXPECT_TRUE(ROSEN_EQ(bounds.z_, TestSrc::limitNumber::floatLimit[1]));
    EXPECT_TRUE(ROSEN_EQ(bounds.w_, TestSrc::limitNumber::floatLimit[0]));
}

/**
 * @tc.name: SetandGetBounds004
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSSurfaceNodeTest, SetandGetBounds004)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    Vector4f quaternion(TestSrc::limitNumber::floatLimit[0], TestSrc::limitNumber::floatLimit[1],
        TestSrc::limitNumber::floatLimit[2], TestSrc::limitNumber::floatLimit[3]);
    surfaceNode->SetBounds(quaternion);
    auto bounds = surfaceNode->GetStagingProperties().GetBounds();
    EXPECT_TRUE(ROSEN_EQ(bounds.x_, TestSrc::limitNumber::floatLimit[0]));
    EXPECT_TRUE(ROSEN_EQ(bounds.y_, TestSrc::limitNumber::floatLimit[1]));
    EXPECT_TRUE(ROSEN_EQ(bounds.z_, TestSrc::limitNumber::floatLimit[2]));
    EXPECT_TRUE(ROSEN_EQ(bounds.w_, TestSrc::limitNumber::floatLimit[3]));
}

/**
 * @tc.name: SetandGetBounds005
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSSurfaceNodeTest, SetandGetBounds005)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    Vector4f quaternion(TestSrc::limitNumber::floatLimit[3], TestSrc::limitNumber::floatLimit[1],
        TestSrc::limitNumber::floatLimit[2], TestSrc::limitNumber::floatLimit[0]);
    surfaceNode->SetBounds(quaternion);
    auto bounds = surfaceNode->GetStagingProperties().GetBounds();
    EXPECT_TRUE(ROSEN_EQ(bounds.x_, TestSrc::limitNumber::floatLimit[3]));
    EXPECT_TRUE(ROSEN_EQ(bounds.y_, TestSrc::limitNumber::floatLimit[1]));
    EXPECT_TRUE(ROSEN_EQ(bounds.z_, TestSrc::limitNumber::floatLimit[2]));
    EXPECT_TRUE(ROSEN_EQ(bounds.w_, TestSrc::limitNumber::floatLimit[0]));
}

/**
 * @tc.name: SetandGetBounds006
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSSurfaceNodeTest, SetandGetBounds006)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    Vector4f quaternion(TestSrc::limitNumber::floatLimit[3], TestSrc::limitNumber::floatLimit[2],
        TestSrc::limitNumber::floatLimit[1], TestSrc::limitNumber::floatLimit[0]);
    surfaceNode->SetBounds(quaternion);
    auto bounds = surfaceNode->GetStagingProperties().GetBounds();
    EXPECT_TRUE(ROSEN_EQ(bounds.x_, TestSrc::limitNumber::floatLimit[3]));
    EXPECT_TRUE(ROSEN_EQ(bounds.y_, TestSrc::limitNumber::floatLimit[2]));
    EXPECT_TRUE(ROSEN_EQ(bounds.z_, TestSrc::limitNumber::floatLimit[1]));
    EXPECT_TRUE(ROSEN_EQ(bounds.w_, TestSrc::limitNumber::floatLimit[0]));
}

/**
 * @tc.name: SetandGetBoundsWidth001
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSSurfaceNodeTest, SetandGetBoundsWidth001)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetBoundsWidth(TestSrc::limitNumber::floatLimit[1]);
    auto bounds = surfaceNode->GetStagingProperties().GetBounds();
    EXPECT_TRUE(ROSEN_EQ(bounds.z_, TestSrc::limitNumber::floatLimit[1]));
}

/**
 * @tc.name: SetandGetBoundsWidth002
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSSurfaceNodeTest, SetandGetBoundsWidth002)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetBoundsWidth(TestSrc::limitNumber::floatLimit[2]);
    auto bounds = surfaceNode->GetStagingProperties().GetBounds();
    EXPECT_TRUE(ROSEN_EQ(bounds.z_, TestSrc::limitNumber::floatLimit[2]));
}

/**
 * @tc.name: SetandGetBoundsWidth003
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSSurfaceNodeTest, SetandGetBoundsWidth003)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetBoundsWidth(TestSrc::limitNumber::floatLimit[3]);
    auto bounds = surfaceNode->GetStagingProperties().GetBounds();
    EXPECT_TRUE(ROSEN_EQ(bounds.z_, TestSrc::limitNumber::floatLimit[3]));
}

/**
 * @tc.name: SetandGetBoundsWidth004
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSSurfaceNodeTest, SetandGetBoundsWidth004)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetBoundsWidth(TestSrc::limitNumber::floatLimit[4]);
    auto bounds = surfaceNode->GetStagingProperties().GetBounds();
    EXPECT_TRUE(ROSEN_EQ(bounds.z_, TestSrc::limitNumber::floatLimit[4]));
}

/**
 * @tc.name: SetandGetBoundsWidth005
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSSurfaceNodeTest, SetandGetBoundsWidth005)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetBoundsWidth(TestSrc::limitNumber::floatLimit[0]);
    auto bounds = surfaceNode->GetStagingProperties().GetBounds();
    EXPECT_TRUE(ROSEN_EQ(bounds.z_, TestSrc::limitNumber::floatLimit[0]));
}

/**
 * @tc.name: SetandGetBoundsHeight001
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSSurfaceNodeTest, SetandGetBoundsHeight001)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetBoundsHeight(TestSrc::limitNumber::floatLimit[1]);
    auto bounds = surfaceNode->GetStagingProperties().GetBounds();
    EXPECT_TRUE(ROSEN_EQ(bounds.w_, TestSrc::limitNumber::floatLimit[1]));
}

/**
 * @tc.name: SetandGetBoundsHeight002
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSSurfaceNodeTest, SetandGetBoundsHeight002)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetBoundsHeight(TestSrc::limitNumber::floatLimit[2]);
    auto bounds = surfaceNode->GetStagingProperties().GetBounds();
    EXPECT_TRUE(ROSEN_EQ(bounds.w_, TestSrc::limitNumber::floatLimit[2]));
}

/**
 * @tc.name: SetandGetBoundsHeight003
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSSurfaceNodeTest, SetandGetBoundsHeight003)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetBoundsHeight(TestSrc::limitNumber::floatLimit[3]);
    auto bounds = surfaceNode->GetStagingProperties().GetBounds();
    EXPECT_TRUE(ROSEN_EQ(bounds.w_, TestSrc::limitNumber::floatLimit[3]));
}

/**
 * @tc.name: SetandGetBoundsHeight004
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSSurfaceNodeTest, SetandGetBoundsHeight004)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetBoundsHeight(TestSrc::limitNumber::floatLimit[4]);
    auto bounds = surfaceNode->GetStagingProperties().GetBounds();
    EXPECT_TRUE(ROSEN_EQ(bounds.w_, TestSrc::limitNumber::floatLimit[4]));
}

/**
 * @tc.name: SetandGetBoundsHeight005
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSSurfaceNodeTest, SetandGetBoundsHeight005)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetBoundsHeight(TestSrc::limitNumber::floatLimit[0]);
    auto bounds = surfaceNode->GetStagingProperties().GetBounds();
    EXPECT_TRUE(ROSEN_EQ(bounds.w_, TestSrc::limitNumber::floatLimit[0]));
}

/**
 * @tc.name: SetSecurityLayer001
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSSurfaceNodeTest, SetSecurityLayer001)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetSecurityLayer(true);
    EXPECT_TRUE(surfaceNode->GetSecurityLayer());
}

/**
 * @tc.name: SetSecurityLayer002
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSSurfaceNodeTest, SetSecurityLayer002)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetSecurityLayer(false);
    EXPECT_FALSE(surfaceNode->GetSecurityLayer());
}

/**
 * @tc.name: GetSecurityLayer001
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSSurfaceNodeTest, GetSecurityLayer001)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    EXPECT_FALSE(surfaceNode->GetSecurityLayer());
}

/**
 * @tc.name: Marshalling001
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSSurfaceNodeTest, Marshalling001)
{
    RSSurfaceNodeConfig c;

    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    ASSERT_TRUE(surfaceNode != nullptr);

    Parcel parcel;
    surfaceNode->Marshalling(parcel);
}

/**
 * @tc.name: Marshalling002
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSSurfaceNodeTest, Marshalling002)
{
    Parcel parcel;
    auto surfaceNode = RSSurfaceNode::Unmarshalling(parcel);
    EXPECT_TRUE(surfaceNode == nullptr);
}

/**
 * @tc.name: Marshalling003
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSSurfaceNodeTest, Marshalling003)
{
    RSSurfaceNodeConfig c;

    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    ASSERT_TRUE(surfaceNode != nullptr);

    Parcel parcel;
    surfaceNode->Marshalling(parcel);
    RSSurfaceNode::Unmarshalling(parcel);
}

/**
 * @tc.name: Marshalling004
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSSurfaceNodeTest, Marshalling004)
{
    RSSurfaceNodeConfig c;

    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    ASSERT_TRUE(surfaceNode != nullptr);

    Parcel parcel;
    surfaceNode->Marshalling(parcel);
    RSSurfaceNode::Unmarshalling(parcel);
    RSSurfaceNode::Unmarshalling(parcel);
}

/**
 * @tc.name: Marshalling005
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSSurfaceNodeTest, Marshalling005)
{
    RSSurfaceNodeConfig c;

    RSSurfaceNode::SharedPtr surfaceNode1 = RSSurfaceNode::Create(c);
    ASSERT_TRUE(surfaceNode1 != nullptr);
    RSSurfaceNode::SharedPtr surfaceNode2 = RSSurfaceNode::Create(c);
    ASSERT_TRUE(surfaceNode2 != nullptr);

    Parcel parcel;
    surfaceNode1->Marshalling(parcel);
    surfaceNode2->Marshalling(parcel);
    RSSurfaceNode::Unmarshalling(parcel);
}

/**
 * @tc.name: GetSurface001
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSSurfaceNodeTest, GetSurface001)
{
    RSSurfaceNodeConfig c;

    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    ASSERT_TRUE(surfaceNode != nullptr);

    surfaceNode->GetSurface();
}

/**
 * @tc.name: GetType001
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSSurfaceNodeTest, GetType001)
{
    RSSurfaceNodeConfig c;

    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    ASSERT_TRUE(surfaceNode != nullptr);
    ASSERT_TRUE(surfaceNode->GetType() == RSUINodeType::SURFACE_NODE);
}

/**
 * @tc.name: SetCornerRadius001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI5J8R1
 */
TEST_F(RSSurfaceNodeTest, SetCornerRadius001)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetCornerRadius(floatData[0]);
    ASSERT_TRUE(surfaceNode != nullptr);
    auto result = surfaceNode->GetStagingProperties().GetCornerRadius()[0];
    EXPECT_TRUE(ROSEN_EQ(floatData[0], result));
}

/**
 * @tc.name: SetCornerRadius002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI5J8R1
 */
TEST_F(RSSurfaceNodeTest, SetCornerRadius002)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetCornerRadius(floatData[1]);
    ASSERT_TRUE(surfaceNode != nullptr);
    auto result = surfaceNode->GetStagingProperties().GetCornerRadius()[1];
    EXPECT_TRUE(ROSEN_EQ(floatData[1], result));
}

/**
 * @tc.name: SetCornerRadius003
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI5J8R1
 */
TEST_F(RSSurfaceNodeTest, SetCornerRadius003)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetCornerRadius(floatData[2]);
    ASSERT_TRUE(surfaceNode != nullptr);
    auto result = surfaceNode->GetStagingProperties().GetCornerRadius()[2];
    EXPECT_TRUE(ROSEN_EQ(floatData[2], result));
}

/**
 * @tc.name: SetBackgroundFilter001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI5J8R1
 */
TEST_F(RSSurfaceNodeTest, SetBackgroundFilter001)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    std::shared_ptr<RSFilter> backgroundFilter = RSFilter::CreateBlurFilter(floatData[0],
    floatData[1]);
    surfaceNode->SetBackgroundFilter(backgroundFilter);
    EXPECT_TRUE(surfaceNode->GetStagingProperties().GetBackgroundFilter() == backgroundFilter);
}

/**
 * @tc.name: SetBackgroundFilter002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI5J8R1
 */
TEST_F(RSSurfaceNodeTest, SetBackgroundFilter002)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    std::shared_ptr<RSFilter> backgroundFilter = RSFilter::CreateBlurFilter(floatData[1],
    floatData[2]);
    surfaceNode->SetBackgroundFilter(backgroundFilter);
    EXPECT_TRUE(surfaceNode->GetStagingProperties().GetBackgroundFilter() == backgroundFilter);
}

/**
 * @tc.name: SetBackgroundFilter003
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI5J8R1
 */
TEST_F(RSSurfaceNodeTest, SetBackgroundFilter003)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    std::shared_ptr<RSFilter> backgroundFilter = RSFilter::CreateBlurFilter(floatData[2],
    floatData[3]);
    surfaceNode->SetBackgroundFilter(backgroundFilter);
    EXPECT_TRUE(surfaceNode->GetStagingProperties().GetBackgroundFilter() == backgroundFilter);
}

/**
 * @tc.name: SetFilter001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI5J8R1
 */
TEST_F(RSSurfaceNodeTest, SetFilter001)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    std::shared_ptr<RSFilter> filter = RSFilter::CreateBlurFilter(floatData[0],
    floatData[1]);
    surfaceNode->SetFilter(filter);
    EXPECT_TRUE(surfaceNode->GetStagingProperties().GetFilter() == filter);
}

/**
 * @tc.name: SetFilter002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI5J8R1
 */
TEST_F(RSSurfaceNodeTest, SetFilter002)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    std::shared_ptr<RSFilter> filter = RSFilter::CreateBlurFilter(floatData[1],
    floatData[2]);
    surfaceNode->SetFilter(filter);
    EXPECT_TRUE(surfaceNode->GetStagingProperties().GetFilter() == filter);
}

/**
 * @tc.name: SetFilter003
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI5J8R1
 */
TEST_F(RSSurfaceNodeTest, SetFilter003)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    std::shared_ptr<RSFilter> filter = RSFilter::CreateBlurFilter(floatData[2],
    floatData[3]);
    surfaceNode->SetFilter(filter);
    EXPECT_TRUE(surfaceNode->GetStagingProperties().GetFilter() == filter);
}

/**
 * @tc.name: SetCompositingFilter001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI5J8R1
 */
TEST_F(RSSurfaceNodeTest, SetCompositingFilter001)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    ASSERT_TRUE(surfaceNode != nullptr);
    auto compositingFilter = RSFilter::CreateBlurFilter(floatData[0],
    floatData[1]);
    surfaceNode->SetCompositingFilter(compositingFilter);
}

/**
 * @tc.name: SetCompositingFilter002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI5J8R1
 */
TEST_F(RSSurfaceNodeTest, SetCompositingFilter002)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    ASSERT_TRUE(surfaceNode != nullptr);
    auto compositingFilter = RSFilter::CreateBlurFilter(floatData[1],
    floatData[2]);
    surfaceNode->SetCompositingFilter(compositingFilter);
}

/**
 * @tc.name: SetCompositingFilter003
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI5J8R1
 */
TEST_F(RSSurfaceNodeTest, SetCompositingFilter003)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    ASSERT_TRUE(surfaceNode != nullptr);
    auto compositingFilter = RSFilter::CreateBlurFilter(floatData[2],
    floatData[3]);
    surfaceNode->SetCompositingFilter(compositingFilter);
}

/**
 * @tc.name: SetShadowOffset001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI5J8R1
 */
TEST_F(RSSurfaceNodeTest, SetShadowOffset001)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetShadowOffset(floatData[2], floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetShadowOffsetX(), floatData[2]));
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetShadowOffsetY(), floatData[3]));
}

/**
 * @tc.name: SetShadowOffset002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI5J8R1
 */
TEST_F(RSSurfaceNodeTest, SetShadowOffset002)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetShadowOffset(floatData[2], floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetShadowOffsetX(), floatData[2]));
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetShadowOffsetY(), floatData[0]));
}

/**
 * @tc.name: SetShadowOffset003
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI5J8R1
 */
TEST_F(RSSurfaceNodeTest, SetShadowOffset003)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetShadowOffset(floatData[1], floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetShadowOffsetX(), floatData[1]));
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetShadowOffsetY(), floatData[3]));
}
/**
 * @tc.name: SetShadowOffsetX001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI5J8R1
 */
TEST_F(RSSurfaceNodeTest, SetShadowOffsetX001)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetShadowOffsetX(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetShadowOffsetX(), floatData[1]));
}

/**
 * @tc.name: SetShadowOffsetX002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI5J8R1
 */
TEST_F(RSSurfaceNodeTest, SetShadowOffsetX002)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetShadowOffsetX(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetShadowOffsetX(), floatData[2]));
}

/**
 * @tc.name: SetShadowOffsetX003
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI5J8R1
 */
TEST_F(RSSurfaceNodeTest, SetShadowOffsetX003)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetShadowOffsetX(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetShadowOffsetX(), floatData[3]));
}

/**
 * @tc.name: SetShadowOffsetY001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI5J8R1
 */
TEST_F(RSSurfaceNodeTest, SetShadowOffsetY001)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetShadowOffsetY(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetShadowOffsetY(), floatData[1]));
}

/**
 * @tc.name: SetShadowOffsetY002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI5J8R1
 */
TEST_F(RSSurfaceNodeTest, SetShadowOffsetY002)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetShadowOffsetY(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetShadowOffsetY(), floatData[2]));
}

/**
 * @tc.name: SetShadowOffsetY003
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI5J8R1
 */
TEST_F(RSSurfaceNodeTest, SetShadowOffsetY003)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetShadowOffsetY(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetShadowOffsetY(), floatData[3]));
}

/**
 * @tc.name: SetShadowAlpha001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI5J8R1
 */
TEST_F(RSSurfaceNodeTest, SetShadowAlpha001)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetShadowAlpha(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetShadowAlpha(), floatData[1]));
}

/**
 * @tc.name: SetShadowAlpha002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI5J8R1
 */
TEST_F(RSSurfaceNodeTest, SetShadowAlpha002)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetShadowAlpha(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetShadowAlpha(), floatData[2]));
}

/**
 * @tc.name: SetShadowAlpha003
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI5J8R1
 */
TEST_F(RSSurfaceNodeTest, SetShadowAlpha003)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetShadowAlpha(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetShadowAlpha(), floatData[3]));
}

/**
 * @tc.name: SetShadowElevation001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI5J8R1
 */
TEST_F(RSSurfaceNodeTest, SetShadowElevation001)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetShadowElevation(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetShadowElevation(), floatData[1]));
}

/**
 * @tc.name: SetShadowElevation002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI5J8R1
 */
TEST_F(RSSurfaceNodeTest, SetShadowElevation002)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetShadowElevation(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetShadowElevation(), floatData[2]));
}

/**
 * @tc.name: SetShadowElevation003
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI5J8R1
 */
TEST_F(RSSurfaceNodeTest, SetShadowElevation003)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetShadowElevation(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetShadowElevation(), floatData[3]));
}

/**
 * @tc.name: SetShadowRadius001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI5J8R1
 */
TEST_F(RSSurfaceNodeTest, SetShadowRadius001)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetShadowRadius(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetShadowRadius(), floatData[1]));
}

/**
 * @tc.name: SetShadowRadius002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI5J8R1
 */
TEST_F(RSSurfaceNodeTest, SetShadowRadius002)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetShadowRadius(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetShadowRadius(), floatData[2]));
}

/**
 * @tc.name: SetShadowRadius003
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI5J8R1
 */
TEST_F(RSSurfaceNodeTest, SetShadowRadius003)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetShadowRadius(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetShadowRadius(), floatData[3]));
}

/**
 * @tc.name: SetShadowColor001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI5J8R1
 */
TEST_F(RSSurfaceNodeTest, SetShadowColor001)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetShadowColor(FIRST_COLOR_VALUE);
    EXPECT_TRUE(surfaceNode->GetStagingProperties().GetShadowColor() == Color::FromArgbInt(FIRST_COLOR_VALUE));
}

/**
 * @tc.name: SetShadowColor002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI5J8R1
 */
TEST_F(RSSurfaceNodeTest, SetShadowColor002)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetShadowColor(SECOND_COLOR_VALUE);
    EXPECT_TRUE(surfaceNode->GetStagingProperties().GetShadowColor() == Color::FromArgbInt(SECOND_COLOR_VALUE));
}

/**
 * @tc.name: SetShadowColor003
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI5J8R1
 */
TEST_F(RSSurfaceNodeTest, SetShadowColor003)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetShadowColor(THIRD_COLOR_VALUE);
    EXPECT_TRUE(surfaceNode->GetStagingProperties().GetShadowColor() == Color::FromArgbInt(THIRD_COLOR_VALUE));
}

/**
 * @tc.name: SetAppFreeze001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI5J8R1
 */
TEST_F(RSSurfaceNodeTest, SetAppFreeze001)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetAppFreeze(true);
}

/**
 * @tc.name: SetContainerWindow001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI5J8R1
 */
TEST_F(RSSurfaceNodeTest, SetContainerWindow001)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetContainerWindow(true, 1.f);
}

/**
 * @tc.name: SetIsNotifyUIBufferAvailable001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI5J8R1
 */
TEST_F(RSSurfaceNodeTest, SetIsNotifyUIBufferAvailable001)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetIsNotifyUIBufferAvailable(true);
}

/**
 * @tc.name: ClearChildren001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI5J8R1
 */
TEST_F(RSSurfaceNodeTest, ClearChildren001)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->CreateNodeInRenderThread();
    RSSurfaceNodeConfig c2;
    RSSurfaceNode::SharedPtr surfaceNode2 = RSSurfaceNode::Create(c2);
    surfaceNode->AddChild(surfaceNode2, -1);
    surfaceNode->RemoveChild(surfaceNode2);
    surfaceNode->ClearChildren();
}

/**
 * @tc.name: SetAppFreeze Test True
 * @tc.desc: SetAppFreeze Test True
 * @tc.type: FUNC
 * @tc.require:
 */
TEST_F(RSSurfaceNodeTest, SetAppFreeze_True)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->SetAppFreeze(true);
}

/**
 * @tc.name: SetAppFreeze Test False
 * @tc.desc: SetAppFreeze Test False
 * @tc.type: FUNC
 * @tc.require:
 */
TEST_F(RSSurfaceNodeTest, SetAppFreeze_False)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->SetAppFreeze(false);
}

/**
 * @tc.name: SetContainerWindow Test True
 * @tc.desc: SetContainerWindow Test True
 * @tc.type: FUNC
 * @tc.require:
 */
TEST_F(RSSurfaceNodeTest, SetContainerWindow_True)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->SetContainerWindow(true, 1.0f);
}

/**
 * @tc.name: SetContainerWindow Test False
 * @tc.desc: SetContainerWindow Test False
 * @tc.type: FUNC
 * @tc.require:
 */
TEST_F(RSSurfaceNodeTest, SetContainerWindow_False)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->SetContainerWindow(false, 1.0f);
}
} // namespace OHOS::Rosen
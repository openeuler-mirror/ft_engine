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

#include "wm_occlusion_region.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace WmOcclusion {
class WmOcclusionRegionTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void WmOcclusionRegionTest::SetUpTestCase()
{
}

void WmOcclusionRegionTest::TearDownTestCase()
{
}

void WmOcclusionRegionTest::SetUp()
{
}

void WmOcclusionRegionTest::TearDown()
{
}

namespace {
/**
 * @tc.name: EventSortByY01
 * @tc.desc: test WmOcclusion::EventSortByY
 * @tc.type: FUNC
 */
HWTEST_F(WmOcclusionRegionTest, EventSortByY, Function | SmallTest | Level2)
{
    Event event1{ 0, Event::Type::OPEN, 0, 0 };
    Event event2{ 0, Event::Type::OPEN, 0, 0 };
    bool result = true;

    event1.y_ = 1;
    event1.type_ = Event::Type::CLOSE;
    event2.y_ = 1;
    event2.type_ = Event::Type::OPEN;
    result = EventSortByY(event1, event2);
    ASSERT_EQ(true, result);

    event1.y_ = 1;
    event1.type_ = Event::Type::OPEN;
    event2.y_ = 1;
    event2.type_ = Event::Type::CLOSE;
    result = EventSortByY(event1, event2);
    ASSERT_EQ(false, result);

    event1.y_ = 1;
    event2.y_ = 2;
    result = EventSortByY(event1, event2);
    ASSERT_EQ(true, result);

    event1.y_ = 2;
    event2.y_ = 1;
    result = EventSortByY(event1, event2);
    ASSERT_EQ(false, result);
}

/**
 * @tc.name: Node::Update01
 * @tc.desc: test WmOcclusion::Node::Update
 * @tc.type: FUNC
 */
HWTEST_F(WmOcclusionRegionTest, Update01, Function | SmallTest | Level2)
{
    Node* rootNode = new Node(0, 2);
    ASSERT_NE(rootNode, nullptr);
    rootNode->positive_count_ = Event::Type::CLOSE;
    rootNode->Update(0, 0, Event::Type::OPEN);
    ASSERT_EQ(Event::Type::CLOSE, rootNode->positive_count_);

    rootNode->positive_count_ = Event::Type::CLOSE;
    rootNode->Update(0, 2, Event::Type::OPEN);
    ASSERT_EQ(0, rootNode->positive_count_);

    rootNode->positive_count_ = Event::Type::CLOSE;
    rootNode->Update(0, 2, Event::Type::CLOSE);
    ASSERT_EQ(-2, rootNode->positive_count_);

    rootNode->negative_count_ = Event::Type::VOID_CLOSE;
    rootNode->Update(0, 2, Event::Type::VOID_OPEN);
    ASSERT_EQ(0, rootNode->negative_count_);

    rootNode->negative_count_ = Event::Type::VOID_CLOSE;
    rootNode->Update(0, 2, Event::Type::VOID_CLOSE);
    ASSERT_EQ(-4, rootNode->negative_count_);

    rootNode->positive_count_ = Event::Type::CLOSE;
    rootNode->Update(0, 1, Event::Type::OPEN);

    ASSERT_NE(nullptr, rootNode->left_);
    ASSERT_NE(nullptr, rootNode->right_);
    ASSERT_EQ(1, rootNode->left_->positive_count_);
    ASSERT_EQ(0, rootNode->right_->positive_count_);

    rootNode->Update(1, 2, Event::Type::CLOSE);
    ASSERT_EQ(1, rootNode->left_->positive_count_);
    ASSERT_EQ(-1, rootNode->right_->positive_count_);

    delete rootNode;
}
/**
 * @tc.name: Node::GetAndRange01
 * @tc.desc: test WmOcclusionRegion WmOcclusion::Node::GetAndRange
 * @tc.type: FUNC
 */
HWTEST_F(WmOcclusionRegionTest, GetAndRange01, Function | SmallTest | Level2)
{
    Node* rootNode = new Node(0, 2);
    ASSERT_NE(rootNode, nullptr);
    rootNode->positive_count_ = 0;
    rootNode->negative_count_ = 0;
    std::vector<Range> res;
    rootNode->GetAndRange(res, false, false);
    ASSERT_EQ(0, res.size());
    rootNode->GetAndRange(res, true, true);
    ASSERT_EQ(1, res.size());
    ASSERT_EQ(0, res.at(0).start_);
    ASSERT_EQ(2, res.at(0).end_);

    rootNode->left_ = new Node { 0, 1 };
    rootNode->left_->positive_count_ = 1;
    rootNode->GetAndRange(res, false, true);
    ASSERT_EQ(2, res.size());
    ASSERT_EQ(0, res[1].start_);
    ASSERT_EQ(1, res[1].end_);

    rootNode->right_ = new Node { 1, 3 };
    rootNode->right_->negative_count_ = 1;
    rootNode->GetAndRange(res, true, false);
    ASSERT_EQ(2, res.size());
    ASSERT_EQ(0, res[1].start_);
    ASSERT_EQ(3, res[1].end_);

    delete rootNode->right_;
    rootNode->right_ = new Node { 1, 4 };
    rootNode->right_->positive_count_ = 1;
    rootNode->right_->negative_count_ = 1;
    rootNode->GetAndRange(res, false, false);
    ASSERT_EQ(3, res.size());
    ASSERT_EQ(1, res[2].start_);
    ASSERT_EQ(4, res[2].end_);

    delete rootNode;
}
/**
 * @tc.name: Node::GetOrRange01
 * @tc.desc: test WmOcclusionRegion WmOcclusion::Node::GetOrRange
 * @tc.type: FUNC
 */
HWTEST_F(WmOcclusionRegionTest, GetOrRange01, Function | SmallTest | Level2)
{
    std::vector<Range> res;
    Node* rootNode = new Node(0, 2);
    ASSERT_NE(rootNode, nullptr);
    rootNode->positive_count_ = 0;
    rootNode->negative_count_ = 0;
    rootNode->GetOrRange(res, true, true);
    ASSERT_EQ(1, res.size());
    ASSERT_EQ(0, res.at(0).start_);
    ASSERT_EQ(2, res.at(0).end_);

    res.clear();
    rootNode->GetOrRange(res, true, false);
    ASSERT_EQ(1, res.size());
    ASSERT_EQ(0, res.at(0).start_);
    ASSERT_EQ(2, res.at(0).end_);

    res.clear();
    rootNode->GetOrRange(res, false, true);
    ASSERT_EQ(1, res.size());
    ASSERT_EQ(0, res.at(0).start_);
    ASSERT_EQ(2, res.at(0).end_);

    res.clear();
    rootNode->GetOrRange(res, false, false);
    ASSERT_EQ(0, res.size());

    rootNode->left_ = new Node { 0, 1 };
    rootNode->left_->positive_count_ = 0;
    rootNode->left_->negative_count_ = 1;
    rootNode->GetOrRange(res, false, false);
    ASSERT_EQ(1, res.size());
    ASSERT_EQ(0, res[0].start_);
    ASSERT_EQ(1, res[0].end_);
    delete rootNode->left_;
    rootNode->left_ = nullptr;

    res.clear();
    rootNode->right_ = new Node { 1, 3 };
    rootNode->right_->positive_count_ = 0;
    rootNode->right_->negative_count_ = 1;
    rootNode->GetOrRange(res, false, false);
    ASSERT_EQ(1, res.size());
    ASSERT_EQ(1, res[0].start_);
    ASSERT_EQ(3, res[0].end_);

    delete rootNode;
}
/**
 * @tc.name: Node::GetXOrRange01
 * @tc.desc: test WmOcclusionRegion WmOcclusion::Node::GetXOrRange
 * @tc.type: FUNC
 */
HWTEST_F(WmOcclusionRegionTest, GetXOrRange01, Function | SmallTest | Level2)
{
    std::vector<Range> res;
    Node* rootNode = new Node(0, 2);
    ASSERT_NE(rootNode, nullptr);
    rootNode->positive_count_ = 0;
    rootNode->negative_count_ = 0;
    rootNode->GetXOrRange(res, true, true);
    ASSERT_EQ(0, res.size());

    rootNode->GetXOrRange(res, true, false);
    ASSERT_EQ(1, res.size());
    ASSERT_EQ(0, res.at(0).start_);
    ASSERT_EQ(2, res.at(0).end_);

    res.clear();
    rootNode->GetXOrRange(res, false, true);
    ASSERT_EQ(1, res.size());
    ASSERT_EQ(0, res.at(0).start_);
    ASSERT_EQ(2, res.at(0).end_);

    res.clear();
    rootNode->GetXOrRange(res, false, false);
    ASSERT_EQ(0, res.size());

    rootNode->left_ = new Node { 0, 1 };
    rootNode->left_->positive_count_ = 0;
    rootNode->left_->negative_count_ = 1;
    rootNode->GetXOrRange(res, false, false);
    ASSERT_EQ(1, res.size());
    ASSERT_EQ(0, res[0].start_);
    ASSERT_EQ(1, res[0].end_);

    delete rootNode->left_;
    rootNode->left_ = nullptr;

    res.clear();
    rootNode->right_ = new Node { 1, 3 };
    rootNode->right_->positive_count_ = 0;
    rootNode->right_->negative_count_ = 1;
    rootNode->GetXOrRange(res, false, false);
    ASSERT_EQ(1, res.size());
    ASSERT_EQ(1, res[0].start_);
    ASSERT_EQ(3, res[0].end_);

    delete rootNode;
}
/**
 * @tc.name: Node::GetSubRange01
 * @tc.desc: test WmOcclusionRegion WmOcclusion::Node::GetSubRange
 * @tc.type: FUNC
 */
HWTEST_F(WmOcclusionRegionTest, GetSubRange01, Function | SmallTest | Level2)
{
    std::vector<Range> res;
    Node* rootNode = new Node(0, 2);
    ASSERT_NE(rootNode, nullptr);
    rootNode->positive_count_ = 0;
    rootNode->negative_count_ = 0;
    rootNode->GetSubRange(res, true, true);
    ASSERT_EQ(0, res.size());

    rootNode->GetSubRange(res, true, false);
    ASSERT_EQ(1, res.size());
    ASSERT_EQ(0, res.at(0).start_);
    ASSERT_EQ(2, res.at(0).end_);

    res.clear();
    rootNode->GetSubRange(res, false, true);
    ASSERT_EQ(0, res.size());

    rootNode->GetSubRange(res, false, false);
    ASSERT_EQ(0, res.size());

    rootNode->left_ = new Node { 0, 1 };
    rootNode->left_->positive_count_ = 1;
    rootNode->left_->negative_count_ = 0;
    rootNode->GetSubRange(res, false, false);
    ASSERT_EQ(1, res.size());
    ASSERT_EQ(0, res[0].start_);
    ASSERT_EQ(1, res[0].end_);

    delete rootNode->left_;
    rootNode->left_ = nullptr;

    res.clear();
    rootNode->right_ = new Node { 1, 3 };
    rootNode->right_->positive_count_ = 1;
    rootNode->right_->negative_count_ = 0;
    rootNode->GetSubRange(res, false, false);
    ASSERT_EQ(1, res.size());
    ASSERT_EQ(1, res[0].start_);
    ASSERT_EQ(3, res[0].end_);

    delete rootNode;
}
/**
 * @tc.name: Region::UpdateRects01
 * @tc.desc: test WmOcclusionRegion WmOcclusion::Region::UpdateRects
 * @tc.type: FUNC
 */
HWTEST_F(WmOcclusionRegionTest, UpdateRects01, Function | SmallTest | Level2)
{
    Region* region = new Region();
    ASSERT_NE(region, nullptr);
    Region::Rects rects;
    rects.preRects = {
        Rect{0, 10, 1, 10},
        Rect{3, 10, 1, 10},
        Rect{1, 10, 3, 10},
    };
    std::vector<Range> ranges = {
        Range{0, 1},
        Range{1, 2},
        Range{3, 4},
    };
    std::vector<int> indexAt = {
        0, 1, 2, 2, 3
    };
    Region regionRes;
    region->UpdateRects(rects, ranges, indexAt, regionRes);

    std::vector<int> resultPreRectsLeft = { 0, 1, 2, 10 };
    ASSERT_EQ(3, rects.preRects.size());
    for (uint32_t i = 0; i < rects.preRects.size(); ++i) {
        ASSERT_EQ(resultPreRectsLeft[i], rects.preRects[i].left_);
    }
    ASSERT_EQ(2, regionRes.GetRegionRects().size());
    auto regionRects = regionRes.GetRegionRects();
    std::vector<int> resultRegionRectsLeft = { 3, 1, 10, };
    for (uint32_t i = 0; i < regionRects.size(); ++i) {
        ASSERT_EQ(resultRegionRectsLeft[i], regionRects[i].left_);
    }
    delete region;
}
/**
 * @tc.name: Region::MakeBound01
 * @tc.desc: test WmOcclusionRegion WmOcclusion::Region::MakeBound
 * @tc.type: FUNC
 */
HWTEST_F(WmOcclusionRegionTest, MakeBound01, Function | SmallTest | Level2)
{
    Region* region = new Region();
    ASSERT_NE(region, nullptr);
    region->bound_ = { 10, 10, 10, 10 };
    region->rects_.clear();
    region->MakeBound();
    ASSERT_EQ(10, region->bound_.left_);
    ASSERT_EQ(10, region->bound_.top_);
    ASSERT_EQ(10, region->bound_.right_);
    ASSERT_EQ(10, region->bound_.bottom_);

    region->rects_ = {
        Rect{5, 5, 5, 5},
        Rect{6, 4, 6, 6},
        Rect{7, 6, 2, 8},
        Rect{8, 7, 7, 3},
    };
    region->MakeBound();
    ASSERT_EQ(5, region->bound_.left_);
    ASSERT_EQ(4, region->bound_.top_);
    ASSERT_EQ(7, region->bound_.right_);
    ASSERT_EQ(8, region->bound_.bottom_);

    delete region;
}
/**
 * @tc.name: Region::RegionOpLocal01
 * @tc.desc: test WmOcclusionRegion WmOcclusion::Region::RegionOpLocal
 * @tc.type: FUNC
 */
HWTEST_F(WmOcclusionRegionTest, RegionOpLocal01, Function | SmallTest | Level2)
{
    Region regionBase;
    Region region1;
    Region region2;
    Region regionRes;
    Region::OP op = Region::OP::SUB;
    regionRes.rects_.emplace_back(Rect{ 10, 10, 10, 10 });
    regionBase.RegionOpLocal(region1, region2, regionRes, op);
    ASSERT_EQ(0, regionRes.GetRegionRects().size());

    region1.rects_.emplace_back(Rect{ 6, 7, 8, 9, });
    region1.rects_.emplace_back(Rect{ 10, 9, 8, 7, });
    region1.rects_.emplace_back(Rect{ 5, 6, 7, 8, });
    region1.rects_.emplace_back(Rect{ 11, 10, 9, 8, });
    regionBase.RegionOpLocal(region1, region2, regionRes, op);
    ASSERT_EQ(3, regionRes.GetRegionRects().size());
}
}
}
} // namespace Rosen
} // namespace OHOS
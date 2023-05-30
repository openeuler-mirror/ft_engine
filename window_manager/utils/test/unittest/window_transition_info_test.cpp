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

#include <iremote_broker.h>
#include <iremote_object.h>
#include "window_transition_info.h"
#include "iremote_object_mocker.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
using RemoteMocker = IRemoteObjectMocker;
class WindowTransitionInfoTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    void SetTransitionInfo(sptr<WindowTransitionInfo> info);
};

void WindowTransitionInfoTest::SetUpTestCase()
{
}

void WindowTransitionInfoTest::TearDownTestCase()
{
}

void WindowTransitionInfoTest::SetUp()
{
}

void WindowTransitionInfoTest::TearDown()
{
}

void WindowTransitionInfoTest::SetTransitionInfo(sptr<WindowTransitionInfo> info)
{
    info->bundleName_ = "bundleName";
    info->abilityName_ = "abilityName";
    info->mode_ = WindowMode::WINDOW_MODE_FULLSCREEN;
    info->windowRect_ = { 10, 10, 10, 10 };
    info->displayId_ = 0;
}

namespace {
/**
 * @tc.name: WindowTransitionInfo01
 * @tc.desc: WindowTransitionInfo::WindowTransitionInfo test
 * @tc.type: FUNC
 */
HWTEST_F(WindowTransitionInfoTest, WindowTransitionInfo01, Function | SmallTest | Level2)
{
    sptr<AAFwk::AbilityTransitionInfo> info = new AAFwk::AbilityTransitionInfo();
    ASSERT_NE(nullptr, info);
    info->bundleName_ = "TestAbilityTransitionInfo1";
    sptr<WindowTransitionInfo> winTransitionInfo1 = new WindowTransitionInfo(info);
    ASSERT_NE(nullptr, winTransitionInfo1);
    ASSERT_EQ(info->bundleName_, winTransitionInfo1->bundleName_);
    ASSERT_EQ(3, winTransitionInfo1->supportWindowModes_.size());
    ASSERT_EQ(AppExecFwk::SupportWindowMode::FULLSCREEN, winTransitionInfo1->supportWindowModes_[0]);
    ASSERT_EQ(AppExecFwk::SupportWindowMode::SPLIT, winTransitionInfo1->supportWindowModes_[1]);
    ASSERT_EQ(AppExecFwk::SupportWindowMode::FLOATING, winTransitionInfo1->supportWindowModes_[2]);

    info->bundleName_ = "TestAbilityTransitionInfo2";
    info->windowModes_.emplace_back(AppExecFwk::SupportWindowMode::SPLIT);
    sptr<WindowTransitionInfo> winTransitionInfo2 = new WindowTransitionInfo(info);
    ASSERT_EQ(info->bundleName_, winTransitionInfo2->bundleName_);
    ASSERT_EQ(1, winTransitionInfo2->supportWindowModes_.size());
    ASSERT_EQ(AppExecFwk::SupportWindowMode::SPLIT, winTransitionInfo2->supportWindowModes_[0]);
}
/**
 * @tc.name: Marshalling
 * @tc.desc: WindowTransitionInfo::Marshalling test
 * @tc.type: FUNC
 */
HWTEST_F(WindowTransitionInfoTest, Marshalling01, Function | SmallTest | Level2)
{
    sptr<AAFwk::AbilityTransitionInfo> info = new AAFwk::AbilityTransitionInfo();
    ASSERT_NE(nullptr, info);

    Parcel parcel;
    sptr<WindowTransitionInfo> winTransitionInfo = new WindowTransitionInfo(info);
    ASSERT_NE(nullptr, winTransitionInfo);

    winTransitionInfo->bundleName_ = "bundleNameValue";
    winTransitionInfo->abilityName_ = "abilityNameValue";

    bool result = winTransitionInfo->Marshalling(parcel);
    ASSERT_EQ(true, result);

    sptr<IRemoteObject> romote = new RemoteMocker();
    winTransitionInfo->abilityToken_ = romote;
    result = winTransitionInfo->Marshalling(parcel);
    ASSERT_EQ(true, result);
}
}
} // namespace Rosen
} // namespace OHOS
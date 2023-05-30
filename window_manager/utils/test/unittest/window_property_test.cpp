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

#include "window_property.h"
#include "wm_common_inner.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class WindowPropertyTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    virtual void SetUp() override;
    virtual void TearDown() override;
};

void WindowPropertyTest::SetUpTestCase()
{
}

void WindowPropertyTest::TearDownTestCase()
{
}

void WindowPropertyTest::SetUp()
{
}

void WindowPropertyTest::TearDown()
{
}

namespace {
/**
 * @tc.name: MarshallingUnmarshalling
 * @tc.desc: Marshalling Unmarshalling test
 * @tc.type: FUNC
 */
HWTEST_F(WindowPropertyTest, MarshallingUnmarshalling, Function | SmallTest | Level2)
{
    WindowProperty winPropSrc;
    winPropSrc.SetPrivacyMode(true);
    winPropSrc.SetTransparent(true);
    winPropSrc.SetTransform(Transform::Identity());

    Parcel parcel;
    winPropSrc.Marshalling(parcel);
    WindowProperty* winPropDst = winPropSrc.Unmarshalling(parcel);

    ASSERT_EQ(winPropDst->GetPrivacyMode(), true);
    ASSERT_EQ(winPropDst->GetTransparent(), true);
    ASSERT_EQ(winPropDst->GetTransform(), Transform::Identity());
    delete winPropDst;
}

/**
 * @tc.name: CopyFrom
 * @tc.desc: CopyFrom test
 * @tc.type: FUNC
 */
HWTEST_F(WindowPropertyTest, CopyFrom, Function | SmallTest | Level2)
{
    const sptr<WindowProperty> winPropSrc = new(std::nothrow) WindowProperty();
    winPropSrc->SetPrivacyMode(true);
    winPropSrc->SetTransparent(true);
    winPropSrc->SetTransform(Transform::Identity());

    WindowProperty winPropDst(winPropSrc); // winPropDst.CopyFrom(winPropSrc);

    ASSERT_EQ(winPropSrc->GetPrivacyMode(), winPropDst.GetPrivacyMode());
    ASSERT_EQ(winPropSrc->GetTransparent(), winPropDst.GetTransparent());
    ASSERT_EQ(winPropSrc->GetTransform(), winPropDst.GetTransform());
}

/**
 * @tc.name: Read
 * @tc.desc: Read test
 * @tc.type: FUNC
 */
HWTEST_F(WindowPropertyTest, Read, Function | SmallTest | Level2)
{
    WindowProperty winPropSrc;
    winPropSrc.SetPrivacyMode(true);
    winPropSrc.SetTransparent(true);

    Parcel parcel;
    winPropSrc.Marshalling(parcel);

    WindowProperty winPropDst;
    winPropDst.Read(parcel, PropertyChangeAction::ACTION_UPDATE_RECT);
    winPropDst.Read(parcel, PropertyChangeAction::ACTION_UPDATE_MODE);
    winPropDst.Read(parcel, PropertyChangeAction::ACTION_UPDATE_FLAGS);
    winPropDst.Read(parcel, PropertyChangeAction::ACTION_UPDATE_OTHER_PROPS);
    winPropDst.Read(parcel, PropertyChangeAction::ACTION_UPDATE_FOCUSABLE);
    winPropDst.Read(parcel, PropertyChangeAction::ACTION_UPDATE_TOUCHABLE);
    winPropDst.Read(parcel, PropertyChangeAction::ACTION_UPDATE_CALLING_WINDOW);
    winPropDst.Read(parcel, PropertyChangeAction::ACTION_UPDATE_ORIENTATION);
    winPropDst.Read(parcel, PropertyChangeAction::ACTION_UPDATE_TURN_SCREEN_ON);
    winPropDst.Read(parcel, PropertyChangeAction::ACTION_UPDATE_KEEP_SCREEN_ON);
    winPropDst.Read(parcel, PropertyChangeAction::ACTION_UPDATE_SET_BRIGHTNESS);
    winPropDst.Read(parcel, PropertyChangeAction::ACTION_UPDATE_MODE_SUPPORT_INFO);
    winPropDst.Read(parcel, PropertyChangeAction::ACTION_UPDATE_TOUCH_HOT_AREA);
    winPropDst.Read(parcel, PropertyChangeAction::ACTION_UPDATE_TRANSFORM_PROPERTY);
    winPropDst.Read(parcel, PropertyChangeAction::ACTION_UPDATE_ANIMATION_FLAG);
    winPropDst.Read(parcel, PropertyChangeAction::ACTION_UPDATE_PRIVACY_MODE);

    ASSERT_EQ(true, winPropDst.GetPrivacyMode());
    ASSERT_EQ(false, winPropDst.GetTransparent());
}

/**
 * @tc.name: Write
 * @tc.desc: Write test
 * @tc.type: FUNC
 */
HWTEST_F(WindowPropertyTest, Write, Function | SmallTest | Level2)
{
    Parcel parcel;
    WindowProperty winPropDst;
    ASSERT_EQ(true, winPropDst.Write(parcel, PropertyChangeAction::ACTION_UPDATE_RECT));
    ASSERT_EQ(true, winPropDst.Write(parcel, PropertyChangeAction::ACTION_UPDATE_MODE));
    ASSERT_EQ(true, winPropDst.Write(parcel, PropertyChangeAction::ACTION_UPDATE_FLAGS));
    ASSERT_EQ(true, winPropDst.Write(parcel, PropertyChangeAction::ACTION_UPDATE_OTHER_PROPS));
    ASSERT_EQ(true, winPropDst.Write(parcel, PropertyChangeAction::ACTION_UPDATE_FOCUSABLE));
    ASSERT_EQ(true, winPropDst.Write(parcel, PropertyChangeAction::ACTION_UPDATE_TOUCHABLE));
    ASSERT_EQ(true, winPropDst.Write(parcel, PropertyChangeAction::ACTION_UPDATE_CALLING_WINDOW));
    ASSERT_EQ(true, winPropDst.Write(parcel, PropertyChangeAction::ACTION_UPDATE_ORIENTATION));
    ASSERT_EQ(true, winPropDst.Write(parcel, PropertyChangeAction::ACTION_UPDATE_TURN_SCREEN_ON));
    ASSERT_EQ(true, winPropDst.Write(parcel, PropertyChangeAction::ACTION_UPDATE_KEEP_SCREEN_ON));
    ASSERT_EQ(true, winPropDst.Write(parcel, PropertyChangeAction::ACTION_UPDATE_SET_BRIGHTNESS));
    ASSERT_EQ(true, winPropDst.Write(parcel, PropertyChangeAction::ACTION_UPDATE_MODE_SUPPORT_INFO));
    ASSERT_EQ(true, winPropDst.Write(parcel, PropertyChangeAction::ACTION_UPDATE_TOUCH_HOT_AREA));
    ASSERT_EQ(true, winPropDst.Write(parcel, PropertyChangeAction::ACTION_UPDATE_TRANSFORM_PROPERTY));
    ASSERT_EQ(true, winPropDst.Write(parcel, PropertyChangeAction::ACTION_UPDATE_ANIMATION_FLAG));
}

/**
 * @tc.name: SetAbilityInfo
 * @tc.desc: Test SetAbilityInfo and GetAbilityInfo
 * @tc.type: FUNC
 */
HWTEST_F(WindowPropertyTest, SetAbilityInfo, Function | SmallTest | Level2)
{
    WindowProperty winPropDst;
    AbilityInfo info;
    info.bundleName_ = "testBundleName";
    info.abilityName_ = "testAbilityName";
    winPropDst.SetAbilityInfo(info);
    ASSERT_EQ("testBundleName", winPropDst.GetAbilityInfo().bundleName_);
    ASSERT_EQ("testAbilityName", winPropDst.GetAbilityInfo().abilityName_);
}

/**
 * @tc.name: ResumeLastWindowMode
 * @tc.desc: Test ResumeLastWindowMode
 * @tc.type: FUNC
 */
HWTEST_F(WindowPropertyTest, ResumeLastWindowMode, Function | SmallTest | Level2)
{
    WindowProperty winPropDst;
    winPropDst.modeSupportInfo_ =  WindowModeSupport::WINDOW_MODE_SUPPORT_PIP;
    winPropDst.lastMode_ =  WindowMode::WINDOW_MODE_PIP;
    winPropDst.mode_ = WindowMode::WINDOW_MODE_UNDEFINED;
    winPropDst.ResumeLastWindowMode();
    ASSERT_EQ(WindowMode::WINDOW_MODE_PIP, winPropDst.mode_);

    winPropDst.modeSupportInfo_ =  WindowModeSupport::WINDOW_MODE_SUPPORT_SPLIT_SECONDARY;
    winPropDst.lastMode_ =  WindowMode::WINDOW_MODE_PIP;
    winPropDst.mode_ = WindowMode::WINDOW_MODE_UNDEFINED;
    winPropDst.ResumeLastWindowMode();
    ASSERT_EQ(WindowMode::WINDOW_MODE_UNDEFINED, winPropDst.mode_);

    winPropDst.modeSupportInfo_ =  WindowModeSupport::WINDOW_MODE_SUPPORT_FLOATING;
    winPropDst.lastMode_ =  WindowMode::WINDOW_MODE_PIP;
    winPropDst.mode_ = WindowMode::WINDOW_MODE_UNDEFINED;
    winPropDst.ResumeLastWindowMode();
    ASSERT_EQ(WindowMode::WINDOW_MODE_FLOATING, winPropDst.mode_);
}
}
} // namespace Rosen
} // namespace OHOS
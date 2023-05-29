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

#include <gtest/gtest.h>
#include "window_option.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace {
    const SystemBarProperty SYS_BAR_PROP_DEFAULT;
    const SystemBarProperty SYS_BAR_PROP_1(true, 0xE5111111, 0xE5222222);
    const SystemBarProperty SYS_BAR_PROP_2(false, 0xE5222222, 0xE5333333);
    const std::unordered_map<WindowType, SystemBarProperty>& SYS_BAR_PROPS_TEST = {
        { WindowType::WINDOW_TYPE_STATUS_BAR, SYS_BAR_PROP_1 },
        { WindowType::WINDOW_TYPE_NAVIGATION_BAR, SYS_BAR_PROP_2 },
    };
    const std::unordered_map<WindowType, SystemBarProperty>& SYS_BAR_PROPS_DEFAULT = {
        { WindowType::WINDOW_TYPE_STATUS_BAR,     SYS_BAR_PROP_DEFAULT },
        { WindowType::WINDOW_TYPE_NAVIGATION_BAR, SYS_BAR_PROP_DEFAULT },
    };
}
class WindowOptionTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    virtual void SetUp() override;
    virtual void TearDown() override;
};
void WindowOptionTest::SetUpTestCase()
{
}

void WindowOptionTest::TearDownTestCase()
{
}

void WindowOptionTest::SetUp()
{
}

void WindowOptionTest::TearDown()
{
}

namespace {
/**
 * @tc.name: WindowRect01
 * @tc.desc: SetWindowRect/GetWindowRect
 * @tc.type: FUNC
 */
HWTEST_F(WindowOptionTest, WindowRect01, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    struct Rect rect = {1, 2, 3u, 4u};
    option->SetWindowRect(rect);

    ASSERT_EQ(1, option->GetWindowRect().posX_);
    ASSERT_EQ(2, option->GetWindowRect().posY_);
    ASSERT_EQ(3u, option->GetWindowRect().width_);
    ASSERT_EQ(4u, option->GetWindowRect().height_);
}

/**
 * @tc.name: WindowType01
 * @tc.desc: SetWindowType/GetWindowType
 * @tc.type: FUNC
 */
HWTEST_F(WindowOptionTest, WindowType01, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    ASSERT_EQ(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW, option->GetWindowType());
}

/**
 * @tc.name: WindowMode01
 * @tc.desc: SetWindowMode/GetWindowMode
 * @tc.type: FUNC
 */
HWTEST_F(WindowOptionTest, WindowMode01, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);
    ASSERT_EQ(WindowMode::WINDOW_MODE_FULLSCREEN, option->GetWindowMode());
}

/**
 * @tc.name: WindowMode02
 * @tc.desc: SetWindowMode/GetWindowMode
 * @tc.type: FUNC
 */
HWTEST_F(WindowOptionTest, WindowMode02, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    WindowMode defaultMode = option->GetWindowMode();
    option->SetWindowMode(WindowMode::WINDOW_MODE_UNDEFINED);
    ASSERT_EQ(defaultMode, option->GetWindowMode());
}

/**
 * @tc.name: WindowMode03
 * @tc.desc: SetWindowMode/GetWindowMode
 * @tc.type: FUNC
 */
HWTEST_F(WindowOptionTest, WindowMode03, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowMode(WindowMode::WINDOW_MODE_SPLIT_PRIMARY);
    ASSERT_EQ(WindowMode::WINDOW_MODE_SPLIT_PRIMARY, option->GetWindowMode());
}

/**
 * @tc.name: WindowMode04
 * @tc.desc: SetWindowMode/GetWindowMode
 * @tc.type: FUNC
 */
HWTEST_F(WindowOptionTest, WindowMode04, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowMode(WindowMode::WINDOW_MODE_SPLIT_SECONDARY);
    ASSERT_EQ(WindowMode::WINDOW_MODE_SPLIT_SECONDARY, option->GetWindowMode());
}

/**
 * @tc.name: WindowMode05
 * @tc.desc: SetWindowMode/GetWindowMode
 * @tc.type: FUNC
 */
HWTEST_F(WindowOptionTest, WindowMode05, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    ASSERT_EQ(WindowMode::WINDOW_MODE_FLOATING, option->GetWindowMode());
}

/**
 * @tc.name: WindowMode06
 * @tc.desc: SetWindowMode/GetWindowMode
 * @tc.type: FUNC
 */
HWTEST_F(WindowOptionTest, WindowMode06, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowMode(WindowMode::WINDOW_MODE_PIP);
    ASSERT_EQ(WindowMode::WINDOW_MODE_PIP, option->GetWindowMode());
}
/**
 * @tc.name: Focusable01
 * @tc.desc: SetFocusable/GetFocusable
 * @tc.type: FUNC
 */
HWTEST_F(WindowOptionTest, Focusable01, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetFocusable(true);
    ASSERT_EQ(true, option->GetFocusable());
}

/**
 * @tc.name: Touchable01
 * @tc.desc: SetTouchable/GetTouchable
 * @tc.type: FUNC
 */
HWTEST_F(WindowOptionTest, Touchable01, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetTouchable(true);
    ASSERT_EQ(true, option->GetTouchable());
}

/**
 * @tc.name: DisplayId01
 * @tc.desc: SetDisplayId/GetDisplayId
 * @tc.type: FUNC
 */
HWTEST_F(WindowOptionTest, DisplayId01, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetDisplayId(1);
    ASSERT_EQ(1, option->GetDisplayId());
}

/**
 * @tc.name: ParentId01
 * @tc.desc: SetParentId/GetParentId
 * @tc.type: FUNC
 */
HWTEST_F(WindowOptionTest, ParentId01, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetParentId(1);
    ASSERT_EQ(1, option->GetParentId());
}

/**
 * @tc.name: WindowName01
 * @tc.desc: SetWindowName/GetWindowName
 * @tc.type: FUNC
 */
HWTEST_F(WindowOptionTest, WindowName01, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("Sub Window");
    ASSERT_EQ("Sub Window", option->GetWindowName());
}

/**
 * @tc.name: WindowFlag01
 * @tc.desc: SetWindowFlags/GetWindowFlags
 * @tc.type: FUNC
 */
HWTEST_F(WindowOptionTest, WindowFlag01, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowFlags(1u);
    ASSERT_EQ(1u, option->GetWindowFlags());
}

/**
 * @tc.name: WindowFlag02
 * @tc.desc: AddWindowFlag/GetWindowFlags
 * @tc.type: FUNC
 */
HWTEST_F(WindowOptionTest, WindowFlag02, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    option->AddWindowFlag(WindowFlag::WINDOW_FLAG_NEED_AVOID);
    ASSERT_EQ(static_cast<uint32_t>(WindowFlag::WINDOW_FLAG_NEED_AVOID), option->GetWindowFlags());
}

/**
 * @tc.name: WindowFlag03
 * @tc.desc: AddWindowFlag/RemoveWindowFlag/GetWindowFlags
 * @tc.type: FUNC
 */
HWTEST_F(WindowOptionTest, WindowFlag03, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    option->AddWindowFlag(WindowFlag::WINDOW_FLAG_NEED_AVOID);
    option->AddWindowFlag(WindowFlag::WINDOW_FLAG_PARENT_LIMIT);
    option->RemoveWindowFlag(WindowFlag::WINDOW_FLAG_NEED_AVOID);
    ASSERT_EQ(static_cast<uint32_t>(WindowFlag::WINDOW_FLAG_PARENT_LIMIT), option->GetWindowFlags());
}

/**
 * @tc.name: SetGetSystemBarProperty01
 * @tc.desc: SetSystemBarProperty with test param and get
 * @tc.type: FUNC
 */
HWTEST_F(WindowOptionTest, SetGetSystemBarProperty01, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetSystemBarProperty(WindowType::WINDOW_TYPE_STATUS_BAR, SYS_BAR_PROP_1);
    option->SetSystemBarProperty(WindowType::WINDOW_TYPE_NAVIGATION_BAR, SYS_BAR_PROP_2);
    ASSERT_EQ(SYS_BAR_PROPS_TEST, option->GetSystemBarProperty());
}

/**
 * @tc.name: SetGetSystemBarProperty02
 * @tc.desc: SetSystemBarProperty with invalid type and get
 * @tc.type: FUNC
 */
HWTEST_F(WindowOptionTest, SetGetSystemBarProperty02, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetSystemBarProperty(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW, SYS_BAR_PROP_1);
    option->SetSystemBarProperty(WindowType::WINDOW_TYPE_MEDIA, SYS_BAR_PROP_2);
    ASSERT_EQ(SYS_BAR_PROPS_DEFAULT, option->GetSystemBarProperty());
}

/**
 * @tc.name: SetGetSystemBarProperty03
 * @tc.desc: GetSystemBarProperty with no set
 * @tc.type: FUNC
 */
HWTEST_F(WindowOptionTest, SetGetSystemBarProperty03, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    ASSERT_EQ(SYS_BAR_PROPS_DEFAULT, option->GetSystemBarProperty());
}

/**
 * @tc.name: HitOffset
 * @tc.desc: HitOffset setter/getter test
 * @tc.type: FUNC
 */
HWTEST_F(WindowOptionTest, HitOffset, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetHitOffset(1, 1);
    PointInfo point = {1, 1};
    ASSERT_EQ(point.x, option->GetHitOffset().x);
    ASSERT_EQ(point.y, option->GetHitOffset().y);
}

/**
 * @tc.name: KeepScreenOn
 * @tc.desc: KeepScreenOn setter/getter test
 * @tc.type: FUNC
 */
HWTEST_F(WindowOptionTest, KeepScreenOn, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetKeepScreenOn(true);
    ASSERT_EQ(true, option->IsKeepScreenOn());
    option->SetKeepScreenOn(false);
    ASSERT_EQ(false, option->IsKeepScreenOn());
}

/**
 * @tc.name: TurnScreenOn
 * @tc.desc: TurnScreenOn setter/getter test
 * @tc.type: FUNC
 */
HWTEST_F(WindowOptionTest, TurnScreenOn, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetTurnScreenOn(true);
    ASSERT_EQ(true, option->IsTurnScreenOn());
    option->SetTurnScreenOn(false);
    ASSERT_EQ(false, option->IsTurnScreenOn());
}

/**
 * @tc.name: Brightness
 * @tc.desc: Brightness setter/getter test
 * @tc.type: FUNC
 */
HWTEST_F(WindowOptionTest, Brightness, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetBrightness(MINIMUM_BRIGHTNESS);
    ASSERT_EQ(MINIMUM_BRIGHTNESS, option->GetBrightness());
    option->SetBrightness(MAXIMUM_BRIGHTNESS);
    ASSERT_EQ(MAXIMUM_BRIGHTNESS, option->GetBrightness());
}

/**
 * @tc.name: CallingWindow
 * @tc.desc: CallingWindow setter/getter test
 * @tc.type: FUNC
 */
HWTEST_F(WindowOptionTest, CallingWindow, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetCallingWindow(1);
    ASSERT_EQ(1, option->GetCallingWindow());
}
}
} // namespace Rosen
} // namespace OHOS

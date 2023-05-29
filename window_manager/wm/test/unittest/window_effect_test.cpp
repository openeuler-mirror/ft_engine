/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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
#include "mock_window_adapter.h"
#include "singleton_mocker.h"
#include "window_impl.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
using Mocker = SingletonMocker<WindowAdapter, MockWindowAdapter>;
class WindowEffectTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    virtual void SetUp() override;
    virtual void TearDown() override;
};
void WindowEffectTest::SetUpTestCase()
{
}

void WindowEffectTest::TearDownTestCase()
{
}

void WindowEffectTest::SetUp()
{
}

void WindowEffectTest::TearDown()
{
}

namespace {
/**
 * @tc.name: WindowEffect01
 * @tc.desc: set window corner radius
 * @tc.type: FUNC
 * @tc.require: issueI5IUGI
 */
HWTEST_F(WindowEffectTest, WindowEffect01, Function | SmallTest | Level2)
{
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    sptr<WindowOption> option = new WindowOption();
    sptr<WindowImpl> window = new WindowImpl(option);
    ASSERT_NE(nullptr, window);
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Create(INVALID_WINDOW_ID));

    ASSERT_EQ(WMError::WM_OK, window->SetCornerRadius(0.0));
    ASSERT_EQ(WMError::WM_OK, window->SetCornerRadius(16.0));
    ASSERT_EQ(WMError::WM_OK, window->SetCornerRadius(1000.0));
    ASSERT_EQ(WMError::WM_ERROR_INVALID_PARAM, window->SetCornerRadius(-1.0));

    EXPECT_CALL(m->Mock(), DestroyWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: WindowEffect02
 * @tc.desc: set window shadow radius
 * @tc.type: FUNC
 * @tc.require: issueI5IUGI
 */
HWTEST_F(WindowEffectTest, WindowEffect02, Function | SmallTest | Level2)
{
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    sptr<WindowOption> option = new WindowOption();
    sptr<WindowImpl> window = new WindowImpl(option);
    ASSERT_NE(nullptr, window);
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Create(INVALID_WINDOW_ID));

    ASSERT_EQ(WMError::WM_OK, window->SetShadowRadius(0.0));
    ASSERT_EQ(WMError::WM_OK, window->SetShadowRadius(16.0));
    ASSERT_EQ(WMError::WM_OK, window->SetShadowRadius(1000.0));
    ASSERT_EQ(WMError::WM_ERROR_INVALID_PARAM, window->SetShadowRadius(-1.0));

    EXPECT_CALL(m->Mock(), DestroyWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: WindowEffect03
 * @tc.desc: set window shadow color
 * @tc.type: FUNC
 * @tc.require: issueI5IUGI
 */
HWTEST_F(WindowEffectTest, WindowEffect03, Function | SmallTest | Level2)
{
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    sptr<WindowOption> option = new WindowOption();
    sptr<WindowImpl> window = new WindowImpl(option);
    ASSERT_NE(nullptr, window);
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Create(INVALID_WINDOW_ID));

    ASSERT_EQ(WMError::WM_OK, window->SetShadowColor("#FF22EE44"));
    ASSERT_EQ(WMError::WM_OK, window->SetShadowColor("#22EE44"));
    ASSERT_EQ(WMError::WM_OK, window->SetShadowColor("#ff22ee44"));

    ASSERT_EQ(WMError::WM_ERROR_INVALID_PARAM, window->SetShadowColor("ff22ee44"));
    ASSERT_EQ(WMError::WM_ERROR_INVALID_PARAM, window->SetShadowColor("22ee44"));
    ASSERT_EQ(WMError::WM_ERROR_INVALID_PARAM, window->SetShadowColor("#ppEE44"));
    ASSERT_EQ(WMError::WM_ERROR_INVALID_PARAM, window->SetShadowColor("#eepp44"));
    ASSERT_EQ(WMError::WM_ERROR_INVALID_PARAM, window->SetShadowColor("#ffeePP44"));
    ASSERT_EQ(WMError::WM_ERROR_INVALID_PARAM, window->SetShadowColor("#ff22ee4422"));
    ASSERT_EQ(WMError::WM_ERROR_INVALID_PARAM, window->SetShadowColor("#ff"));

    EXPECT_CALL(m->Mock(), DestroyWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: WindowEffect04
 * @tc.desc: set window shadow offset
 * @tc.type: FUNC
 * @tc.require: issueI5IUGI
 */
HWTEST_F(WindowEffectTest, WindowEffect04, Function | SmallTest | Level2)
{
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    sptr<WindowOption> option = new WindowOption();
    sptr<WindowImpl> window = new WindowImpl(option);
    ASSERT_NE(nullptr, window);
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Create(INVALID_WINDOW_ID));

    window->SetShadowOffsetX(0.0);
    window->SetShadowOffsetX(16.0);
    window->SetShadowOffsetX(1000.0);
    window->SetShadowOffsetX(-1.0);

    window->SetShadowOffsetY(0.0);
    window->SetShadowOffsetY(16.0);
    window->SetShadowOffsetY(1000.0);
    window->SetShadowOffsetY(-1.0);

    EXPECT_CALL(m->Mock(), DestroyWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: WindowEffect05
 * @tc.desc: set window blur radius
 * @tc.type: FUNC
 * @tc.require: issueI5IUGI
 */
HWTEST_F(WindowEffectTest, WindowEffect05, Function | SmallTest | Level2)
{
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    sptr<WindowOption> option = new WindowOption();
    sptr<WindowImpl> window = new WindowImpl(option);
    ASSERT_NE(nullptr, window);
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Create(INVALID_WINDOW_ID));

    ASSERT_EQ(WMError::WM_OK, window->SetBlur(0.0));
    ASSERT_EQ(WMError::WM_OK, window->SetBlur(16.0));
    ASSERT_EQ(WMError::WM_OK, window->SetBlur(1000.0));
    ASSERT_EQ(WMError::WM_ERROR_INVALID_PARAM, window->SetBlur(-1.0));

    EXPECT_CALL(m->Mock(), DestroyWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: WindowEffect06
 * @tc.desc: set window backdrop blur radius
 * @tc.type: FUNC
 * @tc.require: issueI5IUGI
 */
HWTEST_F(WindowEffectTest, WindowEffect06, Function | SmallTest | Level2)
{
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    sptr<WindowOption> option = new WindowOption();
    sptr<WindowImpl> window = new WindowImpl(option);
    ASSERT_NE(nullptr, window);
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Create(INVALID_WINDOW_ID));

    ASSERT_EQ(WMError::WM_OK, window->SetBackdropBlur(0.0));
    ASSERT_EQ(WMError::WM_OK, window->SetBackdropBlur(16.0));
    ASSERT_EQ(WMError::WM_OK, window->SetBackdropBlur(1000.0));
    ASSERT_EQ(WMError::WM_ERROR_INVALID_PARAM, window->SetBackdropBlur(-1.0));

    EXPECT_CALL(m->Mock(), DestroyWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: WindowEffect07
 * @tc.desc: set window backdrop blur style
 * @tc.type: FUNC
 * @tc.require: issueI5IUGI
 */
HWTEST_F(WindowEffectTest, WindowEffect07, Function | SmallTest | Level2)
{
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    sptr<WindowOption> option = new WindowOption();
    sptr<WindowImpl> window = new WindowImpl(option);
    ASSERT_NE(nullptr, window);
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Create(INVALID_WINDOW_ID));

    ASSERT_EQ(WMError::WM_OK, window->SetBackdropBlurStyle(WindowBlurStyle::WINDOW_BLUR_OFF));
    ASSERT_EQ(WMError::WM_OK, window->SetBackdropBlurStyle(WindowBlurStyle::WINDOW_BLUR_THIN));
    ASSERT_EQ(WMError::WM_OK, window->SetBackdropBlurStyle(WindowBlurStyle::WINDOW_BLUR_REGULAR));
    ASSERT_EQ(WMError::WM_OK, window->SetBackdropBlurStyle(WindowBlurStyle::WINDOW_BLUR_THICK));

    ASSERT_EQ(WMError::WM_ERROR_INVALID_PARAM, window->SetBackdropBlurStyle(static_cast<WindowBlurStyle>(-1)));
    ASSERT_EQ(WMError::WM_ERROR_INVALID_PARAM, window->SetBackdropBlurStyle(static_cast<WindowBlurStyle>(5)));

    EXPECT_CALL(m->Mock(), DestroyWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}
}
} // namespace Rosen
} // namespace OHOS

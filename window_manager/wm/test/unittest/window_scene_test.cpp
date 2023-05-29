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
#include "window_scene.h"
#include "ability_context_impl.h"
#include "mock_static_call.h"
#include "singleton_mocker.h"
#include "window_impl.h"
#include <configuration.h>

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
using Mocker = SingletonMocker<StaticCall, MockStaticCall>;
class WindowSceneTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    virtual void SetUp() override;
    virtual void TearDown() override;

    sptr<WindowScene> scene_ = nullptr;
    std::shared_ptr<AbilityRuntime::AbilityContext> abilityContext_;
};
void WindowSceneTest::SetUpTestCase()
{
}

void WindowSceneTest::TearDownTestCase()
{
}

void WindowSceneTest::SetUp()
{
    DisplayId displayId = 0;
    sptr<IWindowLifeCycle> listener = nullptr;
    scene_ = new WindowScene();
    abilityContext_ = std::make_shared<AbilityRuntime::AbilityContextImpl>();
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    sptr<WindowOption> option = new WindowOption();
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _)).Times(1).WillOnce(Return(new WindowImpl(option)));
    ASSERT_EQ(WMError::WM_OK, scene_->Init(displayId, abilityContext_, listener));
}

void WindowSceneTest::TearDown()
{
    scene_->GoDestroy();
    scene_ = nullptr;
    abilityContext_ = nullptr;
}

namespace {
/**
 * @tc.name: Init01
 * @tc.desc: Init Scene with null abilityContext, null listener
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneTest, Init01, Function | SmallTest | Level2)
{
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    sptr<WindowOption> optionTest = new WindowOption();
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _)).Times(1).WillOnce(Return(new WindowImpl(optionTest)));
    DisplayId displayId = 0;
    sptr<IWindowLifeCycle> listener = nullptr;
    sptr<WindowScene> scene = new WindowScene();
    std::shared_ptr<AbilityRuntime::AbilityContext> abilityContext = nullptr;
    ASSERT_EQ(WMError::WM_OK, scene->Init(displayId, abilityContext, listener));
}

/**
 * @tc.name: Init02
 * @tc.desc: Mock window Create Static Method return nullptr, init Scene with null abilityContext, null listener
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneTest, Init02, Function | SmallTest | Level2)
{
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    sptr<WindowOption> optionTest = new WindowOption();
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _)).Times(1).WillOnce(Return(nullptr));
    DisplayId displayId = 0;
    sptr<IWindowLifeCycle> listener = nullptr;
    sptr<WindowScene> scene = new WindowScene();
    std::shared_ptr<AbilityRuntime::AbilityContext> abilityContext = nullptr;
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, scene->Init(displayId, abilityContext, listener));
}

/**
 * @tc.name: Init03
 * @tc.desc: Init Scene with abilityContext, null listener
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneTest, Init03, Function | SmallTest | Level2)
{
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    sptr<WindowOption> optionTest = new WindowOption();
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _)).Times(1).WillOnce(Return(new WindowImpl(optionTest)));
    DisplayId displayId = 0;
    sptr<IWindowLifeCycle> listener = nullptr;
    sptr<WindowScene> scene = new WindowScene();
    ASSERT_EQ(WMError::WM_OK, scene->Init(displayId, abilityContext_, listener));
}

/**
 * @tc.name: Create01
 * @tc.desc: CreateWindow without windowName
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneTest, Create01, Function | SmallTest | Level2)
{
    sptr<WindowOption> optionTest = new WindowOption();
    sptr<WindowScene> scene = new WindowScene();
    ASSERT_EQ(nullptr, scene->CreateWindow("", optionTest));
}

/**
 * @tc.name: Create02
 * @tc.desc: CreateWindow with windowName and without mainWindow
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneTest, Create02, Function | SmallTest | Level2)
{
    sptr<WindowOption> optionTest = new WindowOption();
    sptr<WindowScene> scene = new WindowScene();
    ASSERT_EQ(nullptr, scene->CreateWindow("WindowSceneTest02", optionTest));
}

/**
 * @tc.name: Create03
 * @tc.desc: CreateWindow with windowName and mainWindow
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneTest, Create03, Function | SmallTest | Level2)
{
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    sptr<WindowOption> optionTest = new WindowOption();
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _)).Times(1).WillOnce(Return(new WindowImpl(optionTest)));
    ASSERT_NE(nullptr, scene_->CreateWindow("WindowSceneTest03", optionTest));
}

/**
 * @tc.name: Create04
 * @tc.desc: Mock window Create Static Method return nullptr, createWindow with windowName and mainWindow
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneTest, Create04, Function | SmallTest | Level2)
{
    sptr<WindowOption> optionTest = new WindowOption();
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _)).Times(1).WillOnce(Return(nullptr));
    ASSERT_EQ(nullptr, scene_->CreateWindow("WindowSceneTest04", optionTest));
}

/**
 * @tc.name: Create05
 * @tc.desc: createWindow with windowName and null option
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneTest, Create05, Function | SmallTest | Level2)
{
    sptr<WindowOption> optionTest = nullptr;
    ASSERT_EQ(nullptr, scene_->CreateWindow("WindowSceneTest05", optionTest));
}

/**
 * @tc.name: GetMainWindow01
 * @tc.desc: GetMainWindow without scene init
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneTest, GetMainWindow01, Function | SmallTest | Level2)
{
    sptr<WindowScene> scene = new WindowScene();
    ASSERT_EQ(nullptr, scene->GetMainWindow());
}

/**
 * @tc.name: GetMainWindow02
 * @tc.desc: GetMainWindow01 with nullptr
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneTest, GetMainWindow02, Function | SmallTest | Level2)
{
    ASSERT_NE(nullptr, scene_->GetMainWindow());
}

/**
 * @tc.name: GetSubWindow01
 * @tc.desc: GetSubWindow without scene init
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneTest, GetSubWindow01, Function | SmallTest | Level2)
{
    sptr<WindowScene> scene = new WindowScene();
    std::vector<sptr<Window>> subWindows = scene->GetSubWindow();
    ASSERT_TRUE(subWindows.empty());
}

/**
 * @tc.name: GetSubWindow02
 * @tc.desc: GetSubWindow without scene init
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneTest, GetSubWindow02, Function | SmallTest | Level2)
{
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    sptr<WindowOption> optionTest = new WindowOption();
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _)).Times(1).WillOnce(Return(new WindowImpl(optionTest)));
    DisplayId displayId = 0;
    sptr<IWindowLifeCycle> listener = nullptr;
    sptr<WindowScene> scene = new WindowScene();
    std::shared_ptr<AbilityRuntime::AbilityContext> abilityContext = nullptr;
    ASSERT_EQ(WMError::WM_OK, scene->Init(displayId, abilityContext, listener));
    std::vector<sptr<Window>> subWindows = scene->GetSubWindow();
    ASSERT_TRUE(subWindows.empty());
}

/**
 * @tc.name: OnNewWant01
 * @tc.desc: OnNewWant nullptr
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneTest, OnNewWant01, Function | SmallTest | Level2)
{
    sptr<WindowScene> scene = new WindowScene();
    AAFwk::Want want;
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, scene->OnNewWant(want));
}

/**
 * @tc.name: OnNewWant02
 * @tc.desc: OnNewWant without scene init
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneTest, OnNewWant02, Function | SmallTest | Level2)
{
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    sptr<WindowOption> optionTest = new WindowOption();
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _)).Times(1).WillOnce(Return(new WindowImpl(optionTest)));
    DisplayId displayId = 0;
    sptr<IWindowLifeCycle> listener = nullptr;
    sptr<WindowScene> scene = new WindowScene();
    std::shared_ptr<AbilityRuntime::AbilityContext> abilityContext = nullptr;
    ASSERT_EQ(WMError::WM_OK, scene->Init(displayId, abilityContext, listener));
    AAFwk::Want want;
    ASSERT_EQ(WMError::WM_OK, scene->OnNewWant(want));
}

/**
 * @tc.name: UpdateConfiguration01
 * @tc.desc: UpdateConfiguration nullptr
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneTest, UpdateConfiguration01, Function | SmallTest | Level2)
{
    sptr<WindowScene> scene = new WindowScene();
    std::shared_ptr<AppExecFwk::Configuration> configuration = nullptr;
    scene->UpdateConfiguration(configuration);
}

/**
 * @tc.name: UpdateConfiguration02
 * @tc.desc: UpdateConfiguration without scene init
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneTest, UpdateConfiguration02, Function | SmallTest | Level2)
{
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    sptr<WindowOption> optionTest = new WindowOption();
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _)).Times(1).WillOnce(Return(new WindowImpl(optionTest)));
    DisplayId displayId = 0;
    sptr<IWindowLifeCycle> listener = nullptr;
    sptr<WindowScene> scene = new WindowScene();
    std::shared_ptr<AbilityRuntime::AbilityContext> abilityContext = nullptr;
    ASSERT_EQ(WMError::WM_OK, scene->Init(displayId, abilityContext, listener));
    std::shared_ptr<AppExecFwk::Configuration> configuration = std::make_shared<AppExecFwk::Configuration>();
    scene->UpdateConfiguration(configuration);
}

/**
 * @tc.name: GetContentInfo01
 * @tc.desc: GetContentInfo nullptr
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneTest, GetContentInfo01, Function | SmallTest | Level2)
{
    sptr<WindowScene> scene = new WindowScene();
    ASSERT_EQ("", scene->GetContentInfo());
}

/**
 * @tc.name: GetContentInfo02
 * @tc.desc: GetContentInfo without scene init
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneTest, GetContentInfo02, Function | SmallTest | Level2)
{
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    sptr<WindowOption> optionTest = new WindowOption();
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _)).Times(1).WillOnce(Return(new WindowImpl(optionTest)));
    DisplayId displayId = 0;
    sptr<IWindowLifeCycle> listener = nullptr;
    sptr<WindowScene> scene = new WindowScene();
    std::shared_ptr<AbilityRuntime::AbilityContext> abilityContext = nullptr;
    ASSERT_EQ(WMError::WM_OK, scene->Init(displayId, abilityContext, listener));
    ASSERT_EQ("", scene->GetContentInfo());
}

/**
 * @tc.name: SetSystemBarProperty01
 * @tc.desc: SetSystemBarProperty nullptr
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneTest, SetSystemBarProperty01, Function | SmallTest | Level2)
{
    sptr<WindowScene> scene = new WindowScene();
    WindowType type = WindowType::WINDOW_TYPE_DIALOG;
    SystemBarProperty property;
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, scene->SetSystemBarProperty(type, property));
}

/**
 * @tc.name: SetSystemBarProperty02
 * @tc.desc: SetSystemBarProperty without scene init
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneTest, SetSystemBarProperty02, Function | SmallTest | Level2)
{
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    sptr<WindowOption> optionTest = new WindowOption();
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _)).Times(1).WillOnce(Return(new WindowImpl(optionTest)));
    DisplayId displayId = 0;
    sptr<IWindowLifeCycle> listener = nullptr;
    sptr<WindowScene> scene = new WindowScene();
    std::shared_ptr<AbilityRuntime::AbilityContext> abilityContext = nullptr;
    ASSERT_EQ(WMError::WM_OK, scene->Init(displayId, abilityContext, listener));
    WindowType type = WindowType::WINDOW_TYPE_DIALOG;
    SystemBarProperty property;
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, scene->SetSystemBarProperty(type, property));
}

/**
 * @tc.name: GoForeground01
 * @tc.desc: GoForeground01 without mainWindow
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneTest, GoForeground01, Function | SmallTest | Level2)
{
    sptr<WindowScene> scene = new WindowScene();
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, scene->GoForeground());
}

/**
 * @tc.name: GoBackground01
 * @tc.desc: GoBackground01 without mainWindow
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneTest, GoBackground01, Function | SmallTest | Level2)
{
    sptr<WindowScene> scene = new WindowScene();
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, scene->GoBackground());
}

/**
 * @tc.name: RequestFocus01
 * @tc.desc: RequestFocus01 without mainWindow
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneTest, RequestFocus01, Function | SmallTest | Level2)
{
    sptr<WindowScene> scene = new WindowScene();
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, scene->RequestFocus());
}

/**
 * @tc.name: NotifyMemoryLevel01
 * @tc.desc: NotifyMemoryLevel without mainWindow
 * @tc.type: FUNC
 * @tc.require: issueI5JQ04
 */
HWTEST_F(WindowSceneTest, NotifyMemoryLevel01, Function | SmallTest | Level2)
{
    sptr<WindowScene> scene = new WindowScene();
    int32_t level = 0;
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, scene->NotifyMemoryLevel(level));
}

/**
 * @tc.name: NotifyMemoryLevel02
 * @tc.desc: NotifyMemoryLevel with level
 * @tc.type: FUNC
 * @tc.require: issueI5JQ04
 */
HWTEST_F(WindowSceneTest, NotifyMemoryLevel02, Function | SmallTest | Level2)
{
    DisplayId displayId = 0;
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    sptr<IWindowLifeCycle> listener = nullptr;
    sptr<WindowScene> scene = new WindowScene();
    sptr<WindowOption> option = new WindowOption();
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _)).Times(1).WillOnce(Return(new WindowImpl(option)));
    ASSERT_EQ(WMError::WM_OK, scene->Init(displayId, abilityContext_, listener));
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, scene->NotifyMemoryLevel(0)); // ui content is null
}
}
} // namespace Rosen
} // namespace OHOS
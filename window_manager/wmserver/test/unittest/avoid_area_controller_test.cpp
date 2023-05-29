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

#include "avoid_area_controller.h"
#include "display_manager.h"
#include "display_manager_config.h"
#include "future.h"
#include "window_node.h"
#include "wm_common.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "AvoidAreaControllerTest"};

    const Rect EMPTY_RECT = { 0, 0, 0, 0 };
    const float BARRATIO = 0.3;
    const long TIME_OUT = 1000;
    const AvoidArea EMPTY_AVOID_AREA = {};
}

class AvoidAreaControllerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    virtual void SetUp() override;
    virtual void TearDown() override;

    static sptr<WindowNode> statusbarWindowNode;
    static sptr<WindowNode> navigationBarWindowNode;
    static sptr<WindowNode> keyboardWindowNode;
    static Rect screenRect;
    static Rect cut_out_rect;
};

sptr<WindowNode> AvoidAreaControllerTest::statusbarWindowNode = nullptr;
sptr<WindowNode> AvoidAreaControllerTest::navigationBarWindowNode = nullptr;
sptr<WindowNode> AvoidAreaControllerTest::keyboardWindowNode = nullptr;
Rect AvoidAreaControllerTest::screenRect;

class WindowListener : public IWindow {
public:
    WMError UpdateWindowRect(const struct Rect& rect, bool decoStatus, WindowSizeChangeReason reason) override
    {
        return WMError::WM_OK;
    }
    WMError UpdateWindowMode(WindowMode mode) override
    {
        return WMError::WM_OK;
    }
    WMError UpdateFocusStatus(bool focused) override
    {
        return WMError::WM_OK;
    }
    WMError UpdateAvoidArea(const sptr<AvoidArea>& avoidArea, AvoidAreaType type) override
    {
        if (type == AvoidAreaType::TYPE_SYSTEM) {
            statusBarAvoidAreaFuture_.SetValue(*avoidArea);
        }
        if (type == AvoidAreaType::TYPE_KEYBOARD) {
            keyboardAvoidAreaFuture_.SetValue(*avoidArea);
        }
        return WMError::WM_OK;
    }
    WMError UpdateWindowModeSupportInfo(uint32_t modeSupportInfo) override
    {
        return WMError::WM_OK;
    }
    WMError UpdateWindowState(WindowState state) override
    {
        return WMError::WM_OK;
    }
    WMError UpdateWindowDragInfo(const PointInfo& point, DragEvent event) override
    {
        return WMError::WM_OK;
    }
    WMError UpdateDisplayId(DisplayId from, DisplayId to) override
    {
        return WMError::WM_OK;
    }
    WMError UpdateOccupiedAreaChangeInfo(const sptr<OccupiedAreaChangeInfo>& info) override
    {
        return WMError::WM_OK;
    }
    WMError UpdateActiveStatus(bool isActive) override
    {
        return WMError::WM_OK;
    }
    sptr<WindowProperty> GetWindowProperty() override
    {
        return nullptr;
    }
    WMError NotifyTouchOutside() override
    {
        return WMError::WM_OK;
    }
    WMError NotifyScreenshot() override
    {
        return WMError::WM_OK;
    }
    WMError NotifyDestroy(void) override
    {
        return WMError::WM_OK;
    }
    WMError NotifyForeground(void) override
    {
        return WMError::WM_OK;
    }
    WMError NotifyBackground(void) override
    {
        return WMError::WM_OK;
    }
    WMError UpdateZoomTransform(const Transform& trans, bool isDisplayZoomOn) override
    {
        return WMError::WM_OK;
    }
    WMError DumpInfo(const std::vector<std::string>& params) override
    {
        return WMError::WM_OK;
    }
    WMError NotifyWindowClientPointUp(const std::shared_ptr<MMI::PointerEvent>& pointerEvent) override
    {
        return WMError::WM_OK;
    }
    WMError RestoreSplitWindowMode(uint32_t mode) override
    {
        return WMError::WM_OK;
    }
    RunnableFuture<AvoidArea> statusBarAvoidAreaFuture_;
    RunnableFuture<AvoidArea> keyboardAvoidAreaFuture_;

    sptr<IRemoteObject> AsObject() override
    {
        return nullptr;
    }
};

void AvoidAreaControllerTest::SetUpTestCase()
{
    auto display = DisplayManager::GetInstance().GetDefaultDisplay();
    ASSERT_TRUE((display != nullptr));
    WLOGFI("GetDefaultDisplay: id %{public}" PRIu64", w %{public}d, h %{public}d, fps %{public}u",
        display->GetId(), display->GetWidth(), display->GetHeight(), display->GetRefreshRate());
    screenRect = { 0, 0, static_cast<uint32_t>(display->GetWidth()), static_cast<uint32_t>(display->GetHeight()) };
    auto barHeight = static_cast<uint32_t>(screenRect.height_ * BARRATIO);
    Rect statusBarRect = { 0, 0, screenRect.width_, barHeight };
    Rect navigationRect = { 0, static_cast<int32_t>(screenRect.height_ - barHeight), screenRect.width_, barHeight };

    sptr<WindowProperty> statusbarProperty = new WindowProperty();
    statusbarProperty->SetWindowId(100u);
    statusbarProperty->SetWindowName("status bar");
    statusbarProperty->SetWindowType(WindowType::WINDOW_TYPE_STATUS_BAR);
    statusbarProperty->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    statusbarProperty->SetWindowRect(statusBarRect);
    sptr<WindowListener> windowListener = new WindowListener();
    statusbarWindowNode = new WindowNode(statusbarProperty, windowListener, nullptr);

    sptr<WindowProperty> navigationBarProperty = new WindowProperty();
    navigationBarProperty->SetWindowId(101u);
    navigationBarProperty->SetWindowName("navigation bar");
    navigationBarProperty->SetWindowType(WindowType::WINDOW_TYPE_NAVIGATION_BAR);
    navigationBarProperty->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    navigationBarProperty->SetWindowRect(navigationRect);
    windowListener = new WindowListener();
    navigationBarWindowNode = new WindowNode(navigationBarProperty, windowListener, nullptr);

    sptr<WindowProperty> keyboardProperty = new WindowProperty();
    keyboardProperty->SetWindowId(101u);
    keyboardProperty->SetWindowName("keyboard bar");
    keyboardProperty->SetWindowType(WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT);
    keyboardProperty->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    Rect keyboardRect = { 0, static_cast<int32_t>(screenRect.height_ / 2), screenRect.width_, screenRect.height_ / 2 };
    keyboardProperty->SetWindowRect(keyboardRect);
    windowListener = new WindowListener();
    keyboardWindowNode = new WindowNode(keyboardProperty, windowListener, nullptr);
}

void AvoidAreaControllerTest::TearDownTestCase()
{
}

void AvoidAreaControllerTest::SetUp()
{
}

void AvoidAreaControllerTest::TearDown()
{
}

bool CheckSameArea(AvoidArea avoidArea, Rect t, Rect l, Rect r, Rect b)
{
    return avoidArea.topRect_ == t && avoidArea.bottomRect_ == b
        && avoidArea.leftRect_ == l && avoidArea.rightRect_ == r;
}

sptr<WindowProperty> createWindowProperty(uint32_t windowId, const std::string& windowName,
    WindowType type, WindowMode mode, const Rect& screenRect)
{
    sptr<WindowProperty> property = new WindowProperty();
    property->SetWindowId(windowId);
    property->SetWindowName(windowName);
    property->SetWindowType(type);
    property->SetWindowMode(mode);
    property->SetWindowRect(screenRect);
    return property;
}

namespace {
/**
 * @tc.name: AvoidArea01
 * @tc.desc: Read and write avoidArea test
 * @tc.type: FUNC
 */
HWTEST_F(AvoidAreaControllerTest, AvoidArea01, Function | SmallTest | Level2)
{
    AvoidArea avoidarea;
    Parcel parcel;
    AvoidArea* readArea = AvoidArea::Unmarshalling(parcel);
    ASSERT_EQ(true, readArea == nullptr);
    ASSERT_EQ(true, avoidarea.Marshalling(parcel));
}

/**
 * @tc.name: GetSystemBarAvoidArea01
 * @tc.desc: Get avoid areas with TYPE_SYSTEM
 * @tc.type: FUNC
 */
HWTEST_F(AvoidAreaControllerTest, GetSystemBarAvoidArea01, Function | SmallTest | Level2)
{
    sptr<WindowProperty> property = createWindowProperty(110u, "test",
        WindowType::APP_WINDOW_BASE, WindowMode::WINDOW_MODE_FULLSCREEN, screenRect);
    sptr<WindowListener> listener = new WindowListener();
    sptr<WindowNode> appWindow = new WindowNode(property, listener, nullptr);
    uint32_t focusedWindow = appWindow->GetWindowId();
    sptr<AvoidAreaController> avoidAreaController = new AvoidAreaController(focusedWindow);
    avoidAreaController->ProcessWindowChange(statusbarWindowNode, AvoidControlType::AVOID_NODE_ADD, nullptr);
    avoidAreaController->ProcessWindowChange(navigationBarWindowNode, AvoidControlType::AVOID_NODE_ADD, nullptr);
    avoidAreaController->ProcessWindowChange(appWindow, AvoidControlType::AVOID_NODE_ADD, nullptr);
    auto avoidArea = avoidAreaController->GetAvoidAreaByType(appWindow, AvoidAreaType::TYPE_SYSTEM);
    ASSERT_EQ(true, CheckSameArea(avoidArea, statusbarWindowNode->GetWindowRect(),
        EMPTY_RECT, EMPTY_RECT, navigationBarWindowNode->GetWindowRect()));

    // set rect
    Rect statusBarRect = statusbarWindowNode->GetWindowRect();
    Rect navigationBarRect = navigationBarWindowNode->GetWindowRect();
    Rect windowRect = { 0, static_cast<int32_t>(statusBarRect.height_), statusBarRect.width_,
        static_cast<uint32_t>(navigationBarRect.posY_ - statusBarRect.height_) };
    property->SetWindowRect(windowRect);
    avoidArea = avoidAreaController->GetAvoidAreaByType(appWindow, AvoidAreaType::TYPE_SYSTEM);
    ASSERT_EQ(true, CheckSameArea(avoidArea, EMPTY_RECT, EMPTY_RECT, EMPTY_RECT, EMPTY_RECT));

    // restore rect
    property->SetWindowRect(screenRect);
    avoidArea = avoidAreaController->GetAvoidAreaByType(appWindow, AvoidAreaType::TYPE_SYSTEM);
    ASSERT_EQ(true, CheckSameArea(avoidArea, statusbarWindowNode->GetWindowRect(),
        EMPTY_RECT, EMPTY_RECT, navigationBarWindowNode->GetWindowRect()));

    avoidAreaController->ProcessWindowChange(statusbarWindowNode, AvoidControlType::AVOID_NODE_REMOVE, nullptr);
    avoidAreaController->ProcessWindowChange(navigationBarWindowNode, AvoidControlType::AVOID_NODE_REMOVE, nullptr);
    avoidAreaController->ProcessWindowChange(appWindow, AvoidControlType::AVOID_NODE_REMOVE, nullptr);
    avoidArea = avoidAreaController->GetAvoidAreaByType(appWindow, AvoidAreaType::TYPE_SYSTEM);
    ASSERT_EQ(true, CheckSameArea(avoidArea, EMPTY_RECT, EMPTY_RECT, EMPTY_RECT, EMPTY_RECT));
}

/**
 * @tc.name: SystemBarAvoidArea02
 * @tc.desc: Get avoid areas with listener, TYPE_SYSTEM.
 * @tc.type: FUNC
 */
HWTEST_F(AvoidAreaControllerTest, SystemBarAvoidArea02, Function | SmallTest | Level2)
{
    sptr<WindowProperty> property = createWindowProperty(110u, "test",
        WindowType::APP_WINDOW_BASE, WindowMode::WINDOW_MODE_FULLSCREEN, screenRect);
    sptr<WindowListener> windowListener = new WindowListener();
    sptr<WindowNode> appWindow = new WindowNode(property, windowListener, nullptr);
    uint32_t focusedWindow = appWindow->GetWindowId();
    sptr<AvoidAreaController> avoidAreaController = new AvoidAreaController(focusedWindow);
    avoidAreaController->ProcessWindowChange(appWindow, AvoidControlType::AVOID_NODE_ADD, nullptr);
    avoidAreaController->UpdateAvoidAreaListener(appWindow, true);

    // add status bar
    avoidAreaController->ProcessWindowChange(statusbarWindowNode, AvoidControlType::AVOID_NODE_ADD, nullptr);
    auto avoidArea = windowListener->statusBarAvoidAreaFuture_.GetResult(TIME_OUT);
    windowListener->statusBarAvoidAreaFuture_.Reset(EMPTY_AVOID_AREA);
    ASSERT_EQ(true, CheckSameArea(avoidArea, statusbarWindowNode->GetWindowRect(),
        EMPTY_RECT, EMPTY_RECT, EMPTY_RECT));

    // add navigation bar
    avoidAreaController->ProcessWindowChange(navigationBarWindowNode, AvoidControlType::AVOID_NODE_ADD, nullptr);
    avoidArea = windowListener->statusBarAvoidAreaFuture_.GetResult(TIME_OUT);
    windowListener->statusBarAvoidAreaFuture_.Reset(EMPTY_AVOID_AREA);
    ASSERT_EQ(true, CheckSameArea(avoidArea, statusbarWindowNode->GetWindowRect(), EMPTY_RECT,
        EMPTY_RECT, navigationBarWindowNode->GetWindowRect()));

    // update appWindow rect
    Rect statusBarRect = statusbarWindowNode->GetWindowRect();
    Rect navigationBarRect = navigationBarWindowNode->GetWindowRect();
    Rect windowRect = { 0, static_cast<int32_t>(statusBarRect.height_), statusBarRect.width_,
        static_cast<uint32_t>(navigationBarRect.posY_ - statusBarRect.height_) };
    property->SetWindowRect(windowRect);
    avoidAreaController->ProcessWindowChange(appWindow, AvoidControlType::AVOID_NODE_UPDATE, nullptr);
    avoidArea = windowListener->statusBarAvoidAreaFuture_.GetResult(TIME_OUT);
    windowListener->statusBarAvoidAreaFuture_.Reset(EMPTY_AVOID_AREA);
    ASSERT_EQ(true, CheckSameArea(avoidArea, EMPTY_RECT, EMPTY_RECT, EMPTY_RECT, EMPTY_RECT));

    // restore appWindow rect
    property->SetWindowRect(screenRect);
    avoidAreaController->ProcessWindowChange(appWindow, AvoidControlType::AVOID_NODE_UPDATE, nullptr);
    avoidArea = windowListener->statusBarAvoidAreaFuture_.GetResult(TIME_OUT);
    windowListener->statusBarAvoidAreaFuture_.Reset(EMPTY_AVOID_AREA);
    ASSERT_EQ(true, CheckSameArea(avoidArea, statusbarWindowNode->GetWindowRect(),
        EMPTY_RECT, EMPTY_RECT, navigationBarWindowNode->GetWindowRect()));

    avoidAreaController->ProcessWindowChange(statusbarWindowNode, AvoidControlType::AVOID_NODE_REMOVE, nullptr);
    avoidAreaController->ProcessWindowChange(navigationBarWindowNode, AvoidControlType::AVOID_NODE_REMOVE, nullptr);
    avoidAreaController->ProcessWindowChange(appWindow, AvoidControlType::AVOID_NODE_REMOVE, nullptr);
}

/**
 * @tc.name: SystemBarAvoidArea03
 * @tc.desc: Get avoid areas with listener, TYPE_SYSTEM.
 * @tc.type: FUNC
 */
HWTEST_F(AvoidAreaControllerTest, SystemBarAvoidArea03, Function | SmallTest | Level2)
{
    sptr<WindowProperty> property = createWindowProperty(110u, "test",
        WindowType::APP_WINDOW_BASE, WindowMode::WINDOW_MODE_FULLSCREEN, screenRect);
    sptr<WindowListener> windowListener = new WindowListener();
    sptr<WindowNode> appWindow = new WindowNode(property, windowListener, nullptr);
    uint32_t focusedWindow = appWindow->GetWindowId();
    sptr<AvoidAreaController> avoidAreaController = new AvoidAreaController(focusedWindow);
    avoidAreaController->ProcessWindowChange(appWindow, AvoidControlType::AVOID_NODE_ADD, nullptr);
    avoidAreaController->UpdateAvoidAreaListener(appWindow, true);

    // add status bar
    avoidAreaController->ProcessWindowChange(statusbarWindowNode, AvoidControlType::AVOID_NODE_ADD, nullptr);
    auto avoidArea = windowListener->statusBarAvoidAreaFuture_.GetResult(TIME_OUT);
    windowListener->statusBarAvoidAreaFuture_.Reset(EMPTY_AVOID_AREA);
    ASSERT_EQ(true, CheckSameArea(avoidArea, statusbarWindowNode->GetWindowRect(), EMPTY_RECT,
        EMPTY_RECT, EMPTY_RECT));

    // add navigation bar
    avoidAreaController->ProcessWindowChange(navigationBarWindowNode, AvoidControlType::AVOID_NODE_ADD, nullptr);
    avoidArea = windowListener->statusBarAvoidAreaFuture_.GetResult(TIME_OUT);
    windowListener->statusBarAvoidAreaFuture_.Reset(EMPTY_AVOID_AREA);
    ASSERT_EQ(true, CheckSameArea(avoidArea, statusbarWindowNode->GetWindowRect(), EMPTY_RECT,
        EMPTY_RECT, navigationBarWindowNode->GetWindowRect()));

    // remove status bar
    avoidAreaController->ProcessWindowChange(statusbarWindowNode, AvoidControlType::AVOID_NODE_REMOVE, nullptr);
    avoidArea = windowListener->statusBarAvoidAreaFuture_.GetResult(TIME_OUT);
    windowListener->statusBarAvoidAreaFuture_.Reset(EMPTY_AVOID_AREA);
    ASSERT_EQ(true, CheckSameArea(avoidArea, EMPTY_RECT, EMPTY_RECT, EMPTY_RECT,
        navigationBarWindowNode->GetWindowRect()));

    // remove navigation bar
    avoidAreaController->ProcessWindowChange(navigationBarWindowNode, AvoidControlType::AVOID_NODE_REMOVE, nullptr);
    avoidArea = windowListener->statusBarAvoidAreaFuture_.GetResult(TIME_OUT);
    windowListener->statusBarAvoidAreaFuture_.Reset(EMPTY_AVOID_AREA);
    ASSERT_EQ(true, CheckSameArea(avoidArea, EMPTY_RECT, EMPTY_RECT, EMPTY_RECT, EMPTY_RECT));
    avoidAreaController->ProcessWindowChange(appWindow, AvoidControlType::AVOID_NODE_REMOVE, nullptr);
}

/**
 * @tc.name: SystemBarAvoidArea01
 * @tc.desc: Get avoid areas with listener, TYPE_SYSTEM.
 * @tc.type: FUNC
 */
HWTEST_F(AvoidAreaControllerTest, SystemBarAvoidArea01, Function | SmallTest | Level2)
{
    sptr<WindowProperty> property = createWindowProperty(110u, "test",
        WindowType::APP_WINDOW_BASE, WindowMode::WINDOW_MODE_FULLSCREEN, screenRect);
    sptr<WindowListener> windowListener = new WindowListener();
    sptr<WindowNode> appWindow = new WindowNode(property, windowListener, nullptr);
    uint32_t focusedWindow = appWindow->GetWindowId();
    sptr<AvoidAreaController> avoidAreaController = new AvoidAreaController(focusedWindow);
    avoidAreaController->ProcessWindowChange(statusbarWindowNode, AvoidControlType::AVOID_NODE_ADD, nullptr);
    avoidAreaController->ProcessWindowChange(navigationBarWindowNode, AvoidControlType::AVOID_NODE_ADD, nullptr);
    avoidAreaController->ProcessWindowChange(appWindow, AvoidControlType::AVOID_NODE_ADD, nullptr);
    avoidAreaController->UpdateAvoidAreaListener(appWindow, true);

    // update status bar window Rect
    Rect statusbarWindowNodeRect = statusbarWindowNode->GetWindowRect();
    statusbarWindowNode->SetWindowRect(EMPTY_RECT);
    avoidAreaController->ProcessWindowChange(statusbarWindowNode, AvoidControlType::AVOID_NODE_UPDATE,
        [](sptr<WindowNode> windowNode) { return true; });
    auto avoidArea = windowListener->statusBarAvoidAreaFuture_.GetResult(TIME_OUT);
    windowListener->statusBarAvoidAreaFuture_.Reset(EMPTY_AVOID_AREA);
    ASSERT_EQ(true, CheckSameArea(avoidArea, EMPTY_RECT, EMPTY_RECT, EMPTY_RECT,
        navigationBarWindowNode->GetWindowRect()));

    // update navigation bar window Rect
    Rect navigationBarWindowNodeRect = navigationBarWindowNode->GetWindowRect();
    navigationBarWindowNode->SetWindowRect(EMPTY_RECT);
    avoidAreaController->ProcessWindowChange(navigationBarWindowNode, AvoidControlType::AVOID_NODE_UPDATE,
        [](sptr<WindowNode> windowNode) { return true; });
    avoidArea = windowListener->statusBarAvoidAreaFuture_.GetResult(TIME_OUT);
    windowListener->statusBarAvoidAreaFuture_.Reset(EMPTY_AVOID_AREA);
    ASSERT_EQ(true, CheckSameArea(avoidArea, EMPTY_RECT, EMPTY_RECT, EMPTY_RECT, EMPTY_RECT));

    // restore status bar window Rect
    statusbarWindowNode->SetWindowRect(statusbarWindowNodeRect);
    avoidAreaController->ProcessWindowChange(appWindow, AvoidControlType::AVOID_NODE_UPDATE, nullptr);
    avoidArea = windowListener->statusBarAvoidAreaFuture_.GetResult(TIME_OUT);
    windowListener->statusBarAvoidAreaFuture_.Reset(EMPTY_AVOID_AREA);
    ASSERT_EQ(true, CheckSameArea(avoidArea, statusbarWindowNode->GetWindowRect(), EMPTY_RECT,
        EMPTY_RECT, EMPTY_RECT));

    // restore navigation bar window Rect
    navigationBarWindowNode->SetWindowRect(navigationBarWindowNodeRect);
    avoidAreaController->ProcessWindowChange(appWindow, AvoidControlType::AVOID_NODE_UPDATE, nullptr);
    avoidArea = windowListener->statusBarAvoidAreaFuture_.GetResult(TIME_OUT);
    windowListener->statusBarAvoidAreaFuture_.Reset(EMPTY_AVOID_AREA);
    ASSERT_EQ(true, CheckSameArea(avoidArea, statusbarWindowNode->GetWindowRect(), EMPTY_RECT,
        EMPTY_RECT, navigationBarWindowNode->GetWindowRect()));

    // remove status bar
    avoidAreaController->ProcessWindowChange(statusbarWindowNode, AvoidControlType::AVOID_NODE_REMOVE, nullptr);
    avoidArea = windowListener->statusBarAvoidAreaFuture_.GetResult(TIME_OUT);
    windowListener->statusBarAvoidAreaFuture_.Reset(EMPTY_AVOID_AREA);
    ASSERT_EQ(true, CheckSameArea(avoidArea, EMPTY_RECT, EMPTY_RECT, EMPTY_RECT,
        navigationBarWindowNode->GetWindowRect()));

    // remove navigation bar
    avoidAreaController->ProcessWindowChange(navigationBarWindowNode, AvoidControlType::AVOID_NODE_REMOVE, nullptr);
    avoidArea = windowListener->statusBarAvoidAreaFuture_.GetResult(TIME_OUT);
    windowListener->statusBarAvoidAreaFuture_.Reset(EMPTY_AVOID_AREA);
    ASSERT_EQ(true, CheckSameArea(avoidArea, EMPTY_RECT, EMPTY_RECT, EMPTY_RECT, EMPTY_RECT));
}

/**
 * @tc.name: KeyboardAvoidArea01
 * @tc.desc: Get avoid areas with TYPE_KEYBOARD.
 * @tc.type: FUNC
 */
HWTEST_F(AvoidAreaControllerTest, KeyboardAvoidArea01, Function | SmallTest | Level2)
{
    sptr<WindowProperty> property = createWindowProperty(110u, "test",
        WindowType::APP_WINDOW_BASE, WindowMode::WINDOW_MODE_FULLSCREEN, screenRect);
    sptr<WindowListener> listener = new WindowListener();
    sptr<WindowNode> appWindow = new WindowNode(property, listener, nullptr);
    uint32_t focusedWindow = 0u;
    sptr<AvoidAreaController> avoidAreaController = new AvoidAreaController(focusedWindow);
    avoidAreaController->ProcessWindowChange(appWindow, AvoidControlType::AVOID_NODE_ADD, nullptr);

    uint32_t start = static_cast<uint32_t>(WindowMode::WINDOW_MODE_FULLSCREEN);
    uint32_t end = static_cast<uint32_t>(WindowMode::WINDOW_MODE_FLOATING);
    for (uint32_t i = start; i <= end; i++) {
        focusedWindow = 0u;
        appWindow->SetWindowMode(static_cast<WindowMode>(i));
        avoidAreaController->ProcessWindowChange(keyboardWindowNode, AvoidControlType::AVOID_NODE_ADD, nullptr);
        auto avoidArea = avoidAreaController->GetAvoidAreaByType(appWindow, AvoidAreaType::TYPE_KEYBOARD);
        ASSERT_EQ(true, CheckSameArea(avoidArea, EMPTY_RECT, EMPTY_RECT, EMPTY_RECT, EMPTY_RECT));

        keyboardWindowNode->SetCallingWindow(appWindow->GetWindowId());
        avoidArea = avoidAreaController->GetAvoidAreaByType(appWindow, AvoidAreaType::TYPE_KEYBOARD);
        ASSERT_EQ(true, CheckSameArea(avoidArea, EMPTY_RECT, EMPTY_RECT, EMPTY_RECT,
            keyboardWindowNode->GetWindowRect()));

        keyboardWindowNode->SetCallingWindow(0);
        focusedWindow = appWindow->GetWindowId();
        avoidArea = avoidAreaController->GetAvoidAreaByType(appWindow, AvoidAreaType::TYPE_KEYBOARD);
        ASSERT_EQ(true, CheckSameArea(avoidArea, EMPTY_RECT, EMPTY_RECT, EMPTY_RECT,
            keyboardWindowNode->GetWindowRect()));

        avoidAreaController->ProcessWindowChange(keyboardWindowNode, AvoidControlType::AVOID_NODE_REMOVE, nullptr);
        avoidArea = avoidAreaController->GetAvoidAreaByType(appWindow, AvoidAreaType::TYPE_KEYBOARD);
        ASSERT_EQ(true, CheckSameArea(avoidArea, EMPTY_RECT, EMPTY_RECT, EMPTY_RECT, EMPTY_RECT));
    }
}

/**
 * @tc.name: KeyboardAvoidArea02
 * @tc.desc: Get avoid areas with listener, TYPE_KEYBOARD.
 * @tc.type: FUNC
 */
HWTEST_F(AvoidAreaControllerTest, KeyboardAvoidArea02, Function | SmallTest | Level2)
{
    sptr<WindowProperty> property = createWindowProperty(110u, "test",
        WindowType::APP_WINDOW_BASE, WindowMode::WINDOW_MODE_FULLSCREEN, screenRect);
    sptr<WindowListener> listener = new WindowListener();
    sptr<WindowNode> appWindow = new WindowNode(property, listener, nullptr);
    uint32_t focusedWindow = appWindow->GetWindowId();
    sptr<AvoidAreaController> avoidAreaController = new AvoidAreaController(focusedWindow);
    avoidAreaController->ProcessWindowChange(appWindow, AvoidControlType::AVOID_NODE_ADD, nullptr);
    avoidAreaController->UpdateAvoidAreaListener(appWindow, true);
    uint32_t start = static_cast<uint32_t>(WindowMode::WINDOW_MODE_FULLSCREEN);
    uint32_t end = static_cast<uint32_t>(WindowMode::WINDOW_MODE_FLOATING);
    for (uint32_t i = start; i <= end; i++) {
        avoidAreaController->ProcessWindowChange(keyboardWindowNode, AvoidControlType::AVOID_NODE_ADD, nullptr);
        auto avoidArea = listener->keyboardAvoidAreaFuture_.GetResult(TIME_OUT);
        listener->keyboardAvoidAreaFuture_.Reset(EMPTY_AVOID_AREA);
        ASSERT_EQ(true, CheckSameArea(avoidArea, EMPTY_RECT, EMPTY_RECT, EMPTY_RECT,
            keyboardWindowNode->GetWindowRect()));
        avoidAreaController->ProcessWindowChange(keyboardWindowNode, AvoidControlType::AVOID_NODE_REMOVE, nullptr);
        avoidArea = listener->keyboardAvoidAreaFuture_.GetResult(TIME_OUT);
        listener->keyboardAvoidAreaFuture_.Reset(EMPTY_AVOID_AREA);
        ASSERT_EQ(true, CheckSameArea(avoidArea, EMPTY_RECT, EMPTY_RECT, EMPTY_RECT, EMPTY_RECT));
    }
    avoidAreaController->ProcessWindowChange(appWindow, AvoidControlType::AVOID_NODE_REMOVE, nullptr);
}
/**
 * @tc.name: KeyboardAvoidArea02
 * @tc.desc: Get avoid areas with listener, TYPE_KEYBOARD.
 * @tc.type: FUNC
 */
HWTEST_F(AvoidAreaControllerTest, UpdateAvoidAreaListener01, Function | SmallTest | Level2)
{
    sptr<WindowProperty> property = createWindowProperty(110u, "test",
        WindowType::APP_WINDOW_BASE, WindowMode::WINDOW_MODE_FULLSCREEN, screenRect);
    sptr<WindowListener> listener = new WindowListener();
    sptr<WindowNode> appWindow = new WindowNode(property, listener, nullptr);
    uint32_t focusedWindow = appWindow->GetWindowId();
    sptr<AvoidAreaController> avoidAreaController = new AvoidAreaController(focusedWindow);

    avoidAreaController->avoidAreaListenerNodes_.clear();
    avoidAreaController->lastUpdatedAvoidArea_.clear();
    avoidAreaController->avoidAreaListenerNodes_.insert(appWindow);
    auto avoidArea = avoidAreaController->GetAvoidAreaByType(appWindow, AvoidAreaType::TYPE_KEYBOARD);
    std::map<AvoidAreaType, AvoidArea> type_area_map;
    auto pair = std::make_pair(AvoidAreaType::TYPE_KEYBOARD, avoidArea);
    type_area_map.insert(pair);
    avoidAreaController->lastUpdatedAvoidArea_.insert(std::make_pair(focusedWindow, type_area_map));
    avoidAreaController->UpdateAvoidAreaListener(appWindow, false);
    ASSERT_EQ(0, avoidAreaController->avoidAreaListenerNodes_.size());
    ASSERT_EQ(0, avoidAreaController->lastUpdatedAvoidArea_.size());

    sptr<WindowNode> node = nullptr;
    avoidAreaController->UpdateAvoidAreaListener(node, true);
    ASSERT_EQ(0, avoidAreaController->avoidAreaListenerNodes_.size());
}
}
} // namespace Rosen
} // namespace OHOS

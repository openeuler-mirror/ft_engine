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

#include "display_manager.h"
#include "display_manager_config.h"
#include "future.h"
#include "wm_math.h"
#include "window_node.h"
#include "window_node_container.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class WindowNodeTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    virtual void SetUp() override;
    virtual void TearDown() override;
};

void WindowNodeTest::SetUpTestCase()
{
}

void WindowNodeTest::TearDownTestCase()
{
}

void WindowNodeTest::SetUp()
{
}

void WindowNodeTest::TearDown()
{
}
class WindowListener : public IWindow {
public:
    virtual WMError UpdateWindowRect(const struct Rect& rect, bool decoStatus, WindowSizeChangeReason reason) override
    {
        return WMError::WM_OK;
    };
    virtual WMError UpdateWindowMode(WindowMode mode) override
    {
        return WMError::WM_OK;
    };
    virtual WMError UpdateWindowModeSupportInfo(uint32_t modeSupportInfo) override
    {
        return WMError::WM_OK;
    };
    virtual WMError UpdateFocusStatus(bool focused) override
    {
        return WMError::WM_OK;
    };
    virtual WMError UpdateAvoidArea(const sptr<AvoidArea>& avoidArea, AvoidAreaType type) override
    {
        return WMError::WM_OK;
    };
    virtual WMError UpdateWindowState(WindowState state) override
    {
        return WMError::WM_OK;
    };
    virtual WMError UpdateWindowDragInfo(const PointInfo& point, DragEvent event) override
    {
        return WMError::WM_OK;
    };
    virtual WMError UpdateDisplayId(DisplayId from, DisplayId to) override
    {
        return WMError::WM_OK;
    };
    virtual WMError UpdateOccupiedAreaChangeInfo(const sptr<OccupiedAreaChangeInfo>& info) override
    {
        return WMError::WM_OK;
    };
    virtual WMError UpdateActiveStatus(bool isActive) override
    {
        return WMError::WM_OK;
    };
    virtual sptr<WindowProperty> GetWindowProperty() override
    {
        return nullptr;
    };
    virtual WMError NotifyTouchOutside() override
    {
        return WMError::WM_OK;
    };
    virtual WMError NotifyScreenshot() override
    {
        return WMError::WM_OK;
    };
    virtual WMError DumpInfo(const std::vector<std::string>& params) override
    {
        return WMError::WM_OK;
    };
    virtual WMError NotifyDestroy(void) override
    {
        return WMError::WM_OK;
    };
    WMError NotifyForeground(void) override
    {
        return WMError::WM_OK;
    };
    WMError NotifyBackground(void) override
    {
        return WMError::WM_OK;
    };
    virtual WMError NotifyWindowClientPointUp(const std::shared_ptr<MMI::PointerEvent>& pointerEvent) override
    {
        return WMError::WM_OK;
    };
    WMError UpdateZoomTransform(const Transform& trans, bool isDisplayZoomOn) override
    {
        return WMError::WM_OK;
    };
    virtual WMError RestoreSplitWindowMode(uint32_t mode) override
    {
        return WMError::WM_OK;
    };

    virtual sptr<IRemoteObject> AsObject() override
    {
        return nullptr;
    };
};

sptr<WindowProperty> CreateWindowProperty(uint32_t windowId, const std::string& windowName)
{
    sptr<WindowProperty> property = new WindowProperty();
    property->SetWindowId(windowId);
    property->SetWindowName(windowName);
    return property;
}

RSSurfaceNode::SharedPtr CreateRSSurfaceNode(std::string windowNode)
{
    struct RSSurfaceNodeConfig rsSurfaceNodeConfig;
    rsSurfaceNodeConfig.SurfaceNodeName = windowNode;
    auto surfaceNode = RSSurfaceNode::Create(rsSurfaceNodeConfig);
    return surfaceNode;
}
namespace {
/**
 * @tc.name: NewWindowNode01
 * @tc.desc: new window node with WindowProperty
 * @tc.type: FUNC
 */
HWTEST_F(WindowNodeTest, NewWindowNode01, Function | SmallTest | Level3)
{
    sptr<WindowNode> windowNode1 = new WindowNode();
    ASSERT_NE(nullptr, windowNode1);
    ASSERT_EQ("", windowNode1->GetWindowName());

    std::string windowName = "WindowNode01";
    auto property = CreateWindowProperty(1, windowName);
    ASSERT_NE(nullptr, property);

    sptr<WindowNode> windowNode2 = new WindowNode(property);
    ASSERT_NE(nullptr, windowNode2);
    ASSERT_EQ(windowName, windowNode2->GetWindowName());
}
/**
 * @tc.name: NewWindowNode02
 * @tc.desc: new window node with WindowProperty, RSSurfaceNode, IWindow
 * @tc.type: FUNC
 */
HWTEST_F(WindowNodeTest, NewWindowNode02, Function | SmallTest | Level1)
{
    std::string windowName = "WindowNode02";
    auto property = CreateWindowProperty(2, windowName);
    ASSERT_NE(nullptr, property);

    auto surfaceNode = CreateRSSurfaceNode(windowName);
    ASSERT_NE(nullptr, surfaceNode);

    sptr<WindowListener> iWindow = new WindowListener();
    ASSERT_NE(nullptr, iWindow);

    sptr<WindowNode> windowNode = new WindowNode(property, iWindow, surfaceNode);
    ASSERT_NE(nullptr, windowNode);
    ASSERT_EQ(windowName, windowNode->GetWindowName());
}
/**
 * @tc.name: NewWindowNode03
 * @tc.desc: new window node with WindowProperty, RSSurfaceNode, IWindow, pid , uid
 * @tc.type: FUNC
 */
HWTEST_F(WindowNodeTest, NewWindowNode03, Function | SmallTest | Level1)
{
    std::string windowName = "WindowNode03";
    auto property = CreateWindowProperty(3, windowName);
    ASSERT_NE(nullptr, property);
    auto surfaceNode = CreateRSSurfaceNode(windowName);
    ASSERT_NE(nullptr, surfaceNode);
    sptr<WindowListener> iWindow = new Rosen::WindowListener();
    ASSERT_NE(nullptr, iWindow);

    int32_t pid = 1;
    int32_t uid = 2;
    sptr<WindowNode> windowNode = new WindowNode(property, iWindow, surfaceNode, pid, uid);
    ASSERT_NE(nullptr, windowNode);

    ASSERT_EQ(1, windowNode->GetInputEventCallingPid());
    ASSERT_EQ(1, windowNode->GetCallingPid());
    ASSERT_EQ(2, windowNode->GetCallingUid());
}
/**
 * @tc.name: SetDisplayId01
 * @tc.desc: SetDisplayId & GetDisplayId
 * @tc.type: FUNC
 */
HWTEST_F(WindowNodeTest, SetDisplayId01, Function | SmallTest | Level1)
{
    std::string windowName = "WindowNode04";
    auto property = CreateWindowProperty(4, windowName);
    ASSERT_NE(nullptr, property);

    sptr<WindowNode> windowNode = new WindowNode(property);
    ASSERT_NE(nullptr, windowNode);
    ASSERT_EQ(0, windowNode->GetDisplayId());

    windowNode->SetDisplayId(1);
    ASSERT_EQ(1, windowNode->GetDisplayId());
}
/**
 * @tc.name: SetEntireWindowTouchHotArea01
 * @tc.desc: SetEntireWindowTouchHotArea & GetEntireWindowTouchHotArea
 * @tc.type: FUNC
 */
HWTEST_F(WindowNodeTest, SetEntireWindowTouchHotArea01, Function | SmallTest | Level1)
{
    std::string windowName = "WindowNode05";
    auto property = CreateWindowProperty(5, windowName);
    ASSERT_NE(nullptr, property);

    sptr<WindowNode> windowNode = new WindowNode(property);
    ASSERT_NE(nullptr, windowNode);

    ASSERT_EQ(0, windowNode->GetEntireWindowTouchHotArea().posX_);
    ASSERT_EQ(0, windowNode->GetEntireWindowTouchHotArea().posY_);
    ASSERT_EQ(0, windowNode->GetEntireWindowTouchHotArea().width_);
    ASSERT_EQ(0, windowNode->GetEntireWindowTouchHotArea().height_);

    Rect testValue = { 10, 10, 255, 255 };
    windowNode->SetEntireWindowTouchHotArea(testValue);
    ASSERT_EQ(testValue.posX_, windowNode->GetEntireWindowTouchHotArea().posX_);
    ASSERT_EQ(testValue.posY_, windowNode->GetEntireWindowTouchHotArea().posY_);
    ASSERT_EQ(testValue.width_, windowNode->GetEntireWindowTouchHotArea().width_);
    ASSERT_EQ(testValue.height_, windowNode->GetEntireWindowTouchHotArea().height_);
}
/**
 * @tc.name: SetEntireWindowPointerHotArea01
 * @tc.desc: SetEntireWindowPointerHotArea & GetEntireWindowPointerHotArea
 * @tc.type: FUNC
 */
HWTEST_F(WindowNodeTest, SetEntireWindowPointerHotArea01, Function | SmallTest | Level1)
{
    std::string windowName = "WindowNode06";
    auto property = CreateWindowProperty(6, windowName);
    ASSERT_NE(nullptr, property);

    sptr<WindowNode> windowNode = new WindowNode(property);
    ASSERT_NE(nullptr, windowNode);

    Rect rect1 = {0, 0, 0, 0};
    windowNode->SetEntireWindowPointerHotArea(rect1);
    ASSERT_EQ(0, windowNode->GetEntireWindowPointerHotArea().posX_);
    ASSERT_EQ(0, windowNode->GetEntireWindowPointerHotArea().posY_);
    ASSERT_EQ(0, windowNode->GetEntireWindowPointerHotArea().width_);
    ASSERT_EQ(0, windowNode->GetEntireWindowPointerHotArea().height_);

    Rect rect2 = {10, 10, 255, 255};
    windowNode->SetEntireWindowPointerHotArea(rect2);
    ASSERT_EQ(10, windowNode->GetEntireWindowPointerHotArea().posX_);
    ASSERT_EQ(10, windowNode->GetEntireWindowPointerHotArea().posY_);
    ASSERT_EQ(255, windowNode->GetEntireWindowPointerHotArea().width_);
    ASSERT_EQ(255, windowNode->GetEntireWindowPointerHotArea().height_);
}
/**
 * @tc.name: SetWindowRect01
 * @tc.desc: SetWindowRect & GetWindowRect
 * @tc.type: FUNC
 */
HWTEST_F(WindowNodeTest, SetWindowRect01, Function | SmallTest | Level1)
{
    std::string windowName = "WindowNode07";
    auto property = CreateWindowProperty(7, windowName);
    ASSERT_NE(nullptr, property);

    sptr<WindowNode> windowNode = new WindowNode(property);
    ASSERT_NE(nullptr, windowNode);

    Rect rect1 = {0, 0, 0, 0};
    windowNode->SetWindowRect(rect1);
    ASSERT_EQ(0, windowNode->GetWindowRect().posX_);
    ASSERT_EQ(0, windowNode->GetWindowRect().posY_);
    ASSERT_EQ(0, windowNode->GetWindowRect().width_);
    ASSERT_EQ(0, windowNode->GetWindowRect().height_);

    Rect rect2 = {10, 10, 255, 255};
    windowNode->SetWindowRect(rect2);
    ASSERT_EQ(10, windowNode->GetWindowRect().posX_);
    ASSERT_EQ(10, windowNode->GetWindowRect().posY_);
    ASSERT_EQ(255, windowNode->GetWindowRect().width_);
    ASSERT_EQ(255, windowNode->GetWindowRect().height_);
}
/**
 * @tc.name: SetDecoStatus01
 * @tc.desc: SetDecoStatus & GetDecoStatus
 * @tc.type: FUNC
 */
HWTEST_F(WindowNodeTest, SetDecoStatus01, Function | SmallTest | Level1)
{
    std::string windowName = "WindowNode08";
    auto property = CreateWindowProperty(8, windowName);
    ASSERT_NE(nullptr, property);
    sptr<WindowNode> windowNode = new WindowNode(property);
    ASSERT_NE(nullptr, windowNode);

    windowNode->SetDecoStatus(true);
    ASSERT_EQ(true, windowNode->GetDecoStatus());
    windowNode->SetDecoStatus(false);
    ASSERT_EQ(false, windowNode->GetDecoStatus());
}
/**
 * @tc.name: SetRequestRect01
 * @tc.desc: SetRequestRect & GetRequestRect
 * @tc.type: FUNC
 */
HWTEST_F(WindowNodeTest, SetRequestRect01, Function | SmallTest | Level1)
{
    std::string windowName = "WindowNode09";
    auto property = CreateWindowProperty(9, windowName);
    ASSERT_NE(nullptr, property);
    sptr<WindowNode> windowNode = new WindowNode(property);
    ASSERT_NE(nullptr, windowNode);

    Rect rect1 = { 0, 0, 0, 0 };
    windowNode->SetRequestRect(rect1);
    ASSERT_EQ(0, windowNode->GetRequestRect().posX_);
    ASSERT_EQ(0, windowNode->GetRequestRect().posY_);
    ASSERT_EQ(0, windowNode->GetRequestRect().width_);
    ASSERT_EQ(0, windowNode->GetRequestRect().height_);

    Rect rect2 = { 10, 10, 255, 255 };
    windowNode->SetRequestRect(rect2);
    ASSERT_EQ(10, windowNode->GetRequestRect().posX_);
    ASSERT_EQ(10, windowNode->GetRequestRect().posY_);
    ASSERT_EQ(255, windowNode->GetRequestRect().width_);
    ASSERT_EQ(255, windowNode->GetRequestRect().height_);
}
/**
 * @tc.name: SetWindowProperty01
 * @tc.desc: SetWindowProperty & GetWindowProperty
 * @tc.type: FUNC
 */
HWTEST_F(WindowNodeTest, SetWindowProperty01, Function | SmallTest | Level1)
{
    std::string windowName = "WindowNode09";
    auto property = CreateWindowProperty(9, windowName);
    ASSERT_NE(nullptr, property);
    sptr<WindowNode> windowNode = new WindowNode(property);
    ASSERT_NE(nullptr, windowNode);
    ASSERT_EQ(property, windowNode->GetWindowProperty());

    auto property2 = CreateWindowProperty(10, windowName);
    ASSERT_NE(nullptr, property2);
    windowNode->SetWindowProperty(property2);
    ASSERT_EQ(property2, windowNode->GetWindowProperty());
}
/**
 * @tc.name: SetSystemBarProperty01
 * @tc.desc: SetSystemBarProperty & GetSystemBarProperty
 * @tc.type: FUNC
 */
HWTEST_F(WindowNodeTest, SetSystemBarProperty01, Function | SmallTest | Level1)
{
    std::string windowName = "WindowNode10";
    auto property = CreateWindowProperty(10, windowName);
    ASSERT_NE(nullptr, property);
    sptr<WindowNode> windowNode = new WindowNode(property);
    ASSERT_NE(nullptr, windowNode);

    SystemBarProperty systemBarProperty1;
    SystemBarProperty systemBarProperty2;
    SystemBarProperty systemBarProperty3;
    windowNode->SetSystemBarProperty(WindowType::WINDOW_TYPE_STATUS_BAR, systemBarProperty1);
    windowNode->SetSystemBarProperty(WindowType::WINDOW_TYPE_NAVIGATION_BAR, systemBarProperty2);
    windowNode->SetSystemBarProperty(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW, systemBarProperty3);

    auto systemBarProperties = windowNode->GetSystemBarProperty();
    ASSERT_EQ(systemBarProperty1, systemBarProperties.find(WindowType::WINDOW_TYPE_STATUS_BAR)->second);
    ASSERT_EQ(systemBarProperty2, systemBarProperties.find(WindowType::WINDOW_TYPE_NAVIGATION_BAR)->second);
    ASSERT_EQ(systemBarProperties.end(), systemBarProperties.find(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW));
}
/**
 * @tc.name: SetWindowMode01
 * @tc.desc: SetWindowMode & GetWindowMode
 * @tc.type: FUNC
 */
HWTEST_F(WindowNodeTest, SetWindowMode01, Function | SmallTest | Level1)
{
    std::string windowName = "WindowNode11";
    auto property = CreateWindowProperty(11, windowName);
    ASSERT_NE(nullptr, property);
    sptr<WindowNode> windowNode = new WindowNode(property);
    ASSERT_NE(nullptr, windowNode);
    ASSERT_EQ(WindowMode::WINDOW_MODE_UNDEFINED, windowNode->GetWindowMode());

    windowNode->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);
    ASSERT_EQ(WindowMode::WINDOW_MODE_FULLSCREEN, windowNode->GetWindowMode());
}
/**
 * @tc.name: SetBrightness01
 * @tc.desc: SetBrightness & GetBrightness
 * @tc.type: FUNC
 */
HWTEST_F(WindowNodeTest, SetBrightness01, Function | SmallTest | Level1)
{
    std::string windowName = "WindowNode12";
    auto property = CreateWindowProperty(12, windowName);
    ASSERT_NE(nullptr, property);
    sptr<WindowNode> windowNode = new WindowNode(property);
    ASSERT_NE(nullptr, windowNode);

    ASSERT_EQ(UNDEFINED_BRIGHTNESS, windowNode->GetBrightness());

    windowNode->SetBrightness(0.5f);
    ASSERT_EQ(0.5f, windowNode->GetBrightness());
    windowNode->SetBrightness(1.1f);
    ASSERT_EQ(1.1f, windowNode->GetBrightness());
}
/**
 * @tc.name: SetTurnScreenOn01
 * @tc.desc: SetTurnScreenOn & IsTurnScreenOn
 * @tc.type: FUNC
 */
HWTEST_F(WindowNodeTest, SetTurnScreenOn01, Function | SmallTest | Level1)
{
    std::string windowName = "WindowNode13";
    auto property = CreateWindowProperty(13, windowName);
    ASSERT_NE(nullptr, property);
    sptr<WindowNode> windowNode = new WindowNode(property);
    ASSERT_NE(nullptr, windowNode);

    ASSERT_EQ(false, windowNode->IsTurnScreenOn());
    windowNode->SetTurnScreenOn(true);
    ASSERT_EQ(true, windowNode->IsTurnScreenOn());
}
/**
 * @tc.name: SetKeepScreenOn01
 * @tc.desc: SetKeepScreenOn & IsKeepScreenOn
 * @tc.type: FUNC
 */
HWTEST_F(WindowNodeTest, SetKeepScreenOn01, Function | SmallTest | Level1)
{
    std::string windowName = "WindowNode14";
    auto property = CreateWindowProperty(14, windowName);
    ASSERT_NE(nullptr, property);
    sptr<WindowNode> windowNode = new WindowNode(property);
    ASSERT_NE(nullptr, windowNode);

    ASSERT_EQ(false, windowNode->IsKeepScreenOn());
    windowNode->SetKeepScreenOn(true);
    ASSERT_EQ(true, windowNode->IsKeepScreenOn());
}
/**
 * @tc.name: SetCallingWindow01
 * @tc.desc: SetCallingWindow & GetCallingWindow
 * @tc.type: FUNC
 */
HWTEST_F(WindowNodeTest, SetCallingWindow01, Function | SmallTest | Level1)
{
    std::string windowName = "WindowNode15";
    auto property = CreateWindowProperty(15, windowName);
    ASSERT_NE(nullptr, property);
    sptr<WindowNode> windowNode = new WindowNode(property);
    ASSERT_NE(nullptr, windowNode);

    ASSERT_EQ(INVALID_WINDOW_ID, windowNode->GetCallingWindow());
    windowNode->SetCallingWindow(100);
    ASSERT_EQ(100, windowNode->GetCallingWindow());
}
/**
 * @tc.name: SetCallingPid01
 * @tc.desc: SetCallingPid & GetCallingPid, SetInputEventCallingPid & GetInputEventCallingPid
 * @tc.type: FUNC
 */
HWTEST_F(WindowNodeTest, SetCallingPid01, Function | SmallTest | Level1)
{
    std::string windowName = "WindowNode16";
    auto property = CreateWindowProperty(16, windowName);
    ASSERT_NE(nullptr, property);
    sptr<WindowNode> windowNode = new WindowNode(property);
    ASSERT_NE(nullptr, windowNode);

    ASSERT_EQ(0, windowNode->GetCallingPid());
    ASSERT_EQ(0, windowNode->GetInputEventCallingPid());

    windowNode->SetCallingPid(1);
    ASSERT_EQ(1, windowNode->GetCallingPid());
    ASSERT_EQ(1, windowNode->GetInputEventCallingPid());

    windowNode->SetInputEventCallingPid(2);
    ASSERT_EQ(1, windowNode->GetCallingPid());
    ASSERT_EQ(2, windowNode->GetInputEventCallingPid());
}
/**
 * @tc.name: SetCallingUid01
 * @tc.desc: SetCallingUid & GetCallingUid
 * @tc.type: FUNC
 */
HWTEST_F(WindowNodeTest, SetCallingUid01, Function | SmallTest | Level1)
{
    std::string windowName = "WindowNode17";
    auto property = CreateWindowProperty(17, windowName);
    ASSERT_NE(nullptr, property);
    sptr<WindowNode> windowNode = new WindowNode(property);
    ASSERT_NE(nullptr, windowNode);

    ASSERT_EQ(0, windowNode->GetCallingUid());

    windowNode->SetCallingUid(1);
    ASSERT_EQ(1, windowNode->GetCallingUid());
}
/**
 * @tc.name: SetWindowSizeChangeReason01
 * @tc.desc: SetWindowSizeChangeReason & GetWindowSizeChangeReason & ResetWindowSizeChangeReason
 * @tc.type: FUNC
 */
HWTEST_F(WindowNodeTest, SetWindowSizeChangeReason01, Function | SmallTest | Level1)
{
    std::string windowName = "WindowNode19";
    auto property = CreateWindowProperty(19, windowName);
    ASSERT_NE(nullptr, property);
    sptr<WindowNode> windowNode = new WindowNode(property);
    ASSERT_NE(nullptr, windowNode);

    ASSERT_EQ(WindowSizeChangeReason::UNDEFINED, windowNode->GetWindowSizeChangeReason());
    windowNode->SetWindowSizeChangeReason(WindowSizeChangeReason::MAXIMIZE);
    ASSERT_EQ(WindowSizeChangeReason::MAXIMIZE, windowNode->GetWindowSizeChangeReason());
    windowNode->ResetWindowSizeChangeReason();
    ASSERT_EQ(WindowSizeChangeReason::UNDEFINED, windowNode->GetWindowSizeChangeReason());
}
/**
 * @tc.name: SetRequestedOrientation01
 * @tc.desc: SetRequestedOrientation & GetRequestedOrientation
 * @tc.type: FUNC
 */
HWTEST_F(WindowNodeTest, SetRequestedOrientation01, Function | SmallTest | Level1)
{
    std::string windowName = "WindowNode20";
    auto property = CreateWindowProperty(20, windowName);
    ASSERT_NE(nullptr, property);
    sptr<WindowNode> windowNode = new WindowNode(property);
    ASSERT_NE(nullptr, windowNode);

    ASSERT_EQ(Orientation::UNSPECIFIED, windowNode->GetRequestedOrientation());
    windowNode->SetRequestedOrientation(Orientation::REVERSE_VERTICAL);
    ASSERT_EQ(Orientation::REVERSE_VERTICAL, windowNode->GetRequestedOrientation());
}
/**
 * @tc.name: SetShowingDisplays01
 * @tc.desc: SetShowingDisplays & GetShowingDisplays
 * @tc.type: FUNC
 */
HWTEST_F(WindowNodeTest, SetShowingDisplays01, Function | SmallTest | Level1)
{
    std::string windowName = "WindowNode21";
    auto property = CreateWindowProperty(21, windowName);
    ASSERT_NE(nullptr, property);
    sptr<WindowNode> windowNode = new WindowNode(property);
    ASSERT_NE(nullptr, windowNode);

    auto displays = windowNode->GetShowingDisplays();
    ASSERT_EQ(true, displays.empty());

    std::vector<DisplayId> emptyDisplayIds;
    windowNode->SetShowingDisplays(emptyDisplayIds);
    ASSERT_EQ(true, windowNode->GetShowingDisplays().empty());

    displays.push_back(static_cast<DisplayId>(0));
    windowNode->SetShowingDisplays(displays);
    ASSERT_EQ(1, windowNode->GetShowingDisplays().size());
}
/**
 * @tc.name: SetModeSupportInfo01
 * @tc.desc: SetModeSupportInfo & GetModeSupportInfo
 * @tc.type: FUNC
 */
HWTEST_F(WindowNodeTest, SetModeSupportInfo01, Function | SmallTest | Level1)
{
    std::string windowName = "WindowNode22";
    auto property = CreateWindowProperty(22, windowName);
    ASSERT_NE(nullptr, property);
    sptr<WindowNode> windowNode = new WindowNode(property);
    ASSERT_NE(nullptr, windowNode);

    ASSERT_EQ(WindowModeSupport::WINDOW_MODE_SUPPORT_ALL, windowNode->GetModeSupportInfo());
    windowNode->SetModeSupportInfo(WindowModeSupport::WINDOW_MODE_SUPPORT_FULLSCREEN);
    ASSERT_EQ(WindowModeSupport::WINDOW_MODE_SUPPORT_FULLSCREEN, windowNode->GetModeSupportInfo());
}
/**
 * @tc.name: SetDragType01
 * @tc.desc: SetDragType & GetDragType
 * @tc.type: FUNC
 */
HWTEST_F(WindowNodeTest, SetDragType01, Function | SmallTest | Level1)
{
    std::string windowName = "WindowNode23";
    auto property = CreateWindowProperty(23, windowName);
    ASSERT_NE(nullptr, property);
    sptr<WindowNode> windowNode = new WindowNode(property);
    ASSERT_NE(nullptr, windowNode);

    ASSERT_EQ(DragType::DRAG_UNDEFINED, windowNode->GetDragType());
    windowNode->SetDragType(DragType::DRAG_BOTTOM_OR_TOP);
    ASSERT_EQ(DragType::DRAG_BOTTOM_OR_TOP, windowNode->GetDragType());
}
/**
 * @tc.name: SetOriginRect01
 * @tc.desc: SetOriginRect & GetOriginRect
 * @tc.type: FUNC
 */
HWTEST_F(WindowNodeTest, SetOriginRect01, Function | SmallTest | Level1)
{
    std::string windowName = "WindowNode24";
    auto property = CreateWindowProperty(24, windowName);
    ASSERT_NE(nullptr, property);
    sptr<WindowNode> windowNode = new WindowNode(property);
    ASSERT_NE(nullptr, windowNode);

    auto defaultRect = windowNode->GetOriginRect();
    ASSERT_EQ(0, defaultRect.posX_);
    ASSERT_EQ(0, defaultRect.posX_);
    ASSERT_EQ(0, defaultRect.width_);
    ASSERT_EQ(0, defaultRect.height_);

    Rect testRect = { 10, 10, 150, 150 };
    windowNode->SetOriginRect(testRect);
    auto resultRect = windowNode->GetOriginRect();
    ASSERT_EQ(testRect, resultRect);
}
/**
 * @tc.name: SetTouchHotAreas01
 * @tc.desc: SetTouchHotAreas & GetTouchHotAreas
 * @tc.type: FUNC
 */
HWTEST_F(WindowNodeTest, SetTouchHotAreas01, Function | SmallTest | Level1)
{
    std::string windowName = "WindowNode25";
    auto property = CreateWindowProperty(25, windowName);
    ASSERT_NE(nullptr, property);
    sptr<WindowNode> windowNode = new WindowNode(property);
    ASSERT_NE(nullptr, windowNode);

    std::vector<Rect> testRects;
    windowNode->GetTouchHotAreas(testRects);
    ASSERT_EQ(true, testRects.empty());

    Rect rect1 = { 10, 10, 10, 10 };
    testRects.push_back(rect1);
    windowNode->SetTouchHotAreas(testRects);

    std::vector<Rect> resultRect;
    windowNode->GetTouchHotAreas(resultRect);
    ASSERT_EQ(1, resultRect.size());
    ASSERT_EQ(rect1, resultRect[0]);
}
/**
 * @tc.name: SetPointerHotAreas01
 * @tc.desc: SetPointerHotAreas & GetPointerHotAreas
 * @tc.type: FUNC
 */
HWTEST_F(WindowNodeTest, SetPointerHotAreas01, Function | SmallTest | Level1)
{
    std::string windowName = "WindowNode26";
    auto property = CreateWindowProperty(26, windowName);
    ASSERT_NE(nullptr, property);
    sptr<WindowNode> windowNode = new WindowNode(property);
    ASSERT_NE(nullptr, windowNode);

    std::vector<Rect> testRects;
    windowNode->GetPointerHotAreas(testRects);
    ASSERT_EQ(true, testRects.empty());

    Rect rect1 = { 10, 10, 10, 10 };
    testRects.push_back(rect1);
    windowNode->SetPointerHotAreas(testRects);
    std::vector<Rect> resultRect;
    windowNode->GetPointerHotAreas(resultRect);
    ASSERT_EQ(1, resultRect.size());
    ASSERT_EQ(rect1, resultRect[0]);
}
/**
 * @tc.name: SetPointerHotAreas01
 * @tc.desc: SetWindowSizeLimits & GetWindowSizeLimits
 * @tc.type: FUNC
 */
HWTEST_F(WindowNodeTest, SetWindowSizeLimits01, Function | SmallTest | Level1)
{
    std::string windowName = "WindowNode27";
    auto property = CreateWindowProperty(27, windowName);
    ASSERT_NE(nullptr, property);
    sptr<WindowNode> windowNode = new WindowNode(property);
    ASSERT_NE(nullptr, windowNode);

    auto defaultValue = windowNode->GetWindowSizeLimits();
    ASSERT_EQ(0, defaultValue.minWidth_);
    ASSERT_EQ(0, defaultValue.minHeight_);
    ASSERT_EQ(0.0f, defaultValue.minRatio_);
    ASSERT_EQ(UINT32_MAX, defaultValue.maxWidth_);
    ASSERT_EQ(UINT32_MAX, defaultValue.maxHeight_);
    ASSERT_EQ(FLT_MAX, defaultValue.maxRatio_);

    WindowSizeLimits testValue = { 200, 200, 50, 50, 2.0f, 2.0f };
    windowNode->SetWindowSizeLimits(testValue);

    auto resultValue = windowNode->GetWindowSizeLimits();
    ASSERT_EQ(testValue.minWidth_, resultValue.minWidth_);
    ASSERT_EQ(testValue.minHeight_, resultValue.minHeight_);
    ASSERT_EQ(testValue.minRatio_, resultValue.minRatio_);
    ASSERT_EQ(testValue.maxWidth_, resultValue.maxWidth_);
    ASSERT_EQ(testValue.maxHeight_, resultValue.maxHeight_);
    ASSERT_EQ(testValue.maxRatio_, resultValue.maxRatio_);
}
/**
 * @tc.name: SetWindowUpdatedSizeLimits01
 * @tc.desc: SetWindowUpdatedSizeLimits & GetWindowUpdatedSizeLimits
 * @tc.type: FUNC
 */
HWTEST_F(WindowNodeTest, SetWindowUpdatedSizeLimits01, Function | SmallTest | Level1)
{
    std::string windowName = "WindowNode28";
    auto property = CreateWindowProperty(28, windowName);
    ASSERT_NE(nullptr, property);
    sptr<WindowNode> windowNode = new WindowNode(property);
    ASSERT_NE(nullptr, windowNode);

    auto defaultValue = windowNode->GetWindowUpdatedSizeLimits();
    ASSERT_EQ(0, defaultValue.minWidth_);
    ASSERT_EQ(0, defaultValue.minHeight_);
    ASSERT_EQ(0.0f, defaultValue.minRatio_);
    ASSERT_EQ(UINT32_MAX, defaultValue.maxWidth_);
    ASSERT_EQ(UINT32_MAX, defaultValue.maxHeight_);
    ASSERT_EQ(FLT_MAX, defaultValue.maxRatio_);

    WindowSizeLimits testValue = { 200, 200, 50, 50, 2.0f, 2.0f };
    windowNode->SetWindowUpdatedSizeLimits(testValue);

    auto resultValue = windowNode->GetWindowUpdatedSizeLimits();
    ASSERT_EQ(testValue.minWidth_, resultValue.minWidth_);
    ASSERT_EQ(testValue.minHeight_, resultValue.minHeight_);
    ASSERT_EQ(testValue.minRatio_, resultValue.minRatio_);
    ASSERT_EQ(testValue.maxWidth_, resultValue.maxWidth_);
    ASSERT_EQ(testValue.maxHeight_, resultValue.maxHeight_);
    ASSERT_EQ(testValue.maxRatio_, resultValue.maxRatio_);
}
/**
 * @tc.name: SetSnapshot01
 * @tc.desc: SetSnapshot & GetSnapshot
 * @tc.type: FUNC
 */
HWTEST_F(WindowNodeTest, SetSnapshot01, Function | SmallTest | Level1)
{
    std::string windowName = "WindowNode29";
    auto property = CreateWindowProperty(29, windowName);
    ASSERT_NE(nullptr, property);
    sptr<WindowNode> windowNode = new WindowNode(property);
    ASSERT_NE(nullptr, windowNode);

    auto defaultValue = windowNode->GetSnapshot();
    ASSERT_EQ(0, defaultValue.use_count());

    std::shared_ptr<Media::PixelMap> pixelMap = std::make_shared<Media::PixelMap>();
    windowNode->SetSnapshot(std::move(pixelMap));

    auto resultValue = windowNode->GetSnapshot();
    ASSERT_EQ(2, resultValue.use_count());
}
/**
 * @tc.name: UpdateZoomTransform01
 * @tc.desc: UpdateZoomTransform & GetZoomTransform
 * @tc.type: FUNC
 */
HWTEST_F(WindowNodeTest, UpdateZoomTransform01, Function | SmallTest | Level1)
{
    std::string windowName = "WindowNode30";
    auto property = CreateWindowProperty(30, windowName);
    ASSERT_NE(nullptr, property);
    sptr<WindowNode> windowNode = new WindowNode(property);
    ASSERT_NE(nullptr, windowNode);

    Transform transformData;
    auto defaultValue = windowNode->GetZoomTransform();
    ASSERT_EQ(transformData, defaultValue);

    transformData.pivotX_ = 1.0f;
    transformData.pivotY_ = 1.0f;
    windowNode->UpdateZoomTransform(transformData, false);

    auto resultValue = windowNode->GetZoomTransform();
    ASSERT_EQ(1.0f, resultValue.pivotX_);
    ASSERT_EQ(1.0f, resultValue.pivotY_);
}
/**
 * @tc.name: SetTransform01
 * @tc.desc: SetTransform & ComputeTransform
 * @tc.type: FUNC
 */
HWTEST_F(WindowNodeTest, SetTransform01, Function | SmallTest | Level1)
{
    std::string windowName = "WindowNode31";
    auto property = CreateWindowProperty(31, windowName);
    ASSERT_NE(nullptr, property);
    sptr<WindowNode> windowNode = new WindowNode(property);
    ASSERT_NE(nullptr, windowNode);

    auto isSameValueMat4 = [](TransformHelper::Matrix4 expectVal, TransformHelper::Matrix4 checkValue) -> bool {
        uint32_t m = 0, n = 0;
        for (uint32_t i = 0; i < 16; i++) {
            m = i / 4;
            n = i % 4;
            if (m > 4 || n > 4 || (expectVal.mat_[m][n] != checkValue.mat_[m][n])) {
                return false;
            }
        }
        return true;
    };

    Transform transformData;
    auto defaultTrans = windowNode->property_->trans_;
    auto defaultWorldTransformMat4 = windowNode->property_->worldTransformMat_;
    ASSERT_EQ(transformData, defaultTrans);
    ASSERT_EQ(true, isSameValueMat4(TransformHelper::Matrix4::Identity, defaultWorldTransformMat4));

    transformData.pivotX_ = 1.0f;
    transformData.pivotY_ = 1.0f;
    transformData.translateX_ = 1.0f;
    transformData.translateY_ = 1.0f;
    windowNode->SetTransform(transformData);
    windowNode->ComputeTransform();

    auto resultTrans = windowNode->property_->trans_;
    auto resultWorldTransformMat4 = windowNode->property_->worldTransformMat_;
    ASSERT_EQ(1.0f, resultTrans.pivotX_);
    ASSERT_EQ(1.0f, resultTrans.pivotY_);

    ASSERT_EQ(1.0f, resultWorldTransformMat4.mat_[3][0]);
    ASSERT_EQ(1.0f, resultWorldTransformMat4.mat_[3][1]);
}
}
}
}
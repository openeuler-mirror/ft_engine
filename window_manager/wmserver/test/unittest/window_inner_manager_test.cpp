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

#include "event_runner.h"
#include "pointer_event.h"
#include "window_inner_manager.h"
#include "wm_common.h"

using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace Rosen {
using InnerTask = std::function<void()>;
using EventRunner = OHOS::AppExecFwk::EventRunner;
using EventHandler = OHOS::AppExecFwk::EventHandler;
using EventPriority = OHOS::AppExecFwk::EventQueue::Priority;

namespace {
    const std::string INNER_WM_THREAD_NAME = "TestInnerWindowManager";
}

class WindowInnerManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void WindowInnerManagerTest::SetUpTestCase()
{
}

void WindowInnerManagerTest::TearDownTestCase()
{
}

void WindowInnerManagerTest::SetUp()
{
}

void WindowInnerManagerTest::TearDown()
{
}

namespace {
/**
 * @tc.name: Stop
 * @tc.desc: test WindowInnerManager Stop
 * @tc.type: FUNC
 */
HWTEST_F(WindowInnerManagerTest, Stop, Function | SmallTest | Level2)
{
    WindowInnerManager& windowInnerManager = WindowInnerManager::GetInstance();
    windowInnerManager.state_ = InnerWMRunningState::STATE_RUNNING;
    windowInnerManager.eventLoop_ = nullptr;
    windowInnerManager.eventHandler_ = nullptr;
    windowInnerManager.moveDragController_ = nullptr;
    windowInnerManager.Stop();
    ASSERT_EQ(InnerWMRunningState::STATE_NOT_START, windowInnerManager.state_);

    windowInnerManager.eventLoop_ = AppExecFwk::EventRunner::Create(INNER_WM_THREAD_NAME);
    windowInnerManager.eventHandler_ = std::make_shared<EventHandler>(windowInnerManager.eventLoop_);
    windowInnerManager.moveDragController_ = new MoveDragController();
    windowInnerManager.Stop();
    ASSERT_EQ(0, windowInnerManager.eventLoop_.use_count());
    ASSERT_EQ(0, windowInnerManager.eventHandler_.use_count());
}

/**
 * @tc.name: NotifyServerReadyToMoveOrDrag
 * @tc.desc: test WindowInnerManager NotifyServerReadyToMoveOrDrag
 * @tc.type: FUNC
 */
HWTEST_F(WindowInnerManagerTest, NotifyServerReadyToMoveOrDrag, Function | SmallTest | Level2)
{
    WindowInnerManager& windowInnerManager = WindowInnerManager::GetInstance();
    windowInnerManager.eventHandler_ = nullptr;
    windowInnerManager.moveDragController_ = new MoveDragController();
    windowInnerManager.moveDragController_->activeWindowId_ = 1;
    uint32_t windowId = 1;
    sptr<WindowProperty> property = new WindowProperty();
    ASSERT_NE(property, nullptr);
    sptr<MoveDragProperty> moveDragProperty = new MoveDragProperty();
    ASSERT_NE(moveDragProperty, nullptr);
    auto result = windowInnerManager.NotifyServerReadyToMoveOrDrag(windowId, property, moveDragProperty);
    ASSERT_EQ(false, result);

    windowInnerManager.moveDragController_->activeWindowId_ = INVALID_WINDOW_ID;
    result = windowInnerManager.NotifyServerReadyToMoveOrDrag(windowId, property, moveDragProperty);
    ASSERT_EQ(true, result);
}

/**
 * @tc.name: NotifyWindowEndUpMovingOrDragging
 * @tc.desc: test WindowInnerManager NotifyWindowEndUpMovingOrDragging
 * @tc.type: FUNC
 */
HWTEST_F(WindowInnerManagerTest, NotifyWindowEndUpMovingOrDragging, Function | SmallTest | Level2)
{
    WindowInnerManager& windowInnerManager = WindowInnerManager::GetInstance();
    windowInnerManager.eventHandler_ = nullptr;
    windowInnerManager.moveDragController_ = new MoveDragController();
    windowInnerManager.moveDragController_->activeWindowId_ = INVALID_WINDOW_ID;
    uint32_t windowId = 1;

    windowInnerManager.NotifyWindowEndUpMovingOrDragging(windowId);

    windowId = INVALID_WINDOW_ID;
    windowInnerManager.NotifyWindowEndUpMovingOrDragging(windowId);
}
/**
 * @tc.name: NotifyWindowRemovedOrDestroyed
 * @tc.desc: test WindowInnerManager NotifyWindowRemovedOrDestroyed
 * @tc.type: FUNC
 */
HWTEST_F(WindowInnerManagerTest, NotifyWindowRemovedOrDestroyed, Function | SmallTest | Level2)
{
    WindowInnerManager& windowInnerManager = WindowInnerManager::GetInstance();
    windowInnerManager.eventHandler_ = nullptr;
    windowInnerManager.moveDragController_ = new MoveDragController();
    windowInnerManager.moveDragController_->activeWindowId_ = INVALID_WINDOW_ID;

    uint32_t windowId = 1;
    windowInnerManager.NotifyWindowRemovedOrDestroyed(windowId);

    windowId = INVALID_WINDOW_ID;
    windowInnerManager.NotifyWindowRemovedOrDestroyed(windowId);
}
/**
 * @tc.name: ConsumePointerEvent
 * @tc.desc: test WindowInnerManager ConsumePointerEvent
 * @tc.type: FUNC
 */
HWTEST_F(WindowInnerManagerTest, ConsumePointerEvent, Function | SmallTest | Level2)
{
    WindowInnerManager& windowInnerManager = WindowInnerManager::GetInstance();
    windowInnerManager.eventHandler_ = nullptr;
    windowInnerManager.moveDragController_ = new MoveDragController();

    auto pointerEvent = MMI::PointerEvent::Create();
    pointerEvent->processedCallback_ = nullptr;
    pointerEvent->sourceType_ = MMI::PointerEvent::SOURCE_TYPE_MOUSE;
    pointerEvent->buttonId_ = MMI::PointerEvent::MOUSE_BUTTON_RIGHT;
    pointerEvent->pointerAction_ = MMI::PointerEvent::POINTER_ACTION_UNKNOWN;

    pointerEvent->agentWindowId_ = 1;
    windowInnerManager.moveDragController_->activeWindowId_ = INVALID_WINDOW_ID;
    windowInnerManager.ConsumePointerEvent(pointerEvent);

    pointerEvent->agentWindowId_ = 1;
    windowInnerManager.moveDragController_->activeWindowId_ = 1;
    windowInnerManager.ConsumePointerEvent(pointerEvent);

    pointerEvent->agentWindowId_ = INVALID_WINDOW_ID;
    windowInnerManager.moveDragController_->activeWindowId_ = INVALID_WINDOW_ID;
    windowInnerManager.ConsumePointerEvent(pointerEvent);
}
}
}
}

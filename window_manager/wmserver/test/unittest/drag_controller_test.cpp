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
#include "drag_controller.h"
#include "window_helper.h"
#include "window_inner_manager.h"
#include "window_manager_service.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class DragControllerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
private:
    static sptr<MoveDragController> moveDragController_;
    static std::shared_ptr<MMI::IInputEventConsumer> inputListener_;
};

sptr<MoveDragController> DragControllerTest::moveDragController_ = nullptr;
std::shared_ptr<MMI::IInputEventConsumer> DragControllerTest::inputListener_ = nullptr;

void DragControllerTest::SetUpTestCase()
{
    WindowInnerManager::GetInstance().Init();
    moveDragController_ = WindowInnerManager::GetInstance().moveDragController_;
    ASSERT_TRUE(moveDragController_);
    inputListener_ = moveDragController_->inputListener_;
    ASSERT_TRUE(inputListener_);
}

void DragControllerTest::TearDownTestCase()
{
    moveDragController_ = nullptr;
    inputListener_ = nullptr;
    WindowInnerManager::GetInstance().Stop();
}

void DragControllerTest::SetUp()
{
}

void DragControllerTest::TearDown()
{
}

namespace {
/**
 * @tc.name: OnInputEvent01
 * @tc.desc: OnInputEven01, keyEvent
 * @tc.type: FUNC
 */
HWTEST_F(DragControllerTest, OnInputEvent01, Function | SmallTest | Level2)
{
    ASSERT_TRUE(inputListener_);
    std::shared_ptr<MMI::KeyEvent> keyEvent;
    inputListener_->OnInputEvent(keyEvent);
    keyEvent = MMI::KeyEvent::Create();
    inputListener_->OnInputEvent(keyEvent);
}

/**
 * @tc.name: OnInputEvent02
 * @tc.desc: OnInputEvent02, axisEvent
 * @tc.type: FUNC
 */
HWTEST_F(DragControllerTest, OnInputEvent02, Function | SmallTest | Level2)
{
    ASSERT_TRUE(inputListener_);
    std::shared_ptr<MMI::AxisEvent> axisEvent;
    inputListener_->OnInputEvent(axisEvent);
    axisEvent = MMI::AxisEvent::Create();
    inputListener_->OnInputEvent(axisEvent);
}

/**
 * @tc.name: OnInputEvent03
 * @tc.desc: OnInputEvent03, pointerEvent
 * @tc.type: FUNC
 */
HWTEST_F(DragControllerTest, OnInputEvent03, Function | SmallTest | Level2)
{
    ASSERT_TRUE(inputListener_);
    std::shared_ptr<MMI::PointerEvent> pointerEvent;
    inputListener_->OnInputEvent(pointerEvent);
    pointerEvent = MMI::PointerEvent::Create();
    ASSERT_TRUE(pointerEvent);
    pointerEvent->SetAgentWindowId(INVALID_WINDOW_ID);
    ASSERT_TRUE(moveDragController_);
    moveDragController_->SetActiveWindowId(1);
    inputListener_->OnInputEvent(pointerEvent);
}

/**
 * @tc.name: Stop01
 * @tc.desc: Stop01
 * @tc.type: FUNC
 */
HWTEST_F(DragControllerTest, Stop01, Function | SmallTest | Level2)
{
    moveDragController_->Stop();
}

/**
 * @tc.name: Stop02
 * @tc.desc: Stop02
 * @tc.type: FUNC
 */
HWTEST_F(DragControllerTest, Stop02, Function | SmallTest | Level2)
{
    ASSERT_TRUE(moveDragController_);
    moveDragController_->Init();
    moveDragController_->Stop();
    moveDragController_->Init();
}

/**
 * @tc.name: HandleEndUpMovingOrDragging
 * @tc.desc: HandleEndUpMovingOrDragging
 * @tc.type: FUNC
 */
HWTEST_F(DragControllerTest, HandleEndUpMovingOrDragging, Function | SmallTest | Level2)
{
    ASSERT_TRUE(moveDragController_);
    uint32_t windowId = 1; // windowId: 1
    moveDragController_->HandleEndUpMovingOrDragging(windowId);
    moveDragController_->activeWindowId_ = windowId;
    moveDragController_->HandleEndUpMovingOrDragging(windowId);
}

/**
 * @tc.name: HandleWindowRemovedOrDestroyed
 * @tc.desc: HandleWindowRemovedOrDestroyed
 * @tc.type: FUNC
 */
HWTEST_F(DragControllerTest, HandleWindowRemovedOrDestroyed, Function | SmallTest | Level2)
{
    ASSERT_TRUE(moveDragController_);
    uint32_t windowId = 5; // windowId: 5
    moveDragController_->HandleWindowRemovedOrDestroyed(windowId);
    moveDragController_->moveDragProperty_ = new MoveDragProperty();
    moveDragController_->HandleWindowRemovedOrDestroyed(windowId);
    moveDragController_->moveDragProperty_->startMoveFlag_ = true;
    moveDragController_->HandleWindowRemovedOrDestroyed(windowId);
    moveDragController_->moveDragProperty_->startMoveFlag_ = false;
    moveDragController_->moveDragProperty_->startDragFlag_ = true;
    moveDragController_->HandleWindowRemovedOrDestroyed(windowId);
    moveDragController_->moveDragProperty_->startMoveFlag_ = true;
    moveDragController_->HandleWindowRemovedOrDestroyed(windowId);
    moveDragController_->moveDragProperty_ = nullptr;
}

/**
 * @tc.name: ConvertPointerPosToDisplayGroupPos
 * @tc.desc: ConvertPointerPosToDisplayGroupPos
 * @tc.type: FUNC
 */
HWTEST_F(DragControllerTest, ConvertPointerPosToDisplayGroupPos, Function | SmallTest | Level2)
{
    ASSERT_TRUE(moveDragController_);
    int32_t posX = 0;
    int32_t posY = 0;
    moveDragController_->ConvertPointerPosToDisplayGroupPos(0, posX, posY);
    Rect displayRect = { 0, 0, 720, 1280 }; // displayRect: 0, 0, 720, 1280

    moveDragController_->displayRectMap_.insert(std::make_pair(0, displayRect));
    moveDragController_->ConvertPointerPosToDisplayGroupPos(1, posX, posY);
    moveDragController_->ConvertPointerPosToDisplayGroupPos(0, posX, posY);
    moveDragController_->displayRectMap_.clear();
}

/**
 * @tc.name: ConsumePointerEvent
 * @tc.desc: ConsumePointerEvent
 * @tc.type: FUNC
 */
HWTEST_F(DragControllerTest, ConsumePointerEvent, Function | SmallTest | Level2)
{
    ASSERT_TRUE(moveDragController_);
    std::shared_ptr<MMI::PointerEvent> pointerEvent;
    moveDragController_->ConsumePointerEvent(pointerEvent);
    pointerEvent = MMI::PointerEvent::Create();
    ASSERT_TRUE(pointerEvent);
    pointerEvent->SetAgentWindowId(1);
    pointerEvent->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_CANCEL);
    moveDragController_->ConsumePointerEvent(pointerEvent);
}

/**
 * @tc.name: OnReceiveVsync
 * @tc.desc: OnReceiveVsync
 * @tc.type: FUNC
 */
HWTEST_F(DragControllerTest, OnReceiveVsync, Function | SmallTest | Level2)
{
    ASSERT_TRUE(moveDragController_);
    std::shared_ptr<MMI::PointerEvent> pointerEvent = MMI::PointerEvent::Create();
    ASSERT_TRUE(pointerEvent);
    moveDragController_->OnReceiveVsync(0);
    moveDragController_->moveEvent_ = pointerEvent;
    moveDragController_->OnReceiveVsync(0);
}

/**
 * @tc.name: GetHotZoneRect
 * @tc.desc: GetHotZoneRect
 * @tc.type: FUNC
 */
HWTEST_F(DragControllerTest, GetHotZoneRect, Function | SmallTest | Level2)
{
    ASSERT_TRUE(moveDragController_);
    Rect displayRect = { 0, 0, 720, 1280 }; // displayRect: 0, 0, 720, 1280
    moveDragController_->displayRectMap_.insert(std::make_pair(0, displayRect));
    moveDragController_->moveDragProperty_ = new MoveDragProperty();
    moveDragController_->moveDragProperty_->targetDisplayId_ = 0;
    moveDragController_->moveDragProperty_->startRectExceptCorner_ = { 0, 0, 40, 40 };

    moveDragController_->moveDragProperty_->startPointPosX_ = 20; // startPointPosX: 20
    moveDragController_->moveDragProperty_->startPointPosY_ = 20; // startPointPosY: 20
    moveDragController_->GetHotZoneRect();

    moveDragController_->moveDragProperty_->startPointPosX_ = -1; // startPointPosX: -1
    moveDragController_->moveDragProperty_->startPointPosY_ = -1; // startPointPosY: -1
    moveDragController_->GetHotZoneRect();

    moveDragController_->moveDragProperty_->startPointPosX_ = -1; // startPointPosX: -1
    moveDragController_->moveDragProperty_->startPointPosY_ = 20; // startPointPosY: 20
    moveDragController_->GetHotZoneRect();

    moveDragController_->moveDragProperty_->startPointPosX_ = 41; // startPointPosX: 41
    moveDragController_->moveDragProperty_->startPointPosY_ = 20; // startPointPosY: 20
    moveDragController_->GetHotZoneRect();

    moveDragController_->moveDragProperty_->startPointPosX_ = 20; // startPointPosX: 20
    moveDragController_->moveDragProperty_->startPointPosY_ = -1; // startPointPosY: -1
    moveDragController_->GetHotZoneRect();

    moveDragController_->moveDragProperty_->startPointPosX_ = 20; // startPointPosX: 20
    moveDragController_->moveDragProperty_->startPointPosY_ = 41; // startPointPosY: 41
    moveDragController_->GetHotZoneRect();

    moveDragController_->moveDragProperty_->startPointPosX_ = 41; // startPointPosX: 41
    moveDragController_->moveDragProperty_->startPointPosY_ = 41; // startPointPosY: 41
    moveDragController_->GetHotZoneRect();
    moveDragController_->moveDragProperty_ = nullptr;
}

/**
 * @tc.name: HandleDragEvent01
 * @tc.desc: HandleDragEvent01
 * @tc.type: FUNC
 */
HWTEST_F(DragControllerTest, HandleDragEvent01, Function | SmallTest | Level2)
{
    ASSERT_TRUE(moveDragController_);
    int32_t posX = 0;
    int32_t posY = 0;
    int32_t pointId = 0;
    int32_t sourceType = 0;
    moveDragController_->HandleDragEvent(posX, posY, pointId, sourceType);

    moveDragController_->moveDragProperty_ = new MoveDragProperty();
    moveDragController_->HandleDragEvent(posX, posY, pointId, sourceType);

    moveDragController_->moveDragProperty_->startDragFlag_ = true;
    moveDragController_->HandleDragEvent(posX, posY, pointId, sourceType);

    pointId = 1;
    moveDragController_->HandleDragEvent(posX, posY, pointId, sourceType);

    pointId = 0;
    moveDragController_->HandleDragEvent(posX, posY, pointId, sourceType);

    sourceType = 1;
    moveDragController_->HandleDragEvent(posX, posY, pointId, sourceType);

    sourceType = 0;
    moveDragController_->HandleDragEvent(posX, posY, pointId, sourceType);

    moveDragController_->moveDragProperty_ = nullptr;
}

/**
 * @tc.name: HandleDragEvent02
 * @tc.desc: HandleDragEvent02
 * @tc.type: FUNC
 */
HWTEST_F(DragControllerTest, HandleDragEvent02, Function | SmallTest | Level2)
{
    ASSERT_TRUE(moveDragController_);
    int32_t posX = 0;
    int32_t posY = 0;
    int32_t pointId = 0;
    int32_t sourceType = 0;
    moveDragController_->HandleDragEvent(posX, posY, pointId, sourceType);

    moveDragController_->moveDragProperty_ = new MoveDragProperty();
    moveDragController_->HandleDragEvent(posX, posY, pointId, sourceType);

    moveDragController_->moveDragProperty_->startDragFlag_ = true;
    moveDragController_->moveDragProperty_->targetDisplayId_ = 0;
    moveDragController_->moveDragProperty_->startRectExceptCorner_ = { 0, 0, 40, 40 }; // hotZone: 0, 0, 40, 40

    moveDragController_->moveDragProperty_->startPointPosX_ = -1; // startPointPosX: -1
    moveDragController_->moveDragProperty_->startPointPosY_ = -1; // startPointPosY: -1
    moveDragController_->HandleDragEvent(posX, posY, pointId, sourceType);

    moveDragController_->moveDragProperty_->startPointPosX_ = 45; // startPointPosX: 45
    moveDragController_->moveDragProperty_->startPointPosY_ = -1; // startPointPosY: -1
    moveDragController_->HandleDragEvent(posX, posY, pointId, sourceType);

    moveDragController_->moveDragProperty_->startPointPosX_ = -1; // startPointPosX: -1
    moveDragController_->moveDragProperty_->startPointPosY_ = 45; // startPointPosY: 45
    moveDragController_->HandleDragEvent(posX, posY, pointId, sourceType);

    moveDragController_->moveDragProperty_->startPointPosX_ = 45; // startPointPosX: 45
    moveDragController_->moveDragProperty_->startPointPosY_ = 45; // startPointPosY: 45
    moveDragController_->HandleDragEvent(posX, posY, pointId, sourceType);

    moveDragController_->moveDragProperty_ = nullptr;
}

/**
 * @tc.name: HandleMoveEvent
 * @tc.desc: HandleMoveEvent
 * @tc.type: FUNC
 */
HWTEST_F(DragControllerTest, HandleMoveEvent, Function | SmallTest | Level2)
{
    ASSERT_TRUE(moveDragController_);
    int32_t posX = 0;
    int32_t posY = 0;
    int32_t pointId = 0;
    int32_t sourceType = 0;
    moveDragController_->HandleMoveEvent(posX, posY, pointId, sourceType);

    moveDragController_->moveDragProperty_ = new MoveDragProperty();
    moveDragController_->HandleMoveEvent(posX, posY, pointId, sourceType);

        moveDragController_->moveDragProperty_->startMoveFlag_ = true;
    moveDragController_->HandleMoveEvent(posX, posY, pointId, sourceType);

    pointId = 1;
    moveDragController_->HandleMoveEvent(posX, posY, pointId, sourceType);

    pointId = 0;
    moveDragController_->HandleMoveEvent(posX, posY, pointId, sourceType);

    sourceType = 1;
    moveDragController_->HandleMoveEvent(posX, posY, pointId, sourceType);

    sourceType = 0;
    moveDragController_->HandleMoveEvent(posX, posY, pointId, sourceType);

    moveDragController_->moveDragProperty_ = nullptr;
}

/**
 * @tc.name: HandlePointerEvent
 * @tc.desc: HandlePointerEvent
 * @tc.type: FUNC
 */
HWTEST_F(DragControllerTest, HandlePointerEvent, Function | SmallTest | Level2)
{
    ASSERT_TRUE(moveDragController_);
    std::shared_ptr<MMI::PointerEvent> pointerEvent = MMI::PointerEvent::Create();
    ASSERT_TRUE(pointerEvent);
    pointerEvent->SetAgentWindowId(1);
    MMI::PointerEvent::PointerItem pointerItem;
    pointerItem.SetPointerId(0);
    pointerItem.SetDisplayX(0);
    pointerItem.SetDisplayY(0);
    pointerEvent->AddPointerItem(pointerItem);
    moveDragController_->HandlePointerEvent(pointerEvent);

    pointerEvent->SetPointerId(0);
    pointerEvent->SetSourceType(MMI::PointerEvent::SOURCE_TYPE_MOUSE);
    pointerEvent->SetButtonId(MMI::PointerEvent::MOUSE_BUTTON_LEFT);
    moveDragController_->HandlePointerEvent(pointerEvent);

    moveDragController_->windowProperty_ = new WindowProperty();
    moveDragController_->moveDragProperty_ = new MoveDragProperty();
    moveDragController_->HandlePointerEvent(pointerEvent);

    pointerEvent->SetSourceType(MMI::PointerEvent::SOURCE_TYPE_MOUSE);
    pointerEvent->SetButtonId(MMI::PointerEvent::MOUSE_BUTTON_RIGHT);
    moveDragController_->HandlePointerEvent(pointerEvent);

    pointerEvent->SetButtonId(MMI::PointerEvent::MOUSE_BUTTON_LEFT);
    moveDragController_->HandlePointerEvent(pointerEvent);

    pointerEvent->SetSourceType(MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN);
    pointerEvent->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_DOWN);
    moveDragController_->HandlePointerEvent(pointerEvent);

    pointerEvent->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_BUTTON_DOWN);
    moveDragController_->HandlePointerEvent(pointerEvent);

    moveDragController_->moveDragProperty_->startPointerId_ = 1;
    moveDragController_->moveDragProperty_->sourceType_ = 2; // sourceType: 2
    moveDragController_->HandlePointerEvent(pointerEvent);

    moveDragController_->moveDragProperty_->startPointerId_ = 0;
    moveDragController_->moveDragProperty_->sourceType_ = 2; // sourceType: 2
    moveDragController_->HandlePointerEvent(pointerEvent);

    pointerEvent->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_MOVE);
    moveDragController_->HandlePointerEvent(pointerEvent);

    pointerEvent->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_UP);
    moveDragController_->HandlePointerEvent(pointerEvent);

    pointerEvent->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_BUTTON_UP);
    moveDragController_->HandlePointerEvent(pointerEvent);

    pointerEvent->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_CANCEL);
    moveDragController_->HandlePointerEvent(pointerEvent);

    moveDragController_->windowProperty_ = nullptr;
    moveDragController_->moveDragProperty_ = nullptr;
}
}
} // namespace Rosen
} // namespace OHOS

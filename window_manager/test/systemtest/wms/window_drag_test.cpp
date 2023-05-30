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

// gtest
#include <gtest/gtest.h>
#include "window_test_utils.h"
#include "wm_common.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
using Utils = WindowTestUtils;
const int WAIT_CALLBACK_US = 100000;  // 100000 us

class TestDragListener : public IWindowDragListener {
public:
    PointInfo point_ { 0, 0 };
    DragEvent event_ = DragEvent::DRAG_EVENT_END;
    void OnDrag(int32_t x, int32_t y, DragEvent event) override;
};

void TestDragListener::OnDrag(int32_t x, int32_t y, DragEvent event)
{
    event_ = event;
    point_.x = x;
    point_.y = y;
}

class WindowDragTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    virtual void SetUp() override;
    virtual void TearDown() override;

    static sptr<TestDragListener> firstWindowDragListener_;
    static sptr<TestDragListener> secondWindowDragListener_;
    Utils::TestWindowInfo dragWindowInfo_;
    Utils::TestWindowInfo firstWindowInfo_;
    Utils::TestWindowInfo secondWindowInfo_;
};

sptr<TestDragListener> WindowDragTest::firstWindowDragListener_ =
    new TestDragListener();
sptr<TestDragListener> WindowDragTest::secondWindowDragListener_ =
    new TestDragListener();

void WindowDragTest::SetUpTestCase() {}

void WindowDragTest::TearDownTestCase() {}

void WindowDragTest::SetUp()
{
    dragWindowInfo_ = {
        .name = "dragWindow",
        .rect = {200, 200, 380, 380},
        .type = WindowType::WINDOW_TYPE_DRAGGING_EFFECT,
        .mode = WindowMode::WINDOW_MODE_FLOATING,
        .needAvoid = false,
        .parentLimit = false,
        .parentId = INVALID_WINDOW_ID,
    };

    firstWindowInfo_ = {
        .name = "firstWindow",
        .rect = Utils::customAppRect_,
        .type = WindowType::WINDOW_TYPE_APP_MAIN_WINDOW,
        .mode = WindowMode::WINDOW_MODE_FULLSCREEN,
        .needAvoid = false,
        .parentLimit = false,
        .parentId = INVALID_WINDOW_ID,
    };

    secondWindowInfo_ = {
        .name = "secondWindow",
        .rect = Utils::customAppRect_,
        .type = WindowType::WINDOW_TYPE_APP_MAIN_WINDOW,
        .mode = WindowMode::WINDOW_MODE_FLOATING,
        .needAvoid = false,
        .parentLimit = false,
        .parentId = INVALID_WINDOW_ID,
    };
}

void WindowDragTest::TearDown() {}

namespace {
/**
 * @tc.name: DragIn
 * @tc.desc: Drag a window to another window
 * @tc.type: FUNC
 */
HWTEST_F(WindowDragTest, DragIn, Function | MediumTest | Level3) {
    const sptr<Window> &firstWindow = Utils::CreateTestWindow(firstWindowInfo_);
    firstWindow->RegisterDragListener(firstWindowDragListener_);
    firstWindow->SetTurnScreenOn(true);
    firstWindow->Show();

    const sptr<Window> &dragWindow = Utils::CreateTestWindow(dragWindowInfo_);
    dragWindow->Show();
    dragWindow->MoveTo(300, 300);
    usleep(WAIT_CALLBACK_US);
    ASSERT_EQ(300, firstWindowDragListener_->point_.x);
    ASSERT_EQ(300, firstWindowDragListener_->point_.y);
    ASSERT_EQ(DragEvent::DRAG_EVENT_IN, firstWindowDragListener_->event_);

    dragWindow->Destroy();
    firstWindow->UnregisterDragListener(firstWindowDragListener_);
    firstWindow->Destroy();
}

/**
 * @tc.name: DragMove
 * @tc.desc: Window Move
 * @tc.type: FUNC
 */
HWTEST_F(WindowDragTest, DragMove, Function | MediumTest | Level3) {
    const sptr<Window> &firstWindow = Utils::CreateTestWindow(firstWindowInfo_);
    firstWindow->RegisterDragListener(firstWindowDragListener_);
    firstWindow->SetTurnScreenOn(true);
    firstWindow->Show();

    const sptr<Window> &dragWindow = Utils::CreateTestWindow(dragWindowInfo_);
    dragWindow->Show();
    dragWindow->MoveTo(300, 300);
    usleep(WAIT_CALLBACK_US);
    ASSERT_EQ(300, firstWindowDragListener_->point_.x);
    ASSERT_EQ(300, firstWindowDragListener_->point_.y);
    ASSERT_EQ(DragEvent::DRAG_EVENT_IN, firstWindowDragListener_->event_);

    dragWindow->MoveTo(400, 400);
    usleep(WAIT_CALLBACK_US);
    ASSERT_EQ(400, firstWindowDragListener_->point_.x);
    ASSERT_EQ(400, firstWindowDragListener_->point_.y);
    ASSERT_EQ(DragEvent::DRAG_EVENT_MOVE, firstWindowDragListener_->event_);

    dragWindow->Destroy();
    firstWindow->UnregisterDragListener(firstWindowDragListener_);
    firstWindow->Destroy();
}

/**
 * @tc.name: DragOut
 * @tc.desc: Drag the drag window out of the current window
 * @tc.type: FUNC
 */
HWTEST_F(WindowDragTest, DragOut, Function | MediumTest | Level3) {
    const sptr<Window> &firstWindow = Utils::CreateTestWindow(firstWindowInfo_);
    firstWindow->RegisterDragListener(firstWindowDragListener_);
    firstWindow->SetTurnScreenOn(true);
    firstWindow->Show();

    secondWindowInfo_.rect = {500, 500, 500, 500};
    const sptr<Window> &secondWindow = Utils::CreateTestWindow(secondWindowInfo_);
    secondWindow->RegisterDragListener(secondWindowDragListener_);
    secondWindow->Show();

    const sptr<Window> &dragWindow = Utils::CreateTestWindow(dragWindowInfo_);
    dragWindow->Show();
    dragWindow->MoveTo(300, 300);
    usleep(WAIT_CALLBACK_US);
    ASSERT_EQ(300, firstWindowDragListener_->point_.x);
    ASSERT_EQ(300, firstWindowDragListener_->point_.y);
    ASSERT_EQ(DragEvent::DRAG_EVENT_IN, firstWindowDragListener_->event_);

    dragWindow->MoveTo(400, 400);
    usleep(WAIT_CALLBACK_US);
    ASSERT_EQ(400, firstWindowDragListener_->point_.x);
    ASSERT_EQ(400, firstWindowDragListener_->point_.y);
    ASSERT_EQ(DragEvent::DRAG_EVENT_MOVE, firstWindowDragListener_->event_);

    dragWindow->MoveTo(600, 600);
    usleep(WAIT_CALLBACK_US);
    ASSERT_EQ(100, secondWindowDragListener_->point_.x);
    ASSERT_EQ(100, secondWindowDragListener_->point_.y);
    ASSERT_EQ(DragEvent::DRAG_EVENT_IN, secondWindowDragListener_->event_);
    ASSERT_EQ(DragEvent::DRAG_EVENT_OUT, firstWindowDragListener_->event_);

    dragWindow->Destroy();
    firstWindow->UnregisterDragListener(firstWindowDragListener_);
    secondWindow->UnregisterDragListener(secondWindowDragListener_);
    firstWindow->Destroy();
    secondWindow->Destroy();
}

/**
 * @tc.name: DragEnd
 * @tc.desc: End window drag
 * @tc.type: FUNC
 */
HWTEST_F(WindowDragTest, DragEnd, Function | MediumTest | Level3) {
    const sptr<Window> &firstWindow = Utils::CreateTestWindow(firstWindowInfo_);
    firstWindow->RegisterDragListener(firstWindowDragListener_);
    firstWindow->SetTurnScreenOn(true);
    firstWindow->Show();

    const sptr<Window> &dragWindow = Utils::CreateTestWindow(dragWindowInfo_);
    dragWindow->Show();
    dragWindow->MoveTo(199, 199);
    usleep(WAIT_CALLBACK_US);
    dragWindow->Destroy();
    usleep(WAIT_CALLBACK_US);
    ASSERT_EQ(DragEvent::DRAG_EVENT_END, firstWindowDragListener_->event_);
    firstWindow->UnregisterDragListener(firstWindowDragListener_);
    firstWindow->Destroy();
}
} // namespace
} // namespace Rosen
} // namespace OHOS

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
#include "window_impl.h"
#include "window_input_channel.h"
#include "mock_window_adapter.h"
#include "singleton_mocker.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
using WindowMocker = SingletonMocker<WindowAdapter, MockWindowAdapter>;
class WindowInputChannelTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    virtual void SetUp() override;
    virtual void TearDown() override;
    sptr<WindowImpl> window_;
};
void WindowInputChannelTest::SetUpTestCase()
{
}

void WindowInputChannelTest::TearDownTestCase()
{
}

void WindowInputChannelTest::SetUp()
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("window");
    window_ = new WindowImpl(option);
    window_->Create(INVALID_WINDOW_ID);
}

void WindowInputChannelTest::TearDown()
{
    window_->Destroy();
    window_ = nullptr;
}

namespace {
/**
 * @tc.name: HandlePointerEvent
 * @tc.desc: consume pointer event when receive callback from input
 * @tc.type: FUNC
 */
HWTEST_F(WindowInputChannelTest, HandlePointerEvent, Function | SmallTest | Level2)
{
    auto pointerEvent = MMI::PointerEvent::Create();
    sptr<WindowInputChannel> inputChannel = new WindowInputChannel(window_);
    window_->ConsumePointerEvent(pointerEvent);
    inputChannel->HandlePointerEvent(pointerEvent);
}

/**
 * @tc.name: HandleKeyEvent
 * @tc.desc: consume key event when receive callback from input
 * @tc.type: FUNC
 */
HWTEST_F(WindowInputChannelTest, HandleKeyEvent, Function | SmallTest | Level2)
{
    auto keyEvent = MMI::KeyEvent::Create();
    sptr<WindowInputChannel> inputChannel = new WindowInputChannel(window_);
    window_->ConsumeKeyEvent(keyEvent);
    inputChannel->HandleKeyEvent(keyEvent);
}
}
} // namespace Rosen
} // namespace OHOS

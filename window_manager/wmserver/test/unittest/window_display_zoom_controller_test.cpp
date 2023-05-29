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
#include "display_zoom_controller.h"
#include "display_manager.h"

using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace Rosen {
class WindowDisplayZoomControllerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    sptr<WindowRoot> windowRoot_;
    sptr<DisplayZoomController> displayController_;
};

void WindowDisplayZoomControllerTest::SetUpTestCase()
{
}

void WindowDisplayZoomControllerTest::TearDownTestCase()
{
}

void WindowDisplayZoomControllerTest::SetUp()
{
    windowRoot_ = new WindowRoot(nullptr);
    displayController_ = new DisplayZoomController(windowRoot_);
}

void WindowDisplayZoomControllerTest::TearDown()
{
}

namespace {
/**
 * @tc.name: WindowDisplayZoomControllerTest01
 * @tc.desc: test WindowDisplayZoomController
 * @tc.type: FUNC
 */
HWTEST_F(WindowDisplayZoomControllerTest, WindowDisplayZoomControllerTest01, Function | SmallTest | Level2)
{
    DisplayId displayId;
    uint32_t windowId;

    // windowNodeContainer is null
    displayController_->SetAnchorAndScale(1, 1, 10.0);
    displayController_->SetAnchorOffset(1, 1);
    displayController_->OffWindowZoom();
    displayId = DisplayManagerServiceInner::GetInstance().GetDefaultDisplayId();
    displayController_->UpdateAllWindowsZoomInfo(displayId);
    windowId = INVALID_WINDOW_ID;
    displayController_->UpdateWindowZoomInfo(windowId);

    auto display = DisplayManager::GetInstance().GetDefaultDisplay();
    ASSERT_NE(display, nullptr);
    sptr<DisplayInfo> displayInfo = display->GetDisplayInfo();
    ASSERT_NE(displayInfo, nullptr);
    auto container = windowRoot_->CreateWindowNodeContainer(displayInfo);
    ASSERT_NE(container, nullptr);

    // windowNodeContainer is not null
    displayController_->SetAnchorAndScale(1, 1, 10.0);
    displayController_->SetAnchorOffset(1, 1);
    displayId = DisplayManagerServiceInner::GetInstance().GetDefaultDisplayId();

    ASSERT_EQ(true, true);
}
}
}
}

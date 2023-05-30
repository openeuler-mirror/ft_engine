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

#include "surface_draw.h"
#include "display.h"
#include "display_info.h"
#include "display_manager.h"
#include "window_impl.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace {
    const std::string IMAGE_PLACE_HOLDER_PNG_PATH = "/etc/window/resources/bg_place_holder.png";
    const int WAIT_FOR_SYNC_US = 1000 * 500;  // 500ms
}
class SurfaceDrawTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

public:
    struct WindowTestInfo {
        std::string name;
        Rect rect;
        WindowType type;
        WindowMode mode;
        bool needAvoid;
        bool parentLimit;
        bool forbidSplitMove {false};
        bool showWhenLocked;
        uint32_t parentId;
    };
    sptr<Window> CreateTestWindow(const std::string& name);

    static inline DisplayId displayId_;
    static inline int32_t displayWidth_;
    static inline int32_t displayHeight_;
    WindowTestInfo windowInfo_;
};

void SurfaceDrawTest::SetUpTestCase()
{
    displayId_ = DisplayManager::GetInstance().GetDefaultDisplayId();
    sptr<Display> display = DisplayManager::GetInstance().GetDefaultDisplay();
    if (display == nullptr) {
        return;
    }
    displayWidth_ = display->GetWidth();
    displayHeight_ = display->GetHeight();
}

void SurfaceDrawTest::TearDownTestCase()
{
}

void SurfaceDrawTest::SetUp()
{
    windowInfo_ = {
        .name = "main",
        .rect = {100, 100, 250, 300},
        .type = WindowType::WINDOW_TYPE_APP_MAIN_WINDOW,
        .mode = WindowMode::WINDOW_MODE_FLOATING,
        .needAvoid = true,
        .parentLimit = false,
        .parentId = INVALID_WINDOW_ID,
    };
}

void SurfaceDrawTest::TearDown()
{
}

sptr<Window> SurfaceDrawTest::CreateTestWindow(const std::string& name)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetDisplayId(displayId_);
    option->SetWindowType(windowInfo_.type);
    option->SetWindowRect(windowInfo_.rect);
    option->SetWindowMode(windowInfo_.mode);
    option->SetWindowName(name);
    sptr<Window> window = Window::Create(option->GetWindowName(), option);
    window->AddWindowFlag(WindowFlag::WINDOW_FLAG_SHOW_WHEN_LOCKED);
    return window;
}

namespace {
/**
 * @tc.name: DrawImage
 * @tc.desc: SurfaceDraw::DrawImage test
 * @tc.type: FUNC
 */
HWTEST_F(SurfaceDrawTest, DrawImage01, Function | SmallTest | Level1)
{
    ASSERT_FALSE(SurfaceDraw::DrawImage(nullptr, 0, 0, ""));
    sptr<Window> window = CreateTestWindow("testDrawImage");
    window->Show();
    usleep(WAIT_FOR_SYNC_US / 20); // wait for rect updated

    auto surfaceNode = window->GetSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    usleep(WAIT_FOR_SYNC_US / 20); // wait for rect updated
    uint32_t width = window->GetRect().width_;
    uint32_t height = window->GetRect().height_;
    ASSERT_TRUE(SurfaceDraw::DrawImage(surfaceNode, width, height, IMAGE_PLACE_HOLDER_PNG_PATH));
    ASSERT_FALSE(SurfaceDraw::DrawImage(surfaceNode, -1, -1, IMAGE_PLACE_HOLDER_PNG_PATH));
    window->Destroy();
}
/**
 * @tc.name: DecodeImageToPixelMap
 * @tc.desc: SurfaceDraw::DecodeImageToPixelMap test
 * @tc.type: FUNC
 */
HWTEST_F(SurfaceDrawTest, DecodeImageToPixelMap01, Function | SmallTest | Level1)
{
    ASSERT_EQ(SurfaceDraw::DecodeImageToPixelMap(""), nullptr);
    ASSERT_NE(SurfaceDraw::DecodeImageToPixelMap(IMAGE_PLACE_HOLDER_PNG_PATH), nullptr);
}
/**
 * @tc.name: DrawMasking
 * @tc.desc: SurfaceDraw::DrawMasking test
 * @tc.type: FUNC
 */
HWTEST_F(SurfaceDrawTest, DrawMasking01, Function | SmallTest | Level1)
{
    OHOS::Rosen::Rect screenRect = {0, 0, 0, 0};
    OHOS::Rosen::Rect transRect = {0, 0, 0, 0};
    ASSERT_FALSE(SurfaceDraw::DrawMasking(nullptr, screenRect, transRect));

    sptr<Window> window = CreateTestWindow("testDrawMasking");
    window->Show();
    usleep(WAIT_FOR_SYNC_US / 20); // wait for rect updated

    auto surfaceNode = window->GetSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    ASSERT_FALSE(SurfaceDraw::DrawMasking(surfaceNode, screenRect, transRect));

    screenRect.width_ = displayWidth_;
    screenRect.height_ = displayHeight_;
    transRect.width_ = displayWidth_;
    transRect.height_ = displayHeight_;
    ASSERT_TRUE(SurfaceDraw::DrawMasking(surfaceNode, screenRect, transRect));
    window->Destroy();
}
/**
 * @tc.name: DoDrawImageRect
 * @tc.desc: SurfaceDraw::DoDrawImageRect test
 * @tc.type: FUNC
 */
HWTEST_F(SurfaceDrawTest, DoDrawImageRect01, Function | SmallTest | Level1)
{
    sptr<Window> window = CreateTestWindow("testDoDrawImageRect");
    ASSERT_NE(window, nullptr);
    window->Show();
    usleep(WAIT_FOR_SYNC_US / 20); // wait for rect updated

    OHOS::Rosen::Rect rect = window->GetRect();
    uint32_t color = 0x00660000;

    auto surfaceNode = window->GetSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    sptr<OHOS::Surface> layer = SurfaceDraw::GetLayer(surfaceNode);
    ASSERT_NE(layer, nullptr);
    sptr<OHOS::SurfaceBuffer> buffer = SurfaceDraw::GetSurfaceBuffer(layer, rect.width_, rect.height_);
    ASSERT_NE(buffer, nullptr);

    ASSERT_FALSE(SurfaceDraw::DoDrawImageRect(buffer, rect, nullptr, color, false));

    std::shared_ptr<Media::PixelMap> pixelMap = SurfaceDraw::DecodeImageToPixelMap(IMAGE_PLACE_HOLDER_PNG_PATH);
    ASSERT_NE(pixelMap, nullptr);

    ASSERT_TRUE(SurfaceDraw::DoDrawImageRect(buffer, rect, pixelMap, color, false));
    window->Destroy();
}
/**
 * @tc.name: GetSurfaceSnapshot
 * @tc.desc: SurfaceDraw::GetSurfaceSnapshot test
 * @tc.type: FUNC
 */
HWTEST_F(SurfaceDrawTest, GetSurfaceSnapshot01, Function | SmallTest | Level1)
{
    sptr<Window> window = CreateTestWindow("testDoDrawImageRect");
    ASSERT_NE(window, nullptr);
    window->Show();
    usleep(WAIT_FOR_SYNC_US / 20); // wait for rect updated

    auto surfaceNode = window->GetSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    
    std::shared_ptr<Media::PixelMap> pixelMap = SurfaceDraw::DecodeImageToPixelMap(IMAGE_PLACE_HOLDER_PNG_PATH);
    ASSERT_NE(pixelMap, nullptr);

    ASSERT_FALSE(SurfaceDraw::GetSurfaceSnapshot(nullptr, pixelMap, 0, 0, 0));
    ASSERT_FALSE(SurfaceDraw::GetSurfaceSnapshot(surfaceNode, pixelMap, 0, 0, 0));
    window->Destroy();
}
}
} // namespace Rosen
} // namespace OHOS
/*
 * Copyright (c) 2022 Huawei Technologies Co., Ltd.
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

#include <iostream>
#include "include/core/SkColor.h"
#include "include/core/SkTextBlob.h"
#include "render_context/render_context.h"
#include "transaction/rs_transaction.h"
#include "transaction/rs_interfaces.h"
#include "ui/rs_surface_extractor.h"
#include "ui/rs_surface_node.h"
#include "wm/window.h"

#include "display_manager.h"
#include "snapshot_utils.h"

#ifdef ENABLE_MMI
#include "input_manager.h"
#endif

using namespace OHOS;
using namespace OHOS::Media;
using namespace OHOS::Rosen;

constexpr int PMAM_TEXT_POS_X = 25;
constexpr int PMAM_TEXT_POS_Y = 25;
constexpr float SNAPSHOT_FONT_SIZE = 24.0f;
constexpr int SNAPSHOT_FONT_STROKE_WIDTH = 2;

int snapshot(int argc, char * const argv[])
{
    CmdArgments cmdArgments;
    cmdArgments.fileName = "";

    if (!SnapShotUtils::ProcessArgs(argc, argv, cmdArgments)) {
        return 0;
    }

    auto display = DisplayManager::GetInstance().GetDisplayById(cmdArgments.displayId);
    if (display == nullptr) {
        std::cout << "error: GetDisplayById " << cmdArgments.displayId << " error!" << std::endl;
        return -1;
    }

    std::cout << "process: display " << cmdArgments.displayId <<
              ": width " << display->GetWidth() << ", height " << display->GetHeight() << std::endl;

    // get PixelMap from DisplayManager API
    std::shared_ptr<OHOS::Media::PixelMap> pixelMap = nullptr;
    if (!cmdArgments.isWidthSet && !cmdArgments.isHeightSet) {
        pixelMap = DisplayManager::GetInstance().GetScreenshot(cmdArgments.displayId); // default width & height
    } else {
        if (!cmdArgments.isWidthSet) {
            cmdArgments.width = display->GetWidth();
            std::cout << "process: reset to display's width " << cmdArgments.width << std::endl;
        }
        if (!cmdArgments.isHeightSet) {
            cmdArgments.height = display->GetHeight();
            std::cout << "process: reset to display's height " << cmdArgments.height << std::endl;
        }
        if (!SnapShotUtils::CheckWidthAndHeightValid(cmdArgments.width, cmdArgments.height)) {
            std::cout << "error: width " << cmdArgments.width << " height " <<
                      cmdArgments.height << " invalid!" << std::endl;
            return -1;
        }
        const Media::Rect rect = {0, 0, display->GetWidth(), display->GetHeight()};
        const Media::Size size = {cmdArgments.width, cmdArgments.height};
        constexpr int rotation = 0;
        pixelMap = DisplayManager::GetInstance().GetScreenshot(cmdArgments.displayId, rect, size, rotation);
    }

    bool ret = false;
    if (pixelMap != nullptr) {
        ret = SnapShotUtils::WriteToJpegWithPixelMap(cmdArgments.fileName, *pixelMap);
    }
    if (!ret) {
        std::cout << "\nerror: snapshot display " << cmdArgments.displayId <<
                  ", write to " << cmdArgments.fileName.c_str() << " as jpeg failed!" << std::endl;
        return -1;
    }

    std::cout << "\nsuccess: snapshot display " << cmdArgments.displayId << " , write to " <<
              cmdArgments.fileName.c_str() << " as jpeg, width " << pixelMap->GetWidth() <<
              ", height " << pixelMap->GetHeight() << std::endl;
    return 0;
}

#ifdef ENABLE_MMI
#ifdef USE_MMI_LISTENER
constexpr int INVALID_COORDINATE = -1;
class SnapshotDemoEventConsumer : public OHOS::MMI::IInputEventConsumer
{
public:
    SnapshotDemoEventConsumer(OHOS::sptr<OHOS::Rosen::Window> window);
    void OnInputEvent(std::shared_ptr<OHOS::MMI::KeyEvent> keyEvent) const override;
    void OnInputEvent(std::shared_ptr<OHOS::MMI::AxisEvent> axisEvent) const override;
    void OnInputEvent(std::shared_ptr<OHOS::MMI::PointerEvent> pointerEvent) const override;

private:
    void HandleMovePointerEvent(std::shared_ptr<OHOS::MMI::PointerEvent> pointerEvent) const;
    OHOS::sptr<OHOS::Rosen::Window> window_ = nullptr;
    mutable bool enableMove = false;
    mutable int lastWindowX = INVALID_COORDINATE;
    mutable int lastWindowY = INVALID_COORDINATE;
    mutable int lastPointerX = INVALID_COORDINATE;
    mutable int lastPointerY = INVALID_COORDINATE;
};

SnapshotDemoEventConsumer::SnapshotDemoEventConsumer(OHOS::sptr<OHOS::Rosen::Window> window)
{
    window_ = window;
}

void SnapshotDemoEventConsumer::OnInputEvent(std::shared_ptr<OHOS::MMI::KeyEvent> keyEvent) const
{
    keyEvent->MarkProcessed();
}

void SnapshotDemoEventConsumer::OnInputEvent(std::shared_ptr<OHOS::MMI::AxisEvent> axisEvent) const
{
    axisEvent->MarkProcessed();
}

void SnapshotDemoEventConsumer::OnInputEvent(std::shared_ptr<OHOS::MMI::PointerEvent> pointerEvent) const
{
    switch (pointerEvent->GetPointerAction()) {
        case OHOS::MMI::PointerEvent::POINTER_ACTION_BUTTON_DOWN:
            enableMove = true;  // 鼠标按键按下，使能移动窗口
            break;
        case OHOS::MMI::PointerEvent::POINTER_ACTION_BUTTON_UP:
            std::cout << "SNAPSHOT\n";// 鼠标按键抬起，静止移动窗口
            snapshot(0, nullptr);
            enableMove = false; // 鼠标按键抬起，静止移动窗口
            break;
        case OHOS::MMI::PointerEvent::POINTER_ACTION_MOVE:
            break;
        default:
            break;
    }
    pointerEvent->MarkProcessed();
}
#else // USE_MMI_LISTENER

class SnapshotDemoEventConsumerWindow : public OHOS::Rosen::IInputEventConsumer
{
public:
    SnapshotDemoEventConsumerWindow(OHOS::sptr<OHOS::Rosen::Window> window)
    {
        window_ = window;
    }
    bool OnInputEvent(const std::shared_ptr<OHOS::MMI::KeyEvent>& keyEvent) const override;
    bool OnInputEvent(const std::shared_ptr<OHOS::MMI::AxisEvent>& axisEvent) const override;
    bool OnInputEvent(const std::shared_ptr<OHOS::MMI::PointerEvent>& pointerEvent) const override;
private:
    OHOS::sptr<OHOS::Rosen::Window> window_ = nullptr;
};

bool SnapshotDemoEventConsumerWindow::OnInputEvent(const std::shared_ptr<OHOS::MMI::KeyEvent>& keyEvent) const
{
    keyEvent->MarkProcessed();
    return true;
}

bool SnapshotDemoEventConsumerWindow::OnInputEvent(const std::shared_ptr<OHOS::MMI::AxisEvent>& axisEvent) const
{
    axisEvent->MarkProcessed();
    return true;
}

bool SnapshotDemoEventConsumerWindow::OnInputEvent(const std::shared_ptr<OHOS::MMI::PointerEvent>& pointerEvent) const
{
    switch (pointerEvent->GetPointerAction()) {
        case OHOS::MMI::PointerEvent::POINTER_ACTION_BUTTON_DOWN:
            break;
        case OHOS::MMI::PointerEvent::POINTER_ACTION_BUTTON_UP:
            std::cout << "SNAPSHOT\n";// 鼠标按键抬起，静止移动窗口
            snapshot(0, nullptr);
            break;
        case OHOS::MMI::PointerEvent::POINTER_ACTION_MOVE:
            break;
        default:
            break;
    }
    pointerEvent->MarkProcessed();
    return true;
}
#endif // USE_MMI_LISTENER
#endif // ENABLE_MMI

// we can make this demo and run it on the device,
// to start a snapshot demo
class SnapshotDemo {
public:
    SnapshotDemo()
    {
#ifdef ENABLE_GPU
        InitContext();
#endif
        int displayId = OHOS::Rosen::RSInterfaces::GetInstance().GetDefaultScreenId();
        auto activeModeInfo = OHOS::Rosen::RSInterfaces::GetInstance().GetScreenActiveMode(displayId);
        int desktopW = activeModeInfo.GetScreenWidth();
        int desktopH = activeModeInfo.GetScreenHeight();

        OHOS::sptr<OHOS::Rosen::WindowOption> option(new WindowOption());
        option->SetDisplayId(0); // default displayId is 0
        option->SetWindowRect({desktopW - windowWidth, desktopH - windowHeight,
                               windowWidth, windowHeight});
        option->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
        option->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
        option->SetWindowName("snapshot");
        option->SetMainHandlerAvailable(false);
        window_ = OHOS::Rosen::Window::Create(option->GetWindowName(), option);
        if (window_ != nullptr) {
            surfaceNode_ = window_->GetSurfaceNode();
            surfaceNode_->SetBounds(desktopW - windowWidth, desktopH - windowHeight,
                                    windowWidth, windowHeight); // surface bounds is window rect.
            RSTransaction::FlushImplicitTransaction();
            window_->Show();
#ifdef ENABLE_MMI
            #ifdef USE_MMI_LISTENER
            auto listener = std::make_shared<SnapshotDemoEventConsumer>(window_);
            const std::string thread = "snapshotEventThread";
            auto runner = OHOS::AppExecFwk::EventRunner::Create(thread);
            auto eventHandler = std::make_shared<OHOS::AppExecFwk::EventHandler>(runner);
            OHOS::MMI::InputManager::GetInstance()->SetWindowInputEventConsumer(listener, eventHandler);
#else  // USE_MMI_LISTENER: no mmi listener, use window input event listener
            auto listener = std::make_shared<SnapshotDemoEventConsumerWindow>(window_);
            window_->SetInputEventConsumer(listener);
#endif // USE_MMI_LISTENER
#endif // ENABLE_MMI
        } else {
            std::cout << "Failed to create window!" << std::endl;
        }

        if (surfaceNode_ == nullptr) {
            return;
        }

        rsSurface = RSSurfaceExtractor::ExtractRSSurface(surfaceNode_);
        if (rsSurface == nullptr) {
            return;
        }

#ifdef ENABLE_GPU
        if (renderContext_ != nullptr) {
            rsSurface->SetRenderContext(renderContext_.get());
        }
#endif
    }

    ~SnapshotDemo() noexcept
    {
        if (window_ != nullptr) {
            window_->Hide();
            window_->Destroy();
        }
    }

#ifdef ENABLE_GPU
    void InitContext()
    {
        renderContext_ = std::make_unique<RenderContext>();
        renderContext_->InitializeEglContext();
    }
#endif

    void DrawText(SkCanvas *canvas)
    {
        SkPaint paint;
        paint.setAntiAlias(true);
        paint.setStyle(SkPaint::kFill_Style);
        paint.setStrokeWidth(SNAPSHOT_FONT_STROKE_WIDTH);
        paint.setColor(SK_ColorBLACK);
        sk_sp<SkTextBlob> textBlob = SkTextBlob::MakeFromString(
                "Snapshot", SkFont(nullptr, SNAPSHOT_FONT_SIZE, 1.0f, 0.0f)); // font size: 24
        canvas->drawTextBlob(textBlob.get(), PMAM_TEXT_POS_X, PMAM_TEXT_POS_Y, paint);
    }

    void Run()
    {
        auto frame = rsSurface->RequestFrame(windowWidth, windowHeight);
        if (frame == nullptr) {
            std::cout << "Failed to create frame!" << std::endl;
            return;
        }
        auto canvas = frame->GetCanvas();
        if (canvas == nullptr) {
            std::cout << "Failed to create canvas!" << std::endl;
            return;
        }
        canvas->clear(SK_ColorBLUE);
        DrawText(canvas);

        frame->SetDamageRegion(0, 0, windowWidth, windowHeight);
        rsSurface->FlushFrame(frame);
        usleep(100000);
        while (true);
    }

private:
#ifdef ENABLE_GPU
    std::unique_ptr<RenderContext> renderContext_ = nullptr;
#endif
    OHOS::sptr<OHOS::Rosen::Window> window_ = nullptr;
    std::shared_ptr<RSSurfaceNode> surfaceNode_ = nullptr;
    std::shared_ptr<RSSurface> rsSurface = nullptr;
    int windowWidth = 150; // window width is set to 150;
    int windowHeight = 50; // window height is set to 50;
};

int main()
{
    std::cout << "snapshot start!" << std::endl;
    SnapshotDemo snapshot;
    snapshot.Run();
    std::cout << "snapshot end!" << std::endl;
    return 0;
}

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
#include <time.h>
#include <cmath>
#include "include/core/SkColor.h"
#include "include/core/SkTextBlob.h"
#include "property/rs_properties_def.h"
#include "refbase.h"
#include "render_context/render_context.h"
#include "transaction/rs_transaction.h"
#include "ui/rs_surface_extractor.h"
#include "ui/rs_surface_node.h"
#include "wm/window.h"

#ifdef ENABLE_MMI
#include "input_manager.h"
#endif

using namespace OHOS::Rosen;

constexpr int KEY_RADIUS = 30;
constexpr int KEY_NUM = 2;
constexpr float KEY_FONT_SIZE = 26.0f;
constexpr int KEY_FONT_STROKE_WIDTH = 3;

#ifdef ENABLE_MMI
class SimulateKeyDemoEventConsumer : public OHOS::Rosen::IInputEventConsumer
{
public:
    SimulateKeyDemoEventConsumer(OHOS::sptr<OHOS::Rosen::Window> window)
    {
        window_ = window;
    }
    bool OnInputEvent(const std::shared_ptr<OHOS::MMI::KeyEvent>& keyEvent) const override;
    bool OnInputEvent(const std::shared_ptr<OHOS::MMI::AxisEvent>& axisEvent) const override;
    bool OnInputEvent(const std::shared_ptr<OHOS::MMI::PointerEvent>& pointerEvent) const override;
private:
    OHOS::sptr<OHOS::Rosen::Window> window_ = nullptr;
    void ClickSimulateKey(std::shared_ptr<OHOS::MMI::PointerEvent> pointerEvent) const;
    std::shared_ptr<OHOS::MMI::KeyEvent> CreateSimulateKey(const int32_t keyCode) const;
    int centerX = KEY_RADIUS;
    int centerY = KEY_RADIUS;
};

bool SimulateKeyDemoEventConsumer::OnInputEvent(const std::shared_ptr<OHOS::MMI::KeyEvent>& keyEvent) const
{
    int keyCode = keyEvent->GetKeyCode();
    switch (keyCode)
    {
    case 2017:
        std::cout << "key code is: A" << std::endl;
        break;
    case 2018:
        std::cout << "key code is: B" << std::endl;
        break;
    }
    keyEvent->MarkProcessed();
    return true;
}

bool SimulateKeyDemoEventConsumer::OnInputEvent(const std::shared_ptr<OHOS::MMI::AxisEvent>& axisEvent) const
{
    axisEvent->MarkProcessed();
    return true;
}

std::shared_ptr<OHOS::MMI::KeyEvent> SimulateKeyDemoEventConsumer::CreateSimulateKey(const int32_t keyCode) const
{
    std::shared_ptr<OHOS::MMI::KeyEvent> injectDownEvent = OHOS::MMI::KeyEvent::Create();
    int64_t downTime = 0;
    OHOS::MMI::KeyEvent::KeyItem kitDown;
    kitDown.SetKeyCode(keyCode);
    kitDown.SetPressed(true);
    kitDown.SetDownTime(downTime);
    injectDownEvent->SetKeyCode(keyCode);
    injectDownEvent->SetKeyAction(OHOS::MMI::KeyEvent::KEY_ACTION_DOWN);
    injectDownEvent->AddPressedKeyItems(kitDown);
    return injectDownEvent;
}

void SimulateKeyDemoEventConsumer::ClickSimulateKey(std::shared_ptr<OHOS::MMI::PointerEvent> pointerEvent) const
{
    if ((window_ == nullptr) || (pointerEvent->GetSourceType() != OHOS::MMI::PointerEvent::SOURCE_TYPE_MOUSE)) {
        return;
    }

    OHOS::MMI::PointerEvent::PointerItem pointerItem;
    int32_t pointId = pointerEvent->GetPointerId();
    if (!pointerEvent->GetPointerItem(pointId, pointerItem)) {
        return;
    }
    if (abs(centerX - pointerItem.GetDisplayX()) <= KEY_RADIUS && abs(centerY - pointerItem.GetDisplayY()) <= KEY_RADIUS)
    {
        std::shared_ptr<OHOS::MMI::KeyEvent> injectDownEvent = CreateSimulateKey(OHOS::MMI::KeyEvent::KEYCODE_A);
        OnInputEvent(injectDownEvent);
    }
    else if (abs(centerX + KEY_RADIUS * 2 - pointerItem.GetDisplayX()) <= KEY_RADIUS && abs(centerY - pointerItem.GetDisplayY()) <= KEY_RADIUS)
    {
        std::shared_ptr<OHOS::MMI::KeyEvent> injectDownEvent = CreateSimulateKey(OHOS::MMI::KeyEvent::KEYCODE_B);
        OnInputEvent(injectDownEvent);
    }
#ifdef OHOS_BUILD_ENABLE_KEYBOARD
    TestSimulateInputEvent(injectDownEvent);
#endif // OHOS_BUILD_ENABLE_KEYBOARD
}

bool SimulateKeyDemoEventConsumer::OnInputEvent(const std::shared_ptr<OHOS::MMI::PointerEvent>& pointerEvent) const
{
    if (pointerEvent->GetPointerAction() == OHOS::MMI::PointerEvent::POINTER_ACTION_BUTTON_DOWN)
    {
        ClickSimulateKey(pointerEvent);
    }
    //window_->StartMove();
    pointerEvent->MarkProcessed();
    return true;
}
#endif // ENABLE_MMI

// we can make this demo and run it on the device,
// to show a simulate keyboard demo
class SimulateKeyboardDemo {
public:
    SimulateKeyboardDemo()
    {
#ifdef ENABLE_GPU
        InitContext();
#endif
        OHOS::sptr<OHOS::Rosen::WindowOption> option(new WindowOption());
        option->SetDisplayId(0); // default displayId is 0
        option->SetWindowRect({ 0, 0, windowWidth, windowHeight });
        option->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
        option->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
        option->SetWindowName("simulate keyboard");
        option->SetMainHandlerAvailable(false);
        window_ = OHOS::Rosen::Window::Create(option->GetWindowName(), option);
        if (window_ != nullptr) {
            surfaceNode_ = window_->GetSurfaceNode();
            surfaceNode_->SetBounds(0, 0, windowWidth, windowHeight);
            RSTransaction::FlushImplicitTransaction();
            window_->Show();
#ifdef ENABLE_MMI
            auto listener = std::make_shared<SimulateKeyDemoEventConsumer>(window_);
            window_->SetInputEventConsumer(listener);
#endif // ENABLE_MMI
        }
        else {
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
        InitText();
    }

    ~SimulateKeyboardDemo() noexcept
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
    void InitText()
    {
        for (int i = 0; i < KEY_NUM; i++)
        {
            char key[2];
            key[0] = 'A' + i;
            keyTextBlob[i] = SkTextBlob::MakeFromString(
                key, SkFont(nullptr, KEY_FONT_SIZE, 1.0f, 0.0f));
        }
    }

    void DrawKey(SkCanvas* canvas)
    {
        SkPaint paint;
        paint.setAntiAlias(true);
        paint.setStyle(SkPaint::kFill_Style);
        paint.setStrokeWidth(2);
        paint.setColor(SK_ColorYELLOW);
        for (int i = 0; i < KEY_NUM; i++)
        {
            canvas->drawCircle(centerX + i * KEY_RADIUS * 2, 30, KEY_RADIUS, paint);
        }
    }

    void DrawText(SkCanvas* canvas)
    {
        SkPaint paint;
        paint.setAntiAlias(true);
        paint.setStyle(SkPaint::kFill_Style);
        paint.setStrokeWidth(KEY_FONT_STROKE_WIDTH);
        paint.setColor(SK_ColorBLACK);
        for (int i = 0; i < KEY_NUM; i++)
        {
            float x = centerX + i * KEY_RADIUS * 2 - KEY_FONT_SIZE / 2.0;
            int y = 30 + KEY_FONT_SIZE / 2.0;
            canvas->drawTextBlob(keyTextBlob[i].get(), x, y, paint);
        }
    }

    void Run()
    {
        while (true) {
            // request a surface buffer(400*400).
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
            canvas->clear(SK_ColorRED);
            DrawKey(canvas);
            DrawText(canvas);

            frame->SetDamageRegion(0, 0, windowWidth, windowHeight);
            rsSurface->FlushFrame(frame);
            // sleep 100ms = 100000us
            usleep(100000);
        }
    }


private:
#ifdef ENABLE_GPU
    std::unique_ptr<RenderContext> renderContext_ = nullptr;
#endif
    OHOS::sptr<OHOS::Rosen::Window> window_ = nullptr;
    std::shared_ptr<RSSurfaceNode> surfaceNode_ = nullptr;
    std::shared_ptr<RSSurface> rsSurface = nullptr;
    sk_sp<SkTextBlob> keyTextBlob[KEY_NUM] = { 0 };
    int windowWidth = 400; // window width is set to 400;
    int windowHeight = 400; // window height is set to 400;
    int centerX = KEY_RADIUS;
    int centerY = KEY_RADIUS;
};

int main()
{
    std::cout << "Simulate keyboard start!" << std::endl;
    SimulateKeyboardDemo simulateKeyboard;
    simulateKeyboard.Run();
    std::cout << "Simulate keyboard end!" << std::endl;
    return 0;
}

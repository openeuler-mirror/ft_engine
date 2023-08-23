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
enum HandType {
    HOUR_HAND,
    MIN_HAND,
    SEC_HAND,
};
constexpr int HOUR_HAND_LEN = 80; // 时针长度
constexpr int NOT_HOUR_HAND_LEN = 120; // 秒针、分针长度
constexpr int HOUR_HAND_STROKE_WIDTH = 3; // 时针的宽度
constexpr int NOT_HOUR_HAND_STROKE_WIDTH = 2; // 秒针和分钟的宽度
constexpr int CLOCK_DOT_RADIUS = 5; // 中心圆点的半径大小
constexpr int CLOCK_DAIL_RADIUS = 180; // 表盘半径大小
constexpr float CLOCK_FONT_SIZE = 24.0f; // 表盘上文字字体大小
constexpr int CLOCK_FONT_STROKE_WIDTH = 2; //表盘文字字体笔画粗细
constexpr int HOUR_FORMAT = 12; // 表盘为12小时制
constexpr int PMAM_TEXT_POS_X = 180; //上午、下午标记位置
constexpr int PMAM_TEXT_POS_Y = 75; //上午、下午标记位置
constexpr int MIN_IN_ONE_HOUR = 60;
constexpr int DAIL_NUM_RADIUS = 165; // 表盘文字环绕半径
constexpr int HOUR_PROPORTION = 60 / HOUR_FORMAT; // 圆周分为60份，1小时表示占5小格

#ifdef ENABLE_MMI
#ifdef USE_MMI_LISTENER
constexpr int INVALID_COORDINATE = -1;
class ClockDemoEventConsumer : public OHOS::MMI::IInputEventConsumer
{
public:
    ClockDemoEventConsumer(OHOS::sptr<OHOS::Rosen::Window> window);
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

ClockDemoEventConsumer::ClockDemoEventConsumer(OHOS::sptr<OHOS::Rosen::Window> window)
{
    window_ = window;
}

void ClockDemoEventConsumer::HandleMovePointerEvent(std::shared_ptr<OHOS::MMI::PointerEvent> pointerEvent) const
{
    if ((window_ == nullptr) || (pointerEvent->GetSourceType() != OHOS::MMI::PointerEvent::SOURCE_TYPE_MOUSE)) {
        return;
    }

    OHOS::MMI::PointerEvent::PointerItem pointerItem;
    int32_t pointId = pointerEvent->GetPointerId();
    if (!pointerEvent->GetPointerItem(pointId, pointerItem)) {
        return;
    }

    // 计算鼠标移动距离
    int diffX = (lastPointerX != INVALID_COORDINATE ? pointerItem.GetDisplayX() - lastPointerX : 0);
    int diffY = (lastPointerY != INVALID_COORDINATE ? pointerItem.GetDisplayY() - lastPointerY : 0);

    // 窗口的新位置 = 当前位置 + 鼠标移动距离
    if (lastWindowX == INVALID_COORDINATE && lastWindowY == INVALID_COORDINATE) {
        lastWindowX = window_->GetRect().posX_;
        lastWindowY = window_->GetRect().posY_;
    }
    int newX = lastWindowX + diffX;
    int newY = lastWindowY + diffY;

    //移动窗口，并记录当前鼠标位置
    if (enableMove && lastPointerX != INVALID_COORDINATE && lastPointerY != INVALID_COORDINATE) {
        if (pointerEvent->GetButtonId() == OHOS::MMI::PointerEvent::MOUSE_BUTTON_LEFT) {
            window_->MoveTo(newX, newY);
            lastWindowX = newX;
            lastWindowY = newY;
        }
    }
    lastPointerX = pointerItem.GetDisplayX();
    lastPointerY = pointerItem.GetDisplayY();
}

void ClockDemoEventConsumer::OnInputEvent(std::shared_ptr<OHOS::MMI::KeyEvent> keyEvent) const
{
    keyEvent->MarkProcessed();
}

void ClockDemoEventConsumer::OnInputEvent(std::shared_ptr<OHOS::MMI::AxisEvent> axisEvent) const
{
    axisEvent->MarkProcessed();
}

void ClockDemoEventConsumer::OnInputEvent(std::shared_ptr<OHOS::MMI::PointerEvent> pointerEvent) const
{
    switch (pointerEvent->GetPointerAction()) {
        case OHOS::MMI::PointerEvent::POINTER_ACTION_BUTTON_DOWN:
            enableMove = true;  // 鼠标按键按下，使能移动窗口
            break;
        case OHOS::MMI::PointerEvent::POINTER_ACTION_BUTTON_UP:
            enableMove = false; // 鼠标按键抬起，静止移动窗口
            break;
        case OHOS::MMI::PointerEvent::POINTER_ACTION_MOVE:
            HandleMovePointerEvent(pointerEvent);
            break;
        default:
            break;
    }
    pointerEvent->MarkProcessed();
}
#else // USE_MMI_LISTENER

class ClockDemoEventConsumerWindow : public OHOS::Rosen::IInputEventConsumer
{
public:
    ClockDemoEventConsumerWindow(OHOS::sptr<OHOS::Rosen::Window> window)
    {
        window_ = window;
    }
    bool OnInputEvent(const std::shared_ptr<OHOS::MMI::KeyEvent>& keyEvent) const override;
    bool OnInputEvent(const std::shared_ptr<OHOS::MMI::AxisEvent>& axisEvent) const override;
    bool OnInputEvent(const std::shared_ptr<OHOS::MMI::PointerEvent>& pointerEvent) const override;
private:
    OHOS::sptr<OHOS::Rosen::Window> window_ = nullptr;
};

bool ClockDemoEventConsumerWindow::OnInputEvent(const std::shared_ptr<OHOS::MMI::KeyEvent>& keyEvent) const
{
    keyEvent->MarkProcessed();
    return true;
}

bool ClockDemoEventConsumerWindow::OnInputEvent(const std::shared_ptr<OHOS::MMI::AxisEvent>& axisEvent) const
{
    axisEvent->MarkProcessed();
    return true;
}

bool ClockDemoEventConsumerWindow::OnInputEvent(const std::shared_ptr<OHOS::MMI::PointerEvent>& pointerEvent) const
{
    window_->StartMove();
    pointerEvent->MarkProcessed();
    return true;
}
#endif // USE_MMI_LISTENER
#endif // ENABLE_MMI

// we can make this demo and run it on the device,
// to show a clock demo
class ClockDemo {
public:
    ClockDemo()
    {
#ifdef ENABLE_GPU
        InitContext();
#endif
        OHOS::sptr<OHOS::Rosen::WindowOption> option(new WindowOption());
        option->SetDisplayId(0); // default displayId is 0
        option->SetWindowRect({0, 0, windowWidth, windowHeight}); // test window size is 600*600
        option->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
        option->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
        option->SetWindowName("clock");
        option->SetMainHandlerAvailable(false);
        window_ = OHOS::Rosen::Window::Create(option->GetWindowName(), option);
        if (window_ != nullptr) {
            surfaceNode_ = window_->GetSurfaceNode();
            surfaceNode_->SetBounds(0, 0, windowWidth, windowHeight); // surface bounds is window rect.
            RSTransaction::FlushImplicitTransaction();
            window_->Show();
#ifdef ENABLE_MMI
#ifdef USE_MMI_LISTENER
            auto listener = std::make_shared<ClockDemoEventConsumer>(window_);
            const std::string thread = "clockEventThread";
            auto runner = OHOS::AppExecFwk::EventRunner::Create(thread);
            auto eventHandler = std::make_shared<OHOS::AppExecFwk::EventHandler>(runner);
            OHOS::MMI::InputManager::GetInstance()->SetWindowInputEventConsumer(listener, eventHandler);
#else  // USE_MMI_LISTENER: no mmi listener, use window input event listener
            auto listener = std::make_shared<ClockDemoEventConsumerWindow>(window_);
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
        InitText();
    }

    ~ClockDemo() noexcept
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

    void DrawCenter(SkCanvas *canvas)
    {
        SkPaint paint;
        paint.setAntiAlias(true);
        paint.setStyle(SkPaint::kFill_Style);
        paint.setStrokeWidth(NOT_HOUR_HAND_STROKE_WIDTH);
        paint.setStrokeJoin(SkPaint::kRound_Join);
        paint.setColor(SK_ColorRED);
        canvas->drawCircle(centerX, centerY, CLOCK_DOT_RADIUS, paint);
    }

    void DrawCircle(SkCanvas *canvas)
    {
        SkPaint paint;
        paint.setAntiAlias(true);
        paint.setStyle(SkPaint::kFill_Style);
        paint.setStrokeWidth(NOT_HOUR_HAND_STROKE_WIDTH);
        paint.setColor(SK_ColorYELLOW);
        // draw clock dial
        canvas->drawCircle(centerX, centerY, CLOCK_DAIL_RADIUS, paint);
    }

    void DrawHand(SkCanvas *canvas, float times, int handType)
    {
        // 3 time clock is 0 degree angle
        double angle = PI * 2 * (times / (double)MIN_IN_ONE_HOUR) - PI / 2;
        double handRadius = handType == HOUR_HAND ? HOUR_HAND_LEN : NOT_HOUR_HAND_LEN;
        SkPaint paint;
        paint.setAntiAlias(true);
        paint.setStyle(SkPaint::kStroke_Style);
        if (handType == SEC_HAND) {
            paint.setColor(SK_ColorCYAN);
        } else {
            paint.setColor(SK_ColorBLACK);
        }
        if (handType == HOUR_HAND) {
            paint.setStrokeWidth(HOUR_HAND_STROKE_WIDTH);
        } else {
            paint.setStrokeWidth(NOT_HOUR_HAND_STROKE_WIDTH);
        }
        SkPath path;
        path.moveTo(centerX, centerY);
        float x = centerX + cos(angle) * handRadius;
        float y = centerY + sin(angle) * handRadius;
        path.lineTo(x, y);
        canvas->drawPath(path, paint);
    }

    struct tm* GetTime()
    {
        time_t timeVal;
	    time(&timeVal);
        return localtime(&timeVal);
    }

    void DrawHands(SkCanvas *canvas)
    {
        struct tm* t = GetTime();
        int hour = t->tm_hour > HOUR_FORMAT ? t->tm_hour - HOUR_FORMAT : t->tm_hour;
        printf("current time:%02d:%02d:%02d\n", t->tm_hour, t->tm_min, t->tm_sec);
        DrawHand(canvas, hour * 5.0 + t->tm_min / MIN_IN_ONE_HOUR * 5, HOUR_HAND);
        DrawHand(canvas, t->tm_min, MIN_HAND);
        DrawHand(canvas, t->tm_sec, SEC_HAND);
        DrawText(canvas, t->tm_hour);
    }

    void InitText()
    {
        for (int i = 0; i < HOUR_FORMAT; i++) {
            std::string number = std::to_string(i+1);
            numTextBlob[i] = SkTextBlob::MakeFromString(
                number.c_str(), SkFont(nullptr, CLOCK_FONT_SIZE, 1.0f, 0.0f)); // font size: 24
        }
    }

    void DrawText(SkCanvas *canvas, int hour)
    {
        SkPaint paint;
        paint.setAntiAlias(true);
        paint.setStyle(SkPaint::kFill_Style);
        paint.setStrokeWidth(CLOCK_FONT_STROKE_WIDTH);
        paint.setColor(SK_ColorBLACK);
        sk_sp<SkTextBlob> textBlob = SkTextBlob::MakeFromString(
                hour > HOUR_FORMAT ? "PM" : "AM", SkFont(nullptr, CLOCK_FONT_SIZE, 1.0f, 0.0f)); // font size: 24
        canvas->drawTextBlob(textBlob.get(), PMAM_TEXT_POS_X, PMAM_TEXT_POS_Y, paint);

        // draw numbers
        for (int i = 0; i < HOUR_FORMAT; i++)
        {
            double angle = PI / 6 * (i + 1 - 3);  // i*PI/6 + PI/6 - PI/2 , 1 clock is 30 degree, so need add PI/6
            int w = numTextBlob[i].get()->bounds().width();
            int h = numTextBlob[i].get()->bounds().height();
            float x = centerX + cos(angle) * DAIL_NUM_RADIUS - w / 2 + 15; // 15 is fine tuning
            float y = centerY + sin(angle) * DAIL_NUM_RADIUS + h / 2 - 5; // 5 is fine tuning
            canvas->drawTextBlob(numTextBlob[i].get(), x, y, paint);
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
            DrawCircle(canvas);
            DrawCenter(canvas);
            DrawHands(canvas);

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
    sk_sp<SkTextBlob> numTextBlob[HOUR_FORMAT] = { 0 };
    int windowWidth = 400; // window width is set to 400;
    int windowHeight = 400; // window height is set to 400;
    float centerX = 200;
    float centerY = 200;
};

int main()
{
    std::cout << "clock start!" << std::endl;
    ClockDemo clock;
    clock.Run();
    std::cout << "clock end!" << std::endl;
    return 0;
}

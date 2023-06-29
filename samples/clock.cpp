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
constexpr int CLOCK_DOT_RADIUS = 5; // 中心原点的半径大小
constexpr int CLOCK_DAIL_RADIUS = 180; // 表盘半径大小
constexpr float CLOCK_FONT_SIZE = 24.0f; // 表盘上文字字体大小
constexpr int CLOCK_FONT_STROKE_WIDTH = 2; //表盘文字字体笔画粗细
constexpr int HOUR_FORMAT = 12; // 表盘为12小时制
constexpr int PMAM_TEXT_POS_X = 180; //上午、下午标记位置
constexpr int PMAM_TEXT_POS_Y = 75; //上午、下午标记位置
constexpr int MIN_IN_ONE_HOUR = 60;
constexpr int DAIL_NUM_RADIUS = 165; // 表盘文字环绕半径
constexpr int HOUR_PROPORTION = 60 / HOUR_FORMAT; // 圆周分为60份，1小时表示占5小格


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
        window_ = OHOS::Rosen::Window::Create(option->GetWindowName(), option);
        if (window_ != nullptr) {
            surfaceNode_ = window_->GetSurfaceNode();
            surfaceNode_->SetBounds(0, 0, windowWidth, windowHeight); // surface bounds is window rect.
            RSTransaction::FlushImplicitTransaction();
            window_->Show();
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

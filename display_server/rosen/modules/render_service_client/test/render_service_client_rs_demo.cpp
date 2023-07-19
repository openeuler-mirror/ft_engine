/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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
#include <surface.h>

#include "include/core/SkCanvas.h"
#include "include/core/SkImageInfo.h"
// RS Base
#include "command/rs_base_node_command.h"
#include "command/rs_display_node_command.h"
#include "command/rs_surface_node_command.h"
#include "common/rs_common_def.h"
#include "pipeline/rs_render_result.h"
#include "pipeline/rs_render_thread.h"
// RS client
#include "ui/rs_canvas_node.h"
#include "ui/rs_surface_extractor.h"
#include "ui/rs_ui_director.h"
#include "ui/rs_display_node.h"
#include "ui/rs_surface_node.h"
#include "transaction/rs_interfaces.h"
// 2d_graphics
#ifdef ACE_ENABLE_GPU
#include "render_context/render_context.h"
#endif

using namespace OHOS;
using namespace OHOS::Rosen;
using namespace std;

namespace OHOS::Rosen {
namespace {
constexpr int SURFACE_NODE_SIZE = 100;
constexpr int SLEEP_TIME = 8;
}
#ifdef ACE_ENABLE_GPU
    RenderContext* rc_ = nullptr;
#endif

namespace pipelineTestUtils {
    constexpr bool wrongExit = false;
    constexpr bool successExit = false;
    struct Circle {
        Circle(float x, float y, float r)
            :x_(x), y_(y), r_(r) {};
        float x_ = 0.0f;
        float y_ = 0.0f;
        float r_ = 0.0f;
    }; // class Circle

    class ToDrawSurface {
    public:
        using drawFun = std::function<void(SkCanvas&, SkPaint&)>;
        ToDrawSurface() {};

        inline ToDrawSurface& SetSurfaceNode(const std::shared_ptr<RSSurfaceNode> surfaceNode)
        {
            surfaceNode_ = surfaceNode;
            return *this;
        }

        inline ToDrawSurface& SetSurfaceNodeSize(SkRect surfaceGeometry)
        {
            surfaceGeometry_ = surfaceGeometry;
            return *this;
        }

        inline ToDrawSurface& SetBufferSize(int width, int height)
        {
            bufferSize_ = SkRect::MakeXYWH(0, 0, width, height);
            return *this;
        }

        inline ToDrawSurface& SetBufferSizeAuto()
        {
            bufferSize_ = surfaceGeometry_;
            return *this;
        }

        inline ToDrawSurface& SetBufferSize(SkRect bufferSize)
        {
            // bufferSize has no XY
            bufferSize_ = bufferSize;
            return *this;
        }

        inline ToDrawSurface& SetShapeColor(uint32_t color)
        {
            color_ = color;
            return *this;
        }

        inline ToDrawSurface& SetDraw(drawFun drawShape)
        {
            drawShape_ = drawShape;
            return *this;
        }

        bool Run()
        {
            printf("ToDrawSurface::Run() start\n");
            if (surfaceNode_ == nullptr) {
                printf("DrawSurface: surfaceNode_ is nullptr\n");
                return false;
            }
            auto x = surfaceGeometry_.x();
            auto y = surfaceGeometry_.y();
            auto width = surfaceGeometry_.width();
            auto height = surfaceGeometry_.height();
            surfaceNode_->SetBounds(x, y, width, height);
            std::shared_ptr<RSSurface> rsSurface = RSSurfaceExtractor::ExtractRSSurface(surfaceNode_);
            if (rsSurface == nullptr) {
                return wrongExit;
            }
#ifdef ACE_ENABLE_GPU
            // SetRenderContext must before rsSurface->RequestFrame, or it will failed.
            if (rc_) {
                rsSurface->SetRenderContext(rc_);
            } else {
                printf("DrawSurface: RenderContext is nullptr\n");
            }
#endif
            auto framePtr = rsSurface->RequestFrame(bufferSize_.width(), bufferSize_.height());
            if (!framePtr) {
                // SetRenderContext must before rsSurface->RequestFrame,
                //      or frameptr will be nullptr.
                printf("DrawSurface: frameptr is nullptr\n");
                return wrongExit;
            }
            auto canvas = framePtr->GetCanvas();
            if (!canvas) {
                printf("DrawSurface: canvas is nullptr\n");
                return wrongExit;
            }
            canvas->clear(SK_ColorTRANSPARENT);
            SkPaint paint;
            paint.setAntiAlias(true);
            paint.setStyle(SkPaint::kFill_Style);
            paint.setStrokeWidth(20);
            paint.setStrokeJoin(SkPaint::kRound_Join);
            paint.setColor(color_);
            if (!drawShape_) {
                printf("DrawSurface: drawShape_ is nullptr\n");
                return wrongExit;
            }
            drawShape_(*(canvas), paint);
            framePtr->SetDamageRegion(0, 0, surfaceGeometry_.width(), surfaceGeometry_.height());
            rsSurface->FlushFrame(framePtr);
            return successExit;
        }
    private:
        SkRect surfaceGeometry_ = {0.f, 0.f, 0.f, 0.f};
        SkRect bufferSize_ = {0.f, 0.f, 0.f, 0.f};
        drawFun drawShape_;
        uint32_t color_ = 0;
        std::shared_ptr<RSSurfaceNode> surfaceNode_ = nullptr;
    }; // class ToDrawSurface

    static std::shared_ptr<RSSurfaceNode> CreateSurface()
    {
        RSSurfaceNodeConfig config;
        return RSSurfaceNode::Create(config);
    }
} // namespace pipelineTestUtils

// Toy DMS.
using DisplayId = ScreenId;
class DmsMock {
private:
    struct Display {
        DisplayId id;
        RSScreenModeInfo activeMode;
    };
    std::unordered_map<DisplayId, Display> displays_;
    mutable std::recursive_mutex mutex_;
    RSInterfaces& rsInterface_;
    DisplayId defaultDisplayId_;

    DmsMock() : rsInterface_(RSInterfaces::GetInstance())
    {
        Init();
    }

    void Init()
    {
        std::lock_guard<std::recursive_mutex> lock(mutex_);
        (void)rsInterface_.SetScreenChangeCallback([this](ScreenId id, ScreenEvent event) {
            switch (event) {
                case ScreenEvent::CONNECTED: {
                    this->OnDisplayConnected(id);
                    break;
                }
                case ScreenEvent::DISCONNECTED: {
                    this->OnDisplayDisConnected(id);
                    break;
                }
                default:
                    break;
            }
        });

        defaultDisplayId_ = rsInterface_.GetDefaultScreenId();
        displays_[defaultDisplayId_] =
            Display { defaultDisplayId_, rsInterface_.GetScreenActiveMode(defaultDisplayId_) };
    }

public:
    inline static DmsMock& GetInstance()
    {
        static DmsMock c;
        return c;
    }

    ~DmsMock() noexcept = default;

    DisplayId GetDefaultDisplayId() const
    {
        std::lock_guard<std::recursive_mutex> lock(mutex_);
        return defaultDisplayId_;
    }

    std::optional<RSScreenModeInfo> GetDisplayActiveMode(DisplayId id) const
    {
        std::lock_guard<std::recursive_mutex> lock(mutex_);
        if (displays_.count(id) == 0) {
            cout << "DmsMock: No display " << id << "!" << endl;
            return {};
        }
        return displays_.at(id).activeMode;
    }

    void OnDisplayConnected(ScreenId id)
    {
        std::lock_guard<std::recursive_mutex> lock(mutex_);
        displays_[id] = Display { id, rsInterface_.GetScreenActiveMode(id) };
        std::cout << "DmsMock: Display " << id << " connected." << endl;
    }

    void OnDisplayDisConnected(ScreenId id)
    {
        std::lock_guard<std::recursive_mutex> lock(mutex_);
        if (displays_.count(id) == 0) {
            cout << "DmsMock: No display " << id << "!" << endl;
        } else {
            std::cout << "DmsMock: Display " << id << " disconnected." << endl;
            displays_.erase(id);
            if (id == defaultDisplayId_) {
                defaultDisplayId_ = rsInterface_.GetDefaultScreenId();
                std::cout << "DmsMock: DefaultDisplayId changed, new DefaultDisplayId is" << defaultDisplayId_ << "."
                          << endl;
            }
        }
    }
}; // class DmsMock

class RSDemoTestCase {
public:
    inline static RSDemoTestCase& GetInstance()
    {
        static RSDemoTestCase c;
        return c;
    }

    void TestInit()
    {
        isInit_ = true;
        std::cout << "Render service Client rs Demo.cpp testInit Start\n";

#ifdef ACE_ENABLE_GPU
        std::cout << "ACE_ENABLE_GPU is enabled\n";
        isGPU_ = true;
#else
        std::cout << "ACE_ENABLE_GPU is disabled\n";
        isGPU_ = false;
#endif

        id_ = DmsMock::GetInstance().GetDefaultDisplayId();
        std::cout << "RS default screen id is " << id_ << ".\n";
        auto activeModeInfo = DmsMock::GetInstance().GetDisplayActiveMode(id_);
        if (activeModeInfo) {
            screenWidth_ = activeModeInfo->GetScreenWidth();
            screenheight_ = activeModeInfo->GetScreenHeight();
            screenRefreshRate_ = static_cast<int>(activeModeInfo->GetScreenRefreshRate());
            std::cout << "Display " << id_ << " active mode info:\n";
            std::cout << "Width: " << screenWidth_ << ", Height: " << screenheight_;
            std::cout << ", RefreshRate: " << screenRefreshRate_ << "Hz.\n";
        } else {
            std::cout << "Display " << id_ << " has no active mode!\n";
        }
        RenderContextInit();
        std::cout << "Render service Client rs Demo.cpp testInit end\n";
        std::cout << "-------------------------------------------------------\n";
        return;
    }

    void TestCaseDefault()
    {
        std::cout << "-------------------------------------------------------\n";
        std::cout << "Render service Client rs Demo.cpp testCaseDefault Start\n";
        auto surfaceNode1 = pipelineTestUtils::CreateSurface();
        auto surfaceNode2 = pipelineTestUtils::CreateSurface();

        pipelineTestUtils::ToDrawSurface()
            .SetSurfaceNode(surfaceNode1)
            .SetShapeColor(0xff00ffff)
            .SetSurfaceNodeSize(SkRect::MakeXYWH(SURFACE_NODE_SIZE, 0, screenWidth_ * 0.4f, screenheight_ * 0.3f))
            .SetBufferSizeAuto()
            .SetDraw([&](SkCanvas &canvas, SkPaint &paint) -> void {
                canvas.drawRect(
                    SkRect::MakeXYWH(0, 0, screenWidth_ * 0.4f, screenheight_ * 0.3f),
                    paint);
            })
            .Run();
        pipelineTestUtils::ToDrawSurface()
            .SetSurfaceNode(surfaceNode2)
            .SetShapeColor(0xffff0000)
            .SetSurfaceNodeSize(SkRect::MakeXYWH(SURFACE_NODE_SIZE, screenheight_ * 0.8f, screenWidth_ * 0.4f,
                screenheight_ * 0.2f))
            .SetBufferSizeAuto()
            .SetDraw([&](SkCanvas &canvas, SkPaint &paint) -> void {
                canvas.drawRect(
                    SkRect::MakeXYWH(0, 0, screenWidth_ * 0.4f, screenheight_ * 0.2f),
                    paint);
            })
            .Run();

        RSDisplayNodeConfig config;
        RSDisplayNode::SharedPtr displayNode = RSDisplayNode::Create(config);
        displayNode->SetScreenId(id_);
        displayNode->SetBounds(0, 0, screenWidth_, screenheight_);
        displayNode->AddChild(surfaceNode1, -1);
        displayNode->AddChild(surfaceNode2, -1);

        auto transactionProxy = RSTransactionProxy::GetInstance();
        if (transactionProxy != nullptr) {
            transactionProxy->FlushImplicitTransaction();
        }
        sleep(SLEEP_TIME);
        displayNode->RemoveFromTree();
        if (transactionProxy != nullptr) {
            transactionProxy->FlushImplicitTransaction();
        }
        std::cout << "Render service Client rs Demo.cpp testCaseDefault end\n";
        std::cout << "-------------------------------------------------------\n";
    }
private:
    RSDemoTestCase() = default;
    static void RenderContextInit()
    {
#ifdef ACE_ENABLE_GPU
        std::cout << "ACE_ENABLE_GPU is true. \n";
        std::cout << "Init RenderContext start. \n";
            rc_ = RenderContextFactory::GetInstance().CreateEngine();
            if (rc_) {
                std::cout << "Init RenderContext success.\n";
                rc_->InitializeEglContext();
            } else {
                std::cout << "Init RenderContext failed, RenderContext is nullptr.\n";
            }
        std::cout << "Init RenderContext start.\n";
#endif
    }

    bool isInit_ = false;
    bool isGPU_ = false;
    int screenWidth_ = 0;
    int screenheight_ = 0;
    int screenRefreshRate_ = 0;
    DisplayId id_;
}; // class RSDemoTestCase
} // namespace OHOS::Rosen

int main()
{
    RSDemoTestCase::GetInstance().TestInit();
    RSDemoTestCase::GetInstance().TestCaseDefault();
    return 0;
}

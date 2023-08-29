/*
 * Copyright (c) 2023 Huawei Technologies Co., Ltd.
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
#include "property/rs_properties_def.h"
#include "render_context/render_context.h"
#include "transaction/rs_transaction.h"
#include "transaction/rs_interfaces.h"
#include "ui/rs_surface_extractor.h"
#include "ui/rs_surface_node.h"
#include "wm/window.h"
#include "pixel_map.h"
#include "image_source.h"


using namespace OHOS::Rosen;

class DesktopDemo {
public:
    DesktopDemo()
    {
        DrawDesktop();
    }

    ~DesktopDemo() noexcept
    {
        if (window_ != nullptr) {
            window_->Hide();
            window_->Destroy();
        }
    }

    std::unique_ptr<OHOS::Media::PixelMap> DecodeImageToPixelMap()
    {
        OHOS::Media::SourceOptions opts;
        opts.formatHint = "image/png";
        uint32_t ret = 0;
        auto imageSource = OHOS::Media::ImageSource::CreateImageSource(desktopPath, opts, ret);
        if (ret != 0) {
            std::cout << "CreateImageSource fail" << std::endl;
            return nullptr;
        }

        std::set<std::string> formats;
        ret = imageSource->GetSupportedFormats(formats);
        if (ret != 0) {
            std::cout << "GetSupportedFormats fail" << std::endl;
            return nullptr;
        }

        OHOS::Media::DecodeOptions decodeOpts;
        decodeOpts.desiredSize = {
            .width = desktopW,
            .height = desktopH
        };
        std::unique_ptr<OHOS::Media::PixelMap> pixelMap = imageSource->CreatePixelMap(decodeOpts, ret);
        if (pixelMap == nullptr) {
            std::cout << "CreatePixelMap fail" << std::endl;
            return nullptr;
        }
        return pixelMap;
    }

    SkColorType PixelFormatConvert(const OHOS::Media::PixelFormat& pixelFormat)
    {
        SkColorType colorType;
        switch (pixelFormat) {
            case OHOS::Media::PixelFormat::BGRA_8888:
                colorType = SkColorType::kBGRA_8888_SkColorType;
                break;
            case OHOS::Media::PixelFormat::RGBA_8888:
                colorType = SkColorType::kRGBA_8888_SkColorType;
                break;
            case OHOS::Media::PixelFormat::RGB_565:
                colorType = SkColorType::kRGB_565_SkColorType;
                break;
            case OHOS::Media::PixelFormat::ALPHA_8:
                colorType = SkColorType::kAlpha_8_SkColorType;
                break;
            default:
                colorType = SkColorType::kUnknown_SkColorType;
                break;
        }
        return colorType;
    }

    void DrawDesktop()
    {
        int displayId = OHOS::Rosen::RSInterfaces::GetInstance().GetDefaultScreenId();
        auto activeModeInfo = OHOS::Rosen::RSInterfaces::GetInstance().GetScreenActiveMode(displayId);
        desktopW = activeModeInfo.GetScreenWidth();
        desktopH = activeModeInfo.GetScreenHeight();
        printf("desktopW=%d, desktopH=%d\n", desktopW, desktopH);

#ifdef ENABLE_GPU
        InitContext();
#endif

        OHOS::sptr<OHOS::Rosen::WindowOption> option(new WindowOption());
        option->SetDisplayId(0);
        option->SetWindowRect({0, 0, desktopW, desktopH});
        option->SetWindowType(WindowType::WINDOW_TYPE_DESKTOP);
        option->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);
        option->SetWindowName("desktop");
        window_ = OHOS::Rosen::Window::Create(option->GetWindowName(), option);
        if (window_ != nullptr) {
            surfaceNode_ = window_->GetSurfaceNode();
            surfaceNode_->SetBounds(0, 0, desktopW, desktopH); // surface bounds is window rect.
            RSTransaction::FlushImplicitTransaction();
            window_->Show();
        } else {
            std::cout << "Failed to window_ window!" << std::endl;
            return;
        }

        if (surfaceNode_ == nullptr) {
            return;
        }

        rsSurface_ = RSSurfaceExtractor::ExtractRSSurface(surfaceNode_);
        if (rsSurface_ == nullptr) {
            return;
        }

#ifdef ENABLE_GPU
        if (renderContext_ != nullptr) {
            rsSurface_->SetRenderContext(renderContext_.get());
        }
#endif

        auto frame = rsSurface_->RequestFrame(desktopW, desktopH);
        if (frame == nullptr) {
            std::cout << "Failed to create frame!" << std::endl;
            return;
        }
        auto canvas = frame->GetCanvas();
        if (canvas == nullptr) {
            std::cout << "Failed to create canvas!" << std::endl;
            return;
        }

        auto pixelmap = DecodeImageToPixelMap();
        if (pixelmap == nullptr) {
            std::cout << "DecodeImageToPixelMap fail!" << std::endl;
            return;
        }
        SkImageInfo imageInfo = SkImageInfo::Make(pixelmap->GetWidth(), pixelmap->GetHeight(),
            PixelFormatConvert(pixelmap->GetPixelFormat()),
            static_cast<SkAlphaType>(pixelmap->GetAlphaType()));
        SkPixmap srcPixmap(imageInfo, pixelmap->GetPixels(), pixelmap->GetRowBytes());
        SkBitmap srcBitmap;
        srcBitmap.installPixels(srcPixmap);
        canvas->drawBitmap(srcBitmap, 0, 0);

        frame->SetDamageRegion(0, 0, desktopW, desktopH);
        rsSurface_->FlushFrame(frame);
    }

#ifdef ENABLE_GPU
    void InitContext()
    {
        renderContext_ = std::make_unique<RenderContext>();
        renderContext_->InitializeEglContext();
    }
#endif

private:
    OHOS::sptr<OHOS::Rosen::Window> window_ = nullptr;
    std::shared_ptr<RSSurfaceNode> surfaceNode_ = nullptr;
    std::shared_ptr<RSSurface> rsSurface_ = nullptr;
    int desktopW = 500;
    int desktopH = 500;
    std::string desktopPath = "/usr/local/share/ft/desktop/desktop.png";
#ifdef ENABLE_GPU
    std::unique_ptr<RenderContext> renderContext_ = nullptr;
#endif
};

int main()
{
    std::cout << "desktop start!" << std::endl;
    DesktopDemo desktop;

    while (true) {
        sleep(10);
    }
    std::cout << "desktop end!" << std::endl;
    return 0;
}

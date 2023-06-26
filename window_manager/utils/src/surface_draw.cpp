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

#include "surface_draw.h"
#include <algorithm>
#include <surface.h>
#include <transaction/rs_interfaces.h>
#include <ui/rs_surface_extractor.h>

#include "image/bitmap.h"
#include "image_source.h"
#include "image_type.h"
#include "image_utils.h"
#include "pixel_map.h"
#include "surface_capture_future.h"
#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "SurfaceDraw"};
    constexpr uint32_t IMAGE_BYTES_STRIDE = 4;
} // namespace

bool SurfaceDraw::DrawImage(std::shared_ptr<RSSurfaceNode> surfaceNode, int32_t bufferWidth,
    int32_t bufferHeight, const std::string& imagePath)
{
    sptr<OHOS::Surface> layer = GetLayer(surfaceNode);
    if (layer == nullptr) {
        WLOGFE("layer is nullptr");
        return false;
    }
    sptr<OHOS::SurfaceBuffer> buffer = GetSurfaceBuffer(layer, bufferWidth, bufferHeight);
    if (buffer == nullptr || buffer->GetVirAddr() == nullptr) {
        return false;
    }
    auto addr = static_cast<uint8_t *>(buffer->GetVirAddr());
    if (!DoDraw(addr, buffer->GetWidth(), buffer->GetHeight(), imagePath)) {
        WLOGE("draw window pixel failed");
        return false;
    }
    OHOS::BufferFlushConfig flushConfig = {
        .damage = {
            .w = buffer->GetWidth(),
            .h = buffer->GetHeight(),
        },
    };
    OHOS::SurfaceError ret = layer->FlushBuffer(buffer, -1, flushConfig);
    if (ret != OHOS::SurfaceError::SURFACE_ERROR_OK) {
        WLOGFE("draw pointer FlushBuffer ret:%{public}s", SurfaceErrorStr(ret).c_str());
        return false;
    }
    return true;
}

bool SurfaceDraw::DrawImage(std::shared_ptr<RSSurfaceNode> surfaceNode, int32_t bufferWidth,
    int32_t bufferHeight, std::shared_ptr<Media::PixelMap> pixelMap)
{
    sptr<OHOS::Surface> layer = GetLayer(surfaceNode);
    if (layer == nullptr) {
        WLOGFE("layer is nullptr");
        return false;
    }
    sptr<OHOS::SurfaceBuffer> buffer = GetSurfaceBuffer(layer, bufferWidth, bufferHeight);
    if (buffer == nullptr || buffer->GetVirAddr() == nullptr) {
        return false;
    }
    auto addr = static_cast<uint8_t *>(buffer->GetVirAddr());
    if (!DoDraw(addr, buffer->GetWidth(), buffer->GetHeight(), pixelMap)) {
        WLOGE("draw window pixel failed");
        return false;
    }
    OHOS::BufferFlushConfig flushConfig = {
        .damage = {
            .w = buffer->GetWidth(),
            .h = buffer->GetHeight(),
        },
    };
    OHOS::SurfaceError ret = layer->FlushBuffer(buffer, -1, flushConfig);
    if (ret != OHOS::SurfaceError::SURFACE_ERROR_OK) {
        WLOGFE("draw pointer FlushBuffer ret:%{public}s", SurfaceErrorStr(ret).c_str());
        return false;
    }
    return true;
}

bool SurfaceDraw::DrawColor(std::shared_ptr<RSSurfaceNode> surfaceNode, int32_t bufferWidth,
    int32_t bufferHeight, uint32_t color)
{
    sptr<OHOS::Surface> layer = GetLayer(surfaceNode);
    if (layer == nullptr) {
        WLOGFE("layer is nullptr");
        return false;
    }
    sptr<OHOS::SurfaceBuffer> buffer = GetSurfaceBuffer(layer, bufferWidth, bufferHeight);
    if (buffer == nullptr || buffer->GetVirAddr() == nullptr) {
        return false;
    }
    auto addr = static_cast<uint8_t *>(buffer->GetVirAddr());
    if (!DoDraw(addr, buffer->GetWidth(), buffer->GetHeight(), color)) {
        WLOGE("draw window color failed");
        return false;
    }
    OHOS::BufferFlushConfig flushConfig = {
        .damage = {
            .w = buffer->GetWidth(),
            .h = buffer->GetHeight(),
        },
    };
    OHOS::SurfaceError ret = layer->FlushBuffer(buffer, -1, flushConfig);
    if (ret != OHOS::SurfaceError::SURFACE_ERROR_OK) {
        WLOGFE("draw pointer FlushBuffer ret:%{public}s", SurfaceErrorStr(ret).c_str());
        return false;
    }
    return true;
}

sptr<OHOS::Surface> SurfaceDraw::GetLayer(std::shared_ptr<RSSurfaceNode> surfaceNode)
{
    if (surfaceNode == nullptr) {
        return nullptr;
    }
    return surfaceNode->GetSurface();
}

sptr<OHOS::SurfaceBuffer> SurfaceDraw::GetSurfaceBuffer(sptr<OHOS::Surface> layer,
    int32_t bufferWidth, int32_t bufferHeight)
{
    sptr<OHOS::SurfaceBuffer> buffer;
    int32_t releaseFence = 0;
    OHOS::BufferRequestConfig config = {
        .width = bufferWidth,
        .height = bufferHeight,
        .strideAlignment = 0x8,
        .format = PIXEL_FMT_RGBA_8888,
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA,
    };

    OHOS::SurfaceError ret = layer->RequestBuffer(buffer, releaseFence, config);
    if (ret != OHOS::SURFACE_ERROR_OK) {
        WLOGFE("request buffer ret:%{public}s", SurfaceErrorStr(ret).c_str());
        return nullptr;
    }
    return buffer;
}

std::unique_ptr<OHOS::Media::PixelMap> SurfaceDraw::DecodeImageToPixelMap(const std::string &imagePath)
{
    OHOS::Media::SourceOptions opts;
    opts.formatHint = "image/png";
    uint32_t ret = 0;
    auto imageSource = OHOS::Media::ImageSource::CreateImageSource(imagePath, opts, ret);
    if (imageSource == nullptr) {
        WLOGFE("invalid image path.");
        return nullptr;
    }
    std::set<std::string> formats;
    ret = imageSource->GetSupportedFormats(formats);
    WLOGFD("get supported format ret:%{public}u", ret);

    OHOS::Media::DecodeOptions decodeOpts;
    std::unique_ptr<OHOS::Media::PixelMap> pixelMap = imageSource->CreatePixelMap(decodeOpts, ret);
    if (pixelMap == nullptr) {
        WLOGFE("pixelMap is nullptr");
    }
    return pixelMap;
}

void SurfaceDraw::DrawPixelmap(Drawing::Canvas &canvas, const std::string& imagePath)
{
#if 0
    std::unique_ptr<OHOS::Media::PixelMap> pixelmap = DecodeImageToPixelMap(imagePath);
    if (pixelmap == nullptr) {
        WLOGFE("drawing pixel map is nullptr");
        return;
    }
    Drawing::Pen pen;
    pen.SetAntiAlias(true);
    pen.SetColor(Drawing::Color::COLOR_BLUE);
    Drawing::scalar penWidth = 1;
    pen.SetWidth(penWidth);
    canvas.AttachPen(pen);
    canvas.DrawBitmap(*pixelmap, 0, 0);
#endif
}

bool SurfaceDraw::DoDraw(uint8_t *addr, uint32_t width, uint32_t height, const std::string& imagePath)
{
#if 0
    Drawing::Bitmap bitmap;
    Drawing::BitmapFormat format { Drawing::ColorType::COLORTYPE_RGBA_8888,
        Drawing::AlphaType::ALPHATYPE_OPAQUE };
    bitmap.Build(width, height, format);
    Drawing::Canvas canvas;
    canvas.Bind(bitmap);
    canvas.Clear(Drawing::Color::COLOR_TRANSPARENT);
    DrawPixelmap(canvas, imagePath);
    uint32_t addrSize = width * height * IMAGE_BYTES_STRIDE;
    errno_t ret = memcpy_s(addr, addrSize, bitmap.GetPixels(), addrSize);
    if (ret != EOK) {
        WLOGFE("draw failed");
        return false;
    }
#endif
    return true;
}

bool SurfaceDraw::DoDraw(uint8_t *addr, uint32_t width, uint32_t height, std::shared_ptr<Media::PixelMap> pixelMap)
{
#if 0
    Drawing::Bitmap bitmap;
    Drawing::Canvas canvas;
    Drawing::BitmapFormat format { Drawing::ColorType::COLORTYPE_RGBA_8888, Drawing::AlphaType::ALPHATYPE_OPAQUE };
    bitmap.Build(width, height, format);
    canvas.Bind(bitmap);
    canvas.Clear(Drawing::Color::COLOR_TRANSPARENT);

    Drawing::Image image;
    Drawing::Bitmap imageBitmap;
    Drawing::SamplingOptions sampling = Drawing::SamplingOptions(Drawing::FilterMode::NEAREST,
        Drawing::MipmapMode::NEAREST);
    imageBitmap.Build(pixelMap->GetWidth(), pixelMap->GetHeight(), format);
    imageBitmap.SetPixels(const_cast<uint8_t*>(pixelMap->GetPixels()));
    image.BuildFromBitmap(imageBitmap);

    Drawing::Rect dst(0, 0, width, height);
    Drawing::Rect src(0, 0, pixelMap->GetWidth(), pixelMap->GetHeight());
    canvas.DrawImageRect(image, src, dst, sampling);

    uint32_t addrSize = width * height * IMAGE_BYTES_STRIDE;
    errno_t ret = memcpy_s(addr, addrSize, bitmap.GetPixels(), addrSize);
    if (ret != EOK) {
        WLOGFE("draw failed");
        return false;
    }
#endif
    return true;
}

bool SurfaceDraw::DoDraw(uint8_t *addr, uint32_t width, uint32_t height, uint32_t color)
{
#if 0
    Drawing::Bitmap bitmap;
    Drawing::BitmapFormat format { Drawing::ColorType::COLORTYPE_RGBA_8888,
        Drawing::AlphaType::ALPHATYPE_OPAQUE };
    bitmap.Build(width, height, format);
    Drawing::Canvas canvas;
    canvas.Bind(bitmap);
    canvas.Clear(color);

    uint32_t addrSize = width * height * IMAGE_BYTES_STRIDE;
    errno_t ret = memcpy_s(addr, addrSize, bitmap.GetPixels(), addrSize);
    if (ret != EOK) {
        WLOGFE("draw failed");
        return false;
    }
#endif
    return true;
}

bool SurfaceDraw::DrawImageRect(std::shared_ptr<RSSurfaceNode> surfaceNode, Rect rect,
    std::shared_ptr<Media::PixelMap> pixelMap, uint32_t color, bool fillWindow)
{
    int32_t winHeight = static_cast<int32_t>(rect.height_);
    int32_t winWidth = static_cast<int32_t>(rect.width_);
    sptr<OHOS::Surface> layer = GetLayer(surfaceNode);
    if (layer == nullptr) {
        WLOGFE("layer is nullptr");
        return false;
    }
    sptr<OHOS::SurfaceBuffer> buffer = GetSurfaceBuffer(layer, winWidth, winHeight);
    if (buffer == nullptr || buffer->GetVirAddr() == nullptr) {
        return false;
    }
    if (!DoDrawImageRect(buffer, rect, pixelMap, color, fillWindow)) {
        WLOGE("draw image rect failed.");
        return false;
    }
    OHOS::BufferFlushConfig flushConfig = {
        .damage = {
            .w = buffer->GetWidth(),
            .h = buffer->GetHeight(),
        },
    };
    OHOS::SurfaceError surfaceRet = layer->FlushBuffer(buffer, -1, flushConfig);
    if (surfaceRet != OHOS::SurfaceError::SURFACE_ERROR_OK) {
        WLOGFE("draw pointer FlushBuffer ret:%{public}s", SurfaceErrorStr(surfaceRet).c_str());
        return false;
    }
    return true;
}

bool SurfaceDraw::DoDrawImageRect(sptr<OHOS::SurfaceBuffer> buffer, const Rect& rect,
    std::shared_ptr<Media::PixelMap> pixelMap, uint32_t color, bool fillWindow)
{
#if 0
    int32_t winWidth = static_cast<int32_t>(rect.width_);
    int32_t winHeight = static_cast<int32_t>(rect.height_);
    // actual width of the surface buffer after alignment
    auto bufferStride = buffer->GetStride();
    int32_t alignWidth = bufferStride / static_cast<int32_t>(IMAGE_BYTES_STRIDE);
    WLOGFD("drawing image rect win width: %{public}d win height: %{public}d align width:%{public}d.",
        winWidth, winHeight, alignWidth);
    if (pixelMap == nullptr) {
        WLOGFE("drawing pixel map failed, because pixel map is nullptr.");
        return false;
    }
    if (pixelMap->GetHeight() <= 0 || pixelMap->GetWidth() <= 0 || winWidth <= 0 || winHeight <= 0) {
        WLOGFE("drawing pixel map failed, because width or height is invalid.");
        return false;
    }
    float xAxis = static_cast<float>(winWidth) / pixelMap->GetWidth();
    float yAxis = static_cast<float>(winHeight) / pixelMap->GetHeight();
    float axis = std::min(xAxis, yAxis);
    if (axis < 1.0) {
        // scale when the size of the pixel map is larger than the window
        // use axis to scale equally
        pixelMap->scale(axis, axis);
    } else if (fillWindow) {
        // scale snapshot to whole window
        pixelMap->scale(xAxis, yAxis);
    }
    int left = (winWidth - pixelMap->GetWidth()) / 2; // 2 is the left and right boundaries of the window
    int top = (winHeight - pixelMap->GetHeight()) / 2; // 2 is the top and bottom boundaries of the window
    WLOGFD("pixelMap width: %{public}d win height: %{public}d left:%{public}d top:%{public}d.",
        pixelMap->GetWidth(), pixelMap->GetHeight(), left, top);
    Drawing::Bitmap bitmap;
    Drawing::BitmapFormat format { Drawing::ColorType::COLORTYPE_RGBA_8888,
        Drawing::AlphaType::ALPHATYPE_OPAQUE };
    bitmap.Build(alignWidth, winHeight, format);
    Drawing::Canvas canvas;
    canvas.Bind(bitmap);
    canvas.Clear(color);
    canvas.DrawBitmap(*pixelMap, left, top);
    // bufferSize is actual size of the surface buffer after alignment
    int32_t bufferSize = bufferStride * winHeight;
    uint8_t* bitmapAddr = static_cast<uint8_t*>(bitmap.GetPixels());
    auto addr = static_cast<uint8_t *>(buffer->GetVirAddr());
    errno_t ret = memcpy_s(addr, bufferSize, bitmapAddr, bufferSize);
    if (ret != EOK) {
        WLOGFE("draw image rect failed, because copy bitmap to buffer failed.");
        return false;
    }
#endif
    return true;
}

bool SurfaceDraw::GetSurfaceSnapshot(const std::shared_ptr<RSSurfaceNode> surfaceNode,
    std::shared_ptr<Media::PixelMap>&pixelMap, int32_t timeoutMs, float scaleW, float scaleH)
{
    if (surfaceNode == nullptr) {
        WLOGFE("surfaceNode is null");
        return false;
    }
    std::shared_ptr<SurfaceCaptureFuture> callback = std::make_shared<SurfaceCaptureFuture>();
    if (RSInterfaces::GetInstance().TakeSurfaceCapture(surfaceNode, callback, scaleW, scaleH)) {
        pixelMap = callback->GetResult(timeoutMs); // get pixelmap time out ms
    }
    if (pixelMap == nullptr) {
        WLOGE("get surface snapshot timeout.");
        return false;
    }
    return true;
}

bool SurfaceDraw::DrawMasking(std::shared_ptr<RSSurfaceNode> surfaceNode, Rect screenRect,
    Rect transparentRect)
{
#if 0
    int32_t screenHeight = static_cast<int32_t>(screenRect.height_);
    int32_t screenWidth = static_cast<int32_t>(screenRect.width_);
    int32_t transparentHeight = static_cast<int32_t>(transparentRect.height_);
    int32_t transparentWidth = static_cast<int32_t>(transparentRect.width_);
    sptr<OHOS::Surface> layer = GetLayer(surfaceNode);
    if (layer == nullptr) {
        WLOGFE("layer is nullptr");
        return false;
    }
    sptr<OHOS::SurfaceBuffer> buffer = GetSurfaceBuffer(layer, screenWidth, screenHeight);
    if (buffer == nullptr || buffer->GetVirAddr() == nullptr) {
        return false;
    }
    auto addr = static_cast<uint8_t *>(buffer->GetVirAddr());
    Drawing::Bitmap fullbitmap;
    Drawing::BitmapFormat fullBitmapFormat { Drawing::ColorType::COLORTYPE_RGBA_8888,
        Drawing::AlphaType::ALPHATYPE_OPAQUE };
    fullbitmap.Build(screenWidth, screenHeight, fullBitmapFormat);
    Drawing::Canvas canvas;
    canvas.Bind(fullbitmap);
    canvas.Clear(0xFF000000);
    Drawing::Bitmap transBitmap;
    Drawing::BitmapFormat transBitmapFormat { Drawing::ColorType::COLORTYPE_RGBA_8888,
        Drawing::AlphaType::ALPHATYPE_OPAQUE };
    transBitmap.Build(transparentWidth, transparentHeight, transBitmapFormat);
    transBitmap.ClearWithColor(0);
    canvas.DrawBitmap(transBitmap, static_cast<Drawing::scalar>(transparentRect.posX_),
        static_cast<Drawing::scalar>(transparentRect.posY_));

    uint32_t addrSize = static_cast<uint32_t>(screenWidth * screenHeight * IMAGE_BYTES_STRIDE);
    errno_t ret = memcpy_s(addr, addrSize, fullbitmap.GetPixels(), addrSize);
    if (ret != EOK) {
        WLOGFE("draw failed");
        return false;
    }
    OHOS::BufferFlushConfig flushConfig = {
        .damage = {
            .w = buffer->GetWidth(),
            .h = buffer->GetHeight(),
        },
    };
    OHOS::SurfaceError surfaceRet = layer->FlushBuffer(buffer, -1, flushConfig);
    if (surfaceRet != OHOS::SurfaceError::SURFACE_ERROR_OK) {
        WLOGFE("draw masking FlushBuffer ret:%{public}s", SurfaceErrorStr(surfaceRet).c_str());
        return false;
    }
#endif
    return true;
}
} // Rosen
} // OHOS
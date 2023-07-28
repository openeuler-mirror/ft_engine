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

#include "pointer_draw_manager.h"

#include "unistd.h"
#include <sys/stat.h>

#include "ui/rs_surface_extractor.h"
#include "transaction/rs_transaction.h"
#include "transaction/rs_interfaces.h"
#include "image_source.h"
#include "display_manager_service_inner.h"


using namespace OHOS;

namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "PointerDrawingManager"};
    constexpr int32_t PATH_MAX = 200;
    const std::string POINTER_PIXEL_PATH = "/usr/local/share/ft/icon/";
    constexpr int32_t FILE_SIZE_MAX = 0X5000;
    constexpr int32_t ICON_WIDTH = 40;
    constexpr int32_t ICON_HEIGHT = 40;
    constexpr float PTR_SURFACE_NODE_Z_ORDER = 99999;
}

std::shared_ptr<IPointerDrawingManager> IPointerDrawingManager::GetInstance()
{
    static std::shared_ptr<IPointerDrawingManager> instance = std::make_shared<PointerDrawingManager>();
    return instance;
}

bool PointerDrawingManager::Init()
{
#ifndef USE_IMITATE_POINTER
    if (InitIconPixel() != WMError::WM_OK) {
        WLOGFE("InitIconPixel fail");
        return false;
    }
#endif

    runner_ = AppExecFwk::EventRunner::Create("PointerDrawingManager");
    handler_ = std::make_shared<AppExecFwk::EventHandler>(runner_);

    return true;
}

void PointerDrawingManager::DrawPointer(int32_t displayId, int32_t physicalX, int32_t physicalY,
    int32_t mouseStyle)
{
    if (!isDrawing_) {
        if (InitSurfaceNode(physicalX, physicalY) != WMError::WM_OK) {
            WLOGFE("init surface node fail");
            return;
        }
        Rosen::DisplayManagerServiceInner::GetInstance().UpdateRSTree(0, 0, surfaceNode_, true, false);
        isDrawing_ = true;
    }

    if (lastMouseStyle_ != mouseStyle) {
        if (DrawPointerByStyle(mouseStyle) != WMError::WM_OK) {
            WLOGFE("draw pointer by style fail");
            return;
        }
        lastMouseStyle_ = (MOUSE_ICON)mouseStyle;
    }

    if (handler_ == nullptr) {
        WLOGFE("EventHandler is nullptr");
        return;
    }

    std::function<void()> task = [this, physicalX, physicalY]() -> void {
        MoveTo(physicalX, physicalY);
    };

    bool ret = handler_->PostTask(task);
    if (ret == false) {
        WLOGFE("EventHandler PostTask Failed");
    }
}

void PointerDrawingManager::SetPointerLocation(int32_t pid, int32_t x, int32_t y)
{
    if (!isDrawing_) {
        WLOGFE("need to invoke DrawPointer before SetPointerLocation");
        return;
    }

    if (handler_ == nullptr) {
        WLOGFE("EventHandler is nullptr");
        return;
    }

    std::function<void()> task = [this, x, y]() -> void {
        MoveTo(x, y);
    };

    bool ret = handler_->PostTask(task);
    if (ret == false) {
        WLOGFE("EventHandler PostTask Failed");
    }
}

void PointerDrawingManager::UpdateDisplayInfo(const ScreenDisplayInfo& displayInfo)
{
    (void) displayInfo;
}

WMError PointerDrawingManager::MoveTo(int32_t x, int32_t y)
{
    if (surfaceNode_ == nullptr) {
        WLOGFE("surfaceNode_ is nullptr");
        return WMError::WM_ERROR_NULLPTR;
    }
    surfaceNode_->SetBounds(x, y, ICON_WIDTH, ICON_HEIGHT);
    Rosen::RSTransaction::FlushImplicitTransaction();

    return WMError::WM_OK;
}

WMError PointerDrawingManager::InitSurfaceNode(int32_t x, int32_t y)
{
    if (isDrawing_) {
        return WMError::WM_OK;  // surface node is inited, just return
    }

    Rosen::RSSurfaceNodeConfig config;
    surfaceNode_ = Rosen::RSSurfaceNode::Create(config);
    if (surfaceNode_ == nullptr) {
        WLOGFE("RSSurfaceNode::Create fail");
        return WMError::WM_ERROR_NULLPTR;
    }

    surfaceNode_->SetBounds(x, y, ICON_WIDTH, ICON_HEIGHT);
    surfaceNode_->SetPositionZ(PTR_SURFACE_NODE_Z_ORDER);
    rsSurface_ = Rosen::RSSurfaceExtractor::ExtractRSSurface(surfaceNode_);
    if (rsSurface_ == nullptr) {
        WLOGFE("ExtractRSSurface fail");
        return WMError::WM_ERROR_NULLPTR;
    }

#ifdef ENABLE_GPU
    renderContext_ = std::make_unique<Rosen::RenderContext>();
    if (renderContext_ != nullptr) {
        WLOGFD("create renderContext success");
        renderContext_->InitializeEglContext();
        rsSurface_->SetRenderContext(renderContext_.get());
    } else {
        WLOGFE("create renderContext fail");
    }
#endif
    return WMError::WM_OK;
}

WMError PointerDrawingManager::DrawPointerByStyle(int mouseStyle)
{
    if (rsSurface_ == nullptr) {
        return WMError::WM_ERROR_NULLPTR;
    }
    auto framePtr = rsSurface_->RequestFrame(ICON_WIDTH, ICON_HEIGHT);
    if (framePtr == nullptr) {
        WLOGFE("RequestFrame fail");
        return WMError::WM_ERROR_NULLPTR;
    }

    auto canvas = framePtr->GetCanvas();
    canvas->clear(SK_ColorTRANSPARENT);

#ifdef USE_IMITATE_POINTER
    SkPaint paint;
    paint.setAntiAlias(true);
    paint.setStyle(SkPaint::kFill_Style);
    paint.setStrokeJoin(SkPaint::kRound_Join);
    paint.setColor(SK_ColorBLUE);
    canvas->drawRect(SkRect::MakeXYWH(0, 0, ICON_WIDTH, ICON_HEIGHT), paint);
#else
    auto it = mouseIcons_.find(MOUSE_ICON(mouseStyle));
    if (it == mouseIcons_.end()) {
        WLOGFE("unsupport mouse style=%{public}d", mouseStyle);
        return WMError::WM_ERROR_INVALID_PARAM;
    }

    auto pixelmap = DecodeImageToPixelMap(mouseIcons_[MOUSE_ICON(mouseStyle)].iconPath);
    if (pixelmap == nullptr) {
        WLOGFE("DecodeImageToPixelMap fail");
        return WMError::WM_ERROR_NULLPTR;
    }

    SkImageInfo imageInfo = SkImageInfo::Make(pixelmap->GetWidth(), pixelmap->GetHeight(),
        PixelFormatConvert(pixelmap->GetPixelFormat()),
        static_cast<SkAlphaType>(pixelmap->GetAlphaType()));
    SkPixmap srcPixmap(imageInfo, pixelmap->GetPixels(), pixelmap->GetRowBytes());
    SkBitmap srcBitmap;
    srcBitmap.installPixels(srcPixmap);
    canvas->drawBitmap(srcBitmap, 0, 0);
#endif
    framePtr->SetDamageRegion(0, 0, ICON_WIDTH, ICON_HEIGHT);
    rsSurface_->FlushFrame(framePtr);
    return WMError::WM_OK;
}

WMError PointerDrawingManager::InitIconPixel()
{
#ifndef USE_IMITATE_POINTER
    mouseIcons_ = {
        {DEFAULT, {ANGLE_NW, POINTER_PIXEL_PATH + "Default.png"}},
        {EAST, {ANGLE_CENTER, POINTER_PIXEL_PATH + "East.png"}},
        {WEST, {ANGLE_CENTER, POINTER_PIXEL_PATH + "West.png"}},
        {SOUTH, {ANGLE_CENTER, POINTER_PIXEL_PATH + "South.png"}},
        {NORTH, {ANGLE_CENTER, POINTER_PIXEL_PATH + "North.png"}},
        {WEST_EAST, {ANGLE_CENTER, POINTER_PIXEL_PATH + "West_East.png"}},
        {NORTH_SOUTH, {ANGLE_CENTER, POINTER_PIXEL_PATH + "North_South.png"}},
        {NORTH_EAST, {ANGLE_CENTER, POINTER_PIXEL_PATH + "North_East.png"}},
        {NORTH_WEST, {ANGLE_CENTER, POINTER_PIXEL_PATH + "North_West.png"}},
        {SOUTH_EAST, {ANGLE_CENTER, POINTER_PIXEL_PATH + "South_East.png"}},
        {SOUTH_WEST, {ANGLE_CENTER, POINTER_PIXEL_PATH + "South_West.png"}},
        {NORTH_EAST_SOUTH_WEST, {ANGLE_CENTER, POINTER_PIXEL_PATH + "North_East_South_West.png"}},
        {NORTH_WEST_SOUTH_EAST, {ANGLE_CENTER, POINTER_PIXEL_PATH + "North_West_South_East.png"}},
        {CROSS, {ANGLE_CENTER, POINTER_PIXEL_PATH + "Cross.png"}},
        {CURSOR_COPY, {ANGLE_NW, POINTER_PIXEL_PATH + "Copy.png"}},
        {CURSOR_FORBID, {ANGLE_NW, POINTER_PIXEL_PATH + "Forbid.png"}},
        {COLOR_SUCKER, {ANGLE_SW, POINTER_PIXEL_PATH + "Colorsucker.png"}},
        {HAND_GRABBING, {ANGLE_CENTER, POINTER_PIXEL_PATH + "Hand_Grabbing.png"}},
        {HAND_OPEN, {ANGLE_CENTER, POINTER_PIXEL_PATH + "Hand_Open.png"}},
        {HAND_POINTING, {ANGLE_NW, POINTER_PIXEL_PATH + "Hand_Pointing.png"}},
        {HELP, {ANGLE_NW, POINTER_PIXEL_PATH + "Help.png"}},
        {CURSOR_MOVE, {ANGLE_CENTER, POINTER_PIXEL_PATH + "Move.png"}},
        {RESIZE_LEFT_RIGHT, {ANGLE_CENTER, POINTER_PIXEL_PATH + "Resize_Left_Right.png"}},
        {RESIZE_UP_DOWN, {ANGLE_CENTER, POINTER_PIXEL_PATH + "Resize_Up_Down.png"}},
        {SCREENSHOT_CHOOSE, {ANGLE_CENTER, POINTER_PIXEL_PATH + "Screenshot_Cross.png"}},
        {SCREENSHOT_CURSOR, {ANGLE_CENTER, POINTER_PIXEL_PATH + "Screenshot_Cursor.png"}},
        {TEXT_CURSOR, {ANGLE_CENTER, POINTER_PIXEL_PATH + "Text_Cursor.png"}},
        {ZOOM_IN, {ANGLE_CENTER, POINTER_PIXEL_PATH + "Zoom_In.png"}},
        {ZOOM_OUT, {ANGLE_CENTER, POINTER_PIXEL_PATH + "Zoom_Out.png"}},
        {MIDDLE_BTN_EAST, {ANGLE_CENTER, POINTER_PIXEL_PATH + "MID_Btn_East.png"}},
        {MIDDLE_BTN_WEST, {ANGLE_CENTER, POINTER_PIXEL_PATH + "MID_Btn_West.png"}},
        {MIDDLE_BTN_SOUTH, {ANGLE_CENTER, POINTER_PIXEL_PATH + "MID_Btn_South.png"}},
        {MIDDLE_BTN_NORTH, {ANGLE_CENTER, POINTER_PIXEL_PATH + "MID_Btn_North.png"}},
        {MIDDLE_BTN_NORTH_SOUTH, {ANGLE_CENTER, POINTER_PIXEL_PATH + "MID_Btn_North_South.png"}},
        {MIDDLE_BTN_NORTH_EAST, {ANGLE_CENTER, POINTER_PIXEL_PATH + "MID_Btn_North_East.png"}},
        {MIDDLE_BTN_NORTH_WEST, {ANGLE_CENTER, POINTER_PIXEL_PATH + "MID_Btn_North_West.png"}},
        {MIDDLE_BTN_SOUTH_EAST, {ANGLE_CENTER, POINTER_PIXEL_PATH + "MID_Btn_South_East.png"}},
        {MIDDLE_BTN_SOUTH_WEST, {ANGLE_CENTER, POINTER_PIXEL_PATH + "MID_Btn_South_West.png"}},
        {MIDDLE_BTN_NORTH_SOUTH_WEST_EAST, {ANGLE_CENTER, POINTER_PIXEL_PATH +
            "MID_Btn_North_South_West_East.png"}},
    };

    for (auto iter = mouseIcons_.begin(); iter != mouseIcons_.end();) {
        if (CheckPixelFile(iter->second.iconPath) != WMError::WM_OK) {
            iter = mouseIcons_.erase(iter);
            continue;
        }
        ++iter;
    }

    if (mouseIcons_.size() == 0) {
        WLOGFE("InitStyle fail");
        return WMError::WM_ERROR_NULLPTR;
    }
    return WMError::WM_OK;
#else
    return WMError::WM_OK;
#endif
}

WMError PointerDrawingManager::CheckPixelFile(const std::string &filePath)
{
    if (filePath.empty()) {
        WLOGFE("Empty path");
        return WMError::WM_ERROR_NULLPTR;
    }

    if (access(filePath.c_str(), F_OK) != 0) {
        WLOGFE("Unreachable path");
        return WMError::WM_ERROR_NULLPTR;
    }

    char realPath[PATH_MAX] = {};
    if (realpath(filePath.c_str(), realPath) == nullptr) {
        WLOGFE("Invalid path");
        return WMError::WM_ERROR_NULLPTR;
    }

    struct stat statbuf = {0};
    if (stat(filePath.c_str(), &statbuf) != 0) {
        WLOGFE("Failed to get size");
        return WMError::WM_ERROR_NULLPTR;
    }
    if (statbuf.st_size <= 0 || statbuf.st_size > FILE_SIZE_MAX) {
        WLOGFE("Invalid size");
        return WMError::WM_ERROR_NULLPTR;
    }

    return WMError::WM_OK;
}

std::unique_ptr<OHOS::Media::PixelMap> PointerDrawingManager::DecodeImageToPixelMap(const std::string &imagePath)
{
    OHOS::Media::SourceOptions opts;
    opts.formatHint = "image/png";
    uint32_t ret = 0;
    auto imageSource = OHOS::Media::ImageSource::CreateImageSource(imagePath, opts, ret);
    if (ret != 0) {
        WLOGFE("CreateImageSource fail");
        return nullptr;
    }

    std::set<std::string> formats;
    ret = imageSource->GetSupportedFormats(formats);
    if (ret != 0) {
        WLOGFE("GetSupportedFormats fail");
        return nullptr;
    }

    OHOS::Media::DecodeOptions decodeOpts;
    decodeOpts.desiredSize = {
        .width = ICON_WIDTH,
        .height = ICON_HEIGHT
    };
    std::unique_ptr<OHOS::Media::PixelMap> pixelMap = imageSource->CreatePixelMap(decodeOpts, ret);
    if (pixelMap == nullptr) {
        WLOGFE("CreatePixelMap fail");
        return nullptr;
    }
    return pixelMap;
}

SkColorType PointerDrawingManager::PixelFormatConvert(const Media::PixelFormat& pixelFormat)
{
    SkColorType colorType;
    switch (pixelFormat) {
        case Media::PixelFormat::BGRA_8888:
            colorType = SkColorType::kBGRA_8888_SkColorType;
            break;
        case Media::PixelFormat::RGBA_8888:
            colorType = SkColorType::kRGBA_8888_SkColorType;
            break;
        case Media::PixelFormat::RGB_565:
            colorType = SkColorType::kRGB_565_SkColorType;
            break;
        case Media::PixelFormat::ALPHA_8:
            colorType = SkColorType::kAlpha_8_SkColorType;
            break;
        default:
            colorType = SkColorType::kUnknown_SkColorType;
            break;
    }
    return colorType;
}

void PointerDrawingManager::GetScreenSize(int32_t &width, int32_t &height)
{
    int32_t displayId = Rosen::RSInterfaces::GetInstance().GetDefaultScreenId();
    auto activeModeInfo = Rosen::RSInterfaces::GetInstance().GetScreenActiveMode(displayId);
    width = activeModeInfo.GetScreenWidth();
    height = activeModeInfo.GetScreenHeight();
    WLOGFD("Screen size: Width=%{public}d, Height=%{public}d", width, height);
}
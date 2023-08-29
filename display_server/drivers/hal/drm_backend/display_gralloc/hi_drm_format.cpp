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

#include "hi_drm_format.h"

#include <drm_fourcc.h>
#include <string>

namespace DrmFormat {
/* Format Convert */
struct DrmFormatConvertTbl {
    uint32_t drmFormat;
    PixelFormat pixFormat;
};

struct ValueStrMap {
    uint32_t value;
    const char *str;
};

const char *GetHdiPixelFmtStr(PixelFormat format)
{
    static const ValueStrMap pixelStrMaps[] = {
        {PIXEL_FMT_CLUT8, "PIXEL_FMT_CLUT8"},
        {PIXEL_FMT_CLUT1, "PIXEL_FMT_CLUT1"},
        {PIXEL_FMT_CLUT4, "PIXEL_FMT_CLUT4"},
        {PIXEL_FMT_RGB_565, "PIXEL_FMT_RGB_565"},
        {PIXEL_FMT_RGBA_5658, "IXEL_FMT_RGBA_5658"},
        {PIXEL_FMT_RGBX_4444, "PIXEL_FMT_RGBX_4444"},
        {PIXEL_FMT_RGBA_4444, "PIXEL_FMT_RGBA_4444"},
        {PIXEL_FMT_RGB_444, "PIXEL_FMT_RGB_444"},
        {PIXEL_FMT_RGBX_5551, "PIXEL_FMT_RGBX_5551"},
        {PIXEL_FMT_RGBA_5551, "PIXEL_FMT_RGBA_5551"},
        {PIXEL_FMT_RGB_555, "PIXEL_FMT_RGB_555"},
        {PIXEL_FMT_RGBX_8888, "PIXEL_FMT_RGBX_8888"},
        {PIXEL_FMT_RGBA_8888, "PIXEL_FMT_RGBA_8888"},
        {PIXEL_FMT_RGB_888, "PIXEL_FMT_RGB_888"},
        {PIXEL_FMT_BGR_565, "PIXEL_FMT_BGR_565"},
        {PIXEL_FMT_BGRX_4444, "PIXEL_FMT_BGRX_4444"},
        {PIXEL_FMT_BGRA_4444, "PIXEL_FMT_BGRA_4444"},
        {PIXEL_FMT_BGRX_5551, "PIXEL_FMT_BGRX_5551"},
        {PIXEL_FMT_BGRA_5551, "PIXEL_FMT_BGRA_5551"},
        {PIXEL_FMT_BGRX_8888, "PIXEL_FMT_BGRX_8888"},
        {PIXEL_FMT_BGRA_8888, "PIXEL_FMT_BGRA_8888"},
        // {PIXEL_FMT_ARGB_8888, "PIXEL_FMT_ARGB_8888"},
        {PIXEL_FMT_YUV_422_I, "PIXEL_FMT_YUV_422_I"},
        {PIXEL_FMT_YUV_422_I, "PIXEL_FMT_YUV_422_I"},
        {PIXEL_FMT_YCBCR_422_SP, "PIXEL_FMT_YCBCR_422_SP"},
        {PIXEL_FMT_YCRCB_422_SP, "PIXEL_FMT_YCRCB_422_SP"},
        {PIXEL_FMT_YCBCR_420_SP, "PIXEL_FMT_YCBCR_420_SP"},
        {PIXEL_FMT_YCRCB_420_SP, "PIXEL_FMT_YCRCB_420_SP"},
        {PIXEL_FMT_YCBCR_422_P, "PIXEL_FMT_YCBCR_422_P"},
        {PIXEL_FMT_YCRCB_422_P, "PIXEL_FMT_YCRCB_422_P"},
        {PIXEL_FMT_YCBCR_420_P, "PIXEL_FMT_YCBCR_420_P"},
        {PIXEL_FMT_YCRCB_420_P, "PIXEL_FMT_YCRCB_420_P"},
        {PIXEL_FMT_YUYV_422_PKG, "PIXEL_FMT_YUYV_422_PKG"},
        {PIXEL_FMT_UYVY_422_PKG, "PIXEL_FMT_UYVY_422_PKG"},
        {PIXEL_FMT_YVYU_422_PKG, "PIXEL_FMT_YVYU_422_PKG"},
        {PIXEL_FMT_VYUY_422_PKG, "PIXEL_FMT_VYUY_422_PKG"},
        {PIXEL_FMT_BUTT, "PIXEL_FMT_BUTT"},
    };
    static const char *unknown = "unknown format";
    for (uint32_t i = 0; i < sizeof(pixelStrMaps) / sizeof(pixelStrMaps[0]); i++) {
        if (pixelStrMaps[i].value == format) {
            return pixelStrMaps[i].str;
        }
    }
    LOG_WARN("GetHdiPixelFmtStr unknown format: %{public}u", format);
    return unknown;
}

const char *GetDrmFmtStr(uint32_t format)
{
    static const ValueStrMap formatStrMaps[] = {
        {DRM_FORMAT_C8, "DRM_FORMAT_C8"},
        {DRM_FORMAT_R8, "DRM_FORMAT_R8"},
        {DRM_FORMAT_R16, "DRM_FORMAT_R16"},
        {DRM_FORMAT_RG88, "DRM_FORMAT_RG88"},
        {DRM_FORMAT_GR88, "DRM_FORMAT_GR88"},
        {DRM_FORMAT_RG1616, "DRM_FORMAT_RG1616"},
        {DRM_FORMAT_GR1616, "DRM_FORMAT_GR1616"},
        {DRM_FORMAT_RGB332, "DRM_FORMAT_RGB332"},
        {DRM_FORMAT_BGR233, "DRM_FORMAT_BGR233"},
        {DRM_FORMAT_XRGB4444, "DRM_FORMAT_XRGB4444"},
        {DRM_FORMAT_XBGR4444, "DRM_FORMAT_XBGR4444"},
        {DRM_FORMAT_RGBX4444, "DRM_FORMAT_RGBX4444"},
        {DRM_FORMAT_BGRX4444, "DRM_FORMAT_BGRX4444"},
        {DRM_FORMAT_ARGB4444, "DRM_FORMAT_ARGB4444"},
        {DRM_FORMAT_ABGR4444, "DRM_FORMAT_ABGR4444"},
        {DRM_FORMAT_RGBA4444, "DRM_FORMAT_RGBA4444"},
        {DRM_FORMAT_BGRA4444, "DRM_FORMAT_BGRA4444"},
        {DRM_FORMAT_XRGB1555, "DRM_FORMAT_XRGB1555"},
        {DRM_FORMAT_XBGR1555, "DRM_FORMAT_XBGR1555"},
        {DRM_FORMAT_RGBX5551, "DRM_FORMAT_RGBX5551"},
        {DRM_FORMAT_BGRX5551, "DRM_FORMAT_BGRX5551"},
        {DRM_FORMAT_ARGB1555, "DRM_FORMAT_ARGB1555"},
        {DRM_FORMAT_ABGR1555, "DRM_FORMAT_ABGR1555"},
        {DRM_FORMAT_RGBA5551, "DRM_FORMAT_RGBA5551"},
        {DRM_FORMAT_BGRA5551, "DRM_FORMAT_BGRA5551"},
        {DRM_FORMAT_RGB565, "DRM_FORMAT_RGB565"},
        {DRM_FORMAT_BGR565, "DRM_FORMAT_BGR565"},
        {DRM_FORMAT_RGB888, "DRM_FORMAT_RGB888"},
        {DRM_FORMAT_BGR888, "DRM_FORMAT_BGR888"},
        {DRM_FORMAT_XRGB8888, "DRM_FORMAT_XRGB8888"},
        {DRM_FORMAT_XBGR8888, "DRM_FORMAT_XBGR8888"},
        {DRM_FORMAT_RGBX8888, "DRM_FORMAT_RGBX8888"},
        {DRM_FORMAT_BGRX8888, "DRM_FORMAT_BGRX8888"},
        {DRM_FORMAT_ARGB8888, "DRM_FORMAT_ARGB8888"},
        {DRM_FORMAT_ABGR8888, "DRM_FORMAT_ABGR8888"},
        {DRM_FORMAT_RGBA8888, "DRM_FORMAT_RGBA8888"},
        {DRM_FORMAT_BGRA8888, "DRM_FORMAT_BGRA8888"},
        {DRM_FORMAT_XRGB2101010, "DRM_FORMAT_XRGB2101010"},
        {DRM_FORMAT_BGRX1010102, "DRM_FORMAT_BGRX1010102"},
        {DRM_FORMAT_ARGB2101010, "DRM_FORMAT_ARGB2101010"},
        {DRM_FORMAT_ABGR2101010, "DRM_FORMAT_ABGR2101010"},
        {DRM_FORMAT_RGBA1010102, "DRM_FORMAT_RGBA1010102"},
        {DRM_FORMAT_YVYU, "DRM_FORMAT_YVYU"},
        {DRM_FORMAT_UYVY, "DRM_FORMAT_UYVY"},
        {DRM_FORMAT_VYUY, "DRM_FORMAT_VYUY"},
        {DRM_FORMAT_AYUV, "DRM_FORMAT_AYUV"},
        {DRM_FORMAT_NV12, "DRM_FORMAT_NV12"},
        {DRM_FORMAT_NV21, "DRM_FORMAT_NV21"},
        {DRM_FORMAT_NV16, "DRM_FORMAT_NV16"},
        {DRM_FORMAT_NV61, "DRM_FORMAT_NV61"},
        {DRM_FORMAT_NV24, "DRM_FORMAT_NV24"},
        {DRM_FORMAT_NV42, "DRM_FORMAT_NV42"},
        {DRM_FORMAT_YUV410, "DRM_FORMAT_YUV410"},
        {DRM_FORMAT_YVU410, "DRM_FORMAT_YVU410"},
        {DRM_FORMAT_YUV411, "DRM_FORMAT_YUV411"},
        {DRM_FORMAT_YVU411, "DRM_FORMAT_YVU411"},
        {DRM_FORMAT_YUV420, "DRM_FORMAT_YUV420"},
        {DRM_FORMAT_YVU420, "DRM_FORMAT_YVU420"},
        {DRM_FORMAT_YUV422, "DRM_FORMAT_YUV422"},
        {DRM_FORMAT_YVU422, "DRM_FORMAT_YVU422"},
        {DRM_FORMAT_YUV444, "DRM_FORMAT_YUV444"},
        {DRM_FORMAT_YVU444, "DRM_FORMAT_YVU444"},
    };

    static const char *unknown = "unknown drm format";
    for (uint32_t i = 0; i < sizeof(formatStrMaps) / sizeof(formatStrMaps[0]); i++) {
        if (formatStrMaps[i].value == format) {
            return formatStrMaps[i].str;
        }
    }
    LOG_WARN("GetDrmFmtStr unknown format: %{public}u", format);
    return unknown;
}

static const DrmFormatConvertTbl convertTable[] = {
    {DRM_FORMAT_RGBX8888, PIXEL_FMT_RGBX_8888}, {DRM_FORMAT_RGBA8888, PIXEL_FMT_RGBA_8888},
    {DRM_FORMAT_RGB888, PIXEL_FMT_RGB_888},     {DRM_FORMAT_RGB565, PIXEL_FMT_BGR_565},
    {DRM_FORMAT_BGRX4444, PIXEL_FMT_BGRX_4444}, {DRM_FORMAT_BGRA4444, PIXEL_FMT_BGRA_4444},
    {DRM_FORMAT_RGBA4444, PIXEL_FMT_RGBA_4444}, {DRM_FORMAT_RGBX4444, PIXEL_FMT_RGBX_4444},
    {DRM_FORMAT_BGRX5551, PIXEL_FMT_BGRX_5551}, {DRM_FORMAT_BGRA5551, PIXEL_FMT_BGRA_5551},
    {DRM_FORMAT_BGRX8888, PIXEL_FMT_BGRX_8888}, {DRM_FORMAT_BGRA8888, PIXEL_FMT_BGRA_8888},
    {DRM_FORMAT_NV12, PIXEL_FMT_YCBCR_420_SP},  {DRM_FORMAT_NV21, PIXEL_FMT_YCRCB_420_SP},
    {DRM_FORMAT_YUV420, PIXEL_FMT_YCBCR_420_P}, {DRM_FORMAT_YVU420, PIXEL_FMT_YCRCB_420_P},
    {DRM_FORMAT_NV16, PIXEL_FMT_YCBCR_422_SP},  {DRM_FORMAT_NV61, PIXEL_FMT_YCRCB_422_SP},
    {DRM_FORMAT_YUV422, PIXEL_FMT_YCBCR_422_P}, {DRM_FORMAT_YVU422, PIXEL_FMT_YCRCB_422_P},
    // {DRM_FORMAT_ARGB8888, PIXEL_FMT_ARGB_8888},
};

uint32_t ConvertPixelFormatToDrmFormat(PixelFormat fmtIn)
{
    uint32_t fmtOut = DRM_FORMAT_INVALID;
    for (uint32_t i = 0; i < sizeof(convertTable) / sizeof(convertTable[0]); i++) {
        if (convertTable[i].pixFormat == fmtIn) {
            fmtOut = convertTable[i].drmFormat;
            break;
        }
    }
    // LOG_DEBUG << "Convert pixelFormat(" << fmtIn << " : " << GetHdiPixelFmtStr(fmtIn) << ") to DrmFormat(" << fmtOut
    //           << " : " << GetDrmFmtStr(fmtOut) << ")";
    // LOG_DEBUG << FT::Fmt(
    //     "Convert PixelFormat(%u:%s) to DrmFormat(%u:%s)",
    //     FT::ECast(fmtIn),
    //     GetHdiPixelFmtStr(fmtIn),
    //     fmtOut,
    //     GetDrmFmtStr(fmtOut));
    return fmtOut;
}

PixelFormat ConvertDrmFormatToPixelFormat(uint32_t fmtIn)
{
    PixelFormat fmtOut = PIXEL_FMT_BUTT;
    for (uint32_t i = 0; i < sizeof(convertTable) / sizeof(convertTable[0]); i++) {
        if (convertTable[i].drmFormat == fmtIn) {
            fmtOut = convertTable[i].pixFormat;
            break;
        }
    }
    // LOG_DEBUG << FT::Fmt(
    //     "Convert DrmFormat(%u:%s) to PixelFormat(%u:%s)",
    //     fmtIn,
    //     GetDrmFmtStr(fmtIn),
    //     FT::ECast(fmtOut),
    //     GetHdiPixelFmtStr(fmtOut));
    return fmtOut;
}

/* Format Info */

static const PlaneLayoutInfo g_yuv420SPLayout = {
    .numPlanes = 2,
    .radio = {4, 2},
};

static const PlaneLayoutInfo g_yuv420PLayout = {
    .numPlanes = 3,
    .radio = {4, 1, 1},
};

static const PlaneLayoutInfo g_yuv422SPLayout = {
    .numPlanes = 2,
    .radio = {4, 4},
};

static const PlaneLayoutInfo g_yuv422PLayout = {
    .numPlanes = 3,
    .radio = {4, 2, 2},
};

static const DrmFormatInfo fmtInfos[] = {
    {DRM_FORMAT_RGBX8888, 32, nullptr},       {DRM_FORMAT_RGBA8888, 32, nullptr},
    {DRM_FORMAT_BGRX8888, 32, nullptr},       {DRM_FORMAT_BGRA8888, 32, nullptr},
    {DRM_FORMAT_RGB888, 24, nullptr},         {DRM_FORMAT_RGB565, 16, nullptr},
    {DRM_FORMAT_BGRX4444, 16, nullptr},       {DRM_FORMAT_BGRA4444, 16, nullptr},
    {DRM_FORMAT_RGBA4444, 16, nullptr},       {DRM_FORMAT_RGBX4444, 16, nullptr},
    {DRM_FORMAT_BGRX5551, 16, nullptr},       {DRM_FORMAT_BGRA5551, 16, nullptr},
    {DRM_FORMAT_NV12, 8, &g_yuv420SPLayout},  {DRM_FORMAT_NV21, 8, &g_yuv420SPLayout},
    {DRM_FORMAT_NV16, 8, &g_yuv422SPLayout},  {DRM_FORMAT_NV61, 8, &g_yuv422SPLayout},
    {DRM_FORMAT_YUV420, 8, &g_yuv420PLayout}, {DRM_FORMAT_YVU420, 8, &g_yuv420PLayout},
    {DRM_FORMAT_YUV422, 8, &g_yuv422PLayout}, {DRM_FORMAT_YVU422, 8, &g_yuv422PLayout},
};

const DrmFormatInfo *GetDrmFormatInfo(uint32_t format)
{
    constexpr size_t fmtInfosSize = sizeof(fmtInfos) / sizeof(DrmFormatInfo);

    for (uint32_t i = 0; i < fmtInfosSize; i++) {
        if (fmtInfos[i].format == format) {
            return &fmtInfos[i];
        }
    }
    LOG_WARN("Failed to find format: %{public}u", format);
    return nullptr;
}
} // namespace DrmFormat

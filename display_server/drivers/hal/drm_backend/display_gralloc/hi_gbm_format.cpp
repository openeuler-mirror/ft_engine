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

#include "hi_gbm_format.h"

#include "hi_drm_format.h"
#include "gbm.h"

namespace GbmFormat {

struct PixelFormatConvertTbl {
    uint32_t drmFormat;
    PixelFormat pixFormat;
};

struct ValueStrMap {
    uint32_t value;
    const char *str;
};

const char *GetDrmFmtStr(uint32_t format)
{
    static const ValueStrMap formatStrMaps[] = {
        {GBM_FORMAT_RGBX8888, "GBM_FORMAT_RGBX8888"}, {GBM_FORMAT_RGBA8888, "GBM_FORMAT_RGBA8888"},
        {GBM_FORMAT_RGB888, "GBM_FORMAT_RGB888"},     {GBM_FORMAT_BGR565, "GBM_FORMAT_BGR565"},
        {GBM_FORMAT_BGRX4444, "GBM_FORMAT_BGRX4444"}, {GBM_FORMAT_BGRA4444, "GBM_FORMAT_BGRA4444"},
        {GBM_FORMAT_RGBA4444, "GBM_FORMAT_RGBA4444"}, {GBM_FORMAT_RGBX4444, "GBM_FORMAT_RGBX4444"},
        {GBM_FORMAT_BGRX5551, "GBM_FORMAT_BGRX5551"}, {GBM_FORMAT_BGRA5551, "GBM_FORMAT_BGRA5551"},
        {GBM_FORMAT_BGRX8888, "GBM_FORMAT_BGRX8888"}, {GBM_FORMAT_BGRA8888, "GBM_FORMAT_BGRA8888"},
        {GBM_FORMAT_NV12, "GBM_FORMAT_NV12"},         {GBM_FORMAT_NV21, "GBM_FORMAT_NV21"},
        {GBM_FORMAT_YUV420, "GBM_FORMAT_YUV420"},     {GBM_FORMAT_YVU420, "GBM_FORMAT_YVU420"},
        {GBM_FORMAT_NV16, "GBM_FORMAT_NV16"},         {GBM_FORMAT_NV61, "GBM_FORMAT_NV61"},
        {GBM_FORMAT_YUV422, "GBM_FORMAT_YUV422"},     {GBM_FORMAT_YVU422, "GBM_FORMAT_YVU422"},
    };

    for (uint32_t i = 0; i < sizeof(formatStrMaps) / sizeof(formatStrMaps[0]); i++) {
        if (formatStrMaps[i].value == format) {
            return formatStrMaps[i].str;
        }
    }
    LOG_WARN("GetGbmFmtStr unknown format: %{public}u", format);
    return "unknown gbm format";
}

uint32_t ConvertPixelFormatToGbmFormat(PixelFormat fmtIn)
{
    static const PixelFormatConvertTbl convertTable[] = {
        {GBM_FORMAT_RGBX8888, PIXEL_FMT_RGBX_8888}, {GBM_FORMAT_RGBA8888, PIXEL_FMT_RGBA_8888},
        {GBM_FORMAT_RGB888, PIXEL_FMT_RGB_888},     {GBM_FORMAT_BGR565, PIXEL_FMT_BGR_565},
        {GBM_FORMAT_BGRX4444, PIXEL_FMT_BGRX_4444}, {GBM_FORMAT_BGRA4444, PIXEL_FMT_BGRA_4444},
        {GBM_FORMAT_RGBA4444, PIXEL_FMT_RGBA_4444}, {GBM_FORMAT_RGBX4444, PIXEL_FMT_RGBX_4444},
        {GBM_FORMAT_BGRX5551, PIXEL_FMT_BGRX_5551}, {GBM_FORMAT_BGRA5551, PIXEL_FMT_BGRA_5551},
        {GBM_FORMAT_BGRX8888, PIXEL_FMT_BGRX_8888}, {GBM_FORMAT_BGRA8888, PIXEL_FMT_BGRA_8888},
        {GBM_FORMAT_NV12, PIXEL_FMT_YCBCR_420_SP},  {GBM_FORMAT_NV21, PIXEL_FMT_YCRCB_420_SP},
        {GBM_FORMAT_YUV420, PIXEL_FMT_YCBCR_420_P}, {GBM_FORMAT_YVU420, PIXEL_FMT_YCRCB_420_P},
        {GBM_FORMAT_NV16, PIXEL_FMT_YCBCR_422_SP},  {GBM_FORMAT_NV61, PIXEL_FMT_YCRCB_422_SP},
        {GBM_FORMAT_YUV422, PIXEL_FMT_YCBCR_422_P}, {GBM_FORMAT_YVU422, PIXEL_FMT_YCRCB_422_P},
    };
    uint32_t fmtOut = 0;
    for (uint32_t i = 0; i < sizeof(convertTable) / sizeof(convertTable[0]); i++) {
        if (convertTable[i].pixFormat == fmtIn) {
            fmtOut = convertTable[i].drmFormat;
        }
    }
    // LOG_DEBUG << "Convert pixelFormat(" << fmtIn << " : " << DrmFormat::GetHdiPixelFmtStr(fmtIn) << ") to DrmFormat("
    //           << fmtOut << " : " << GetDrmFmtStr(fmtOut) << ")";
    return fmtOut;
}
} // namespace GbmFormat

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

#include "surface_reader.h"
#include "window_manager_hilog.h"
#include "unique_fd.h"

#include <securec.h>

using namespace OHOS::Media;

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_DISPLAY, "SurfaceReader"};
} // namespace
const int BPP = 4; // bytes per pixel

SurfaceReader::SurfaceReader()
{
}

SurfaceReader::~SurfaceReader()
{
    if (csurface_ != nullptr) {
        csurface_->UnregisterConsumerListener();
    }
    psurface_ = nullptr;
    csurface_ = nullptr;
}

bool SurfaceReader::Init()
{
    csurface_ = Surface::CreateSurfaceAsConsumer();
    if (csurface_ == nullptr) {
        return false;
    }

    auto producer = csurface_->GetProducer();
    psurface_ = Surface::CreateSurfaceAsProducer(producer);
    if (psurface_ == nullptr) {
        return false;
    }

    listener_ = new BufferListener(*this);
    SurfaceError ret = csurface_->RegisterConsumerListener(listener_);
    if (ret != SURFACE_ERROR_OK) {
        return false;
    }
    return true;
}

void SurfaceReader::OnVsync()
{
    WLOGFI("SurfaceReader::OnVsync");

    sptr<SurfaceBuffer> cbuffer = nullptr;
    int32_t fence = -1;
    int64_t timestamp = 0;
    Rect damage;
    auto sret = csurface_->AcquireBuffer(cbuffer, fence, timestamp, damage);
    if (cbuffer == nullptr || sret != OHOS::SURFACE_ERROR_OK) {
        WLOGFE("SurfaceReader::OnVsync: surface buffer is null");
        return;
    }

    if (!ProcessBuffer(cbuffer)) {
        WLOGFE("SurfaceReader::OnVsync: ProcessBuffer failed");
        return;
    }

    if (cbuffer != prevBuffer_) {
        if (prevBuffer_ != nullptr) {
            SurfaceError ret = csurface_->ReleaseBuffer(prevBuffer_, -1);
            if (ret != SURFACE_ERROR_OK) {
                WLOGFE("SurfaceReader::OnVsync: release buffer error");
                return;
            }
        }

        prevBuffer_ = cbuffer;
    }
}

sptr<Surface> SurfaceReader::GetSurface() const
{
    return psurface_;
}

void SurfaceReader::SetHandler(sptr<SurfaceReaderHandler> handler)
{
    handler_ = handler;
}

bool SurfaceReader::ProcessBuffer(const sptr<SurfaceBuffer> &buf)
{
    if (handler_ == nullptr) {
        WLOGFE("SurfaceReaderHandler not set");
        return false;
    }

    BufferHandle *bufferHandle =  buf->GetBufferHandle();
    if (bufferHandle == nullptr) {
        WLOGFE("bufferHandle nullptr");
        return false;
    }

    uint32_t width = static_cast<uint32_t>(bufferHandle->width);
    uint32_t height = static_cast<uint32_t>(bufferHandle->height);
    uint32_t stride = static_cast<uint32_t>(bufferHandle->stride);
    uint8_t *addr = (uint8_t *)buf->GetVirAddr();

    auto data = (uint8_t *)malloc(width * height * BPP);
    if (data == nullptr) {
        WLOGFE("data malloc failed");
        return false;
    }
    for (uint32_t i = 0; i < height; i++) {
        errno_t ret = memcpy_s(data + width * i * BPP,  width * BPP, addr + stride * i, width * BPP);
        if (ret != EOK) {
            WLOGFE("memcpy failed");
            free(data);
            return false;
        }
    }

    sptr<PixelMap> pixelMap = new(std::nothrow) PixelMap();
    if (pixelMap == nullptr) {
        WLOGFE("create pixelMap failed");
        free(data);
        return false;
    }

    ImageInfo info;
    info.size.width = static_cast<int32_t>(width);
    info.size.height = static_cast<int32_t>(height);
    info.pixelFormat = OHOS::Media::PixelFormat::RGBA_8888;
    info.colorSpace = ColorSpace::SRGB;
    pixelMap->SetImageInfo(info);

    pixelMap->SetPixelsAddr(data, nullptr, width * height, AllocatorType::HEAP_ALLOC, nullptr);

    handler_->OnImageAvailable(pixelMap);
    return true;
}
}
}

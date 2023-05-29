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

#include "display_test_utils.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_DISPLAY, "DisplayTestUtils"};
}

DisplayTestUtils::~DisplayTestUtils()
{
    if (csurface_ != nullptr) {
        if (prevBuffer_ != nullptr) {
            SurfaceError ret = csurface_->ReleaseBuffer(prevBuffer_, -1);
            if (ret != SURFACE_ERROR_OK) {
                WLOGFE("buffer release failed");
                return;
            }
            WLOGFI("prevBuffer_ release success");
        }
        csurface_->UnregisterConsumerListener();
    }

    csurface_ = nullptr;
    psurface_ = nullptr;
    listener_ = nullptr;
    prevBuffer_ = nullptr;
    bufferHandle_ = nullptr;
}

bool DisplayTestUtils::SizeEqualToDisplay(const sptr<Display>& display, const Media::Size cur)
{
    int32_t dWidth = display->GetWidth();
    int32_t dHeight = display->GetHeight();

    bool res = ((cur.width == dWidth) && (cur.height == dHeight));
    if (!res) {
        WLOGFE("DisplaySize: %d %d, CurrentSize: %d %d", dWidth, dHeight, cur.width, cur.height);
    }
    return res;
}

bool DisplayTestUtils::SizeEqual(const Media::Size dst, const Media::Size cur)
{
    bool res = ((cur.width == dst.width) && (cur.height == dst.height));
    if (!res) {
        WLOGFE("Desired Size: %d %d, Current Size: %d %d", dst.width, dst.height, cur.width, cur.height);
    }
    return res;
}

bool DisplayTestUtils::CreateSurface()
{
    csurface_ = Surface::CreateSurfaceAsConsumer();
    if (csurface_ == nullptr) {
        WLOGFE("csurface create failed");
        return false;
    }

    auto producer = csurface_->GetProducer();
    psurface_ = Surface::CreateSurfaceAsProducer(producer);
    if (psurface_ == nullptr) {
        WLOGFE("csurface create failed");
        return false;
    }

    listener_ = new BufferListener(*this);
    SurfaceError ret = csurface_->RegisterConsumerListener(listener_);
    if (ret != SURFACE_ERROR_OK) {
        WLOGFE("listener register failed");
        return false;
    }
    return true;
}

void DisplayTestUtils::OnVsync()
{
    std::lock_guard<std::mutex> lock(mutex_);
    WLOGFI("DisplayTestUtils::OnVsync");
    sptr<SurfaceBuffer> cbuffer = nullptr;
    int32_t fence = -1;
    int64_t timestamp = 0;
    OHOS::Rect damage;
    if (csurface_ == nullptr) {
        WLOGFE("csurface_ is null");
        return;
    }
    auto sret = csurface_->AcquireBuffer(cbuffer, fence, timestamp, damage);
    UniqueFd fenceFd(fence);
    if (cbuffer == nullptr || sret != OHOS::SURFACE_ERROR_OK) {
        WLOGFE("acquire buffer failed");
        return;
    }
    bufferHandle_ = cbuffer->GetBufferHandle();
    if (bufferHandle_ == nullptr) {
        WLOGFE("get bufferHandle failed");
        return;
    }
    if (defaultWidth_ == static_cast<uint32_t>(bufferHandle_->width) &&
        defaultHeight_ == static_cast<uint32_t>(bufferHandle_->height)) {
        successCount_++;
        WLOGFI("compareWH is successful in onVsync: %d", successCount_);
    } else {
        failCount_++;
    }
    if (cbuffer != prevBuffer_) {
        if (prevBuffer_ != nullptr) {
            SurfaceError ret = csurface_->ReleaseBuffer(prevBuffer_, -1);
            if (ret != SURFACE_ERROR_OK) {
                WLOGFE("buffer release failed");
                return;
            }
        }
        prevBuffer_ = cbuffer;
    }
}

void DisplayTestUtils::SetDefaultWH(const sptr<Display>& display)
{
    defaultWidth_ = display->GetWidth();
    defaultHeight_ = display->GetHeight();
}
} // namespace ROSEN
} // namespace OHOS
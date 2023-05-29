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
#include "surface_reader_handler_impl.h"
#include "window_manager_hilog.h"

namespace OHOS::Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_DISPLAY, "SurfaceReaderHandlerImpl"};
} // namespace
bool SurfaceReaderHandlerImpl::OnImageAvailable(sptr<Media::PixelMap> pixelMap)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (!flag_) {
        flag_ = true;
        pixelMap_ = pixelMap;
        WLOGFI("Get an Image!");
    }
    return true;
}

bool SurfaceReaderHandlerImpl::IsImageOk()
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    return flag_;
}

void SurfaceReaderHandlerImpl::ResetFlag()
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (flag_) {
        flag_ = false;
    }
}

sptr<Media::PixelMap> SurfaceReaderHandlerImpl::GetPixelMap()
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    return pixelMap_;
}
} // namespace OHOS::Rosen
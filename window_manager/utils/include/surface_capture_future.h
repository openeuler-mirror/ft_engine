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

#ifndef SURFACE_CAPTURE_FUTURE_H
#define SURFACE_CAPTURE_FUTURE_H

#include "future.h"
#include "pixel_map.h"
#include "transaction/rs_render_service_client.h"

namespace OHOS {
namespace Rosen {
class SurfaceCaptureFuture : public SurfaceCaptureCallback, public Future<std::shared_ptr<Media::PixelMap>> {
public:
    SurfaceCaptureFuture() = default;
    ~SurfaceCaptureFuture() {};
    void OnSurfaceCapture(std::shared_ptr<Media::PixelMap> pixelmap) override
    {
        FutureCall(pixelmap);
    }
protected:
    void Call(std::shared_ptr<Media::PixelMap> pixelmap) override
    {
        if (!flag_) {
            pixelMap_ = pixelmap;
            flag_ = true;
        }
    }
    bool IsReady() override
    {
        return flag_;
    }
    std::shared_ptr<Media::PixelMap> FetchResult() override
    {
        return pixelMap_;
    }
private:
    bool flag_ = false;
    std::shared_ptr<Media::PixelMap> pixelMap_ = nullptr;
};
} // Rosen
} // OHOS
#endif  // SURFACE_CAPTURE_FUTURE_H
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

#ifndef SURFACE_READER_HANDLER_IMPL_H
#define SURFACE_READER_HANDLER_IMPL_H

#include <mutex>
#include <pixel_map.h>
#include <refbase.h>
#include "surface_reader_handler.h"

namespace OHOS::Rosen {
class SurfaceReaderHandlerImpl : public SurfaceReaderHandler {
public:
    bool OnImageAvailable(sptr<Media::PixelMap> pixelMap) override;
    bool IsImageOk();
    void ResetFlag();
    sptr<Media::PixelMap> GetPixelMap();

private:
    bool flag_ = false;
    sptr<Media::PixelMap> pixelMap_ = nullptr;
    std::recursive_mutex mutex_;
};
} // namespace OHOS::Rosen

#endif // SURFACE_READER_HANDLER_IMPL_H

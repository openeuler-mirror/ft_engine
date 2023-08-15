/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef INTERFACES_INNERKITS_SURFACE_SURFACE_UTILS_H
#define INTERFACES_INNERKITS_SURFACE_SURFACE_UTILS_H

#include <unordered_map>
#include <mutex>
#include "surface.h"

namespace OHOS {
class SurfaceUtils {
public:
    static SurfaceUtils* GetInstance();

    // get surface by uniqueId.
    sptr<Surface> GetSurface(uint64_t uniqueId);
    // maintenance map with uniqueId and surface.
    SurfaceError Add(uint64_t uniqueId, const sptr<Surface> &surface);
    // remove surface by uniqueId.
    SurfaceError Remove(uint64_t uniqueId);

private:
    SurfaceUtils() = default;
    virtual ~SurfaceUtils();

    std::unordered_map<uint64_t, sptr<Surface>> surfaceCache_;
    std::mutex mutex_;
};
} // namespace OHOS

#endif // INTERFACES_INNERKITS_SURFACE_SURFACE_UTILS_H

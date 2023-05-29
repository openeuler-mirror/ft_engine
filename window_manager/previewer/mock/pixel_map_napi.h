/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

#ifndef INTERFACES_KITS_JS_COMMON_INCLUDE_PIXEL_MAP_NAPI_H_
#define INTERFACES_KITS_JS_COMMON_INCLUDE_PIXEL_MAP_NAPI_H_

#include "pixel_map.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"

namespace OHOS {
namespace Media {
class PixelMapNapi {
public:
    PixelMapNapi();
    ~PixelMapNapi();

    static napi_value CreatePixelMap(napi_env env, std::shared_ptr<PixelMap> pixelmap);
    napi_env getEnv();
    napi_ref getWrapper();
private:
    napi_env env_ = nullptr;
    napi_ref wrapper_ = nullptr;
};
} // namespace Media
} // namespace OHOS
#endif // INTERFACES_KITS_JS_COMMON_INCLUDE_PIXEL_MAP_NAPI_H_

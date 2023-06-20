/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "allocator_adapter.h"

namespace OHOS {
namespace HDI {
namespace Display {
namespace V1_0 {
AllocatorAdapter::AllocatorAdapter()
{
    if (GrallocInitialize(&grallocFuncs_) != HDF_SUCCESS) {
        LOG_ERROR("%{public}s: grallocFuncs_ init failed", __func__);
    }
}

AllocatorAdapter::~AllocatorAdapter()
{
    if (GrallocUninitialize(grallocFuncs_) != HDF_SUCCESS) {
        LOG_ERROR("%{public}s: grallocFuncs_ uninit failed", __func__);
    }
}

int32_t AllocatorAdapter::AllocMem(const AllocInfo &info, BufferHandle *&handle)
{
    if (grallocFuncs_ == nullptr) {
        LOG_ERROR("%{public}s: grallocFuncs_ is nullptr", __func__);
        return HDF_FAILURE;
    }
    return grallocFuncs_->AllocMem(&info, &handle);
}

sptr<IRemoteObject> AllocatorAdapter::AsObject()
{
    return nullptr;
}

} // namespace V1_0
} // namespace Display
} // namespace HDI
} // namespace OHOS
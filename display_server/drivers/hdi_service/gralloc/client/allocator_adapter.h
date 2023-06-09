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

#ifndef HDI_ALLOCATOR_ADAPTER_V1_0_H
#define HDI_ALLOCATOR_ADAPTER_V1_0_H

#include "display_gralloc.h"
#include "display_type.h"
#include "idisplay_allocator.h"

namespace OHOS {
namespace HDI {
namespace Display {
namespace V1_0 {
class AllocatorAdapter : public IDisplayAllocator {
public:
    AllocatorAdapter();
    virtual ~AllocatorAdapter();
    virtual int32_t AllocMem(const AllocInfo &info, BufferHandle *&handle) override;
    virtual sptr<IRemoteObject> AsObject() override;

private:
    GrallocFuncs *grallocFuncs_ = nullptr;
};
} // namespace V1_0
} // namespace Display
} // namespace HDI
} // namespace OHOS

#endif // HDI_MAPPER_ADAPTER_V1_0_H

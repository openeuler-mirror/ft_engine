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

#include "allocator_controller.h"

#include "gbm_allocator.h"
#include "dumb_allocator.h"
#include "shm_allocator.h"
#include "display_type.h"
#include "log.h"

namespace oewm {
namespace HDI {
namespace DISPLAY {

int32_t AllocatorController::Init()
{
    LOG_DEBUG("[Gralloc::AllocatorController::Init] Init.");

    gbmAllocator_.reset(new GbmAllocator());
    gbmAllocator_->Init();
    dumbAllocator_.reset(new DumbAllocator());
    dumbAllocator_->Init();
    shmAllocator_.reset(new ShmAllocator());
    shmAllocator_->Init();

    isEnabled_ = true;
    return DISPLAY_SUCCESS;
}

int32_t AllocatorController::Uninit()
{
    LOG_DEBUG("[Gralloc::AllocatorController::Uninit] Uninit.");

    gbmAllocator_.reset();
    dumbAllocator_.reset();
    shmAllocator_.reset();

    isEnabled_ = false;
    return DISPLAY_SUCCESS;
}

std::shared_ptr<Allocator> AllocatorController::GetAllocator(uint64_t usage)
{
    // If user need DMA buffer, we could alloc buffer by:
    // * DRM: Dumb Buffer for CPU Rendering (when usage include `CPU_WRITE`)
    // * GBM: Smart Buffer for GPU Rendering (other case)
    if (usage & HBM_USE_MEM_DMA) {
        if (usage & HBM_USE_CPU_WRITE) {
            LOG_DEBUG("[Gralloc::AllocatorController::GetAllocator] Choose Dumb Allocator.");
            return dumbAllocator_;
        } else {
#ifdef DRM_BACKEND_USE_GBM
            LOG_DEBUG("[Gralloc::AllocatorController::GetAllocator] Choose GBM Allocator.");
            return gbmAllocator_;
#else
            LOG_ERROR("[Gralloc::AllocatorController::GetAllocator] DRM Backend not support GBM.");
            return nullptr;
#endif // DRM_BACKEND_USE_GBM
        }
    }

    // If user need CPU buffer for CPU Rendering, we will provide shared memory.
    if (!(usage & HBM_USE_MEM_DMA) && (usage & HBM_USE_CPU_WRITE)) {
        return shmAllocator_;
    }

    LOG_ERROR("[Gralloc::AllocatorController::GetAllocator] This usage is not supported: %{public}" PRIu64, usage);
    return nullptr;
}

} // namespace DISPLAY
} // namespace HDI
} // namespace oewm
/*
 * Copyright (c) 2022 Huawei Technologies Co., Ltd.
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

// Tips:
// Current Gralloc Module only support usage below:
// * "HBM_USE_MEM_DMA": if you try to allocate DMABUF, it'll provide GBM for GPU Rendering and provide DRM Dumb Buffer
//                    for CPU Rendering.
// * "HBM_USE_CPU_WRITE & !HBM_USE_MEM_DMA": if you try to allocate a buffer which will be rendered by CPU, it can't be
//                                         a DMABUF.
// * Other: not support yet.
//
// Recommanded usage:
// * GPU Rendering: "HBM_USE_MEM_DMA" + "HBM_USE_MEM_FB"
// * CPU Rendering: "HBM_USE_CPU_READ" + "HBM_USE_CPU_WRITE" + "HBM_USE_MEM_DMA" + "HBM_USE_MEM_FB"

#include "display_gralloc.h"

#include "allocator_controller.h"
#include "display_type.h"
#include "log.h"

using namespace oewm::HDI::DISPLAY;

int32_t AllocMem(const AllocInfo *info, BufferHandle **handle)
{
    /**
     * the memory should complete some important tasks below:
     * 1. create buffer depends on buffer usage & local env
     * 2. get buffer fd for interprocess transfering
     */

    if (info == nullptr) {
        LOG_ERROR("Get nullptr param: `info`");
        return DISPLAY_PARAM_ERR;
    }

    if (handle == nullptr) {
        LOG_ERROR("Get nullptr param: `handle`");
        return DISPLAY_PARAM_ERR;
    }

    auto allocator = AllocatorController::GetInstance().GetAllocator(info->usage);
    if (allocator == nullptr) {
        return DISPLAY_NULL_PTR;
    }

    return allocator->AllocMem(*info, handle);
}

void FreeMem(BufferHandle *handle)
{
    if (handle == nullptr) {
        LOG_ERROR("[Gralloc::FreeMem] Get nullptr param: `handle`");
        return;
    }

    auto allocator = AllocatorController::GetInstance().GetAllocator(handle->usage);
    if (allocator == nullptr) {
        return;
    }

    allocator->FreeMem(handle);
}

void *Mmap(BufferHandle *handle)
{
    if (handle == nullptr) {
        LOG_ERROR("[Gralloc::Mmap] Get nullptr param: `handle`");
        return nullptr;
    }

    auto allocator = AllocatorController::GetInstance().GetAllocator(handle->usage);
    if (allocator == nullptr) {
        return nullptr;
    }

    return allocator->Mmap(*handle);
}

int32_t Unmap(BufferHandle *handle)
{
    if (handle == nullptr) {
        LOG_ERROR("[Gralloc::Unmap] Get nullptr param: `handle`");
        return DISPLAY_PARAM_ERR;
    }

    auto allocator = AllocatorController::GetInstance().GetAllocator(handle->usage);
    if (allocator == nullptr) {
        return DISPLAY_NULL_PTR;
    }
        
    return allocator->InvalidateCache(*handle);
}

int32_t FlushCache(BufferHandle *handle)
{
    if (handle == nullptr) {
        LOG_ERROR("[Gralloc::FlushCache] Get nullptr param: `handle`");
        return DISPLAY_PARAM_ERR;
    }

    auto allocator = AllocatorController::GetInstance().GetAllocator(handle->usage);
    if (allocator == nullptr) {
        return DISPLAY_NULL_PTR;
    }

    return allocator->FlushCache(*handle);
}

int32_t InvalidateCache(BufferHandle *handle)
{
    if (handle == nullptr) {
        LOG_ERROR("[Gralloc::InvalidateCache] Get nullptr param: `handle`");
        return DISPLAY_PARAM_ERR;
    }

    auto allocator = AllocatorController::GetInstance().GetAllocator(handle->usage);
    if (allocator == nullptr) {
        return DISPLAY_NULL_PTR;
    } else {
        return allocator->InvalidateCache(*handle);
    }
}

int32_t GrallocUninitialize(GrallocFuncs *funcs)
{
    if (funcs == nullptr) {
        LOG_ERROR("[Gralloc::GrallocUninitialize] Get nullptr param: `funcs`");
        return DISPLAY_PARAM_ERR;
    }

    delete funcs;

    return AllocatorController::GetInstance().Uninit();
}

int32_t GrallocInitialize(GrallocFuncs **funcs)
{
    if (funcs == nullptr) {
        LOG_ERROR("[Gralloc::GrallocInitialize] Get nullptr param: `funcs`");
        return DISPLAY_PARAM_ERR;
    }

    if (AllocatorController::GetInstance().Init() != DISPLAY_SUCCESS) {
        LOG_ERROR("[Gralloc::GrallocInitialize] Failed to init allocator controller");
        return DISPLAY_FAILURE;
    }

    GrallocFuncs *grallocFuncs = new GrallocFuncs();

    grallocFuncs->AllocMem = AllocMem;
    grallocFuncs->FreeMem = FreeMem;
    grallocFuncs->Mmap = Mmap;
    grallocFuncs->Unmap = Unmap;
    grallocFuncs->InvalidateCache = InvalidateCache;
    grallocFuncs->FlushCache = FlushCache;
    *funcs = grallocFuncs;

    return DISPLAY_SUCCESS;
}
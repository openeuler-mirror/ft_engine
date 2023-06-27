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

#pragma once

#include "allocator.h"

#include <mutex>
#include <unordered_map>
#include <gbm.h>

namespace oewm {
namespace HDI {
namespace DISPLAY {

class GbmAllocator : public Allocator {
public:
    GbmAllocator() = default;
    ~GbmAllocator() noexcept override;

    virtual int32_t Init() override;

    virtual int32_t AllocMem(const AllocInfo &info, BufferHandle **bufferPtr) override;
    virtual int32_t FreeMem(BufferHandle *buffer) override;
    virtual void *Mmap(BufferHandle &buffer) override;
    virtual int32_t Unmap(BufferHandle &buffer) override;
    virtual int32_t FlushCache(BufferHandle &buffer) override;
    virtual int32_t InvalidateCache(BufferHandle &buffer) override;

private:
    bool AddGbmBo(int32_t handle, struct gbm_bo *gbmBo);
    struct gbm_bo *GetGbmBo(int32_t handle);
    bool DestroyGbmBo(int32_t handle);

    int32_t AllocMemWithUsage(const AllocInfo &info, BufferHandle **bufferPtr, uint32_t usage);

private:
    std::mutex mutex_;

    struct gbm_device *gbmContext_ = nullptr;
    std::unordered_map<uint32_t, struct gbm_bo *> boMap_; // handle -> gbm bo
};
} // namespace DISPLAY
} // namespace HDI
} // namespace oewm

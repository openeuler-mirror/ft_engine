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

namespace FT {
namespace HDI {
namespace DISPLAY {

class DumbAllocator : public Allocator {
public:
    DumbAllocator() = default;
    ~DumbAllocator() noexcept = default;

    virtual int32_t Init() override;

    virtual int32_t AllocMem(const AllocInfo &info, BufferHandle **bufferPtr) override;
    virtual int32_t FreeMem(BufferHandle *buffer) override;
    virtual void *Mmap(BufferHandle &buffer) override;
    virtual int32_t Unmap(BufferHandle &buffer) override;
    virtual int32_t FlushCache(BufferHandle &buffer) override;
    virtual int32_t InvalidateCache(BufferHandle &buffer) override;

private:
    int32_t DestroyGemDumbHandle(unsigned int handle);
    bool GetSupportGbmModifyFromSession();

private:
    std::mutex mutex_;
    int drmFd_ = -1;
};

} // namespace DISPLAY
} // namespace HDI
} // namespace FT

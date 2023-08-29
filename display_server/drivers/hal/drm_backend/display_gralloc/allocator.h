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

#include "display_type.h"

#include <cstdint>

namespace FT {
namespace HDI {
namespace DISPLAY {

class Allocator {
public:
    Allocator() = default;
    virtual ~Allocator() noexcept = default;

    virtual int32_t Init()
    {
        return DISPLAY_SUCCESS;
    }

    /**
     * @brief Allocates memory based on the parameters passed by the GUI.
     *
     * @param info Indicates the pointer to the description info of the memory to allocate.
     * @param bufferPtr Indicates the double pointer to the buffer of the memory to allocate.
     * @return Returns DISPLAY_SUCCESS(0) if the operation is successful; returns an error code otherwise.
     */
    virtual int32_t AllocMem(const AllocInfo &info, BufferHandle **bufferPtr) = 0;
    /**
     * @brief Releases memory.
     *
     * @param buffer Indicates the pointer to the buffer of the memory to release.
     * @return Returns DISPLAY_SUCCESS(0) if the operation is successful; returns an error code otherwise.
     */
    virtual int32_t FreeMem(BufferHandle *buffer) = 0;
    /**
     * @brief Maps memory to memory without cache in the process's address space.
     *
     * @param buffer Indicates the pointer to the buffer of the memory to map.
     * @return Returns the pointer to a valid address if the operation is successful; returns nullptr otherwise.
     */
    virtual void *Mmap(BufferHandle &buffer) = 0;
    /**
     * @brief Unmaps memory, that is, removes any mappings from the process's address space.
     *
     * @param buffer Indicates the pointer to the buffer of the memory to unmap.
     * @return Returns DISPLAY_SUCCESS(0) if the operation is successful; returns an error code defined otherwise.
     */
    virtual int32_t Unmap(BufferHandle &buffer) = 0;
    /**
     * @brief Flushes data from the cache mapped via Mmap() to memory and invalidates the data in the cache.
     *
     * @param buffer Indicates the pointer to the buffer of the cache to flush.
     * @return Returns DISPLAY_SUCCESS(0) if the operation is successful; returns an error code defined otherwise.
     */
    virtual int32_t FlushCache(BufferHandle &buffer) = 0;
    /**
     * @brief Invalidates the cache to update it from memory.
     *
     * @param buffer Indicates the pointer to the buffer of the cache, which will been invalidated.
     * @return Returns DISPLAY_SUCCESS(0) if the operation is successful; returns an error code defined otherwise.
     */
    virtual int32_t InvalidateCache(BufferHandle &buffer) = 0;
};

} // namespace DISPLAY
} // namespace HDI
} // namespace FT

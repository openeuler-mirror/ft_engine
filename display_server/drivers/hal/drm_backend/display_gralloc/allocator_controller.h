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

#include <cstdint>
#include <memory>

namespace oewm {
namespace HDI {
namespace DISPLAY {

// singleton
class AllocatorController {
public:
    static AllocatorController &GetInstance()
    {
        static AllocatorController instance;
        return instance;
    }

    AllocatorController(const AllocatorController &) = delete;
    ~AllocatorController() noexcept = default;

    /**
     * @brief Init AllocatorController
     *
     * init it before call `GetAllocator()`.
     * @return int32_t DispErrCode
     */
    int32_t Init();
    /**
     * @brief Uninit AllocatorController
     *
     * free allocators.
     * @return int32_t DispErrCode
     */
    int32_t Uninit();
    /**
     * @brief Get the buffer allocator depending on buffer usage.
     *
     * @param usage Buffer usage
     * @return std::shared_ptr<Allocator> return Allocator. If failed, return nullptr.
     */
    std::shared_ptr<Allocator> GetAllocator(uint64_t usage);

private:
    AllocatorController() {}

private:
    bool isEnabled_ = false;

    std::shared_ptr<Allocator> gbmAllocator_ = nullptr;
    std::shared_ptr<Allocator> dumbAllocator_ = nullptr;
    std::shared_ptr<Allocator> shmAllocator_ = nullptr;
};

} // namespace DISPLAY
} // namespace HDI
} // namespace oewm

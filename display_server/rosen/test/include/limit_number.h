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

#ifndef ROSEN_TEST_LIMIT_H
#define ROSEN_TEST_LIMIT_H

#include <limits>

namespace OHOS::Rosen::TestSrc::limitNumber {
    constexpr static float floatLimit[] = {
        0.0f, 485.44f, -34.4f,
        std::numeric_limits<float>::max(), std::numeric_limits<float>::min(),
        };

    static constexpr uint64_t Uint64[] = {
        std::numeric_limits<uint64_t>::min(),
        std::numeric_limits<uint64_t>::max(),
        std::numeric_limits<int64_t>::max(),
        std::numeric_limits<int64_t>::min(),
        1,
        255,
        256,
        300
    };
} // namespace OHOS::Rosen::TestSrc::limitNumber

#endif // ROSEN_TEST_LIMIT_H

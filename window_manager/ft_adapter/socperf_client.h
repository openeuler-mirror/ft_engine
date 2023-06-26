/*
 * Copyright (c) 2023 Huawei Technologies Co., Ltd.
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

#ifndef SOC_PERF_CLIENT
#define SOC_PERF_CLIENT

#include <cstdint>
#include <string>

namespace OHOS {
namespace SOCPERF {
class SocPerfClient {
public:
    void PerfRequest(int32_t cmdId, const std::string& msg) {}
    void PerfRequestEx(int32_t cmdId, bool onOffTag, const std::string& msg) {}

    static SocPerfClient& GetInstance() {
        static SocPerfClient client;
        return client;
    }
};
} // namespace SOCPERF
} // namespace OHOS

#endif // SOC_PERF_CLIENT

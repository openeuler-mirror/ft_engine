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

#ifndef OHOS_ROSEN_WM_UTILS_PERFORM_REPORTER_H
#define OHOS_ROSEN_WM_UTILS_PERFORM_REPORTER_H

#include <atomic>
#include <chrono>
#include <string>
#include <vector>
#include <map>

namespace OHOS {
namespace Rosen {
class PerformReporter {
public:
    PerformReporter(const std::string& tag, const std::vector<int64_t>& timeSpiltsMs, uint32_t reportInterval = 50);
    void start();
    void end();
private:
    void count(int64_t costTime);
    bool report();
    void clear();

    std::string tag_;
    std::atomic<uint32_t> totalCount_;
    std::map<int64_t, std::atomic<uint32_t>> timeSplitCount_;
    std::chrono::steady_clock::time_point startTime_;
    uint32_t reportInterval_;

    static constexpr auto BARRIER = std::numeric_limits<int64_t>::max();
};
}
}
#endif
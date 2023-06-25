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

#ifndef RELIABILITY_WATCHDOG_FANTIAN_H
#define RELIABILITY_WATCHDOG_FANTIAN_H

#include <string>
#include "event_handler.h"

namespace OHOS {
namespace HiviewDFX {
class Watchdog {
public:
    static Watchdog& GetInstance() {
        static Watchdog ins;
        return ins;
    }

    int AddThread(const std::string &name, std::shared_ptr<AppExecFwk::EventHandler> handler) {
        return 0;
    }
};
} // end of namespace HiviewDFX
} // end of namespace OHOS
#endif

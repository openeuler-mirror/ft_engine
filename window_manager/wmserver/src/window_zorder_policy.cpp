/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "window_manager_hilog.h"
#include "window_zorder_policy.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "WindowZorderPolicy"};
}

int32_t WindowZorderPolicy::GetWindowPriority(WindowType type) const
{
    if (windowPriorityMap_.count(type) == 0) {
        WLOGFE("invalid window type");
        return windowPriorityMap_.at(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    }
    return windowPriorityMap_.at(type);
}
}
}
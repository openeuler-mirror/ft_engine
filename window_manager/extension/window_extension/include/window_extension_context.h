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

#ifndef WINDOW_EXTENSION_CONTEXT_H
#define WINDOW_EXTENSION_CONTEXT_H

#include <ability_manager_client.h>
#include <extension_context.h>
#include "wm_common.h"

namespace OHOS {
namespace Rosen {
class WindowExtensionContext : public AbilityRuntime::ExtensionContext {
public:
    WindowExtensionContext() = default;
    ~WindowExtensionContext() = default;

    /**
     * @brief Starts a new ability.
     * An ability using the AbilityInfo.AbilityType.SERVICE or AbilityInfo.AbilityType.PAGE template uses this method
     * to start a specific ability. The system locates the target ability from installed abilities based on the value
     * of the want parameter and the startOptions parameter and then starts it. You can specify the ability to start
     * using the want parameter.
     *
     * @param want Indicates the Want containing information about the target ability to start.
     * @param startOptions Indicates the startOptions containing information about WindowMode and displayId to start.
     *
     * @return errCode WM_OK on success, others on failure.
     */
    WMError StartAbility(const AAFwk::Want &want, const AAFwk::StartOptions &startOptions) const;

    static const size_t CONTEXT_TYPE_ID;
private:
    static int ILLEGAL_REQUEST_CODE;
};
} // namespace Rosen
} // namespace OHOS
#endif // WINDOW_EXTENSION_CONTEXT_HP
/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#ifndef RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_MODIFIER_MANAGER_H
#define RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_MODIFIER_MANAGER_H

#include <memory>
#include <set>

#include "common/rs_macros.h"

namespace OHOS {
namespace Rosen {
class RSModifier;

class RSC_EXPORT RSModifierManager {
public:
    RSModifierManager() = default;
    virtual ~RSModifierManager() = default;

    void AddModifier(const std::shared_ptr<RSModifier>& modifier);
    void Draw();

private:
    std::set<std::shared_ptr<RSModifier>> modifiers_;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_MODIFIER_MANAGER_H

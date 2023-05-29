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

#ifndef FRAMEWORKS_WM_TEST_UT_MOCK_STATIC_CALL_H
#define FRAMEWORKS_WM_TEST_UT_MOCK_STATIC_CALL_H
#include <gmock/gmock.h>

#include "ability_context_impl.h"
#include "foundation/ability/ability_runtime/interfaces/kits/native/appkit/ability_runtime/context/context.h"
#include "static_call.h"

namespace OHOS {
namespace Rosen {
class MockStaticCall : public StaticCall {
public:
    MOCK_METHOD3(CreateWindow, sptr<Window>(const std::string& windowName,
        sptr<WindowOption>& option, std::shared_ptr<AbilityRuntime::Context> abilityContext));
};
} // namepsace Rosen
} // namespace OHOS

#endif // FRAMEWORKS_WM_TEST_UT_MOCK_STATIC_CALL_H+

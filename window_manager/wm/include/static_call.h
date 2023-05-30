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

#ifndef OHOS_STATIC_CALL_H
#define OHOS_STATIC_CALL_H

#include <iremote_object.h>
#include "singleton_delegator.h"
#include "wm_single_instance.h"
#include "window.h"
#include "window_option.h"
namespace OHOS {
namespace Rosen {
class StaticCall {
WM_DECLARE_SINGLE_INSTANCE_BASE(StaticCall);
public:
    virtual sptr<Window> CreateWindow(const std::string& windowName,
        sptr<WindowOption>& option, std::shared_ptr<AbilityRuntime::Context> context = nullptr);
    virtual std::vector<sptr<Window>> GetSubWindow(uint32_t parentId);
protected:
    StaticCall() = default;
private:
    static inline SingletonDelegator<StaticCall> delegator_;
};
} // namespace ROSEN
} // namespace OHOS

#endif // FRAMEWORKS_WM_TEST_UT_STATIC_CALL_H
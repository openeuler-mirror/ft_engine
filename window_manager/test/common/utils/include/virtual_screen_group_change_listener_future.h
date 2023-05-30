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

#ifndef VIRTUAL_SCREEN_GROUP_CHANGE_LISTENER_FUTURE_H
#define VIRTUAL_SCREEN_GROUP_CHANGE_LISTENER_FUTURE_H

#include "future.h"
#include "screen_manager.h"

namespace OHOS {
namespace Rosen {
class VirtualScreenGroupChangeListenerFuture : public ScreenManager::IVirtualScreenGroupListener {
public:
    virtual void OnMirrorChange(const ChangeInfo& info) override
    {
        mirrorChangeFuture_.SetValue(info);
    }
    RunnableFuture<ChangeInfo> mirrorChangeFuture_;
};
} // Rosen
} // OHOS
#endif  // VIRTUAL_SCREEN_GROUP_CHANGE_LISTENER_FUTURE_H
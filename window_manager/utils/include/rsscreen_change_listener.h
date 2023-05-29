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

#ifndef OHOS_ROSEN_RSSCREEN_CHANGE_LISTENER_H
#define OHOS_ROSEN_RSSCREEN_CHANGE_LISTENER_H

#include <refbase.h>

namespace OHOS {
namespace Rosen {
using OnRSScreenConnectedCb = std::function<void()>;
struct IRSScreenChangeListener : public RefBase {
public:
    OnRSScreenConnectedCb onConnected_;
    OnRSScreenConnectedCb onDisconnected_;
};
}
}
#endif // OHOS_ROSEN_RSSCREEN_CHANGE_LISTENER_H

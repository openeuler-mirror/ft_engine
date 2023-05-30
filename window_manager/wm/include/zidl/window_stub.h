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

#ifndef OHOS_WINDOW_STUB_H
#define OHOS_WINDOW_STUB_H

#include "window_interface.h"
#include "iremote_stub.h"

namespace OHOS {
namespace Rosen {
class WindowStub : public IRemoteStub<IWindow> {
public:
    WindowStub() = default;
    ~WindowStub() = default;

    virtual int OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply,
        MessageOption &option) override;
private:
    static inline const class StaticDestroyMonitor {
    public:
        StaticDestroyMonitor() : destroyed_(false) {}
        ~StaticDestroyMonitor()
        {
            destroyed_ = true;
        }
        bool IsDestroyed() const
        {
            return destroyed_;
        }
    private:
        bool destroyed_;
    } staticDestroyMonitor_;
};
} // namespace Rosen
} // namespace OHOS
#endif // OHOS_WINDOW_STUB_H

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

#ifndef WINDOW_EXTENSION_PROXY_H
#define WINDOW_EXTENSION_PROXY_H

#include <iremote_proxy.h>
#include "window_extension_interface.h"

namespace OHOS {
namespace Rosen {
class WindowExtensionProxy : public IRemoteProxy<IWindowExtension> {
public:
    explicit WindowExtensionProxy(const sptr<IRemoteObject>& impl)
        : IRemoteProxy<IWindowExtension>(impl) {};
    ~WindowExtensionProxy() {};

    void SetBounds(const Rect& rect) override;
    void Hide() override;
    void Show() override;
    void RequestFocus() override;
    void GetExtensionWindow(sptr<IWindowExtensionClient>& token) override;
private:
    static inline BrokerDelegator<WindowExtensionProxy> delegator_;
};
} // namespace Rosen
} // namespace OHOS
#endif // WINDOW_EXTENSION_PROXY_H
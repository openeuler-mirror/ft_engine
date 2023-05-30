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

#ifndef WINDOW_EXTENSION_STUB_IMPL_H
#define WINDOW_EXTENSION_STUB_IMPL_H

#include <string>

#include "window.h"
#include "window_extension_stub.h"
#include "window_extension_client_interface.h"
#include "wm_common.h"

namespace OHOS {
namespace Rosen {
class WindowExtensionStubImpl : public WindowExtensionStub {
public:
    WindowExtensionStubImpl(const std::string& windowName);
    ~WindowExtensionStubImpl();

    virtual void SetBounds(const Rect& rect) override;
    virtual void Hide() override;
    virtual void Show() override;
    virtual void RequestFocus() override;
    virtual void GetExtensionWindow(sptr<IWindowExtensionClient>& token) override;

    sptr<Window> CreateWindow(
        const Rect& rect, uint32_t parentWindowId, const std::shared_ptr<AbilityRuntime::Context>& context);
    void DestroyWindow();
    sptr<Window> GetWindow() const;
private:
    sptr<IDispatchInputEventListener> dispatchInputEventListener_;
    sptr<Window> window_;
    std::string windowName_;
    sptr<IWindowExtensionClient> token_;
};
} // namespace Rosen
} // namespace OHOS
#endif // WINDOW_EXTENSION_STUB_IMPL_H
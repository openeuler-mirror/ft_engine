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

#ifndef WINDOW_EXTENSION_H
#define WINDOW_EXTENSION_H

#include <extension_base.h>
#include <service_extension.h>

namespace OHOS {
namespace Rosen {
class WindowExtensionContext;

class WindowExtension : public AbilityRuntime::ExtensionBase<WindowExtensionContext> {
public:
    WindowExtension() = default;
    virtual ~WindowExtension() = default;

    /**
     * @brief Create and init context.
     *
     * @param record the extension record.
     * @param application the application info.
     * @param handler the extension handler.
     * @param token the remote token.
     * @return The created context.
     */
    virtual std::shared_ptr<WindowExtensionContext> CreateAndInitContext(
        const std::shared_ptr<AbilityRuntime::AbilityLocalRecord>& record,
        const std::shared_ptr<AbilityRuntime::OHOSApplication>& application,
        std::shared_ptr<AbilityRuntime::AbilityHandler>& handler,
        const sptr<IRemoteObject>& token) override;

    /**
     * @brief Init the extension.
     *
     * @param record the extension record.
     * @param application the application info.
     * @param handler the extension handler.
     * @param token the remote token.
     */
    virtual void Init(const std::shared_ptr<AbilityRuntime::AbilityLocalRecord>& record,
        const std::shared_ptr<AbilityRuntime::OHOSApplication>& application,
        std::shared_ptr<AbilityRuntime::AbilityHandler>& handler,
        const sptr<IRemoteObject>& token) override;

    /**
     * @brief Create Extension.
     *
     * @param runtime The runtime.
     * @return The WindowExtension instance.
     */
    static WindowExtension* Create(const std::unique_ptr<AbilityRuntime::Runtime>& runtime);
};
} // namespace Rosen
} // namespace OHOS
#endif
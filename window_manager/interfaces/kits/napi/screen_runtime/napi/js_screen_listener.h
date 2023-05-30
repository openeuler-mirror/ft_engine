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

#ifndef OHOS_JS_SCREEN_LISTENER_H
#define OHOS_JS_SCREEN_LISTENER_H

#include <mutex>
#include "native_engine/native_engine.h"
#include "native_engine/native_value.h"
#include "screen_manager.h"

namespace OHOS {
namespace Rosen {
class JsScreenListener : public ScreenManager::IScreenListener {
public:
    explicit JsScreenListener(NativeEngine* engine) : engine_(engine) {}
    ~JsScreenListener() override = default;
    void AddCallback(const std::string& type, NativeValue* jsListenerObject);
    void RemoveAllCallback();
    void RemoveCallback(const std::string& type, NativeValue* jsListenerObject);
    void OnConnect(ScreenId id) override;
    void OnDisconnect(ScreenId id) override;
    void OnChange(ScreenId id) override;

private:
    void CallJsMethod(const std::string& methodName, NativeValue* const* argv = nullptr, size_t argc = 0);
    NativeEngine* engine_ = nullptr;
    std::mutex mtx_;
    std::map<std::string, std::vector<std::unique_ptr<NativeReference>>> jsCallBack_;
    NativeValue* CreateScreenIdArray(NativeEngine& engine, const std::vector<ScreenId>& data);
};
const std::string EVENT_CONNECT = "connect";
const std::string EVENT_DISCONNECT = "disconnect";
const std::string EVENT_CHANGE = "change";
}  // namespace Rosen
}  // namespace OHOS
#endif /* OHOS_JS_SCREEN_LISTENER_H */
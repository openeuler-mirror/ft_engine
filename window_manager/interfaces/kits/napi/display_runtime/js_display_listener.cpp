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
#include "js_display_listener.h"
#include "js_runtime_utils.h"
#include "window_manager_hilog.h"
namespace OHOS {
namespace Rosen {
using namespace AbilityRuntime;
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_DISPLAY, "JsDisplayListener"};
}

void JsDisplayListener::AddCallback(const std::string& type, NativeValue* jsListenerObject)
{
    WLOGFI("JsDisplayListener::AddCallback is called");
    std::unique_ptr<NativeReference> callbackRef;
    if (engine_ == nullptr) {
        WLOGFE("engine_ nullptr");
        return;
    }
    callbackRef.reset(engine_->CreateReference(jsListenerObject, 1));
    std::lock_guard<std::mutex> lock(mtx_);
    jsCallBack_[type].emplace_back(std::move(callbackRef));
    WLOGFI("JsDisplayListener::AddCallback success jsCallBack_ size: %{public}u!",
        static_cast<uint32_t>(jsCallBack_[type].size()));
}

void JsDisplayListener::RemoveAllCallback()
{
    std::lock_guard<std::mutex> lock(mtx_);
    jsCallBack_.clear();
}

void JsDisplayListener::RemoveCallback(const std::string& type, NativeValue* jsListenerObject)
{
    std::lock_guard<std::mutex> lock(mtx_);
    auto it = jsCallBack_.find(type);
    if (it == jsCallBack_.end()) {
        WLOGE("JsDisplayListener::RemoveCallback no callback to remove");
        return;
    }
    auto& listeners = it->second;
    for (auto iter = listeners.begin(); iter != listeners.end();) {
        if (jsListenerObject->StrictEquals((*iter)->Get())) {
            listeners.erase(iter);
        } else {
            iter++;
        }
    }
    WLOGFI("JsDisplayListener::RemoveCallback success jsCallBack_ size: %{public}u!",
        static_cast<uint32_t>(listeners.size()));
}

void JsDisplayListener::CallJsMethod(const std::string& methodName, NativeValue* const* argv, size_t argc)
{
    if (methodName.empty()) {
        WLOGFE("empty method name str, call method failed");
        return;
    }
    WLOGFI("CallJsMethod methodName = %{public}s", methodName.c_str());
    if (engine_ == nullptr) {
        WLOGFE("engine_ nullptr");
        return;
    }
    for (auto& callback : jsCallBack_[methodName]) {
        NativeValue* method = callback->Get();
        if (method == nullptr) {
            WLOGFE("Failed to get method callback from object");
            continue;
        }
        engine_->CallFunction(engine_->CreateUndefined(), method, argv, argc);
    }
}

void JsDisplayListener::OnCreate(DisplayId id)
{
    std::lock_guard<std::mutex> lock(mtx_);
    WLOGFI("JsDisplayListener::OnCreate is called, displayId: %{public}d", static_cast<uint32_t>(id));
    if (jsCallBack_.empty()) {
        WLOGFE("JsDisplayListener::OnCreate not register!");
        return;
    }
    if (jsCallBack_.find(EVENT_ADD) == jsCallBack_.end()) {
        WLOGE("JsDisplayListener::OnCreate not this event, return");
        return;
    }

    std::unique_ptr<AsyncTask::CompleteCallback> complete = std::make_unique<AsyncTask::CompleteCallback> (
        [=] (NativeEngine &engine, AsyncTask &task, int32_t status) {
            NativeValue* argv[] = {CreateJsValue(*engine_, static_cast<uint32_t>(id))};
            CallJsMethod(EVENT_ADD, argv, ArraySize(argv));
        }
    );

    NativeReference* callback = nullptr;
    std::unique_ptr<AsyncTask::ExecuteCallback> execute = nullptr;
    AsyncTask::Schedule("JsDisplayListener::OnCreate", *engine_, std::make_unique<AsyncTask>(
        callback, std::move(execute), std::move(complete)));
}

void JsDisplayListener::OnDestroy(DisplayId id)
{
    std::lock_guard<std::mutex> lock(mtx_);
    WLOGFI("JsDisplayListener::OnDestroy is called, displayId: %{public}d", static_cast<uint32_t>(id));
    if (jsCallBack_.empty()) {
        WLOGFE("JsDisplayListener::OnDestroy not register!");
        return;
    }
    if (jsCallBack_.find(EVENT_REMOVE) == jsCallBack_.end()) {
        WLOGE("JsDisplayListener::OnDestroy not this event, return");
        return;
    }

    std::unique_ptr<AsyncTask::CompleteCallback> complete = std::make_unique<AsyncTask::CompleteCallback> (
        [=] (NativeEngine &engine, AsyncTask &task, int32_t status) {
            NativeValue* argv[] = {CreateJsValue(*engine_, static_cast<uint32_t>(id))};
            CallJsMethod(EVENT_REMOVE, argv, ArraySize(argv));
        }
    );

    NativeReference* callback = nullptr;
    std::unique_ptr<AsyncTask::ExecuteCallback> execute = nullptr;
    AsyncTask::Schedule("JsDisplayListener::OnDestroy", *engine_, std::make_unique<AsyncTask>(
            callback, std::move(execute), std::move(complete)));
}

void JsDisplayListener::OnChange(DisplayId id)
{
    std::lock_guard<std::mutex> lock(mtx_);
    WLOGFI("JsDisplayListener::OnChange is called, displayId: %{public}d", static_cast<uint32_t>(id));
    if (jsCallBack_.empty()) {
        WLOGFE("JsDisplayListener::OnChange not register!");
        return;
    }
    if (jsCallBack_.find(EVENT_CHANGE) == jsCallBack_.end()) {
        WLOGE("JsDisplayListener::OnChange not this event, return");
        return;
    }

    std::unique_ptr<AsyncTask::CompleteCallback> complete = std::make_unique<AsyncTask::CompleteCallback> (
        [=] (NativeEngine &engine, AsyncTask &task, int32_t status) {
            NativeValue* argv[] = {CreateJsValue(*engine_, static_cast<uint32_t>(id))};
            CallJsMethod(EVENT_CHANGE, argv, ArraySize(argv));
        }
    );

    NativeReference* callback = nullptr;
    std::unique_ptr<AsyncTask::ExecuteCallback> execute = nullptr;
    AsyncTask::Schedule("JsDisplayListener::OnChange", *engine_, std::make_unique<AsyncTask>(
            callback, std::move(execute), std::move(complete)));
}
} // namespace Rosen
} // namespace OHOS

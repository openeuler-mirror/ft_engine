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

#include "js_window_extension_context.h"

#include <js_runtime_utils.h>
#include <js_extension_context.h>

#include "napi_common_start_options.h"
#include "napi_common_want.h"
#include "service_extension_context.h"
#include "start_options.h"
#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
using namespace AbilityRuntime;

namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "JSWindowExtensionContext"};
    std::shared_ptr<AppExecFwk::EventHandler> handler_;
}

class JsWindowExtensionContext final {
public:
    explicit JsWindowExtensionContext(
        const std::shared_ptr<WindowExtensionContext>& context) : context_(context) {}
    ~JsWindowExtensionContext() = default;

    static void Finalizer(NativeEngine* engine, void* data, void* hint)
    {
        WLOGFI("JsWindowExtensionContext::Finalizer is called");
        std::unique_ptr<JsWindowExtensionContext>(static_cast<JsWindowExtensionContext*>(data));
    }

    static NativeValue* StartAbility(NativeEngine* engine, NativeCallbackInfo* info)
    {
        JsWindowExtensionContext* me = CheckParamsAndGetThis<JsWindowExtensionContext>(engine, info);
        return (me != nullptr) ? me->OnStartAbility(*engine, *info) : nullptr;
    }
private:
    std::weak_ptr<WindowExtensionContext> context_;

    bool CheckStartAbilityInputParam(
        NativeEngine& engine, NativeCallbackInfo& info,
        AAFwk::Want& want, AAFwk::StartOptions& startOptions) const
    {
        if (info.argc < 1) {
            return false;
        }
        // Check input want
        if (!CheckWantParam(engine, info.argv[0], want)) {
            return false;
        }
        if (info.argc > 1 && info.argv[1]->TypeOf() == NATIVE_OBJECT) {
            WLOGFI("OnStartAbility start options is used.");
            AppExecFwk::UnwrapStartOptions(reinterpret_cast<napi_env>(&engine),
                reinterpret_cast<napi_value>(info.argv[1]), startOptions);
        }
        return true;
    }

    bool CheckWantParam(NativeEngine& engine, NativeValue* value, AAFwk::Want& want) const
    {
        if (!OHOS::AppExecFwk::UnwrapWant(reinterpret_cast<napi_env>(&engine),
            reinterpret_cast<napi_value>(value), want)) {
            WLOGFE("The input want is invalid.");
            return false;
        }
        WLOGFI("UnwrapWant, BundleName: %{public}s, AbilityName: %{public}s.",
            want.GetBundle().c_str(),
            want.GetElement().GetAbilityName().c_str());
        return true;
    }

    NativeValue* OnStartAbility(NativeEngine& engine, NativeCallbackInfo& info)
    {
        WLOGFI("OnStartAbility is called");
        if (info.argc < 2) {// at least two argc
            WLOGFE("Start ability failed, not enough params.");
            engine.Throw(CreateJsError(engine, static_cast<int32_t>(WmErrorCode::WM_ERROR_INVALID_PARAM)));
            return engine.CreateUndefined();
        }

        AAFwk::Want want;
        AAFwk::StartOptions startOptions;
        if (!CheckStartAbilityInputParam(engine, info, want, startOptions)) {
            engine.Throw(CreateJsError(engine, static_cast<int32_t>(WmErrorCode::WM_ERROR_INVALID_PARAM)));
            return engine.CreateUndefined();
        }

        AsyncTask::CompleteCallback complete =
            [weak = context_, want, startOptions](NativeEngine& engine, AsyncTask& task, int32_t status) {
                WLOGFI("startAbility begin");
                auto context = weak.lock();
                if (!context) {
                    WLOGFW("context is released");
                    task.Reject(engine, CreateJsError(engine,
                        static_cast<int32_t>(WmErrorCode::WM_ERROR_STATE_ABNORMALLY)));
                    return;
                }

                WmErrorCode ret = WM_JS_TO_ERROR_CODE_MAP.at(context->StartAbility(want, startOptions));
                if (ret == WmErrorCode::WM_OK) {
                    task.Resolve(engine, engine.CreateUndefined());
                } else {
                    task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(ret)));
                }
            };

        NativeValue* lastParam = (info.argc <= 2) ? nullptr : info.argv[2];// at least two argc
        NativeValue* result = nullptr;
        AsyncTask::Schedule("JSServiceExtensionContext::OnStartAbility",
            engine, CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
        return result;
    }
};

void JsWindowConnectCallback::OnAbilityConnectDone(const AppExecFwk::ElementName& element,
                                                   const sptr<IRemoteObject>& remoteObject, int resultCode)
{
    WLOGFI("called");
}

void JsWindowConnectCallback::OnAbilityDisconnectDone(const AppExecFwk::ElementName& element, int resultCode)
{
    WLOGFI("called");
}

NativeValue* CreateJsWindowExtensionContext(NativeEngine& engine,
                                            const std::shared_ptr<WindowExtensionContext>& context)
{
    WLOGFI("CreateJsWindowExtensionContext begin");
    NativeValue* objValue = CreateJsExtensionContext(engine, context);
    NativeObject* object = AbilityRuntime::ConvertNativeValueTo<NativeObject>(objValue);

    std::unique_ptr<JsWindowExtensionContext> jsContext
        = std::make_unique<JsWindowExtensionContext>(context);
    object->SetNativePointer(jsContext.release(), JsWindowExtensionContext::Finalizer, nullptr);

    // make handler
    handler_ = std::make_shared<AppExecFwk::EventHandler>(AppExecFwk::EventRunner::GetMainEventRunner());

    const char *moduleName = "JsWindowExtensionContext";
    BindNativeFunction(engine, *object, "startAbility", moduleName, JsWindowExtensionContext::StartAbility);

    return objValue;
}
} // namespace Rosen
} // namespace OHOS
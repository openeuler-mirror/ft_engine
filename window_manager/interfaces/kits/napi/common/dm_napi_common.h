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

#ifndef INTERFACES_KITS_NAPI_GRAPHIC_COMMON_COMMON_H
#define INTERFACES_KITS_NAPI_GRAPHIC_COMMON_COMMON_H

#include <cstdint>
#include <node_api.h>
#include <node_api_types.h>
#include <memory>
#include <string>

#include "js_native_api.h"
#include "js_native_api_types.h"
#include "window_manager_hilog.h"
#include "dm_common.h"

constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, OHOS::Rosen::HILOG_DOMAIN_WINDOW,
                                                "NapiWindowManagerCommonLayer" };

const int PARAM_NUMBER = 2; // 2: callback func input number, also reused by Promise

#define GNAPI_LOG(fmt, ...) OHOS::HiviewDFX::HiLog::Info(LABEL, \
    "%{public}s:%{public}d " fmt, __func__, __LINE__, ##__VA_ARGS__)

#define GNAPI_ASSERT(env, assertion, fmt, ...)  \
    do {                                        \
        if (assertion) {                        \
            GNAPI_LOG(fmt, ##__VA_ARGS__);      \
            return nullptr;                     \
        }                                       \
    } while (0)

#define GNAPI_INNER(call)                         \
    do {                                          \
        napi_status s = (call);                   \
        if (s != napi_ok) {                       \
            GNAPI_LOG(#call " is %{public}d", s); \
            return s;                             \
        }                                         \
    } while (0)

namespace OHOS {
napi_status SetMemberInt32(napi_env env, napi_value result, const char *key, int32_t value);
napi_status SetMemberUint32(napi_env env, napi_value result, const char *key, uint32_t value);
napi_status SetMemberUndefined(napi_env env, napi_value result, const char *key);

bool CheckCallingPermission(const std::string& permission);
void SetErrorInfo(napi_env env, Rosen::DmErrorCode wret, const std::string& errMessage,
    napi_value result[], int count);
void ProcessPromise(napi_env env, Rosen::DmErrorCode wret, napi_deferred deferred,
    napi_value result[], int count);
void ProcessCallback(napi_env env, napi_ref ref, napi_value result[], int count);
bool NAPICall(napi_env env, napi_status status);

template<typename ParamT>
napi_value AsyncProcess(napi_env env,
                        const std::string& funcname,
                        void(*async)(napi_env env, std::unique_ptr<ParamT>& param),
                        napi_value(*resolve)(napi_env env, std::unique_ptr<ParamT>& param),
                        napi_ref& callbackRef,
                        std::unique_ptr<ParamT>& param)
{
    struct AsyncCallbackInfo {
        napi_async_work asyncWork;
        napi_deferred deferred;
        void (*async)(napi_env env, std::unique_ptr<ParamT>& param);
        napi_value (*resolve)(napi_env env, std::unique_ptr<ParamT>& param);
        std::unique_ptr<ParamT> param;
        napi_ref ref;
    };

    AsyncCallbackInfo *info = new AsyncCallbackInfo {
        .async = async,
        .resolve = resolve,
        .param = std::move(param),
        .ref = callbackRef,
    };

    napi_value resourceName = nullptr;
    if (!NAPICall(env, napi_create_string_latin1(env, funcname.c_str(), NAPI_AUTO_LENGTH, &resourceName))) {
        delete info;
        return nullptr;
    }

    // decide use promise or callback
    napi_value result = nullptr;
    if (info->ref == nullptr) {
        if (!NAPICall(env, napi_create_promise(env, &info->deferred, &result))) {
            delete info;
            return nullptr;
        }
    } else {
        if (!NAPICall(env, napi_get_undefined(env, &result))) {
            delete info;
            return nullptr;
        }
    }

    auto asyncFunc = [](napi_env env, void *data) {
        AsyncCallbackInfo *info = reinterpret_cast<AsyncCallbackInfo *>(data);
        if (info->async) {
            info->async(env, info->param);
        }
    };

    auto completeFunc = [](napi_env env, napi_status status, void *data) {
        AsyncCallbackInfo *info = reinterpret_cast<AsyncCallbackInfo *>(data);
        napi_value result[PARAM_NUMBER] = {0};
        if (info->param->wret == Rosen::DmErrorCode::DM_OK) {
            napi_get_undefined(env, &result[0]);
            result[1] = info->resolve(env, info->param);
        } else {
            SetErrorInfo(env, info->param->wret, info->param->errMessage, result, PARAM_NUMBER);
        }
        if (info->deferred) {
            ProcessPromise(env, info->param->wret, info->deferred, result, PARAM_NUMBER);
        } else {
            ProcessCallback(env, info->ref, result, PARAM_NUMBER);
        }
        napi_delete_async_work(env, info->asyncWork);
        delete info;
    };
    if (!NAPICall(env, napi_create_async_work(env, nullptr, resourceName, asyncFunc,
        completeFunc, reinterpret_cast<void *>(info), &info->asyncWork))) {
        delete info;
        return nullptr;
    }
    if (!NAPICall(env, napi_queue_async_work(env, info->asyncWork))) {
        delete info;
        return nullptr;
    }

    return result;
};
} // namespace OHOS

#endif // INTERFACES_KITS_NAPI_GRAPHIC_COMMON_COMMON_H

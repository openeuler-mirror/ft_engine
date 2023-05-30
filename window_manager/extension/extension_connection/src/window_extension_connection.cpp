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

#include "window_extension_connection.h"

#include <ability_connect_callback_stub.h>
#include <ability_manager_client.h>
#include <element_name.h>
#include <hitrace_meter.h>
#include <iremote_object.h>

#include "window_extension_proxy.h"
#include "window_extension_client_stub_impl.h"
#include "window_manager_hilog.h"
#include "wm_common.h"
#include "wm_common_inner.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "WindowExtensionConnection"};
}

class WindowExtensionConnection::Impl : public AAFwk::AbilityConnectionStub {
public:
    Impl() = default;
    ~Impl() = default;
    void OnAbilityConnectDone(const AppExecFwk::ElementName& element,
        const sptr<IRemoteObject>& remoteObject, int resultCode) override;
    void OnAbilityDisconnectDone(const AppExecFwk::ElementName& element, int resultCode) override;

    int ConnectExtension(const AppExecFwk::ElementName& element, const Rect& rect,
        uint32_t uid, uint32_t windowId, const sptr<IWindowExtensionCallback>& callback);
    void DisconnectExtension();
    void Show() const;
    void Hide() const;
    void SetBounds(const Rect& rect) const;
    void RequestFocus() const;
private:
    class WindowExtensionClientRecipient
        final : public IRemoteObject::DeathRecipient {
    public:
        explicit WindowExtensionClientRecipient(sptr<IWindowExtensionCallback> callback);
        ~WindowExtensionClientRecipient() = default;
        void OnRemoteDied(const wptr<IRemoteObject>& remote);
    private:
        sptr<IWindowExtensionCallback> callback_;
    };

    sptr<IWindowExtensionCallback> componentCallback_;
    sptr<IWindowExtension> proxy_;
    sptr<WindowExtensionClientRecipient> deathRecipient_;
};

WindowExtensionConnection::WindowExtensionConnection()
    : pImpl_(new Impl())
{
}

WindowExtensionConnection::~WindowExtensionConnection()
{
    DisconnectExtension();
    WLOGI("disconnect");
}

WindowExtensionConnection::Impl::WindowExtensionClientRecipient::WindowExtensionClientRecipient(
    sptr<IWindowExtensionCallback> callback)
    : callback_(callback)
{
}

void WindowExtensionConnection::Impl::WindowExtensionClientRecipient::OnRemoteDied(const wptr<IRemoteObject>& wptrDeath)
{
    if (wptrDeath == nullptr) {
        WLOGFE("wptrDeath is null");
        return;
    }

    sptr<IRemoteObject> object = wptrDeath.promote();
    if (!object) {
        WLOGFE("object is null");
        return;
    }

    if (callback_ != nullptr) {
        WLOGFI("on extension disconnected");
        callback_->OnExtensionDisconnected();
    }
    WLOGFI("Remote died");
}

int WindowExtensionConnection::Impl::ConnectExtension(const AppExecFwk::ElementName& element,
    const Rect& rect, uint32_t uid, uint32_t windowId, const sptr<IWindowExtensionCallback>& callback)
{
    AAFwk::Want want;
    want.SetElement(element);
    StartAsyncTraceArgs(HITRACE_TAG_WINDOW_MANAGER, static_cast<int32_t>(TraceTaskId::CONNECT_EXTENSION),
        "WindowExtension %s-%s", element.GetBundleName().c_str(), element.GetAbilityName().c_str());
    want.SetParam(RECT_FORM_KEY_POS_X, rect.posX_);
    want.SetParam(RECT_FORM_KEY_POS_Y, rect.posY_);
    want.SetParam(RECT_FORM_KEY_WIDTH, static_cast<int>(rect.width_));
    want.SetParam(RECT_FORM_KEY_HEIGHT, static_cast<int>(rect.height_));
    want.SetParam(WINDOW_ID, static_cast<int>(windowId));
     // 100 default userId
    auto ret = AAFwk::AbilityManagerClient::GetInstance()->ConnectAbility(want, this, nullptr, uid);
    if (ret == ERR_OK) {
        componentCallback_ = callback;
    }
    WLOGFI("Connection extension end ret = %{public}d windowId = %{public}u", ret, windowId);
    return ret;
}

void WindowExtensionConnection::Impl::Show() const
{
    if (proxy_ != nullptr) {
        proxy_->Show();
        WLOGFI("show window");
    }
}

void WindowExtensionConnection::Impl::Hide() const
{
    if (proxy_ != nullptr) {
        proxy_->Hide();
        WLOGFI("hide window");
    }
}

void WindowExtensionConnection::Impl::RequestFocus() const
{
    if (proxy_ != nullptr) {
        proxy_->RequestFocus();
    }
}

void WindowExtensionConnection::Impl::SetBounds(const Rect& rect) const
{
    if (proxy_ != nullptr) {
        proxy_->SetBounds(rect);
    }
}

void WindowExtensionConnection::Impl::DisconnectExtension()
{
    if (AAFwk::AbilityManagerClient::GetInstance()->DisconnectAbility(this) != ERR_OK) {
        WLOGFE("ConnectAbility failed!");
        return;
    }
}

void WindowExtensionConnection::Impl::OnAbilityConnectDone(const AppExecFwk::ElementName& element,
                                                           const sptr<IRemoteObject>& remoteObject, int resultCode)
{
    if (!remoteObject) {
        WLOGFE("remote object is null");
        return;
    }
    if (!proxy_) {
        proxy_ = new(std::nothrow) WindowExtensionProxy(remoteObject);
        if (!proxy_) {
            WLOGFE("get proxy failed");
            return;
        }
    }
    if (!deathRecipient_) {
        deathRecipient_ = new(std::nothrow) WindowExtensionClientRecipient(componentCallback_);
        if (!deathRecipient_) {
            WLOGFE("get death recipient failed");
            return;
        }
    }
    if (!proxy_->AsObject() || !proxy_->AsObject()->AddDeathRecipient(deathRecipient_)) {
        WLOGFE("Failed to add death recipient");
    }
    sptr<IWindowExtensionClient> clientToken(new WindowExtensionClientStubImpl(componentCallback_));
    if (clientToken != nullptr) {
        proxy_->GetExtensionWindow(clientToken);
        WLOGFI("GetExtensionWindow");
    }
    WLOGFI("call end");
    FinishAsyncTraceArgs(HITRACE_TAG_WINDOW_MANAGER, static_cast<int32_t>(TraceTaskId::CONNECT_EXTENSION),
        "WindowExtension %s-%s", element.GetBundleName().c_str(), element.GetAbilityName().c_str());
}

void WindowExtensionConnection::Impl::OnAbilityDisconnectDone(const AppExecFwk::ElementName& element, int resultCode)
{
    if (componentCallback_ != nullptr) {
        componentCallback_->OnExtensionDisconnected();
    }
    WLOGFI("end");
}

int WindowExtensionConnection::ConnectExtension(const AppExecFwk::ElementName& element,
    const Rect& rect, uint32_t uid, uint32_t windowId, const sptr<IWindowExtensionCallback>& callback) const
{
    return pImpl_->ConnectExtension(element, rect, uid, windowId, callback);
}

void WindowExtensionConnection::Show() const
{
    pImpl_->Show();
}

void WindowExtensionConnection::Hide() const
{
    pImpl_->Hide();
}

void WindowExtensionConnection::RequestFocus() const
{
    pImpl_->RequestFocus();
}

void WindowExtensionConnection::SetBounds(const Rect& rect) const
{
    pImpl_->SetBounds(rect);
}

void WindowExtensionConnection::DisconnectExtension() const
{
    pImpl_->DisconnectExtension();
}
} // namespace Rosen
} // namespace OHOS

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

#include "window.h"
#include "window_helper.h"
#include "window_impl.h"
#include "window_manager_hilog.h"
#include "wm_common.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "WindowImpl"};
}
sptr<Window> Window::Create(const std::string& windowName, sptr<WindowOption>& option,
    const std::shared_ptr<OHOS::AbilityRuntime::Context>& context)
{
    if (windowName.empty()) {
        WLOGFE("window name is empty");
        return nullptr;
    }
    if (option == nullptr) {
        option = new(std::nothrow) WindowOption();
        if (option == nullptr) {
            WLOGFE("malloc option failed");
            return nullptr;
        }
    }
    WindowType type = option->GetWindowType();
    if (!(WindowHelper::IsAppWindow(type) || WindowHelper::IsSystemWindow(type))) {
        WLOGFE("window type is invalid %{public}d", type);
        return nullptr;
    }
    option->SetWindowName(windowName);
    sptr<WindowImpl> windowImpl = new(std::nothrow) WindowImpl(option);
    if (windowImpl == nullptr) {
        WLOGFE("malloc windowImpl failed");
        return nullptr;
    }
    WMError error = windowImpl->Create(option->GetParentId(), context);
    if (error != WMError::WM_OK) {
        return nullptr;
    }
    return windowImpl;
}

sptr<Window> Window::Find(const std::string& windowName)
{
    return WindowImpl::Find(windowName);
}

sptr<Window> Window::GetTopWindowWithContext(const std::shared_ptr<AbilityRuntime::Context>& context)
{
    return WindowImpl::GetTopWindowWithContext(context);
}

sptr<Window> Window::GetTopWindowWithId(uint32_t mainWinId)
{
    return WindowImpl::GetTopWindowWithId(mainWinId);
}

std::vector<sptr<Window>> Window::GetSubWindow(uint32_t parentId)
{
    return WindowImpl::GetSubWindow(parentId);
}

void Window::UpdateConfigurationForAll(const std::shared_ptr<AppExecFwk::Configuration>& configuration)
{
    return WindowImpl::UpdateConfigurationForAll(configuration);
}

bool OccupiedAreaChangeInfo::Marshalling(Parcel& parcel) const
{
    return parcel.WriteInt32(rect_.posX_) && parcel.WriteInt32(rect_.posY_) &&
        parcel.WriteUint32(rect_.width_) && parcel.WriteUint32(rect_.height_) &&
        parcel.WriteUint32(static_cast<uint32_t>(type_));
}

OccupiedAreaChangeInfo* OccupiedAreaChangeInfo::Unmarshalling(Parcel& parcel)
{
    OccupiedAreaChangeInfo* occupiedAreaChangeInfo = new OccupiedAreaChangeInfo();
    bool res = parcel.ReadInt32(occupiedAreaChangeInfo->rect_.posX_) &&
        parcel.ReadInt32(occupiedAreaChangeInfo->rect_.posY_) &&
        parcel.ReadUint32(occupiedAreaChangeInfo->rect_.width_) &&
        parcel.ReadUint32(occupiedAreaChangeInfo->rect_.height_);
    if (!res) {
        delete occupiedAreaChangeInfo;
        return nullptr;
    }
    occupiedAreaChangeInfo->type_ = static_cast<OccupiedAreaType>(parcel.ReadUint32());
    return occupiedAreaChangeInfo;
}
} // namespace Rosen
} // namespace OHOS

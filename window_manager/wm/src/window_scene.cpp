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

#include "window_scene.h"
#include <new>
#include <configuration.h>

#include "static_call.h"
#include "window_impl.h"
#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "WindowScene"};
}

const std::string WindowScene::MAIN_WINDOW_ID = "main window";

WindowScene::~WindowScene()
{
    WLOGFI("~WindowScene");
    if (mainWindow_ != nullptr) {
        mainWindow_->Destroy();
        mainWindow_ = nullptr;
    }
}

WMError WindowScene::Init(DisplayId displayId, const std::shared_ptr<AbilityRuntime::Context>& context,
    sptr<IWindowLifeCycle>& listener, sptr<WindowOption> option)
{
    displayId_ = displayId;
    if (option == nullptr) {
        option = new(std::nothrow) WindowOption();
        if (option == nullptr) {
            WLOGFW("alloc WindowOption failed");
            return WMError::WM_ERROR_NULLPTR;
        }
    }
    option->SetDisplayId(displayId);
    option->SetWindowTag(WindowTag::MAIN_WINDOW);

    mainWindow_ = SingletonContainer::Get<StaticCall>().CreateWindow(
        GenerateMainWindowName(context), option, context);
    if (mainWindow_ == nullptr) {
        return WMError::WM_ERROR_NULLPTR;
    }
    mainWindow_->RegisterLifeCycleListener(listener);

    return WMError::WM_OK;
}

std::string WindowScene::GenerateMainWindowName(const std::shared_ptr<AbilityRuntime::Context>& context) const
{
    if (context == nullptr) {
        return MAIN_WINDOW_ID + std::to_string(count++);
    } else {
        std::string windowName = context->GetBundleName() + std::to_string(count++);
        std::size_t pos = windowName.find_last_of('.');
        return (pos == std::string::npos) ? windowName : windowName.substr(pos + 1); // skip '.'
    }
}

sptr<Window> WindowScene::CreateWindow(const std::string& windowName, sptr<WindowOption>& option) const
{
    if (windowName.empty() || mainWindow_ == nullptr || option == nullptr) {
        WLOGFE("WindowScene Name: %{public}s", windowName.c_str());
        return nullptr;
    }
    option->SetParentId(mainWindow_->GetWindowId());
    option->SetWindowTag(WindowTag::SUB_WINDOW);
    return SingletonContainer::Get<StaticCall>().CreateWindow(windowName, option, mainWindow_->GetContext());
}

const sptr<Window>& WindowScene::GetMainWindow() const
{
    return mainWindow_;
}

std::vector<sptr<Window>> WindowScene::GetSubWindow()
{
    if (mainWindow_ == nullptr) {
        WLOGFE("Get sub window failed, because main window is null");
        return std::vector<sptr<Window>>();
    }
    uint32_t parentId = mainWindow_->GetWindowId();
    return SingletonContainer::Get<StaticCall>().GetSubWindow(parentId);
}

WMError WindowScene::GoForeground(uint32_t reason)
{
    WLOGFI("reason:%{public}u", reason);
    if (mainWindow_ == nullptr) {
        WLOGFE("Go foreground failed, because main window is null");
        return WMError::WM_ERROR_NULLPTR;
    }
    return mainWindow_->Show(reason);
}

WMError WindowScene::GoBackground(uint32_t reason)
{
    WLOGFI("reason:%{public}u", reason);
    if (mainWindow_ == nullptr) {
        WLOGFE("Go background failed, because main window is null");
        return WMError::WM_ERROR_NULLPTR;
    }
    return mainWindow_->Hide(reason);
}

WMError WindowScene::GoDestroy()
{
    if (mainWindow_ == nullptr) {
        return WMError::WM_ERROR_NULLPTR;
    }

    WMError ret = mainWindow_->Destroy();
    if (ret != WMError::WM_OK) {
        WLOGFE("WindowScene go destroy failed name: %{public}s", mainWindow_->GetWindowName().c_str());
        return ret;
    }
    mainWindow_ = nullptr;
    return WMError::WM_OK;
}

WMError WindowScene::OnNewWant(const AAFwk::Want& want)
{
    if (mainWindow_ == nullptr) {
        WLOGFE("On new want failed, because main window is null");
        return WMError::WM_ERROR_NULLPTR;
    }
    mainWindow_->OnNewWant(want);
    return WMError::WM_OK;
}

WMError WindowScene::SetSystemBarProperty(WindowType type, const SystemBarProperty& property) const
{
    if (mainWindow_ == nullptr) {
        WLOGFE("Set systembar property failed, because main window is null");
        return WMError::WM_ERROR_NULLPTR;
    }
    return mainWindow_->SetSystemBarProperty(type, property);
}

WMError WindowScene::RequestFocus() const
{
    if (mainWindow_ == nullptr) {
        WLOGFE("Request focus failed, because main window is null");
        return WMError::WM_ERROR_NULLPTR;
    }
    return mainWindow_->RequestFocus();
}

void WindowScene::UpdateConfiguration(const std::shared_ptr<AppExecFwk::Configuration>& configuration)
{
    if (mainWindow_ == nullptr) {
        WLOGFE("Update configuration failed, because main window is null");
        return;
    }
    WLOGFI("notify mainWindow winId:%{public}u", mainWindow_->GetWindowId());
    mainWindow_->UpdateConfiguration(configuration);
}

std::string WindowScene::GetContentInfo() const
{
    if (mainWindow_ == nullptr) {
        WLOGFE("Get content info failed, because main window is null");
        return "";
    }
    return mainWindow_->GetContentInfo();
}

WMError WindowScene::NotifyMemoryLevel(int32_t level) const
{
    WLOGFI("Notify memory level: %{public}d", level);
    if (mainWindow_ == nullptr) {
        WLOGFE("Notify memory level failed, because main window is null");
        return WMError::WM_ERROR_NULLPTR;
    }
    return mainWindow_->NotifyMemoryLevel(level);
}
} // namespace Rosen
} // namespace OHOS

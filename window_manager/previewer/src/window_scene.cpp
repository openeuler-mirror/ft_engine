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

#include <configuration.h>

#include "window_impl.h"
#include "window_manager_hilog.h"
#include "window_scene.h"

namespace OHOS {
namespace Rosen {
const std::string WindowScene::MAIN_WINDOW_ID = "main window";

WindowScene::~WindowScene()
{
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

    mainWindow_ = Window::Create(GenerateMainWindowName(context), option, context);
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
        std::string windowName = "MainWinodw" + std::to_string(count++);
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
    return Window::Create(windowName, option, mainWindow_->GetContext());
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
    return Window::GetSubWindow(parentId);
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

void WindowScene::UpdateConfiguration(const std::shared_ptr<AppExecFwk::Configuration>& configuration)
{
    if (mainWindow_ == nullptr) {
        WLOGFE("Update configuration failed, because main window is null");
        return;
    }
    WLOGFI("notify mainWindow winId:%{public}u", mainWindow_->GetWindowId());
    mainWindow_->UpdateConfiguration(configuration);
}
} // namespace Rosen
} // namespace OHOS

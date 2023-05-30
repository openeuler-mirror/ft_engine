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

#include "inner_window.h"

#include "window_manager_hilog.h"
#include "surface_draw.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "InnerWindow"};
    const std::string IMAGE_PLACE_HOLDER_PNG_PATH = "/etc/window/resources/bg_place_holder.png";
}
WM_IMPLEMENT_SINGLE_INSTANCE(PlaceHolderWindow)

void PlaceholderWindowListener::OnTouchOutside() const
{
    WLOGFD("place holder touch outside");
    PlaceHolderWindow::GetInstance().Destroy();
}

void PlaceholderWindowListener::AfterUnfocused()
{
    WLOGFD("place holder after unfocused");
    PlaceHolderWindow::GetInstance().Destroy();
}

bool PlaceholderInputEventConsumer::OnInputEvent(const std::shared_ptr<MMI::KeyEvent>& keyEvent) const
{
    WLOGFD("place holder get key event");
    PlaceHolderWindow::GetInstance().Destroy();
    return true;
}

bool PlaceholderInputEventConsumer::OnInputEvent(const std::shared_ptr<MMI::PointerEvent>& pointerEvent) const
{
    WLOGFD("place holder get point event");
    PlaceHolderWindow::GetInstance().Destroy();
    return true;
}

bool PlaceholderInputEventConsumer::OnInputEvent(const std::shared_ptr<MMI::AxisEvent>& axisEvent) const
{
    // do nothing
    return false;
}

void PlaceHolderWindow::Create(std::string name, DisplayId displyId, Rect rect, WindowMode mode)
{
    WLOGFD("create inner display id: %{public}" PRIu64"", displyId);
    if (window_ != nullptr) {
        WLOGFW("window has created.");
        return;
    }
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    if (option == nullptr) {
        WLOGFE("window option is nullptr.");
        return;
    }
    option->SetFocusable(false);
    option->SetWindowMode(mode);
    option->SetWindowRect(rect);
    option->SetWindowType(WindowType::WINDOW_TYPE_PLACEHOLDER);
    window_ = Window::Create(name, option);
    if (window_ == nullptr) {
        WLOGFE("window is nullptr.");
        return;
    }
    window_->AddWindowFlag(WindowFlag::WINDOW_FLAG_FORBID_SPLIT_MOVE);
    RegisterWindowListener();
    SetInputEventConsumer();
    if (!OHOS::Rosen::SurfaceDraw::DrawImage(window_->GetSurfaceNode(), rect.width_, rect.height_,
        IMAGE_PLACE_HOLDER_PNG_PATH)) {
        WLOGE("draw surface failed");
        return;
    }
    window_->Show();
    WLOGFD("create palce holder Window end");
}

void PlaceHolderWindow::RegisterWindowListener()
{
    if (window_ == nullptr) {
        WLOGFE("Window is nullptr, register window listener failed.");
        return;
    }
    if (windowListener_ == nullptr) {
        windowListener_ = new (std::nothrow) PlaceholderWindowListener();
    }
    window_->RegisterTouchOutsideListener(windowListener_);
    window_->RegisterLifeCycleListener(windowListener_);
}

void PlaceHolderWindow::UnRegisterWindowListener()
{
    if (window_ == nullptr || windowListener_ == nullptr) {
        WLOGFE("Window or listener is nullptr, unregister window listener failed.");
        return;
    }
    window_->UnregisterTouchOutsideListener(windowListener_);
    window_->UnregisterLifeCycleListener(windowListener_);
}

void PlaceHolderWindow::SetInputEventConsumer()
{
    if (window_ == nullptr) {
        WLOGFE("Window is nullptr, set window input event consumer failed.");
        return;
    }
    if (inputEventConsumer_ == nullptr) {
        inputEventConsumer_ = std::make_shared<PlaceholderInputEventConsumer>();
    }
    window_->SetInputEventConsumer(inputEventConsumer_);
}

void PlaceHolderWindow::Destroy()
{
    WLOGFI("destroy place holder window begin.");
    if (window_ != nullptr) {
        WLOGFI("destroy place holder window not nullptr.");
        UnRegisterWindowListener();
        window_->SetInputEventConsumer(nullptr);
        window_->Destroy();
    }
    window_ = nullptr;
    WLOGFI("destroy place holder window end.");
}
} // Rosen
} // OHOS

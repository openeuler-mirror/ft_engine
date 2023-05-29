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

#include "window_input_channel.h"
#ifdef IMF_ENABLE
#include <input_method_controller.h>
#endif // IMF_ENABLE
#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "WindowInputChannel"};
}
WindowInputChannel::WindowInputChannel(const sptr<Window>& window): window_(window), isAvailable_(true)
{
}

WindowInputChannel::~WindowInputChannel()
{
    WLOGFI("windowName: %{public}s, windowId: %{public}d", window_->GetWindowName().c_str(), window_->GetWindowId());
    window_->SetNeedRemoveWindowInputChannel(false);
}

void WindowInputChannel::HandleKeyEvent(std::shared_ptr<MMI::KeyEvent>& keyEvent)
{
    if (keyEvent == nullptr) {
        WLOGFE("keyEvent is nullptr");
        return;
    }
    WLOGFD("Receive key event, windowId: %{public}u, keyCode: %{public}d",
        window_->GetWindowId(), keyEvent->GetKeyCode());
    if (window_->GetType() == WindowType::WINDOW_TYPE_DIALOG) {
        if (keyEvent->GetAgentWindowId() != keyEvent->GetTargetWindowId()) {
            window_->NotifyTouchDialogTarget();
            keyEvent->MarkProcessed();
            return;
        }
        if (keyEvent->GetKeyCode() == MMI::KeyEvent::KEYCODE_BACK) {
            keyEvent->MarkProcessed();
            return;
        }
    }

    bool inputMethodHasProcessed = false;
#ifdef IMF_ENABLE
    bool isKeyboardEvent = IsKeyboardEvent(keyEvent);
    if (isKeyboardEvent) {
        WLOGFI("dispatch keyEvent to input method");
        inputMethodHasProcessed = MiscServices::InputMethodController::GetInstance()->dispatchKeyEvent(keyEvent);
    }
#endif // IMF_ENABLE
    if (!inputMethodHasProcessed) {
        WLOGFI("dispatch keyEvent to ACE");
        window_->ConsumeKeyEvent(keyEvent);
    }
}

void WindowInputChannel::HandlePointerEvent(std::shared_ptr<MMI::PointerEvent>& pointerEvent)
{
    if (pointerEvent == nullptr) {
        WLOGFE("pointerEvent is nullptr");
        return;
    }
    WLOGFD("Receive pointer event, windowId: %{public}u, action: %{public}d",
        window_->GetWindowId(), pointerEvent->GetPointerAction());
    if ((window_->GetType() == WindowType::WINDOW_TYPE_DIALOG) &&
        (pointerEvent->GetAgentWindowId() != pointerEvent->GetTargetWindowId())) {
        if (pointerEvent->GetPointerAction() == MMI::PointerEvent::POINTER_ACTION_DOWN ||
            pointerEvent->GetPointerAction() == MMI::PointerEvent::POINTER_ACTION_BUTTON_DOWN) {
            window_->NotifyTouchDialogTarget();
        }
        pointerEvent->MarkProcessed();
        return;
    }
    window_->ConsumePointerEvent(pointerEvent);
}

void WindowInputChannel::Destroy()
{
    std::lock_guard<std::mutex> lock(mtx_);
    WLOGFI("Destroy WindowInputChannel, windowId:%{public}u", window_->GetWindowId());
    isAvailable_ = false;
}

bool WindowInputChannel::IsKeyboardEvent(const std::shared_ptr<MMI::KeyEvent>& keyEvent) const
{
    int32_t keyCode = keyEvent->GetKeyCode();
    bool isKeyFN = (keyCode == MMI::KeyEvent::KEYCODE_FN);
    bool isKeyBack = (keyCode == MMI::KeyEvent::KEYCODE_BACK);
    bool isKeyboard = (keyCode >= MMI::KeyEvent::KEYCODE_0 && keyCode <= MMI::KeyEvent::KEYCODE_NUMPAD_RIGHT_PAREN);
    WLOGFI("isKeyFN: %{public}d, isKeyboard: %{public}d", isKeyFN, isKeyboard);
    return (isKeyFN || isKeyboard || isKeyBack);
}
}
}
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

#include "window_node_state_machine.h"

#include "parameters.h"
#include "remote_animation.h"
#include "window_helper.h"
#include "window_manager_hilog.h"
namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "WindowNodeStateMachine"};
} // namespace

WindowNodeStateMachine::WindowNodeStateMachine()
{
}

WindowNodeStateMachine::~WindowNodeStateMachine()
{
}

void WindowNodeStateMachine::SetDestroyTaskParam(bool onlySelf)
{
    destroyOnlySelf_ = destroyOnlySelf_ && onlySelf;
}

bool WindowNodeStateMachine::GetDestroyTaskParam()
{
    return destroyOnlySelf_;
}

bool WindowNodeStateMachine::GetDestroyTask(StateTask& task)
{
    if (!RemoteAnimation::IsAnimationFirst()) {
        return false;
    }
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (destroyTask_ != nullptr) {
        task = destroyTask_;
        WLOGFI("GetDestroyTask success:%{public}u", windowId_);
        return true;
    }
    return false;
}

void WindowNodeStateMachine::SetDestroyTask(StateTask task)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    destroyTask_ = task;
}

void WindowNodeStateMachine::TransitionTo(WindowNodeState state)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (WindowHelper::IsSystemWindow(type_)) {
        WLOGFI("system window no need to use stateMachine");
        return;
    }
    currState_ = state;
}

void WindowNodeStateMachine::UpdateAnimationTaskCount(bool isAdd)
{
    if (!RemoteAnimation::IsAnimationFirst()) {
        WLOGFI("not animation first!");
        return;
    }
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (isAdd) {
        taskCount_++;
        count1++;
        WLOGFD("after add UpdateAnimationTaskCount1: %{public}u id:%{public}u", count1, windowId_);
    } else {
        taskCount_--;
        count2++;
        WLOGFD("after sub UpdateAnimationTaskCount1: %{public}u id:%{public}u", count2, windowId_);
    }
}

void WindowNodeStateMachine::ResetAnimationTaskCount(int32_t taskCount)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    taskCount_ = taskCount;
}

int32_t WindowNodeStateMachine::GetAnimationCount()
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    return taskCount_;
}

bool WindowNodeStateMachine::IsRemoteAnimationPlaying()
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    WLOGFD("IsRemoteAnimationPlaying id:%{public}u state:%{public}u", windowId_, static_cast<uint32_t>(currState_));
    if (currState_ == WindowNodeState::SHOW_ANIMATION_PLAYING ||
        currState_ == WindowNodeState::HIDE_ANIMATION_PLAYING) {
            return true;
    }
    return false;
}

bool WindowNodeStateMachine::IsShowAnimationPlaying()
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    WLOGFD("IsShowAnimationPlaying id:%{public}u state:%{public}u", windowId_, static_cast<uint32_t>(currState_));
    return currState_ == WindowNodeState::SHOW_ANIMATION_PLAYING;
}

bool WindowNodeStateMachine::IsHideAnimationPlaying()
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    WLOGFD("IsHideAnimationPlaying id:%{public}u state:%{public}u", windowId_, static_cast<uint32_t>(currState_));
    return currState_ == WindowNodeState::HIDE_ANIMATION_PLAYING;
}

bool WindowNodeStateMachine::IsWindowNodeShownOrShowing()
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    WLOGFD("IsWindowNodeShownOrShowing id:%{public}u state:%{public}u", windowId_, static_cast<uint32_t>(currState_));
    if (currState_ == WindowNodeState::SHOW_ANIMATION_PLAYING ||
        currState_ == WindowNodeState::SHOW_ANIMATION_DONE || currState_ == WindowNodeState::SHOWN) {
            return true; // not play show animation again when
    }
    return false;
}

bool WindowNodeStateMachine::IsWindowNodeHiddenOrHiding()
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    WLOGFD("IsWindowNodeHiddenOrHiding id:%{public}u state:%{public}u", windowId_, static_cast<uint32_t>(currState_));
    if (currState_ == WindowNodeState::HIDE_ANIMATION_PLAYING ||
        currState_ == WindowNodeState::HIDE_ANIMATION_DONE || currState_ == WindowNodeState::HIDDEN) {
            return true; // not play show animation again when
    }
    return false;
}

WindowNodeState WindowNodeStateMachine::GetCurrentState()
{
    return currState_;
}
} // Rosen
} // OHOS
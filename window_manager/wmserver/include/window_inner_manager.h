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

#ifndef OHOS_ROSEN_WINDOW_INNER_MANAGER_H
#define OHOS_ROSEN_WINDOW_INNER_MANAGER_H

#include <refbase.h>
#include "event_handler.h"
#include "event_runner.h"

#include "drag_controller.h"
#include "inner_window.h"
#include "wm_common.h"
#include "window_node.h"
#include "wm_single_instance.h"

enum class InnerWMRunningState {
    STATE_NOT_START,
    STATE_RUNNING,
};
namespace OHOS {
namespace Rosen {
using InnerTask = std::function<void()>;
using EventRunner = OHOS::AppExecFwk::EventRunner;
using EventHandler = OHOS::AppExecFwk::EventHandler;
using EventPriority = OHOS::AppExecFwk::EventQueue::Priority;
class WindowInnerManager : public RefBase {
WM_DECLARE_SINGLE_INSTANCE_BASE(WindowInnerManager);
public:
    void Start(bool enableRecentholder);
    void Stop();
    void CreateInnerWindow(std::string name, DisplayId displayId, Rect rect, WindowType type, WindowMode mode);
    void DestroyInnerWindow(DisplayId displayId, WindowType type);
    void UpdateInnerWindow(DisplayId displayId, WindowType type, uint32_t width, uint32_t height);
    void PostTask(InnerTask &&callback, std::string name = "WindowInnerManagerTask",
        EventPriority priority = EventPriority::LOW);

    // asynchronously calls the functions of AbilityManager
    void MinimizeAbility(const wptr<WindowNode> &node, bool isFromUser);
    void TerminateAbility(const wptr<WindowNode> &node);
    void CloseAbility(const wptr<WindowNode> &node);
    void CompleteFirstFrameDrawing(const wptr<WindowNode> &node);

    void ConsumePointerEvent(const std::shared_ptr<MMI::PointerEvent>& pointerEvent);
    void NotifyDisplayChange(const std::map<DisplayId, Rect>& displayRectMap);
    bool NotifyServerReadyToMoveOrDrag(uint32_t windowId, sptr<WindowProperty>& windowProperty,
        sptr<MoveDragProperty>& moveDragProperty);
    void NotifyWindowEndUpMovingOrDragging(uint32_t windowId);
    void NotifyWindowRemovedOrDestroyed(uint32_t windowId);
    pid_t GetPid();
    void SetInputEventConsumer();

protected:
    WindowInnerManager();
    ~WindowInnerManager();

private:
    bool Init();

    pid_t pid_ = INVALID_PID;
    bool isRecentHolderEnable_ = false;
    sptr<MoveDragController> moveDragController_;
    // event handle for inner window
    std::shared_ptr<EventHandler> eventHandler_;
    std::shared_ptr<EventRunner> eventLoop_;
    InnerWMRunningState state_;
    const std::string INNER_WM_THREAD_NAME = "InnerWindowManager";
};
} // namespace Rosen
} // namespace OHOS
#endif // OHOS_ROSEN_WINDOW_INNER_MANAGER_H
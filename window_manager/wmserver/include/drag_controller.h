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

#ifndef OHOS_ROSEN_DRAG_CONTROLLER_H
#define OHOS_ROSEN_DRAG_CONTROLLER_H

#include <refbase.h>

#include "event_handler.h"
#include "event_runner.h"
#include "input_manager.h"
#include "pointer_event.h"
#include "window_root.h"
#include "wm_common.h"

namespace OHOS {
namespace Rosen {
using EventRunner = OHOS::AppExecFwk::EventRunner;
using EventHandler = OHOS::AppExecFwk::EventHandler;
/*
 * DragController is the class which is used to handle drag cross window
 */
class DragController : public RefBase {
public:
    explicit DragController(sptr<WindowRoot>& root) : windowRoot_(root) {}
    ~DragController() = default;
    void StartDrag(uint32_t windowId);
    void UpdateDragInfo(uint32_t windowId);
    void FinishDrag(uint32_t windowId);
private:
    sptr<WindowNode> GetHitWindow(DisplayId id, const PointInfo point);
    bool GetHitPoint(uint32_t windowId, PointInfo& point);
    sptr<WindowRoot> windowRoot_;
    uint64_t hitWindowId_ = 0;
};

class DragInputEventListener : public MMI::IInputEventConsumer {
public:
    DragInputEventListener() = default;
    void OnInputEvent(std::shared_ptr<MMI::PointerEvent> pointerEvent) const override;
    void OnInputEvent(std::shared_ptr<MMI::KeyEvent> keyEvent) const override;
    void OnInputEvent(std::shared_ptr<MMI::AxisEvent> axisEvent) const override;
};

/*
 * MoveDragController is the class which is used to handle move or drag floating window
 */
class MoveDragController : public RefBase {
public:
    MoveDragController() : windowProperty_(new WindowProperty()), moveDragProperty_(new MoveDragProperty())
    {
        vsyncCallback_->onCallback = std::bind(&MoveDragController::OnReceiveVsync, this, std::placeholders::_1);
    }
    ~MoveDragController() = default;

    bool Init();
    void Stop();
    void HandleReadyToMoveOrDrag(uint32_t windowId, sptr<WindowProperty>& windowProperty,
        sptr<MoveDragProperty>& moveDragProperty);
    void HandleEndUpMovingOrDragging(uint32_t windowId);
    void HandleWindowRemovedOrDestroyed(uint32_t windowId);
    void ConsumePointerEvent(const std::shared_ptr<MMI::PointerEvent>& pointerEvent);
    uint32_t GetActiveWindowId() const;
    void HandleDisplayChange(const std::map<DisplayId, Rect>& displayRectMap);
    void SetInputEventConsumer();

private:
    void SetDragProperty(const sptr<MoveDragProperty>& moveDragProperty);
    void SetWindowProperty(const sptr<WindowProperty>& windowProperty);
    void SetActiveWindowId(uint32_t);
    const sptr<MoveDragProperty>& GetMoveDragProperty() const;
    const sptr<WindowProperty>& GetWindowProperty() const;
    Rect GetHotZoneRect();
    void ConvertPointerPosToDisplayGroupPos(DisplayId displayId, int32_t& posX, int32_t& posY);

    void HandlePointerEvent(const std::shared_ptr<MMI::PointerEvent>& pointerEvent);
    void HandleDragEvent(int32_t posX, int32_t posY, int32_t pointId, int32_t sourceType);
    void HandleMoveEvent(int32_t posX, int32_t posY, int32_t pointId, int32_t sourceType);
    void OnReceiveVsync(int64_t timeStamp);
    void ResetMoveOrDragState();

    sptr<WindowProperty> windowProperty_;
    sptr<MoveDragProperty> moveDragProperty_;
    uint32_t activeWindowId_ = INVALID_WINDOW_ID;
    std::shared_ptr<MMI::PointerEvent> moveEvent_ = nullptr;
    std::shared_ptr<MMI::IInputEventConsumer> inputListener_ = nullptr;
    std::shared_ptr<VsyncCallback> vsyncCallback_ = std::make_shared<VsyncCallback>(VsyncCallback());
    std::map<DisplayId, Rect> displayRectMap_;

    // event handler for input event
    std::shared_ptr<EventHandler> inputEventHandler_;
    const std::string INNER_WM_INPUT_THREAD_NAME = "InnerInputManager";
};
}
}
#endif // OHOS_ROSEN_DRAG_CONTROLLER_H


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

#ifndef OHOS_ROSEN_INNER_WINDOW_H
#define OHOS_ROSEN_INNER_WINDOW_H

#include "window.h"
#include "wm_common_inner.h"
#include "wm_single_instance.h"

namespace OHOS {
namespace Rosen {
class IInnerWindow : virtual public RefBase {
public:
    virtual void Create(std::string name, DisplayId displyId, Rect rect, WindowMode mode) = 0;
    virtual void Update(uint32_t width, uint32_t height) = 0;
    virtual void Destroy() = 0;
};

class PlaceholderWindowListener : public IWindowLifeCycle, public ITouchOutsideListener {
public:
    // touch outside listener
    virtual void OnTouchOutside() const;
    // lifecycle listener
    virtual void AfterUnfocused();
    // lifecycle do nothing
    virtual void AfterForeground() {};
    virtual void AfterBackground() {};
    virtual void AfterFocused() {};
    virtual void AfterInactive() {};
};

class PlaceholderInputEventConsumer : public IInputEventConsumer {
public:
    ~PlaceholderInputEventConsumer() override = default;
    virtual bool OnInputEvent(const std::shared_ptr<MMI::KeyEvent>& keyEvent) const override;
    virtual bool OnInputEvent(const std::shared_ptr<MMI::PointerEvent>& pointerEvent) const override;
    virtual bool OnInputEvent(const std::shared_ptr<MMI::AxisEvent>& axisEvent) const override;
};

class PlaceHolderWindow : public IInnerWindow {
WM_DECLARE_SINGLE_INSTANCE(PlaceHolderWindow);
public:
    virtual void Create(std::string name, DisplayId displyId, Rect rect, WindowMode mode);
    virtual void Update(uint32_t width, uint32_t height) {};
    virtual void Destroy();

private:
    void RegisterWindowListener();
    void UnRegisterWindowListener();
    void SetInputEventConsumer();

private:
    sptr<OHOS::Rosen::Window> window_;
    sptr<PlaceholderWindowListener> windowListener_;
    std::shared_ptr<IInputEventConsumer> inputEventConsumer_;
};

} // namespace Rosen
} // namespace OHOS
#endif // OHOS_ROSEN_INNER_WINDOW_H

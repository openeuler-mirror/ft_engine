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

#ifndef WINDOW_EXTENSION_CONNECTION_H
#define WINDOW_EXTENSION_CONNECTION_H

#include <string>
#include <element_name.h>
#include <i_input_event_consumer.h>
#include <input_manager.h>
#include <key_event.h>
#include <pointer_event.h>
#include <memory>

#include "wm_common.h"

namespace OHOS {
namespace Rosen {
namespace {
    const std::string RECT_FORM_KEY_POS_X = "ext_pos_x";
    const std::string RECT_FORM_KEY_POS_Y = "ext_pos_y";
    const std::string RECT_FORM_KEY_HEIGHT = "ext_pos_heigh";
    const std::string RECT_FORM_KEY_WIDTH = "ext_pos_width";
    const std::string WINDOW_ID = "ext_window_id";
}

class RSSurfaceNode;
class IWindowExtensionCallback : virtual public RefBase {
public:
    virtual void OnWindowReady(const std::shared_ptr<RSSurfaceNode>& rsSurfaceNode) = 0;
    virtual void OnExtensionDisconnected() = 0;
    virtual void OnKeyEvent(const std::shared_ptr<MMI::KeyEvent>& event) = 0;
    virtual void OnPointerEvent(const std::shared_ptr<MMI::PointerEvent>& event) = 0;
    virtual void OnBackPress() = 0;
};

class WindowExtensionConnection : public RefBase {
public:
    WindowExtensionConnection();
    ~WindowExtensionConnection();
    int ConnectExtension(const AppExecFwk::ElementName& element, const Rect& rect,
        uint32_t uid, uint32_t windowId, const sptr<IWindowExtensionCallback>& callback) const;
    void DisconnectExtension() const;
    void Show() const;
    void Hide() const;
    void RequestFocus() const;
    void SetBounds(const Rect& rect) const;
private:
    class Impl;
    sptr<Impl> pImpl_;
};
} // namespace Rosen
} // namespace OHOS
#endif // WINDOW_EXTENSION_CONNECTION_H
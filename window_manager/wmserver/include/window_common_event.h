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

#ifndef WINDOW_COMMON_EVENT_H
#define WINDOW_COMMON_EVENT_H

#include <common_event_data.h>
#include <common_event_manager.h>
#include <common_event_support.h>
#include <event_handler.h>

#include "window_root.h"

namespace OHOS {
namespace Rosen {
class WindowCommonEvent : public RefBase, public std::enable_shared_from_this<WindowCommonEvent> {
public:
    WindowCommonEvent();
    ~WindowCommonEvent();
    void SubscriberEvent();
    void UnSubscriberEvent();
    void OnReceiveEvent(const EventFwk::CommonEventData& data);
private:
    class EventSubscriber : public EventFwk::CommonEventSubscriber {
    public:
        EventSubscriber(const EventFwk::CommonEventSubscribeInfo &subscriberInfo,
            const std::shared_ptr<WindowCommonEvent>& eventHandler)
            : EventFwk::CommonEventSubscriber(subscriberInfo), eventHandler_(eventHandler) {};
        ~EventSubscriber() = default;
        void OnReceiveEvent(const EventFwk::CommonEventData& data) override
        {
            eventHandler_->OnReceiveEvent(data);
        }
    private:
        std::shared_ptr<WindowCommonEvent> eventHandler_;
    };
    void SubscriberEventInner(int retry);
    void HandleAccountSwitched(const EventFwk::CommonEventData& data) const;

    typedef void (WindowCommonEvent::*HandleCommonEventFunc)(const EventFwk::CommonEventData& data) const;

    std::map<std::string, HandleCommonEventFunc> handleCommonEventFuncs_;
    std::shared_ptr<EventSubscriber> subscriber_;
    std::shared_ptr<AppExecFwk::EventHandler> eventHandler_;
};
} // Rosen
} // OHOS
#endif // WINDOW_COMMON_EVENT_H
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

#ifndef POINTER_EVENT_H
#define POINTER_EVENT_H

#include <array>
#include <list>
#include <memory>
#include <ostream>
#include <set>
#include <vector>

#include "input_event.h"

namespace OHOS {
namespace MMI {
class PointerEvent : public InputEvent {
public:
    static constexpr int32_t POINTER_ACTION_UNKNOWN = 0;
    static constexpr int32_t POINTER_ACTION_CANCEL = 1;
    static constexpr int32_t POINTER_ACTION_DOWN = 2;
    static constexpr int32_t POINTER_ACTION_MOVE = 3;
    static constexpr int32_t POINTER_ACTION_UP = 4;
    static constexpr int32_t POINTER_ACTION_AXIS_BEGIN = 5;
    static constexpr int32_t POINTER_ACTION_AXIS_UPDATE = 6;
    static constexpr int32_t POINTER_ACTION_AXIS_END = 7;
    static constexpr int32_t POINTER_ACTION_BUTTON_DOWN = 8;
    static constexpr int32_t POINTER_ACTION_BUTTON_UP = 9;
    static constexpr int32_t POINTER_ACTION_ENTER_WINDOW = 10;
    static constexpr int32_t POINTER_ACTION_LEAVE_WINDOW = 11;

    int32_t GetPointerAction() const
    {
        return 0;
    }

    class PointerItem {
    public:
        int32_t pointerId_ {};
        int32_t GetDisplayX() const
        {
            return 0;
        }

        int32_t GetDisplayY() const
        {
            return 0;
        }

        void SetDisplayY(int32_t y)
        {
            pointerId_ = y;
        }

        void SetDisplayX(int32_t x)
        {
            pointerId_ = x;
        }

        void SetWindowY(int32_t y)
        {
            pointerId_ = y;
        }

        void SetWindowX(int32_t x)
        {
            pointerId_ = x;
        }
    };

    bool GetPointerItem(int32_t pointerId, PointerItem &pointerItem)
    {
        pointerItem = PointerItem();
        pointerItem.pointerId_ = pointerId;
        return true;
    }

    void UpdatePointerItem(int32_t pointerId, PointerItem &pointerItem)
    {
        pointerItem.pointerId_ = pointerId;
    }

    int32_t GetPointerId() const
    {
        return 0;
    }
};
} // namespace MMI
} // namespace OHOS
#endif // POINTER_EVENT_H
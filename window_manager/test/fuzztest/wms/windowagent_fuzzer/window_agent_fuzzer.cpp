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

#include "marshalling_helper.h"
#include <parcel.h>

#include <securec.h>

#include <want.h>
#include "window.h"
#include "window_agent.h"
#include "window_impl.h"
#include "window_manager.h"

using namespace OHOS::Rosen;

namespace OHOS {
namespace {
    constexpr size_t DATA_MIN_SIZE = 2;
}

template<class T>
size_t GetObject(T &object, const uint8_t *data, size_t size)
{
    size_t objectSize = sizeof(object);
    if (objectSize > size) {
        return 0;
    }
    return memcpy_s(&object, objectSize, data, objectSize) == EOK ? objectSize : 0;
}

void CheckWindowAgentFunctionsPart1(sptr<WindowAgent> agent, const uint8_t* data, size_t size)
{
    if (agent == nullptr || data == nullptr || size < DATA_MIN_SIZE) {
        return;
    }
    size_t startPos = 0;
    OHOS::Rosen::Rect rect;
    bool boolVal;
    OHOS::Rosen::WindowSizeChangeReason reason;
    startPos += GetObject<OHOS::Rosen::Rect>(rect, data + startPos, size - startPos);
    startPos += GetObject<bool>(boolVal, data + startPos, size - startPos);
    startPos += GetObject<OHOS::Rosen::WindowSizeChangeReason>(reason, data + startPos, size - startPos);
    agent->UpdateWindowRect(rect, boolVal, reason);

    WindowMode mode;
    startPos += GetObject<WindowMode>(mode, data + startPos, size - startPos);
    agent->UpdateWindowMode(mode);

    uint32_t modeSupportInfo;
    startPos += GetObject<uint32_t>(modeSupportInfo, data + startPos, size - startPos);
    agent->UpdateWindowModeSupportInfo(modeSupportInfo);
    agent->UpdateFocusStatus(boolVal);

    sptr<AvoidArea> avoidArea = new AvoidArea();
    startPos += GetObject<OHOS::Rosen::Rect>(rect, data + startPos, size - startPos);
    avoidArea->topRect_ = rect;
    startPos += GetObject<OHOS::Rosen::Rect>(rect, data + startPos, size - startPos);
    avoidArea->leftRect_ = rect;
    startPos += GetObject<OHOS::Rosen::Rect>(rect, data + startPos, size - startPos);
    avoidArea->rightRect_ = rect;
    startPos += GetObject<OHOS::Rosen::Rect>(rect, data + startPos, size - startPos);
    avoidArea->bottomRect_ = rect;
    AvoidAreaType type;
    startPos += GetObject<AvoidAreaType>(type, data + startPos, size - startPos);
    agent->UpdateAvoidArea(avoidArea, type);

    WindowState state;
    GetObject<WindowState>(state, data + startPos, size - startPos);
    agent->UpdateWindowState(state);
}

void CheckWindowAgentFunctionsPart2(sptr<WindowAgent> agent, const uint8_t* data, size_t size)
{
    if (agent == nullptr || data == nullptr || size < DATA_MIN_SIZE) {
        return;
    }
    size_t startPos = 0;

    PointInfo point;
    DragEvent dragEvent;
    startPos += GetObject<PointInfo>(point, data + startPos, size - startPos);
    startPos += GetObject<DragEvent>(dragEvent, data + startPos, size - startPos);
    agent->UpdateWindowDragInfo(point, dragEvent);

    DisplayId from;
    DisplayId to;
    startPos += GetObject<DisplayId>(from, data + startPos, size - startPos);
    startPos += GetObject<DisplayId>(to, data + startPos, size - startPos);
    agent->UpdateDisplayId(from, to);

    OHOS::Rosen::Rect rect;
    OccupiedAreaType type;
    startPos += GetObject<OHOS::Rosen::Rect>(rect, data + startPos, size - startPos);
    startPos += GetObject<OccupiedAreaType>(type, data + startPos, size - startPos);
    sptr<OccupiedAreaChangeInfo> info = new OccupiedAreaChangeInfo(type, rect);
    if (info != nullptr) {
        agent->UpdateOccupiedAreaChangeInfo(info);
    }

    bool boolVal;
    startPos += GetObject<bool>(boolVal, data + startPos, size - startPos);
    agent->UpdateActiveStatus(boolVal);
    agent->GetWindowProperty();
    agent->NotifyTouchOutside();
    agent->NotifyScreenshot();

    Transform trans;
    startPos += GetObject<Transform>(trans, data + startPos, size - startPos);
    agent->UpdateZoomTransform(trans, boolVal);

    uint32_t mode;
    GetObject<uint32_t>(mode, data + startPos, size - startPos);
    agent->RestoreSplitWindowMode(mode);
}

void WindowAgentFuzzTest(const uint8_t* data, size_t size)
{
    sptr<OHOS::Rosen::WindowOption> option = new OHOS::Rosen::WindowOption();
    sptr<OHOS::Rosen::WindowImpl> window = new(std::nothrow) OHOS::Rosen::WindowImpl(option);
    if (window == nullptr) {
        return;
    }
    OHOS::Rosen::WMError error = window->Create(option->GetParentId(), nullptr);
    if (error != OHOS::Rosen::WMError::WM_OK) {
        return;
    }
    window->Show();

    sptr<WindowAgent> windowAgent = new WindowAgent(window);
    OHOS::CheckWindowAgentFunctionsPart1(windowAgent, data, size);
    OHOS::CheckWindowAgentFunctionsPart2(windowAgent, data, size);

    window->Hide();
    window->Destroy();
}
} // namespace.OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::WindowAgentFuzzTest(data, size);
    return 0;
}
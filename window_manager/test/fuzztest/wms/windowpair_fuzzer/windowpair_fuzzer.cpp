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

#include "windowpair_fuzzer.h"

#include <ability_manager_client.h>
#include "display_manager.h"
#include "common_event_manager.h"
#include "window_manager_hilog.h"
#include "window_helper.h"
#include "surface_draw.h"
#include "window_pair.h"

using namespace OHOS::Rosen;

namespace OHOS {
namespace {
    constexpr size_t DATA_MIN_SIZE = 2;
    constexpr size_t LEN = 10;
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

size_t InitWindowNode1(sptr<OHOS::Rosen::WindowNode> &windowNode, const uint8_t *data, size_t size)
{
    size_t startPos = 0;
    sptr<OHOS::Rosen::WindowProperty> property = new WindowProperty();
    Parcel parcel;
    if (parcel.WriteBuffer(data + startPos, size - startPos)) {
        property = OHOS::Rosen::WindowProperty::Unmarshalling(parcel);
    }
    windowNode = new OHOS::Rosen::WindowNode(property);
    if (windowNode == nullptr) {
        return startPos;
    }
    return startPos;
}

bool DoSomethingInterestingWithMyAPI2(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }
    size_t startPos = 0;
    DisplayId displayId = OHOS::Rosen::DisplayManager::GetInstance().GetDefaultDisplayId();
    sptr<OHOS::Rosen::WindowPair> windowPair = new OHOS::Rosen::WindowPair(displayId);
    sptr<OHOS::Rosen::WindowNode> windowNode;
    startPos += InitWindowNode1(windowNode, data + startPos, size -startPos);
    std::vector<int32_t> vec;
    int32_t num;
    for(uint32_t i = 0; i < LEN; ++i) {
        startPos += GetObject(num, data + startPos, size - startPos);
        vec.emplace_back(num);
    }
    windowPair->IsDockSliceInExitSplitModeArea(vec);
    windowPair->ExitSplitMode();
    windowPair->Clear();
    windowPair->IsSplitRelated(windowNode);
    windowPair->GetOrderedPair(windowNode);
    windowPair->GetPairedWindows();
    windowPair->UpdateIfSplitRelated(windowNode);
    windowPair->UpdateWindowPairStatus();
    windowPair->SwitchPosition();
    windowPair->HandlePairedNodesChange();
    windowPair->UpdateWindowPairStatus();
    windowPair->Insert(windowNode);
    windowPair->DumpPairInfo();
    windowPair->HandleRemoveWindow(windowNode);
    OHOS::Rosen::Rect rect;
    GetObject(rect, data + startPos, size - startPos);
    windowPair->RotateDividerWindow(rect);
    windowPair->SetDividerRect(rect);
    windowPair->ClearPairSnapshot();
    return true;
}

bool DoSomethingInterestingWithMyAPI(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }
    OHOS::Rosen::DisplayId displayId = OHOS::Rosen::DisplayManager::GetInstance().GetDefaultDisplayId();
    sptr<OHOS::Rosen::WindowPair> windowPair = new OHOS::Rosen::WindowPair(displayId);
    size_t startPos = 0;
    OHOS::Rosen::SplitEventMsgType msg;
    startPos += GetObject<OHOS::Rosen::SplitEventMsgType>(msg, data + startPos, size - startPos);
    int32_t missionId;
    startPos += GetObject<int32_t>(missionId, data + startPos, size - startPos);
    sptr<WindowNode> windowNode;
    startPos += InitWindowNode1(windowNode, data + startPos, size -startPos);
    bool isDestroy;
    startPos += GetObject<bool>(isDestroy, data + startPos, size -startPos);
    windowPair->NotifyCreateOrDestroyDivider(windowNode, isDestroy);
    windowPair->Find(windowNode);
    windowPair->IsPaired();
    float ratio;
    GetObject<float>(ratio, data + startPos, size - startPos);
    windowPair->SetSplitRatio(ratio);
    windowPair->GetSplitRatio();
    windowPair->GetPairStatus();
    windowPair->GetDividerWindow();
    windowPair->IsForbidDockSliceMove();
    return true;
}
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::DoSomethingInterestingWithMyAPI(data, size);
    OHOS::DoSomethingInterestingWithMyAPI2(data, size);
    return 0;
}


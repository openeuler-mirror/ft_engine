/*
 * Copyright (c) 2023 Huawei Technologies Co., Ltd.
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

#ifndef OHOS_ABILITY_RUNTIME_WINDOW_INFO_H
#define OHOS_ABILITY_RUNTIME_WINDOW_INFO_H

#include "ability_info.h"
#include "iremote_object.h"
#include "parcel.h"

namespace OHOS {
namespace AAFwk {
enum class TransitionReason : uint32_t {
    MINIMIZE = 0,
    CLOSE,
    ABILITY_TRANSITION,
    BACK_TRANSITION,
};

struct AbilityTransitionInfo : public Parcelable {
    std::string bundleName_;
    std::string abilityName_;
    uint32_t mode_ = 0;
    std::vector<AppExecFwk::SupportWindowMode> windowModes_;
    sptr<IRemoteObject> abilityToken_ = nullptr;
    uint64_t displayId_ = 0;
    bool isShowWhenLocked_ = false;
    bool isRecent_ = false;
    double maxWindowRatio_;
    double minWindowRatio_;
    uint32_t maxWindowWidth_;
    uint32_t minWindowWidth_;
    uint32_t maxWindowHeight_;
    uint32_t minWindowHeight_;
    int32_t missionId_;
    TransitionReason reason_ = TransitionReason::ABILITY_TRANSITION;
};
} // namespace AAFwk
} // namespace OHOS
#endif // OHOS_ABILITY_RUNTIME_WINDOW_INFO_H

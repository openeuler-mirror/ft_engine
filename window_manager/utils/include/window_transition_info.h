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

#ifndef OHOS_ROSEN_WINDOW_TRANSITION_INFO_H
#define OHOS_ROSEN_WINDOW_TRANSITION_INFO_H
#include <iremote_object.h>
#include <string>

#include "ability_info.h"
#include "window_info.h"
#include "wm_common.h"
#include "wm_common_inner.h"

namespace OHOS {
namespace Rosen {
enum class TransitionReason : uint32_t {
    MINIMIZE = 0,
    CLOSE,
    ABILITY_TRANSITION,
    BACK_TRANSITION,
};

class WindowTransitionInfo : public Parcelable {
public:
    WindowTransitionInfo() = default;
    ~WindowTransitionInfo() = default;
    WindowTransitionInfo(sptr<AAFwk::AbilityTransitionInfo> info);

    virtual bool Marshalling(Parcel& parcel) const override;
    static WindowTransitionInfo* Unmarshalling(Parcel& parcel);
    void SetBundleName(std::string name);
    std::string GetBundleName();
    void SetAbilityName(std::string name);
    std::string GetAbilityName();
    void SetWindowMode(WindowMode mode);
    WindowMode GetWindowMode();
    void SetWindowRect(Rect rect);
    Rect GetWindowRect();
    void SetAbilityToken(sptr<IRemoteObject> abilityToken);
    sptr<IRemoteObject> GetAbilityToken();
    void SetDisplayId(DisplayId displayId);
    DisplayId GetDisplayId();
    void SetWindowType(WindowType windowType);
    WindowType GetWindowType();
    void SetShowFlagWhenLocked(bool isShow);
    void SetWindowSupportModes(const std::vector<AppExecFwk::SupportWindowMode> supportModes);
    std::vector<AppExecFwk::SupportWindowMode> GetWindowSupportModes();
    bool GetShowFlagWhenLocked();
    void SetTransitionReason(TransitionReason reason);
    TransitionReason GetTransitionReason();
    void SetIsRecent(bool isRecent);
    bool GetIsRecent() const;
    WindowSizeLimits GetWindowSizeLimits() const;
    void SetMissionId(int32_t missionId);
    int32_t GetMissionId() const;
private:
    std::string bundleName_;
    std::string abilityName_;
    WindowMode mode_ = WindowMode::WINDOW_MODE_FULLSCREEN;
    Rect windowRect_ = {0, 0, 0, 0};
    sptr<IRemoteObject> abilityToken_ = nullptr;
    DisplayId displayId_ = 0;
    WindowType windowType_ = WindowType::WINDOW_TYPE_APP_MAIN_WINDOW;
    bool isShowWhenLocked_ = false;
    bool isRecent_ = false;
    TransitionReason reason_ = TransitionReason::ABILITY_TRANSITION;
    std::vector<AppExecFwk::SupportWindowMode> supportWindowModes_;
    WindowSizeLimits sizeLimits_;
    int32_t missionId_ = -1;
};
} // Rosen
} // OHOS
#endif
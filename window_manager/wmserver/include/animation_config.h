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
#ifndef OHOS_ROSEN_ANIMATION_CONFIG_H
#define OHOS_ROSEN_ANIMATION_CONFIG_H

#include "animation/rs_animation_timing_curve.h"
#include "animation/rs_animation_timing_protocol.h"
#include "common/rs_vector3.h"
#include "common/rs_vector4.h"

namespace OHOS {
namespace Rosen {
struct AnimationConfig {
    struct WindowAnimationConfig {
        struct AnimationTiming {
            RSAnimationTimingProtocol timingProtocol_ = 200;
            RSAnimationTimingCurve timingCurve_ = RSAnimationTimingCurve::EASE_OUT;
        } animationTiming_;
        float opacity_ = 0;
        Vector3f scale_ { 0.7f, 0.7f, 1.0f };
        Vector3f translate_ { 0, 0, 0 };
        Vector4f rotation_ { 0, 0, 1, 0 };
    } windowAnimationConfig_;
    struct KeyboardAnimationConfig {
        RSAnimationTimingCurve curve_ = RSAnimationTimingCurve::CreateCubicCurve(0.2f, 0.0f, 0.2f, 1.0f);
        RSAnimationTimingProtocol durationIn_ = 500;
        RSAnimationTimingProtocol durationOut_ = 300;
    } keyboardAnimationConfig_;
};
} // namespace Rosen
} // namespace OHOS
#endif // OHOS_ROSEN_ANIMATION_CONFIG_H
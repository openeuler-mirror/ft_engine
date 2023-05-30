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

#ifndef OHOS_ROSEN_SCREEN_ROTATION_CONTROLLER_H
#define OHOS_ROSEN_SCREEN_ROTATION_CONTROLLER_H

#include <map>
#include <refbase.h>

#include "sensor_agent.h"

#include "dm_common.h"
#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
enum class SensorRotation: int32_t {
    INVALID = -1,
    ROTATION_0 = 0,
    ROTATION_90,
    ROTATION_180,
    ROTATION_270,
};

enum class DeviceRotation: int32_t {
    INVALID = -1,
    ROTATION_PORTRAIT = 0,
    ROTATION_LANDSCAPE,
    ROTATION_PORTRAIT_INVERTED,
    ROTATION_LANDSCAPE_INVERTED,
};

class ScreenRotationController : public RefBase {
public:
    ScreenRotationController() = delete;
    ~ScreenRotationController() = default;
    static void Init();
    static void HandleSensorEventInput(DeviceRotation deviceRotation);
    static bool IsScreenRotationLocked();
    static void SetScreenRotationLocked(bool isLocked);
    static void SetDefaultDeviceRotationOffset(uint32_t defaultDeviceRotationOffset);
    static void ProcessOrientationSwitch(Orientation orientation);

    static bool IsDefaultDisplayRotationPortrait();
    static bool IsDisplayRotationVertical(Rotation rotation);
    static bool IsDisplayRotationHorizontal(Rotation rotation);
    static DeviceRotation ConvertSensorToDeviceRotation(SensorRotation sensorRotation);
private:
    static void HandleGravitySensorEventCallback(SensorEvent *event);
    static Rotation GetCurrentDisplayRotation();
    static Orientation GetPreferredOrientation();
    static void SetScreenRotation(Rotation targetRotation);
    static Rotation CalcTargetDisplayRotation(Orientation requestedOrientation,
        DeviceRotation sensorRotationConverted);
    static DeviceRotation CalcDeviceRotation(SensorRotation sensorRotation);
    static Rotation ConvertDeviceToDisplayRotation(DeviceRotation sensorRotationConverted);

    static bool IsDeviceRotationVertical(DeviceRotation deviceRotation);
    static bool IsDeviceRotationHorizontal(DeviceRotation deviceRotation);
    static bool IsCurrentDisplayVertical();
    static bool IsCurrentDisplayHorizontal();
    static bool IsSensorRelatedOrientation(Orientation orientation);
    
    static void ProcessRotationMapping();
    static void ProcessSwitchToAutoRotationPortrait(DeviceRotation rotation);
    static void ProcessSwitchToAutoRotationLandscape(DeviceRotation rotation);
    static void ProcessSwitchToAutoRotation(DeviceRotation rotation);
    static void ProcessSwitchToAutoRotationPortraitRestricted();
    static void ProcessSwitchToAutoRotationLandscapeRestricted();
    static void ProcessSwitchToSensorRelatedOrientation(Orientation orientation, DeviceRotation deviceRotation);
    static void ProcessSwitchToSensorUnrelatedOrientation(Orientation orientation);
    static Rotation ProcessAutoRotationPortraitOrientation(DeviceRotation sensorRotationConveted);
    static Rotation ProcessAutoRotationLandscapeOrientation(DeviceRotation sensorRotationConveted);

    static DisplayId defaultDisplayId_;
    static uint32_t defaultDeviceRotationOffset_;
    static uint32_t defaultDeviceRotation_;
    static std::map<SensorRotation, DeviceRotation> sensorToDeviceRotationMap_;
    static std::map<DeviceRotation, Rotation> deviceToDisplayRotationMap_;
    static Orientation lastOrientationType_;
    static Rotation currentDisplayRotation_;
    static Rotation lastSensorDecidedRotation_;
    static Rotation rotationLockedRotation_;
    static bool isScreenRotationLocked_;
    static DeviceRotation lastSensorRotationConverted_;
};
} // Rosen
} // OHOS
#endif // OHOS_ROSEN_SCREEN_ROTATION_CONTROLLER_H

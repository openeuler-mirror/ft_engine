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

#include "screen_rotation_controller.h"

#include <chrono>
#include <securec.h>

#include "display_manager_service_inner.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_DISPLAY, "ScreenRotationController"};
}

DisplayId ScreenRotationController::defaultDisplayId_ = 0;
Rotation ScreenRotationController::currentDisplayRotation_;
bool ScreenRotationController::isScreenRotationLocked_ = true;
uint32_t ScreenRotationController::defaultDeviceRotationOffset_ = 0;
Orientation ScreenRotationController::lastOrientationType_ = Orientation::UNSPECIFIED;
Rotation ScreenRotationController::lastSensorDecidedRotation_;
Rotation ScreenRotationController::rotationLockedRotation_;
uint32_t ScreenRotationController::defaultDeviceRotation_ = 0;
std::map<SensorRotation, DeviceRotation> ScreenRotationController::sensorToDeviceRotationMap_;
std::map<DeviceRotation, Rotation> ScreenRotationController::deviceToDisplayRotationMap_;
DeviceRotation ScreenRotationController::lastSensorRotationConverted_ = DeviceRotation::INVALID;

void ScreenRotationController::Init()
{
    ProcessRotationMapping();
    currentDisplayRotation_ = GetCurrentDisplayRotation();
    lastSensorDecidedRotation_ = currentDisplayRotation_;
    rotationLockedRotation_ = currentDisplayRotation_;
}

bool ScreenRotationController::IsScreenRotationLocked()
{
    return isScreenRotationLocked_;
}

void ScreenRotationController::SetScreenRotationLocked(bool isLocked)
{
    if (isLocked) {
        rotationLockedRotation_ = GetCurrentDisplayRotation();
    }
    isScreenRotationLocked_ = isLocked;
}

void ScreenRotationController::SetDefaultDeviceRotationOffset(uint32_t defaultDeviceRotationOffset)
{
    // Available options for defaultDeviceRotationOffset: {0, 90, 180, 270}
    if (defaultDeviceRotationOffset < 0 || defaultDeviceRotationOffset > 270 || defaultDeviceRotationOffset % 90 != 0) {
        return;
    }
    defaultDeviceRotationOffset_ = defaultDeviceRotationOffset;
}

void ScreenRotationController::HandleSensorEventInput(DeviceRotation deviceRotation)
{
    Orientation orientation = GetPreferredOrientation();

    currentDisplayRotation_ = GetCurrentDisplayRotation();
    lastSensorRotationConverted_ = deviceRotation;
    if (!IsSensorRelatedOrientation(orientation)) {
        return;
    }
    if (deviceRotation == DeviceRotation::INVALID) {
        return;
    }
    if (currentDisplayRotation_ == ConvertDeviceToDisplayRotation(deviceRotation)) {
        return;
    }
    Rotation targetDisplayRotation = CalcTargetDisplayRotation(orientation, deviceRotation);
    SetScreenRotation(targetDisplayRotation);
}

Rotation ScreenRotationController::GetCurrentDisplayRotation()
{
    sptr<DisplayInfo> defaultDisplayInfo = DisplayManagerServiceInner::GetInstance().GetDefaultDisplay();
    if (defaultDisplayInfo == nullptr) {
        WLOGFE("Cannot get default display info");
        return defaultDeviceRotation_ == 0 ? ConvertDeviceToDisplayRotation(DeviceRotation::ROTATION_PORTRAIT) :
            ConvertDeviceToDisplayRotation(DeviceRotation::ROTATION_LANDSCAPE);
    }
    return defaultDisplayInfo->GetRotation();
}

Orientation ScreenRotationController::GetPreferredOrientation()
{
    sptr<ScreenInfo> screenInfo = DisplayManagerServiceInner::GetInstance().GetScreenInfoByDisplayId(defaultDisplayId_);
    if (screenInfo == nullptr) {
        WLOGFE("Cannot get default screen info");
        return Orientation::UNSPECIFIED;
    }
    return screenInfo->GetOrientation();
}

Rotation ScreenRotationController::CalcTargetDisplayRotation(
    Orientation requestedOrientation, DeviceRotation sensorRotationConverted)
{
    switch (requestedOrientation) {
        case Orientation::SENSOR: {
            lastSensorDecidedRotation_ = ConvertDeviceToDisplayRotation(sensorRotationConverted);
            return ConvertDeviceToDisplayRotation(sensorRotationConverted);
        }
        case Orientation::SENSOR_VERTICAL: {
            return ProcessAutoRotationPortraitOrientation(sensorRotationConverted);
        }
        case Orientation::SENSOR_HORIZONTAL: {
            return ProcessAutoRotationLandscapeOrientation(sensorRotationConverted);
        }
        case Orientation::UNSPECIFIED:
        case Orientation::AUTO_ROTATION_RESTRICTED: {
            if (isScreenRotationLocked_) {
                return currentDisplayRotation_;
            }
            lastSensorDecidedRotation_ = ConvertDeviceToDisplayRotation(sensorRotationConverted);
            return ConvertDeviceToDisplayRotation(sensorRotationConverted);
        }
        case Orientation::AUTO_ROTATION_PORTRAIT_RESTRICTED: {
            if (isScreenRotationLocked_) {
                return currentDisplayRotation_;
            }
            return ProcessAutoRotationPortraitOrientation(sensorRotationConverted);
        }
        case Orientation::AUTO_ROTATION_LANDSCAPE_RESTRICTED: {
            if (isScreenRotationLocked_) {
                return currentDisplayRotation_;
            }
            return ProcessAutoRotationLandscapeOrientation(sensorRotationConverted);
        }
        default: {
            return currentDisplayRotation_;
        }
    }
}

Rotation ScreenRotationController::ProcessAutoRotationPortraitOrientation(DeviceRotation sensorRotationConverted)
{
    if (IsDeviceRotationHorizontal(sensorRotationConverted)) {
        return currentDisplayRotation_;
    }
    lastSensorDecidedRotation_ = ConvertDeviceToDisplayRotation(sensorRotationConverted);
    return ConvertDeviceToDisplayRotation(sensorRotationConverted);
}

Rotation ScreenRotationController::ProcessAutoRotationLandscapeOrientation(DeviceRotation sensorRotationConverted)
{
    if (IsDeviceRotationVertical(sensorRotationConverted)) {
        return currentDisplayRotation_;
    }
    lastSensorDecidedRotation_ = ConvertDeviceToDisplayRotation(sensorRotationConverted);
    return ConvertDeviceToDisplayRotation(sensorRotationConverted);
}

void ScreenRotationController::SetScreenRotation(Rotation targetRotation)
{
    if (targetRotation == GetCurrentDisplayRotation()) {
        return;
    }
    DisplayManagerServiceInner::GetInstance().GetDefaultDisplay()->SetRotation(targetRotation);
    DisplayManagerServiceInner::GetInstance().SetRotationFromWindow(defaultDisplayId_, targetRotation);
    WLOGFI("dms: Set screen rotation: %{public}u", targetRotation);
}

DeviceRotation ScreenRotationController::CalcDeviceRotation(SensorRotation sensorRotation)
{
    if (sensorRotation == SensorRotation::INVALID) {
        return DeviceRotation::INVALID;
    }
    // offset(in degree) divided by 90 to get rotation bias
    int32_t bias = static_cast<int32_t>(defaultDeviceRotationOffset_ / 90);
    int32_t deviceRotationValue = static_cast<int32_t>(sensorRotation) - bias;
    while (deviceRotationValue < 0) {
        // +4 is used to normalize the values into the range 0~3, corresponding to the four rotations.
        deviceRotationValue += 4;
    }
    if (defaultDeviceRotation_ == 1) {
        deviceRotationValue += static_cast<int32_t>(defaultDeviceRotation_);
        // %2 to determine whether the rotation is horizontal or vertical.
        if (deviceRotationValue % 2 == 0) {
            // if device's default rotation is landscape, use -2 to swap 0 and 90, 180 and 270.
            deviceRotationValue -= 2;
        }
    }
    return static_cast<DeviceRotation>(deviceRotationValue);
}

bool ScreenRotationController::IsSensorRelatedOrientation(Orientation orientation)
{
    if ((orientation >= Orientation::UNSPECIFIED && orientation <= Orientation::REVERSE_HORIZONTAL) ||
        orientation == Orientation::LOCKED) {
        return false;
    }
    return true;
}

void ScreenRotationController::ProcessSwitchToSensorRelatedOrientation(
    Orientation orientation, DeviceRotation sensorRotationConverted)
{
    lastOrientationType_ = orientation;
    switch (orientation) {
        case Orientation::AUTO_ROTATION_RESTRICTED: {
            if (isScreenRotationLocked_) {
                SetScreenRotation(rotationLockedRotation_);
                return;
            }
            [[fallthrough]];
        }
        case Orientation::SENSOR: {
            ProcessSwitchToAutoRotation(sensorRotationConverted);
            return;
        }
        case Orientation::AUTO_ROTATION_PORTRAIT_RESTRICTED: {
            if (isScreenRotationLocked_) {
                ProcessSwitchToAutoRotationPortraitRestricted();
                return;
            }
            [[fallthrough]];
        }
        case Orientation::SENSOR_VERTICAL: {
            ProcessSwitchToAutoRotationPortrait(sensorRotationConverted);
            return;
        }
        case Orientation::AUTO_ROTATION_LANDSCAPE_RESTRICTED: {
            if (isScreenRotationLocked_) {
                ProcessSwitchToAutoRotationLandscapeRestricted();
                return;
            }
            [[fallthrough]];
        }
        case Orientation::SENSOR_HORIZONTAL: {
            ProcessSwitchToAutoRotationLandscape(sensorRotationConverted);
            return;
        }
        default: {
            return;
        }
    }
}

void ScreenRotationController::ProcessSwitchToAutoRotation(DeviceRotation rotation)
{
    if (rotation != DeviceRotation::INVALID) {
        return;
    }
}

void ScreenRotationController::ProcessSwitchToAutoRotationPortrait(DeviceRotation rotation)
{
    if (IsCurrentDisplayVertical()) {
        return;
    }
    if (IsDeviceRotationVertical(rotation)) {
        return;
    }
    SetScreenRotation(ConvertDeviceToDisplayRotation(DeviceRotation::ROTATION_PORTRAIT));
}

void ScreenRotationController::ProcessSwitchToAutoRotationLandscape(DeviceRotation rotation)
{
    if (IsCurrentDisplayHorizontal()) {
        return;
    }
    if (IsDeviceRotationHorizontal(rotation)) {
        return;
    }
    SetScreenRotation(ConvertDeviceToDisplayRotation(DeviceRotation::ROTATION_LANDSCAPE));
}

void ScreenRotationController::ProcessSwitchToAutoRotationPortraitRestricted()
{
    if (IsCurrentDisplayVertical()) {
        return;
    }
    if (IsDisplayRotationVertical(rotationLockedRotation_)) {
        SetScreenRotation(rotationLockedRotation_);
        return;
    }
    SetScreenRotation(ConvertDeviceToDisplayRotation(DeviceRotation::ROTATION_PORTRAIT));
}

void ScreenRotationController::ProcessSwitchToAutoRotationLandscapeRestricted()
{
    if (IsCurrentDisplayHorizontal()) {
        return;
    }
    if (IsDisplayRotationHorizontal(rotationLockedRotation_)) {
        SetScreenRotation(rotationLockedRotation_);
        return;
    }
    SetScreenRotation(ConvertDeviceToDisplayRotation(DeviceRotation::ROTATION_LANDSCAPE));
}

DeviceRotation ScreenRotationController::ConvertSensorToDeviceRotation(SensorRotation sensorRotation)
{
    if (sensorToDeviceRotationMap_.empty()) {
        ProcessRotationMapping();
    }
    return sensorToDeviceRotationMap_.at(sensorRotation);
}

Rotation ScreenRotationController::ConvertDeviceToDisplayRotation(DeviceRotation deviceRotation)
{
    if (deviceRotation == DeviceRotation::INVALID) {
        return GetCurrentDisplayRotation();
    }
    if (deviceToDisplayRotationMap_.empty()) {
        ProcessRotationMapping();
    }
    return deviceToDisplayRotationMap_.at(deviceRotation);
}

void ScreenRotationController::ProcessRotationMapping()
{
    sptr<SupportedScreenModes> modes =
        DisplayManagerServiceInner::GetInstance().GetScreenModesByDisplayId(defaultDisplayId_);
    // 0 means PORTRAIT, 1 means LANDSCAPE.
    defaultDeviceRotation_ = modes->width_ < modes->height_ ? 0 : 1;
    if (deviceToDisplayRotationMap_.empty()) {
        deviceToDisplayRotationMap_ = {
            {DeviceRotation::ROTATION_PORTRAIT,
                defaultDeviceRotation_ == 0 ? Rotation::ROTATION_0 : Rotation::ROTATION_90},
            {DeviceRotation::ROTATION_LANDSCAPE,
                defaultDeviceRotation_ == 1 ? Rotation::ROTATION_0 : Rotation::ROTATION_90},
            {DeviceRotation::ROTATION_PORTRAIT_INVERTED,
                defaultDeviceRotation_ == 0 ? Rotation::ROTATION_180 : Rotation::ROTATION_270},
            {DeviceRotation::ROTATION_LANDSCAPE_INVERTED,
                defaultDeviceRotation_ == 1 ? Rotation::ROTATION_180 : Rotation::ROTATION_270},
        };
    }
    if (sensorToDeviceRotationMap_.empty()) {
        sensorToDeviceRotationMap_ = {
            {SensorRotation::ROTATION_0, CalcDeviceRotation(SensorRotation::ROTATION_0)},
            {SensorRotation::ROTATION_90, CalcDeviceRotation(SensorRotation::ROTATION_90)},
            {SensorRotation::ROTATION_180, CalcDeviceRotation(SensorRotation::ROTATION_180)},
            {SensorRotation::ROTATION_270, CalcDeviceRotation(SensorRotation::ROTATION_270)},
            {SensorRotation::INVALID, DeviceRotation::INVALID},
        };
    }
}

bool ScreenRotationController::IsDeviceRotationVertical(DeviceRotation deviceRotation)
{
    return (deviceRotation == DeviceRotation::ROTATION_PORTRAIT) ||
        (deviceRotation == DeviceRotation::ROTATION_PORTRAIT_INVERTED);
}

bool ScreenRotationController::IsDeviceRotationHorizontal(DeviceRotation deviceRotation)
{
    return (deviceRotation == DeviceRotation::ROTATION_LANDSCAPE) ||
        (deviceRotation == DeviceRotation::ROTATION_LANDSCAPE_INVERTED);
}

bool ScreenRotationController::IsCurrentDisplayVertical()
{
    return IsDisplayRotationVertical(GetCurrentDisplayRotation());
}

bool ScreenRotationController::IsCurrentDisplayHorizontal()
{
    return IsDisplayRotationHorizontal(GetCurrentDisplayRotation());
}

bool ScreenRotationController::IsDefaultDisplayRotationPortrait()
{
    return Rotation::ROTATION_0 == ConvertDeviceToDisplayRotation(DeviceRotation::ROTATION_PORTRAIT);
}

bool ScreenRotationController::IsDisplayRotationVertical(Rotation rotation)
{
    return (rotation == ConvertDeviceToDisplayRotation(DeviceRotation::ROTATION_PORTRAIT)) ||
        (rotation == ConvertDeviceToDisplayRotation(DeviceRotation::ROTATION_PORTRAIT_INVERTED));
}

bool ScreenRotationController::IsDisplayRotationHorizontal(Rotation rotation)
{
    return (rotation == ConvertDeviceToDisplayRotation(DeviceRotation::ROTATION_LANDSCAPE)) ||
        (rotation == ConvertDeviceToDisplayRotation(DeviceRotation::ROTATION_LANDSCAPE_INVERTED));
}

void ScreenRotationController::ProcessSwitchToSensorUnrelatedOrientation(Orientation orientation)
{
    if (lastOrientationType_ == orientation) {
        return;
    }
    lastOrientationType_ = orientation;
    switch (orientation) {
        case Orientation::UNSPECIFIED: {
            SetScreenRotation(Rotation::ROTATION_0);
            break;
        }
        case Orientation::VERTICAL: {
            SetScreenRotation(ConvertDeviceToDisplayRotation(DeviceRotation::ROTATION_PORTRAIT));
            break;
        }
        case Orientation::REVERSE_VERTICAL: {
            SetScreenRotation(ConvertDeviceToDisplayRotation(DeviceRotation::ROTATION_PORTRAIT_INVERTED));
            break;
        }
        case Orientation::HORIZONTAL: {
            SetScreenRotation(ConvertDeviceToDisplayRotation(DeviceRotation::ROTATION_LANDSCAPE));
            break;
        }
        case Orientation::REVERSE_HORIZONTAL: {
            SetScreenRotation(ConvertDeviceToDisplayRotation(DeviceRotation::ROTATION_LANDSCAPE_INVERTED));
            break;
        }
        default: {
            return;
        }
    }
}

void ScreenRotationController::ProcessOrientationSwitch(Orientation orientation)
{
    if (!IsSensorRelatedOrientation(orientation)) {
        ProcessSwitchToSensorUnrelatedOrientation(orientation);
    } else {
        ProcessSwitchToSensorRelatedOrientation(orientation, lastSensorRotationConverted_);
    }
}
} // Rosen
} // OHOS
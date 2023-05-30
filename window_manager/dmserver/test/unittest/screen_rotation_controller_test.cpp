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

#include <gtest/gtest.h>

#include "display_manager_service.h"
#include "sensor_connector.h"
#include "screen_rotation_controller.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class ScreenRotationControllerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void ScreenRotationControllerTest::SetUpTestCase()
{
    DisplayManagerService::GetInstance().abstractScreenController_->defaultRsScreenId_ = 0;
    DisplayManagerService::GetInstance().abstractScreenController_->screenIdManager_.rs2DmsScreenIdMap_.clear();
    DisplayManagerService::GetInstance().abstractScreenController_->screenIdManager_.rs2DmsScreenIdMap_ = {
        {0, 0}
    };
    DisplayManagerService::GetInstance().abstractScreenController_->screenIdManager_.dms2RsScreenIdMap_.clear();
    DisplayManagerService::GetInstance().abstractScreenController_->screenIdManager_.dms2RsScreenIdMap_ = {
        {0, 0}
    };

    std::string name = "testDisplay";
    sptr<SupportedScreenModes> info = new SupportedScreenModes();
    info->width_ = 100; // 100 width
    info->height_ = 200; // 200 height
    sptr<AbstractScreen> absScreen = new AbstractScreen(DisplayManagerService::GetInstance().abstractScreenController_,
        name, 0, 0);
    absScreen->activeIdx_ = 0;
    absScreen->modes_.clear();
    absScreen->modes_ = { { info } };
    sptr<AbstractDisplay> absDisplay = new AbstractDisplay(0, name, info, absScreen);
    DisplayManagerService::GetInstance().abstractDisplayController_->abstractDisplayMap_ = {
        {0, absDisplay}
    };
    DisplayManagerService::GetInstance().abstractScreenController_->dmsScreenMap_ = {
        {0, absScreen}
    };
}

void ScreenRotationControllerTest::TearDownTestCase()
{
}

void ScreenRotationControllerTest::SetUp()
{
}

void ScreenRotationControllerTest::TearDown()
{
}

namespace {
/**
 * @tc.name: GravitySensor
 * @tc.desc: Subscribe and unsubscribe gravity sensor
 * @tc.type: FUNC
 */
HWTEST_F(ScreenRotationControllerTest, GravitySensor, Function | SmallTest | Level3)
{
    GravitySensorSubscriber::isGravitySensorSubscribed_ = true;
    GravitySensorSubscriber::SubscribeGravitySensor();
    ASSERT_EQ(true, GravitySensorSubscriber::isGravitySensorSubscribed_);

    GravitySensorSubscriber::isGravitySensorSubscribed_ = false;
    GravitySensorSubscriber::SubscribeGravitySensor();

    GravitySensorSubscriber::isGravitySensorSubscribed_ = false;
    GravitySensorSubscriber::UnsubscribeGravitySensor();

    GravitySensorSubscriber::isGravitySensorSubscribed_ = true;
    GravitySensorSubscriber::UnsubscribeGravitySensor();
}

/**
 * @tc.name: ScreenRotationLocked
 * @tc.desc: Set and get screen rotation locked
 * @tc.type: FUNC
 */
HWTEST_F(ScreenRotationControllerTest, ScreenRotationLocked, Function | SmallTest | Level3)
{
    ScreenRotationController::SetScreenRotationLocked(false);
    ASSERT_EQ(false, ScreenRotationController::IsScreenRotationLocked());

    ScreenRotationController::SetScreenRotationLocked(true);
    ASSERT_EQ(true, ScreenRotationController::IsScreenRotationLocked());
}

/**
 * @tc.name: DefaultDeviceRotationOffset
 * @tc.desc: Set default device rotation offset
 * @tc.type: FUNC
 */
HWTEST_F(ScreenRotationControllerTest, DefaultDeviceRotationOffset, Function | SmallTest | Level3)
{
    ScreenRotationController::defaultDeviceRotationOffset_ = 90;

    ScreenRotationController::SetDefaultDeviceRotationOffset(360);
    ASSERT_EQ(90, ScreenRotationController::defaultDeviceRotationOffset_);

    ScreenRotationController::SetDefaultDeviceRotationOffset(10);
    ASSERT_EQ(90, ScreenRotationController::defaultDeviceRotationOffset_);

    ScreenRotationController::SetDefaultDeviceRotationOffset(180);
    ASSERT_EQ(180, ScreenRotationController::defaultDeviceRotationOffset_);
}

/**
 * @tc.name: CheckCallbackTimeInterval
 * @tc.desc: Check callbcak time interval
 * @tc.type: FUNC
 */
HWTEST_F(ScreenRotationControllerTest, CheckCallbackTimeInterval, Function | SmallTest | Level3)
{
    std::chrono::milliseconds ms = std::chrono::time_point_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now()).time_since_epoch();
    long currentTimeInMillitm = ms.count();

    GravitySensorSubscriber::lastCallbackTime_ = currentTimeInMillitm;
    ASSERT_EQ(false, GravitySensorSubscriber::CheckCallbackTimeInterval());

    GravitySensorSubscriber::lastCallbackTime_ = currentTimeInMillitm - 200;
    ASSERT_EQ(true, GravitySensorSubscriber::CheckCallbackTimeInterval());
}

/**
 * @tc.name: HandleGravitySensorEventCallback
 * @tc.desc: Handel gravity sensor event callback
 * @tc.type: FUNC
 */
HWTEST_F(ScreenRotationControllerTest, HandleGravitySensorEventCallback, Function | SmallTest | Level3)
{
    SensorEvent event;
    GravityData data = {0.f, 0.f, 0.f};
    event.sensorTypeId = SENSOR_TYPE_ID_NONE;
    event.data = reinterpret_cast<uint8_t*>(&data);

    std::chrono::milliseconds ms = std::chrono::time_point_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now()).time_since_epoch();
    long currentTimeInMillitm = ms.count();

    GravitySensorSubscriber::lastCallbackTime_ = currentTimeInMillitm;
    GravitySensorSubscriber::HandleGravitySensorEventCallback(&event);

    GravitySensorSubscriber::lastCallbackTime_ = currentTimeInMillitm - 200;
    GravitySensorSubscriber::HandleGravitySensorEventCallback(&event);

    event.sensorTypeId = SENSOR_TYPE_ID_GRAVITY;
    GravitySensorSubscriber::lastCallbackTime_ = currentTimeInMillitm - 200;
    GravitySensorSubscriber::HandleGravitySensorEventCallback(&event);

    data.z = 1.f;
    GravitySensorSubscriber::lastCallbackTime_ = currentTimeInMillitm - 200;
    GravitySensorSubscriber::HandleGravitySensorEventCallback(&event);
    ASSERT_EQ(DeviceRotation::INVALID, ScreenRotationController::lastSensorRotationConverted_);
}

/**
 * @tc.name: CalcRotationDegree
 * @tc.desc: Calc rotation degree
 * @tc.type: FUNC
 */
HWTEST_F(ScreenRotationControllerTest, CalcRotationDegree, Function | SmallTest | Level3)
{
    GravityData data0 = {0.f, 0.f, 0.f};
    ASSERT_EQ(270, GravitySensorSubscriber::CalcRotationDegree(&data0));

    GravityData data1 = {0.f, 0.f, 1.f};
    ASSERT_EQ(-1, GravitySensorSubscriber::CalcRotationDegree(&data1));

    GravityData data2 = {1.f, 1.f, 1.f};
    ASSERT_EQ(315, GravitySensorSubscriber::CalcRotationDegree(&data2));
}

/**
 * @tc.name: CalcTargetDisplayRotation
 * @tc.desc: Calc target display rotation
 * @tc.type: FUNC
 */
HWTEST_F(ScreenRotationControllerTest, CalcTargetDisplayRotation, Function | SmallTest | Level3)
{
    ScreenRotationController::ProcessRotationMapping();
    ScreenRotationController::currentDisplayRotation_ = Rotation::ROTATION_0;

    DeviceRotation deviceRitation = DeviceRotation::ROTATION_PORTRAIT;

    Orientation orientation = Orientation::SENSOR;
    ASSERT_EQ(Rotation::ROTATION_0, ScreenRotationController::CalcTargetDisplayRotation(orientation, deviceRitation));

    orientation = Orientation::SENSOR_VERTICAL;
    ASSERT_EQ(Rotation::ROTATION_0, ScreenRotationController::CalcTargetDisplayRotation(orientation, deviceRitation));

    orientation = Orientation::SENSOR_HORIZONTAL;
    ASSERT_EQ(Rotation::ROTATION_0, ScreenRotationController::CalcTargetDisplayRotation(orientation, deviceRitation));

    orientation = Orientation::UNSPECIFIED;
    ScreenRotationController::isScreenRotationLocked_ = true;
    ASSERT_EQ(Rotation::ROTATION_0, ScreenRotationController::CalcTargetDisplayRotation(orientation, deviceRitation));
    ScreenRotationController::isScreenRotationLocked_ = false;
    ASSERT_EQ(Rotation::ROTATION_0, ScreenRotationController::CalcTargetDisplayRotation(orientation, deviceRitation));

    orientation = Orientation::AUTO_ROTATION_RESTRICTED;
    ScreenRotationController::isScreenRotationLocked_ = true;
    ASSERT_EQ(Rotation::ROTATION_0, ScreenRotationController::CalcTargetDisplayRotation(orientation, deviceRitation));
    ScreenRotationController::isScreenRotationLocked_ = false;
    ASSERT_EQ(Rotation::ROTATION_0, ScreenRotationController::CalcTargetDisplayRotation(orientation, deviceRitation));

    orientation = Orientation::AUTO_ROTATION_PORTRAIT_RESTRICTED;
    ScreenRotationController::isScreenRotationLocked_ = true;
    ASSERT_EQ(Rotation::ROTATION_0, ScreenRotationController::CalcTargetDisplayRotation(orientation, deviceRitation));
    ScreenRotationController::isScreenRotationLocked_ = false;
    ASSERT_EQ(Rotation::ROTATION_0, ScreenRotationController::CalcTargetDisplayRotation(orientation, deviceRitation));

    orientation = Orientation::AUTO_ROTATION_LANDSCAPE_RESTRICTED;
    ScreenRotationController::isScreenRotationLocked_ = true;
    ASSERT_EQ(Rotation::ROTATION_0, ScreenRotationController::CalcTargetDisplayRotation(orientation, deviceRitation));
    ScreenRotationController::isScreenRotationLocked_ = false;
    ASSERT_EQ(Rotation::ROTATION_0, ScreenRotationController::CalcTargetDisplayRotation(orientation, deviceRitation));

    orientation = Orientation::VERTICAL;
    ASSERT_EQ(Rotation::ROTATION_0, ScreenRotationController::CalcTargetDisplayRotation(orientation, deviceRitation));
}

/**
 * @tc.name: ProcessAutoRotation
 * @tc.desc: Process auto rotation
 * @tc.type: FUNC
 */
HWTEST_F(ScreenRotationControllerTest, ProcessAutoRotation, Function | SmallTest | Level3)
{
    ScreenRotationController::currentDisplayRotation_ = Rotation::ROTATION_90;
    ScreenRotationController::ProcessRotationMapping();

    DeviceRotation deviceRitation = DeviceRotation::ROTATION_LANDSCAPE;
    ASSERT_EQ(Rotation::ROTATION_90, ScreenRotationController::ProcessAutoRotationPortraitOrientation(deviceRitation));
    deviceRitation = DeviceRotation::ROTATION_LANDSCAPE_INVERTED;
    ASSERT_EQ(Rotation::ROTATION_90, ScreenRotationController::ProcessAutoRotationPortraitOrientation(deviceRitation));
    deviceRitation = DeviceRotation::ROTATION_PORTRAIT_INVERTED;
    ASSERT_EQ(Rotation::ROTATION_180, ScreenRotationController::ProcessAutoRotationPortraitOrientation(deviceRitation));

    deviceRitation = DeviceRotation::ROTATION_PORTRAIT;
    ASSERT_EQ(Rotation::ROTATION_90, ScreenRotationController::ProcessAutoRotationLandscapeOrientation(deviceRitation));
    deviceRitation = DeviceRotation::ROTATION_PORTRAIT_INVERTED;
    ASSERT_EQ(Rotation::ROTATION_90, ScreenRotationController::ProcessAutoRotationLandscapeOrientation(deviceRitation));
    deviceRitation = DeviceRotation::ROTATION_LANDSCAPE;
    ASSERT_EQ(Rotation::ROTATION_90, ScreenRotationController::ProcessAutoRotationLandscapeOrientation(deviceRitation));
}

/**
 * @tc.name: CalcDeviceRotation
 * @tc.desc: Calc device rotation
 * @tc.type: FUNC
 */
HWTEST_F(ScreenRotationControllerTest, CalcDeviceRotation, Function | SmallTest | Level3)
{
    SensorRotation rotation = SensorRotation::INVALID;
    ASSERT_EQ(DeviceRotation::INVALID, ScreenRotationController::CalcDeviceRotation(rotation));

    rotation = SensorRotation::ROTATION_0;
    ScreenRotationController::defaultDeviceRotationOffset_ = 0;
    ScreenRotationController::defaultDeviceRotation_ = 0;
    ASSERT_EQ(DeviceRotation::ROTATION_PORTRAIT, ScreenRotationController::CalcDeviceRotation(rotation));

    rotation = SensorRotation::ROTATION_0;
    ScreenRotationController::defaultDeviceRotationOffset_ = 90;
    ScreenRotationController::defaultDeviceRotation_ = 0;
    ASSERT_EQ(DeviceRotation::ROTATION_LANDSCAPE_INVERTED, ScreenRotationController::CalcDeviceRotation(rotation));

    rotation = SensorRotation::ROTATION_0;
    ScreenRotationController::defaultDeviceRotationOffset_ = 90;
    ScreenRotationController::defaultDeviceRotation_ = 1;
    ASSERT_EQ(DeviceRotation::ROTATION_PORTRAIT_INVERTED, ScreenRotationController::CalcDeviceRotation(rotation));

    rotation = SensorRotation::ROTATION_0;
    ScreenRotationController::defaultDeviceRotationOffset_ = 0;
    ScreenRotationController::defaultDeviceRotation_ = 1;
    ASSERT_EQ(DeviceRotation::ROTATION_LANDSCAPE, ScreenRotationController::CalcDeviceRotation(rotation));
}

/**
 * @tc.name: IsSensorRelatedOrientation
 * @tc.desc: Is sensor related orientation
 * @tc.type: FUNC
 */
HWTEST_F(ScreenRotationControllerTest, IsSensorRelatedOrientation, Function | SmallTest | Level3)
{
    ASSERT_EQ(false, ScreenRotationController::IsSensorRelatedOrientation(Orientation::LOCKED));
    ASSERT_EQ(false, ScreenRotationController::IsSensorRelatedOrientation(Orientation::UNSPECIFIED));
    ASSERT_EQ(false, ScreenRotationController::IsSensorRelatedOrientation(Orientation::VERTICAL));
    ASSERT_EQ(false, ScreenRotationController::IsSensorRelatedOrientation(Orientation::HORIZONTAL));
    ASSERT_EQ(false, ScreenRotationController::IsSensorRelatedOrientation(Orientation::REVERSE_VERTICAL));
    ASSERT_EQ(false, ScreenRotationController::IsSensorRelatedOrientation(Orientation::REVERSE_HORIZONTAL));
    ASSERT_EQ(true, ScreenRotationController::IsSensorRelatedOrientation(Orientation::SENSOR));
    ASSERT_EQ(true, ScreenRotationController::IsSensorRelatedOrientation(Orientation::SENSOR_VERTICAL));
    ASSERT_EQ(true, ScreenRotationController::IsSensorRelatedOrientation(Orientation::SENSOR_HORIZONTAL));
    ASSERT_EQ(true, ScreenRotationController::IsSensorRelatedOrientation(Orientation::AUTO_ROTATION_RESTRICTED));
    ASSERT_EQ(true, ScreenRotationController::IsSensorRelatedOrientation(
        Orientation::AUTO_ROTATION_PORTRAIT_RESTRICTED));
    ASSERT_EQ(true, ScreenRotationController::IsSensorRelatedOrientation(
        Orientation::AUTO_ROTATION_LANDSCAPE_RESTRICTED));
}

/**
 * @tc.name: ProcessSwitchToSensorRelatedOrientation
 * @tc.desc: Process switch to sensor related orientation
 * @tc.type: FUNC
 */
HWTEST_F(ScreenRotationControllerTest, ProcessSwitchToSensorRelatedOrientation, Function | SmallTest | Level3)
{
    Orientation orientation = Orientation::SENSOR;
    DeviceRotation deviceRitation = DeviceRotation::ROTATION_LANDSCAPE;

    ScreenRotationController::lastOrientationType_ = Orientation::SENSOR;
    ScreenRotationController::ProcessSwitchToSensorRelatedOrientation(orientation, deviceRitation);

    ScreenRotationController::lastOrientationType_ = Orientation::UNSPECIFIED;
    ScreenRotationController::isScreenRotationLocked_ = true;
    orientation = Orientation::AUTO_ROTATION_RESTRICTED;
    ScreenRotationController::ProcessSwitchToSensorRelatedOrientation(orientation, deviceRitation);

    ScreenRotationController::lastOrientationType_ = Orientation::UNSPECIFIED;
    ScreenRotationController::isScreenRotationLocked_ = false;
    orientation = Orientation::AUTO_ROTATION_RESTRICTED;
    ScreenRotationController::ProcessSwitchToSensorRelatedOrientation(orientation, deviceRitation);

    ScreenRotationController::lastOrientationType_ = Orientation::UNSPECIFIED;
    orientation = Orientation::SENSOR;
    ScreenRotationController::ProcessSwitchToSensorRelatedOrientation(orientation, deviceRitation);

    ScreenRotationController::lastOrientationType_ = Orientation::UNSPECIFIED;
    ScreenRotationController::isScreenRotationLocked_ = true;
    orientation = Orientation::AUTO_ROTATION_PORTRAIT_RESTRICTED;
    ScreenRotationController::ProcessSwitchToSensorRelatedOrientation(orientation, deviceRitation);

    ScreenRotationController::lastOrientationType_ = Orientation::UNSPECIFIED;
    ScreenRotationController::isScreenRotationLocked_ = false;
    orientation = Orientation::AUTO_ROTATION_PORTRAIT_RESTRICTED;
    ScreenRotationController::ProcessSwitchToSensorRelatedOrientation(orientation, deviceRitation);

    ScreenRotationController::lastOrientationType_ = Orientation::UNSPECIFIED;
    orientation = Orientation::SENSOR_VERTICAL;
    ScreenRotationController::ProcessSwitchToSensorRelatedOrientation(orientation, deviceRitation);

    ScreenRotationController::lastOrientationType_ = Orientation::UNSPECIFIED;
    ScreenRotationController::isScreenRotationLocked_ = true;
    orientation = Orientation::AUTO_ROTATION_LANDSCAPE_RESTRICTED;
    ScreenRotationController::ProcessSwitchToSensorRelatedOrientation(orientation, deviceRitation);

    ScreenRotationController::lastOrientationType_ = Orientation::UNSPECIFIED;
    ScreenRotationController::isScreenRotationLocked_ = false;
    orientation = Orientation::AUTO_ROTATION_LANDSCAPE_RESTRICTED;
    ScreenRotationController::ProcessSwitchToSensorRelatedOrientation(orientation, deviceRitation);

    ScreenRotationController::lastOrientationType_ = Orientation::UNSPECIFIED;
    orientation = Orientation::SENSOR_HORIZONTAL;
    ScreenRotationController::ProcessSwitchToSensorRelatedOrientation(orientation, deviceRitation);
    ASSERT_EQ(Orientation::SENSOR_HORIZONTAL, ScreenRotationController::lastOrientationType_);

    ScreenRotationController::lastOrientationType_ = Orientation::UNSPECIFIED;
    orientation = Orientation::LOCKED;
    ScreenRotationController::ProcessSwitchToSensorRelatedOrientation(orientation, deviceRitation);
    ASSERT_EQ(Orientation::LOCKED, ScreenRotationController::lastOrientationType_);
}

/**
 * @tc.name: ProcessSwitchToAutoRotation
 * @tc.desc: Process switch to auto rotation
 * @tc.type: FUNC
 */
HWTEST_F(ScreenRotationControllerTest, ProcessSwitchToAutoRotation, Function | SmallTest | Level3)
{
    DeviceRotation deviceRotation = DeviceRotation::INVALID;
    ScreenRotationController::ProcessSwitchToAutoRotation(deviceRotation);
    deviceRotation = DeviceRotation::ROTATION_PORTRAIT;
    ScreenRotationController::ProcessSwitchToAutoRotation(deviceRotation);

    deviceRotation = DeviceRotation::INVALID;
    ScreenRotationController::ProcessSwitchToAutoRotationPortrait(deviceRotation);
    deviceRotation = DeviceRotation::ROTATION_PORTRAIT;
    ScreenRotationController::ProcessSwitchToAutoRotationPortrait(deviceRotation);

    deviceRotation = DeviceRotation::INVALID;
    ScreenRotationController::ProcessSwitchToAutoRotationLandscape(deviceRotation);
    deviceRotation = DeviceRotation::ROTATION_LANDSCAPE;
    ScreenRotationController::ProcessSwitchToAutoRotationLandscape(deviceRotation);

    ScreenRotationController::ProcessSwitchToAutoRotationPortraitRestricted();
    ScreenRotationController::ProcessSwitchToAutoRotationLandscapeRestricted();
}

/**
 * @tc.name: CalcSensorRotation
 * @tc.desc: Calc sensor rotation
 * @tc.type: FUNC
 */
HWTEST_F(ScreenRotationControllerTest, CalcSensorRotation, Function | SmallTest | Level3)
{
    ASSERT_EQ(SensorRotation::INVALID, GravitySensorSubscriber::CalcSensorRotation(-30));
    ASSERT_EQ(SensorRotation::ROTATION_0, GravitySensorSubscriber::CalcSensorRotation(15));
    ASSERT_EQ(SensorRotation::ROTATION_0, GravitySensorSubscriber::CalcSensorRotation(345));
    ASSERT_EQ(SensorRotation::ROTATION_90, GravitySensorSubscriber::CalcSensorRotation(75));
    ASSERT_EQ(SensorRotation::ROTATION_180, GravitySensorSubscriber::CalcSensorRotation(180));
    ASSERT_EQ(SensorRotation::ROTATION_270, GravitySensorSubscriber::CalcSensorRotation(270));
    ASSERT_EQ(SensorRotation::ROTATION_0, GravitySensorSubscriber::CalcSensorRotation(600));
}

/**
 * @tc.name: ConvertRotation
 * @tc.desc: Convert rotation
 * @tc.type: FUNC
 */
HWTEST_F(ScreenRotationControllerTest, ConvertRotation, Function | SmallTest | Level3)
{
    ScreenRotationController::sensorToDeviceRotationMap_.clear();
    SensorRotation rotation = SensorRotation::INVALID;
    ASSERT_EQ(DeviceRotation::INVALID, ScreenRotationController::ConvertSensorToDeviceRotation(rotation));
    ASSERT_EQ(DeviceRotation::INVALID, ScreenRotationController::ConvertSensorToDeviceRotation(rotation));

    DeviceRotation deviceRotation = DeviceRotation::INVALID;
    ASSERT_EQ(Rotation::ROTATION_0, ScreenRotationController::ConvertDeviceToDisplayRotation(deviceRotation));

    ScreenRotationController::deviceToDisplayRotationMap_.clear();
    deviceRotation = DeviceRotation::ROTATION_PORTRAIT;
    ASSERT_EQ(Rotation::ROTATION_0, ScreenRotationController::ConvertDeviceToDisplayRotation(deviceRotation));
    ASSERT_EQ(Rotation::ROTATION_0, ScreenRotationController::ConvertDeviceToDisplayRotation(deviceRotation));
}

/**
 * @tc.name: IsDeviceRotationVerticalOrHorizontal
 * @tc.desc: Check device rotation
 * @tc.type: FUNC
 */
HWTEST_F(ScreenRotationControllerTest, IsDeviceRotationVerticalOrHorizontal, Function | SmallTest | Level3)
{
    ASSERT_EQ(true, ScreenRotationController::IsDeviceRotationVertical(DeviceRotation::ROTATION_PORTRAIT));
    ASSERT_EQ(true, ScreenRotationController::IsDeviceRotationVertical(DeviceRotation::ROTATION_PORTRAIT_INVERTED));
    ASSERT_EQ(false, ScreenRotationController::IsDeviceRotationVertical(DeviceRotation::ROTATION_LANDSCAPE));

    ASSERT_EQ(true, ScreenRotationController::IsDeviceRotationHorizontal(DeviceRotation::ROTATION_LANDSCAPE));
    ASSERT_EQ(true, ScreenRotationController::IsDeviceRotationHorizontal(DeviceRotation::ROTATION_LANDSCAPE_INVERTED));
    ASSERT_EQ(false, ScreenRotationController::IsDeviceRotationHorizontal(DeviceRotation::ROTATION_PORTRAIT));
}

/**
 * @tc.name: ProcessOrientationSwitch
 * @tc.desc: Process orientation switch
 * @tc.type: FUNC
 */
HWTEST_F(ScreenRotationControllerTest, ProcessOrientationSwitch, Function | SmallTest | Level3)
{
    ScreenRotationController::ProcessOrientationSwitch(Orientation::UNSPECIFIED);
    ScreenRotationController::ProcessOrientationSwitch(Orientation::VERTICAL);
    ScreenRotationController::ProcessOrientationSwitch(Orientation::HORIZONTAL);
    ScreenRotationController::ProcessOrientationSwitch(Orientation::REVERSE_VERTICAL);
    ScreenRotationController::ProcessOrientationSwitch(Orientation::SENSOR);
    ScreenRotationController::ProcessOrientationSwitch(Orientation::SENSOR_VERTICAL);
    ScreenRotationController::ProcessOrientationSwitch(Orientation::SENSOR_HORIZONTAL);
    ScreenRotationController::ProcessOrientationSwitch(Orientation::AUTO_ROTATION_RESTRICTED);
    ScreenRotationController::ProcessOrientationSwitch(Orientation::AUTO_ROTATION_PORTRAIT_RESTRICTED);
    ScreenRotationController::ProcessOrientationSwitch(Orientation::AUTO_ROTATION_LANDSCAPE_RESTRICTED);
    ScreenRotationController::ProcessOrientationSwitch(Orientation::LOCKED);
    ASSERT_EQ(Orientation::LOCKED, ScreenRotationController::lastOrientationType_);
}
#ifdef WM_SUBSCRIBE_MOTION_ENABLE
/**
 * @tc.name: SubscribeMotionSensor
 * @tc.desc: check function MotionSubscriber::SubscribeMotionSensor
 * @tc.type: FUNC
 */
HWTEST_F(ScreenRotationControllerTest, SubscribeMotionSensor, Function | SmallTest | Level3)
{
    MotionSubscriber::isMotionSensorSubscribed_ = true;
    MotionSubscriber::SubscribeMotionSensor();
    ASSERT_EQ(true, MotionSubscriber::isMotionSensorSubscribed_);

    MotionSubscriber::isMotionSensorSubscribed_ = false;
    MotionSubscriber::SubscribeMotionSensor();
    ASSERT_EQ(true, MotionSubscriber::isMotionSensorSubscribed_);

    MotionSubscriber::isMotionSensorSubscribed_ = false;
    MotionSubscriber::UnsubscribeMotionSensor();
    ASSERT_EQ(false, MotionSubscriber::isMotionSensorSubscribed_);

    MotionSubscriber::isMotionSensorSubscribed_ = true;
    MotionSubscriber::UnsubscribeMotionSensor();
    ASSERT_EQ(false, MotionSubscriber::isMotionSensorSubscribed_);
}

/**
 * @tc.name: OnMotionChanged
 * @tc.desc: check function RotationMotionEventCallback->SubscribeMotionSensor
 * @tc.type: FUNC
 */
HWTEST_F(ScreenRotationControllerTest, OnMotionChanged, Function | SmallTest | Level3)
{
    DeviceRotation currentRotation = ScreenRotationController::lastSensorRotationConverted_;
    DeviceRotation motionRotation = DeviceRotation::INVALID;

    MotionData motionData;
    motionData.result = OHOS::Msdp::MotionStatusResult::VALUE_INVALID;
    motionData.rotateAction = 1;
    MotionSubscriber::motionEventCallback_->OnMotionChanged(motionData);
    ASSERT_EQ(currentRotation, ScreenRotationController::lastSensorRotationConverted_);

    motionData.result = OHOS::Msdp::MotionStatusResult::VALUE_ENTER;
    motionData.rotateAction = 0;
    motionRotation = DeviceRotation::ROTATION_PORTRAIT;
    MotionSubscriber::motionEventCallback_->OnMotionChanged(motionData);
    ASSERT_EQ(motionRotation, ScreenRotationController::lastSensorRotationConverted_);

    motionData.rotateAction = 1;
    MotionSubscriber::motionEventCallback_->OnMotionChanged(motionData);
    motionRotation = ScreenRotationController::IsDefaultDisplayRotationPortrait() ?
        DeviceRotation::ROTATION_LANDSCAPE_INVERTED : DeviceRotation::ROTATION_LANDSCAPE;
    ASSERT_EQ(motionRotation, ScreenRotationController::lastSensorRotationConverted_);

    motionData.rotateAction = 2;
    MotionSubscriber::motionEventCallback_->OnMotionChanged(motionData);
    motionRotation = DeviceRotation::ROTATION_PORTRAIT_INVERTED;
    ASSERT_EQ(motionRotation, ScreenRotationController::lastSensorRotationConverted_);

    motionData.rotateAction = 3;
    MotionSubscriber::motionEventCallback_->OnMotionChanged(motionData);
    motionRotation = ScreenRotationController::IsDefaultDisplayRotationPortrait() ?
        DeviceRotation::ROTATION_LANDSCAPE : DeviceRotation::ROTATION_LANDSCAPE_INVERTED;
    ASSERT_EQ(motionRotation, ScreenRotationController::lastSensorRotationConverted_);

    ScreenRotationController::HandleSensorEventInput(currentRotation);
}
#endif
}
} // namespace Rosen
} // namespace OHOS

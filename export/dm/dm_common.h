/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef OHOS_ROSEN_DM_COMMON_H
#define OHOS_ROSEN_DM_COMMON_H

#include <refbase.h>
#include <string>
#include <map>

namespace OHOS {
namespace Rosen {
using DisplayId = uint64_t;
using ScreenId = uint64_t;
namespace {
    constexpr DisplayId DISPLAY_ID_INVALID = -1ULL;
    constexpr ScreenId SCREEN_ID_INVALID = -1ULL;
    constexpr int DOT_PER_INCH = 160;
    const static std::string DEFAULT_SCREEN_NAME = "buildIn";
    constexpr int DOT_PER_INCH_MAXIMUM_VALUE = 640;
    constexpr int DOT_PER_INCH_MINIMUM_VALUE = 80;
    constexpr uint32_t BASELINE_DENSITY = 160;
}

enum class PowerStateChangeReason : uint32_t {
    POWER_BUTTON,
};

enum class ScreenPowerState : uint32_t {
    POWER_ON,
    POWER_STAND_BY,
    POWER_SUSPEND,
    POWER_OFF,
    POWER_BUTT,
    INVALID_STATE,
};

enum class DisplayState : uint32_t {
    ON,
    OFF,
    UNKNOWN,
};

enum class DisplayEvent : uint32_t {
    UNLOCK,
    KEYGUARD_DRAWN,
};

enum class DMError : int32_t {
    DM_OK = 0,
    DM_ERROR_INIT_DMS_PROXY_LOCKED = 100,
    DM_ERROR_IPC_FAILED = 101,
    DM_ERROR_REMOTE_CREATE_FAILED = 110,
    DM_ERROR_NULLPTR = 120,
    DM_ERROR_INVALID_PARAM = 130,
    DM_ERROR_WRITE_INTERFACE_TOKEN_FAILED = 140,
    DM_ERROR_DEATH_RECIPIENT = 150,
    DM_ERROR_INVALID_MODE_ID = 160,
    DM_ERROR_WRITE_DATA_FAILED = 170,
    DM_ERROR_RENDER_SERVICE_FAILED = 180,
    DM_ERROR_UNREGISTER_AGENT_FAILED = 190,
    DM_ERROR_INVALID_CALLING = 200,
    DM_ERROR_INVALID_PERMISSION = 201,
    DM_ERROR_UNKNOWN = -1,
};

enum class DmErrorCode : int32_t {
    DM_OK = 0,
    DM_ERROR_NO_PERMISSION = 201,
    DM_ERROR_INVALID_PARAM = 401,
    DM_ERROR_DEVICE_NOT_SUPPORT = 801,
    DM_ERROR_INVALID_SCREEN = 1400001,
    DM_ERROR_INVALID_CALLING = 1400002,
    DM_ERROR_SYSTEM_INNORMAL = 1400003,
};

const std::map<DMError, DmErrorCode> DM_JS_TO_ERROR_CODE_MAP {
    {DMError::DM_OK,                                    DmErrorCode::DM_OK                          },
    {DMError::DM_ERROR_INVALID_PERMISSION,              DmErrorCode::DM_ERROR_NO_PERMISSION         },
    {DMError::DM_ERROR_INIT_DMS_PROXY_LOCKED,           DmErrorCode::DM_ERROR_SYSTEM_INNORMAL       },
    {DMError::DM_ERROR_IPC_FAILED,                      DmErrorCode::DM_ERROR_SYSTEM_INNORMAL       },
    {DMError::DM_ERROR_REMOTE_CREATE_FAILED,            DmErrorCode::DM_ERROR_SYSTEM_INNORMAL       },
    {DMError::DM_ERROR_NULLPTR,                         DmErrorCode::DM_ERROR_INVALID_SCREEN        },
    {DMError::DM_ERROR_INVALID_PARAM,                   DmErrorCode::DM_ERROR_INVALID_PARAM         },
    {DMError::DM_ERROR_WRITE_INTERFACE_TOKEN_FAILED,    DmErrorCode::DM_ERROR_SYSTEM_INNORMAL       },
    {DMError::DM_ERROR_DEATH_RECIPIENT,                 DmErrorCode::DM_ERROR_SYSTEM_INNORMAL       },
    {DMError::DM_ERROR_INVALID_MODE_ID,                 DmErrorCode::DM_ERROR_SYSTEM_INNORMAL       },
    {DMError::DM_ERROR_WRITE_DATA_FAILED,               DmErrorCode::DM_ERROR_SYSTEM_INNORMAL       },
    {DMError::DM_ERROR_RENDER_SERVICE_FAILED,           DmErrorCode::DM_ERROR_SYSTEM_INNORMAL       },
    {DMError::DM_ERROR_UNREGISTER_AGENT_FAILED,         DmErrorCode::DM_ERROR_SYSTEM_INNORMAL       },
    {DMError::DM_ERROR_INVALID_CALLING,                 DmErrorCode::DM_ERROR_INVALID_CALLING       },
    {DMError::DM_ERROR_UNKNOWN,                         DmErrorCode::DM_ERROR_SYSTEM_INNORMAL       },
};

using DisplayStateCallback = std::function<void(DisplayState)>;

enum class DisplayPowerEvent : uint32_t {
    WAKE_UP,
    SLEEP,
    DISPLAY_ON,
    DISPLAY_OFF,
    DESKTOP_READY,
};

enum class EventStatus : uint32_t {
    BEGIN,
    END,
};

class IDisplayPowerEventListener : public RefBase {
public:
    virtual void OnDisplayPowerEvent(DisplayPowerEvent event, EventStatus status) = 0;
};

enum class ScreenChangeEvent : uint32_t {
    UPDATE_ORIENTATION,
    UPDATE_ROTATION,
    CHANGE_MODE,
    VIRTUAL_PIXEL_RATIO_CHANGED,
};

enum class ScreenGroupChangeEvent : uint32_t {
    ADD_TO_GROUP,
    REMOVE_FROM_GROUP,
    CHANGE_GROUP,
};

enum class Rotation : uint32_t {
    ROTATION_0,
    ROTATION_90,
    ROTATION_180,
    ROTATION_270,
};

enum class Orientation : uint32_t {
    BEGIN = 0,
    UNSPECIFIED = BEGIN,
    VERTICAL = 1,
    HORIZONTAL = 2,
    REVERSE_VERTICAL = 3,
    REVERSE_HORIZONTAL = 4,
    SENSOR = 5,
    SENSOR_VERTICAL = 6,
    SENSOR_HORIZONTAL = 7,
    AUTO_ROTATION_RESTRICTED = 8,
    AUTO_ROTATION_PORTRAIT_RESTRICTED = 9,
    AUTO_ROTATION_LANDSCAPE_RESTRICTED = 10,
    LOCKED = 11,
    END = LOCKED,
};

enum class DisplayChangeEvent : uint32_t {
    UPDATE_ORIENTATION,
    UPDATE_ROTATION,
    DISPLAY_SIZE_CHANGED,
    DISPLAY_FREEZED,
    DISPLAY_UNFREEZED,
    DISPLAY_VIRTUAL_PIXEL_RATIO_CHANGED,
    UNKNOWN,
};
}
}
#endif // OHOS_ROSEN_DM_COMMON_H
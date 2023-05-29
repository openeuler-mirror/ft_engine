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

#ifndef OHOS_ROSEN_WM_COMMON_H
#define OHOS_ROSEN_WM_COMMON_H

#include <parcel.h>
#include <map>

namespace OHOS {
namespace Rosen {
using DisplayId = uint64_t;
enum class WindowType : uint32_t {
    APP_WINDOW_BASE = 1,
    APP_MAIN_WINDOW_BASE = APP_WINDOW_BASE,
    WINDOW_TYPE_APP_MAIN_WINDOW = APP_MAIN_WINDOW_BASE,
    APP_MAIN_WINDOW_END,

    APP_SUB_WINDOW_BASE = 1000,
    WINDOW_TYPE_MEDIA = APP_SUB_WINDOW_BASE,
    WINDOW_TYPE_APP_SUB_WINDOW,
    WINDOW_TYPE_APP_COMPONENT,
    APP_SUB_WINDOW_END,
    APP_WINDOW_END = APP_SUB_WINDOW_END,

    SYSTEM_WINDOW_BASE = 2000,
    BELOW_APP_SYSTEM_WINDOW_BASE = SYSTEM_WINDOW_BASE,
    WINDOW_TYPE_WALLPAPER = SYSTEM_WINDOW_BASE,
    WINDOW_TYPE_DESKTOP,
    BELOW_APP_SYSTEM_WINDOW_END,

    ABOVE_APP_SYSTEM_WINDOW_BASE = 2100,
    WINDOW_TYPE_APP_LAUNCHING = ABOVE_APP_SYSTEM_WINDOW_BASE,
    WINDOW_TYPE_DOCK_SLICE,
    WINDOW_TYPE_INCOMING_CALL,
    WINDOW_TYPE_SEARCHING_BAR,
    WINDOW_TYPE_SYSTEM_ALARM_WINDOW,
    WINDOW_TYPE_INPUT_METHOD_FLOAT,
    WINDOW_TYPE_FLOAT,
    WINDOW_TYPE_TOAST,
    WINDOW_TYPE_STATUS_BAR,
    WINDOW_TYPE_PANEL,
    WINDOW_TYPE_KEYGUARD,
    WINDOW_TYPE_VOLUME_OVERLAY,
    WINDOW_TYPE_NAVIGATION_BAR,
    WINDOW_TYPE_DRAGGING_EFFECT,
    WINDOW_TYPE_POINTER,
    WINDOW_TYPE_LAUNCHER_RECENT,
    WINDOW_TYPE_LAUNCHER_DOCK,
    WINDOW_TYPE_BOOT_ANIMATION,
    WINDOW_TYPE_FREEZE_DISPLAY,
    WINDOW_TYPE_VOICE_INTERACTION,
    WINDOW_TYPE_FLOAT_CAMERA,
    WINDOW_TYPE_PLACEHOLDER,
    WINDOW_TYPE_DIALOG,
    WINDOW_TYPE_SCREENSHOT,
    ABOVE_APP_SYSTEM_WINDOW_END,

    SYSTEM_SUB_WINDOW_BASE = 2500,
    WINDOW_TYPE_SYSTEM_SUB_WINDOW = SYSTEM_SUB_WINDOW_BASE,
    SYSTEM_SUB_WINDOW_END,

    SYSTEM_WINDOW_END = SYSTEM_SUB_WINDOW_END,
};

enum class WindowMode : uint32_t {
    WINDOW_MODE_UNDEFINED = 0,
    WINDOW_MODE_FULLSCREEN = 1,
    WINDOW_MODE_SPLIT_PRIMARY = 100,
    WINDOW_MODE_SPLIT_SECONDARY,
    WINDOW_MODE_FLOATING,
    WINDOW_MODE_PIP
};

enum WindowModeSupport : uint32_t {
    WINDOW_MODE_SUPPORT_FULLSCREEN = 1 << 0,
    WINDOW_MODE_SUPPORT_FLOATING = 1 << 1,
    WINDOW_MODE_SUPPORT_SPLIT_PRIMARY = 1 << 2,
    WINDOW_MODE_SUPPORT_SPLIT_SECONDARY = 1 << 3,
    WINDOW_MODE_SUPPORT_PIP = 1 << 4,
    WINDOW_MODE_SUPPORT_ALL = WINDOW_MODE_SUPPORT_FULLSCREEN |
                              WINDOW_MODE_SUPPORT_SPLIT_PRIMARY |
                              WINDOW_MODE_SUPPORT_SPLIT_SECONDARY |
                              WINDOW_MODE_SUPPORT_FLOATING |
                              WINDOW_MODE_SUPPORT_PIP
};

enum class WindowBlurStyle : uint32_t {
    WINDOW_BLUR_OFF = 0,
    WINDOW_BLUR_THIN,
    WINDOW_BLUR_REGULAR,
    WINDOW_BLUR_THICK
};

enum class WindowState : uint32_t {
    STATE_INITIAL,
    STATE_CREATED,
    STATE_SHOWN,
    STATE_HIDDEN,
    STATE_FROZEN,
    STATE_DESTROYED,
    STATE_BOTTOM = STATE_DESTROYED,
    STATE_UNFROZEN,
};

enum class WMError : int32_t {
    WM_OK = 0,
    WM_DO_NOTHING,
    WM_ERROR_NO_MEM,
    WM_ERROR_DESTROYED_OBJECT,
    WM_ERROR_INVALID_WINDOW,
    WM_ERROR_INVALID_WINDOW_MODE_OR_SIZE,
    WM_ERROR_INVALID_OPERATION,
    WM_ERROR_INVALID_PERMISSION,
    WM_ERROR_NO_REMOTE_ANIMATION,

    WM_ERROR_DEVICE_NOT_SUPPORT = 801, // the value do not change.It is defined on all system

    WM_ERROR_NEED_REPORT_BASE = 1000, // error code > 1000 means need report
    WM_ERROR_NULLPTR,
    WM_ERROR_INVALID_TYPE,
    WM_ERROR_INVALID_PARAM,
    WM_ERROR_SAMGR,
    WM_ERROR_IPC_FAILED,
    WM_ERROR_NEED_REPORT_END,
    WM_ERROR_START_ABILITY_FAILED,
};

enum class WmErrorCode : int32_t {
    WM_OK = 0,
    WM_ERROR_NO_PERMISSION = 201,
    WM_ERROR_INVALID_PARAM = 401,
    WM_ERROR_DEVICE_NOT_SUPPORT = 801,
    WM_ERROR_REPEAT_OPERATION = 1300001,
    WM_ERROR_STATE_ABNORMALLY = 1300002,
    WM_ERROR_SYSTEM_ABNORMALLY = 1300003,
    WM_ERROR_INVALID_CALLING = 1300004,
    WM_ERROR_STAGE_ABNORMALLY = 1300005,
    WM_ERROR_CONTEXT_ABNORMALLY = 1300006,
    WM_ERROR_START_ABILITY_FAILED = 1300007,
};

const std::map<WMError, WmErrorCode> WM_JS_TO_ERROR_CODE_MAP {
    {WMError::WM_OK,                                   WmErrorCode::WM_OK                         },
    {WMError::WM_DO_NOTHING,                           WmErrorCode::WM_ERROR_STATE_ABNORMALLY     },
    {WMError::WM_ERROR_DESTROYED_OBJECT,               WmErrorCode::WM_ERROR_STATE_ABNORMALLY     },
    {WMError::WM_ERROR_DEVICE_NOT_SUPPORT,             WmErrorCode::WM_ERROR_DEVICE_NOT_SUPPORT   },
    {WMError::WM_ERROR_INVALID_OPERATION,              WmErrorCode::WM_ERROR_STATE_ABNORMALLY     },
    {WMError::WM_ERROR_INVALID_PARAM,                  WmErrorCode::WM_ERROR_INVALID_PARAM        },
    {WMError::WM_ERROR_INVALID_PERMISSION,             WmErrorCode::WM_ERROR_NO_PERMISSION        },
    {WMError::WM_ERROR_INVALID_TYPE,                   WmErrorCode::WM_ERROR_STATE_ABNORMALLY     },
    {WMError::WM_ERROR_INVALID_WINDOW,                 WmErrorCode::WM_ERROR_STATE_ABNORMALLY     },
    {WMError::WM_ERROR_INVALID_WINDOW_MODE_OR_SIZE,    WmErrorCode::WM_ERROR_STATE_ABNORMALLY     },
    {WMError::WM_ERROR_IPC_FAILED,                     WmErrorCode::WM_ERROR_SYSTEM_ABNORMALLY    },
    {WMError::WM_ERROR_NO_MEM,                         WmErrorCode::WM_ERROR_SYSTEM_ABNORMALLY    },
    {WMError::WM_ERROR_NO_REMOTE_ANIMATION,            WmErrorCode::WM_ERROR_SYSTEM_ABNORMALLY    },
    {WMError::WM_ERROR_NULLPTR,                        WmErrorCode::WM_ERROR_STATE_ABNORMALLY     },
    {WMError::WM_ERROR_SAMGR,                          WmErrorCode::WM_ERROR_SYSTEM_ABNORMALLY    },
    {WMError::WM_ERROR_START_ABILITY_FAILED,           WmErrorCode::WM_ERROR_START_ABILITY_FAILED },
};

enum class WindowFlag : uint32_t {
    WINDOW_FLAG_NEED_AVOID = 1,
    WINDOW_FLAG_PARENT_LIMIT = 1 << 1,
    WINDOW_FLAG_SHOW_WHEN_LOCKED = 1 << 2,
    WINDOW_FLAG_FORBID_SPLIT_MOVE = 1 << 3,
    WINDOW_FLAG_END = 1 << 4,
};

enum class WindowSizeChangeReason : uint32_t {
    UNDEFINED = 0,
    MAXIMIZE,
    RECOVER,
    ROTATION,
    DRAG,
    DRAG_START,
    DRAG_END,
    RESIZE,
    MOVE,
    HIDE,
    TRANSFORM,
    CUSTOM_ANIMATION_SHOW,
    END,
};

enum class WindowLayoutMode : uint32_t {
    BASE = 0,
    CASCADE = BASE,
    TILE = 1,
    END,
};

enum class DragEvent : uint32_t {
    DRAG_EVENT_IN  = 1,
    DRAG_EVENT_OUT,
    DRAG_EVENT_MOVE,
    DRAG_EVENT_END,
};

enum class WindowTag : uint32_t {
    MAIN_WINDOW = 0,
    SUB_WINDOW = 1,
    SYSTEM_WINDOW = 2,
};

struct PointInfo {
    int32_t x;
    int32_t y;
};

namespace {
    constexpr uint32_t SYSTEM_COLOR_WHITE = 0xE5FFFFFF;
    constexpr uint32_t SYSTEM_COLOR_BLACK = 0x66000000;
    constexpr uint32_t INVALID_WINDOW_ID = 0;
    constexpr float UNDEFINED_BRIGHTNESS = -1.0f;
    constexpr float MINIMUM_BRIGHTNESS = 0.0f;
    constexpr float MAXIMUM_BRIGHTNESS = 1.0f;
    constexpr int32_t INVALID_PID = -1;
    constexpr int32_t INVALID_UID = -1;
}

class Transform {
public:
    Transform()
        : pivotX_(0.5f), pivotY_(0.5f), scaleX_(1.f), scaleY_(1.f), scaleZ_(1.f), rotationX_(0.f),
          rotationY_(0.f), rotationZ_(0.f), translateX_(0.f), translateY_(0.f), translateZ_(0.f)
    {}
    ~Transform() {}

    bool operator==(const Transform& right) const
    {
        return NearZero(pivotX_ - right.pivotX_) &&
            NearZero(pivotY_ - right.pivotY_) &&
            NearZero(scaleX_ - right.scaleX_) &&
            NearZero(scaleY_ - right.scaleY_) &&
            NearZero(scaleZ_ - right.scaleZ_) &&
            NearZero(rotationX_ - right.rotationX_) &&
            NearZero(rotationY_ - right.rotationY_) &&
            NearZero(rotationZ_ - right.rotationZ_) &&
            NearZero(translateX_ - right.translateX_) &&
            NearZero(translateY_ - right.translateY_) &&
            NearZero(translateZ_ - right.translateZ_);
    }

    bool operator!=(const Transform& right) const
    {
        return !(*this == right);
    }

    float pivotX_;
    float pivotY_;
    float scaleX_;
    float scaleY_;
    float scaleZ_;
    float rotationX_;
    float rotationY_;
    float rotationZ_;
    float translateX_;
    float translateY_;
    float translateZ_;

    static const Transform& Identity()
    {
        static Transform I;
        return I;
    }

    bool Marshalling(Parcel& parcel) const
    {
        return parcel.WriteFloat(pivotX_) && parcel.WriteFloat(pivotY_) &&
            parcel.WriteFloat(scaleX_) && parcel.WriteFloat(scaleY_) && parcel.WriteFloat(scaleZ_) &&
            parcel.WriteFloat(rotationX_) && parcel.WriteFloat(rotationY_) &&
            parcel.WriteFloat(rotationZ_) && parcel.WriteFloat(translateX_) &&
            parcel.WriteFloat(translateY_) && parcel.WriteFloat(translateZ_);
    }

    void Unmarshalling(Parcel& parcel)
    {
        pivotX_ = parcel.ReadFloat();
        pivotY_ = parcel.ReadFloat();
        scaleX_ = parcel.ReadFloat();
        scaleY_ = parcel.ReadFloat();
        scaleZ_ = parcel.ReadFloat();
        rotationX_ = parcel.ReadFloat();
        rotationY_ = parcel.ReadFloat();
        rotationZ_ = parcel.ReadFloat();
        translateX_ = parcel.ReadFloat();
        translateY_ = parcel.ReadFloat();
        translateZ_ = parcel.ReadFloat();
    }
private:
    static inline bool NearZero(float val)
    {
        return val < 0.001f && val > -0.001f;
    }
};

struct SystemBarProperty {
    bool enable_;
    uint32_t backgroundColor_;
    uint32_t contentColor_;
    SystemBarProperty() : enable_(true), backgroundColor_(SYSTEM_COLOR_BLACK), contentColor_(SYSTEM_COLOR_WHITE) {}
    SystemBarProperty(bool enable, uint32_t background, uint32_t content)
        : enable_(enable), backgroundColor_(background), contentColor_(content) {}
    bool operator == (const SystemBarProperty& a) const
    {
        return (enable_ == a.enable_ && backgroundColor_ == a.backgroundColor_ && contentColor_ == a.contentColor_);
    }
};

struct Rect {
    int32_t posX_;
    int32_t posY_;
    uint32_t width_;
    uint32_t height_;

    bool operator==(const Rect& a) const
    {
        return (posX_ == a.posX_ && posY_ == a.posY_ && width_ == a.width_ && height_ == a.height_);
    }

    bool operator!=(const Rect& a) const
    {
        return !this->operator==(a);
    }

    bool isUninitializedRect() const
    {
        return (posX_ == 0 && posY_ == 0 && width_ == 0 && height_ == 0);
    }

    bool IsInsideOf(const Rect& a) const
    {
        return (posX_ >= a.posX_ && posY_ >= a.posY_ &&
            posX_ + width_ <= a.posX_ + a.width_ && posY_ + height_ <= a.posY_ + a.height_);
    }
};

enum class AvoidAreaType : uint32_t {
    TYPE_SYSTEM,           // area of SystemUI
    TYPE_CUTOUT,           // cutout of screen
    TYPE_SYSTEM_GESTURE,   // area for system gesture
    TYPE_KEYBOARD,         // area for soft input keyboard
};

enum class OccupiedAreaType : uint32_t {
    TYPE_INPUT, // area of input window
};

enum class ColorSpace : uint32_t {
    COLOR_SPACE_DEFAULT = 0, // Default color space.
    COLOR_SPACE_WIDE_GAMUT,  // Wide gamut color space. The specific wide color gamut depends on the screen.
};

enum class WindowAnimation : uint32_t {
    NONE,
    DEFAULT,
    INPUTE,
    CUSTOM,
};

class AvoidArea : public Parcelable {
public:
    Rect topRect_ { 0, 0, 0, 0 };
    Rect leftRect_ { 0, 0, 0, 0 };
    Rect rightRect_ { 0, 0, 0, 0 };
    Rect bottomRect_ { 0, 0, 0, 0 };

    bool operator==(const AvoidArea& a) const
    {
        return (leftRect_ == a.leftRect_ && topRect_ == a.topRect_ &&
            rightRect_ == a.rightRect_ && bottomRect_ == a.bottomRect_);
    }

    bool operator!=(const AvoidArea& a) const
    {
        return !this->operator==(a);
    }

    bool isEmptyAvoidArea() const
    {
        return topRect_.isUninitializedRect() && leftRect_.isUninitializedRect() &&
            rightRect_.isUninitializedRect() && bottomRect_.isUninitializedRect();
    }

    static inline bool WriteParcel(Parcel& parcel, const Rect& rect)
    {
        return parcel.WriteInt32(rect.posX_) && parcel.WriteInt32(rect.posY_) &&
            parcel.WriteUint32(rect.width_) && parcel.WriteUint32(rect.height_);
    }

    static inline bool ReadParcel(Parcel& parcel, Rect& rect)
    {
        return parcel.ReadInt32(rect.posX_) && parcel.ReadInt32(rect.posY_) &&
            parcel.ReadUint32(rect.width_) && parcel.ReadUint32(rect.height_);
    }

    virtual bool Marshalling(Parcel& parcel) const override
    {
        return (WriteParcel(parcel, leftRect_) && WriteParcel(parcel, topRect_) &&
            WriteParcel(parcel, rightRect_) && WriteParcel(parcel, bottomRect_));
    }

    static AvoidArea* Unmarshalling(Parcel& parcel)
    {
        AvoidArea *avoidArea = new(std::nothrow) AvoidArea();
        if (avoidArea == nullptr) {
            return nullptr;
        }
        if (ReadParcel(parcel, avoidArea->leftRect_) && ReadParcel(parcel, avoidArea->topRect_) &&
            ReadParcel(parcel, avoidArea->rightRect_) && ReadParcel(parcel, avoidArea->bottomRect_)) {
            return avoidArea;
        }
        delete avoidArea;
        return nullptr;
    }
};

enum class WindowUpdateType : int32_t {
    WINDOW_UPDATE_ADDED = 1,
    WINDOW_UPDATE_REMOVED,
    WINDOW_UPDATE_FOCUSED,
    WINDOW_UPDATE_BOUNDS,
    WINDOW_UPDATE_ACTIVE,
    WINDOW_UPDATE_PROPERTY,
};

using OnCallback = std::function<void(int64_t)>;
struct VsyncCallback {
    OnCallback onCallback;
};
}
}
#endif // OHOS_ROSEN_WM_COMMON_H

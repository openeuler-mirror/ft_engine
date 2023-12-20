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

#ifndef OHOS_ROSEN_WINDOW_H
#define OHOS_ROSEN_WINDOW_H

#include <refbase.h>
#include <parcel.h>
#include <pixel_map.h>
#include <iremote_object.h>

#include "wm_common.h"
#include "window_option.h"

class NativeValue;
class NativeEngine;
namespace OHOS::MMI {
    class PointerEvent;
    class KeyEvent;
    class AxisEvent;
}
namespace OHOS::AppExecFwk {
    class Configuration;
    class Ability;
}

namespace OHOS::AbilityRuntime {
    class AbilityContext;
    class Context;
}

namespace OHOS::AAFwk {
    class Want;
}

namespace OHOS::Ace {
    class UIContent;
}

namespace OHOS {
namespace Rosen {
using NotifyNativeWinDestroyFunc = std::function<void(std::string windowName)>;
class RSSurfaceNode;

class IWindowLifeCycle : virtual public RefBase {
public:
    virtual void AfterForeground() {}
    virtual void AfterBackground() {}
    virtual void AfterFocused() {}
    virtual void AfterUnfocused() {}
    virtual void ForegroundFailed(int32_t ret) {}
    virtual void AfterActive() {}
    virtual void AfterInactive() {}
};

class IWindowChangeListener : virtual public RefBase {
public:
    virtual void OnSizeChange(Rect rect, WindowSizeChangeReason reason) {}
    virtual void OnModeChange(WindowMode mode) {}
};

class IAvoidAreaChangedListener : virtual public RefBase {
public:
    virtual void OnAvoidAreaChanged(const AvoidArea avoidArea, AvoidAreaType type) {}
};

class IWindowDragListener : virtual public RefBase {
public:
    virtual void OnDrag(int32_t x, int32_t y, DragEvent event) {}
};

class IDisplayMoveListener : virtual public RefBase {
public:
    virtual void OnDisplayMove(DisplayId from, DisplayId to) {}
};

class IDispatchInputEventListener : virtual public RefBase {
public:
    virtual void OnDispatchPointerEvent(std::shared_ptr<MMI::PointerEvent>& inputEvent) = 0;
    virtual void OnDispatchKeyEvent(std::shared_ptr<MMI::KeyEvent>& keyEvent) = 0;
};

class OccupiedAreaChangeInfo : public Parcelable {
public:
    OccupiedAreaChangeInfo() = default;
    OccupiedAreaChangeInfo(OccupiedAreaType type, Rect rect) : type_(type), rect_(rect) {};
    ~OccupiedAreaChangeInfo() = default;

    virtual bool Marshalling(Parcel& parcel) const override;
    static OccupiedAreaChangeInfo* Unmarshalling(Parcel& parcel);

    OccupiedAreaType type_ = OccupiedAreaType::TYPE_INPUT;
    Rect rect_ = { 0, 0, 0, 0 };
};

class IOccupiedAreaChangeListener : virtual public RefBase {
public:
    virtual void OnSizeChange(const sptr<OccupiedAreaChangeInfo>& info) {}
};

class IAceAbilityHandler : virtual public RefBase {
public:
    virtual void SetBackgroundColor(uint32_t color) = 0;
    virtual uint32_t GetBackgroundColor() = 0;
};

class IInputEventConsumer {
public:
    IInputEventConsumer() = default;
    virtual ~IInputEventConsumer() = default;
    virtual bool OnInputEvent(const std::shared_ptr<MMI::KeyEvent>& keyEvent) const = 0;
    virtual bool OnInputEvent(const std::shared_ptr<MMI::PointerEvent>& pointerEvent) const = 0;
    virtual bool OnInputEvent(const std::shared_ptr<MMI::AxisEvent>& axisEvent) const = 0;
};

class ITouchOutsideListener : virtual public RefBase {
public:
    virtual void OnTouchOutside() const {}
};

class IAnimationTransitionController : virtual public RefBase {
public:
    virtual void AnimationForShown() = 0;
    virtual void AnimationForHidden() = 0;
};

class IScreenshotListener : virtual public RefBase {
public:
    virtual void OnScreenshot() {}
};

class IDialogTargetTouchListener : virtual public RefBase {
public:
    virtual void OnDialogTargetTouch() const {}
};

class IDialogDeathRecipientListener : virtual public RefBase {
public:
    virtual void OnDialogDeathRecipient() const = 0;
};

class Window : public RefBase {
public:
    /**
     * @brief create window, include main_window/sub_window/system_window
     *
     * @param windowName window name, identify window instance
     * @param option window propertion
     * @param context ability context
     * @return sptr<Window> If create window success,return window instance;Otherwise, return nullptr
     */
    static sptr<Window> Create(const std::string& windowName,
        sptr<WindowOption>& option, const std::shared_ptr<AbilityRuntime::Context>& context = nullptr);
    /**
     * @brief find window by windowName
     *
     * @param windowName
     * @return sptr<Window> Return the window instance founded
     */
    static sptr<Window> Find(const std::string& windowName);
    /**
     * @brief Get the final show window by context. Its implemented in api8
     *
     * @param context Indicates the context on which the window depends
     * @return sptr<Window>
     */
    static sptr<Window> GetTopWindowWithContext(const std::shared_ptr<AbilityRuntime::Context>& context = nullptr);
    /**
     * @brief Get the final show window by id. Its implemented in api8
     *
     * @param mainWinId main window id?
     * @return sptr<Window>
     */
    static sptr<Window> GetTopWindowWithId(uint32_t mainWinId);
    /**
     * @brief Get the all sub windows by parent
     *
     * @param parentId parent window id
     * @return std::vector<sptr<Window>>
     */
    static std::vector<sptr<Window>> GetSubWindow(uint32_t parentId);

    /**
     * @brief Update configuration for all windows
     *
     * @param configuration configuration for app
     */
    static void UpdateConfigurationForAll(const std::shared_ptr<AppExecFwk::Configuration>& configuration);
    virtual std::shared_ptr<RSSurfaceNode> GetSurfaceNode() const = 0;
    virtual const std::shared_ptr<AbilityRuntime::Context> GetContext() const = 0;
    /**
     * @brief Get the window show rect
     *
     * @return Rect window rect
     */
    virtual Rect GetRect() const = 0;
    virtual Rect GetRequestRect() const = 0;
    /**
     * @brief Get the window type
     *
     * @return WindowType window type
     */
    virtual WindowType GetType() const = 0;
    virtual WindowMode GetMode() const = 0;
    virtual float GetAlpha() const = 0;
    virtual const std::string& GetWindowName() const = 0;
    virtual uint32_t GetWindowId() const = 0;
    virtual uint32_t GetWindowFlags() const = 0;
    virtual WindowState GetWindowState() const = 0;
    virtual WMError SetFocusable(bool isFocusable) = 0;
    virtual bool GetFocusable() const = 0;
    virtual WMError SetTouchable(bool isTouchable) = 0;
    virtual bool GetTouchable() const = 0;
    virtual SystemBarProperty GetSystemBarPropertyByType(WindowType type) const = 0;
    /**
     * @brief judge this window is full screen.
     *
     * @return true If SetFullScreen(true) is called , return true.
     * @return false default return false
     */
    virtual bool IsFullScreen() const = 0;
    /**
     * @brief judge window layout is full screen
     *
     * @return true this window layout is full screen
     * @return false this window layout is not full screen
     */
    virtual bool IsLayoutFullScreen() const = 0;
    /**
     * @brief Set the Window Type
     *
     * @param type window type
     * @return WMError
     */
    virtual WMError SetWindowType(WindowType type) = 0;
    /**
     * @brief Set the Window Mode
     *
     * @param mode window mode
     * @return WMError
     */
    virtual WMError SetWindowMode(WindowMode mode) = 0;
    virtual void SetAlpha(float alpha) = 0;
    virtual void SetTransform(const Transform& trans) = 0;
    virtual const Transform& GetTransform() const = 0;
    virtual WMError AddWindowFlag(WindowFlag flag) = 0;
    virtual WMError RemoveWindowFlag(WindowFlag flag) = 0;
    virtual WMError SetWindowFlags(uint32_t flags) = 0;
    /**
     * @brief Set the System Bar(include status bar and nav bar) Property
     *
     * @param type WINDOW_TYPE_STATUS_BAR or WINDOW_TYPE_NAVIGATION_BAR
     * @param property system bar prop,include content color, background color
     * @return WMError
     */
    virtual WMError SetSystemBarProperty(WindowType type, const SystemBarProperty& property) = 0;
    /**
     * @brief Get the Avoid Area By Type object
     *
     * @param type avoid area type.@see reference
     * @param avoidArea
     * @return WMError
     */
    virtual WMError GetAvoidAreaByType(AvoidAreaType type, AvoidArea& avoidArea) = 0;
    /**
     * @brief Set this window layout full screen, with hide status bar and nav bar above on this window
     *
     * @param status
     * @return WMError
     */
    virtual WMError SetLayoutFullScreen(bool status) = 0;
    /**
     * @brief Set this window full screen, with hide status bar and nav bar
     *
     * @param status if true, hide status bar and nav bar; Otherwise, show status bar and nav bar
     * @return WMError
     */
    virtual WMError SetFullScreen(bool status) = 0;
    /**
     * @brief destroy window
     *
     * @return WMError
     */
    virtual WMError Destroy() = 0;
    virtual WMError Show(uint32_t reason = 0, bool withAnimation = false) = 0;
    virtual WMError Hide(uint32_t reason = 0, bool withAnimation = false) = 0;
    /**
     * @brief move the window to (x, y)
     *
     * @param x
     * @param y
     * @return WMError
     */
    virtual WMError MoveTo(int32_t x, int32_t y) = 0;
    /**
     * @brief resize the window instance (w,h)
     *
     * @param width
     * @param height
     * @return WMError
     */
    virtual WMError Resize(uint32_t width, uint32_t height) = 0;
    /**
     * @brief Set the screen always on
     *
     * @param keepScreenOn
     * @return WMError
     */
    virtual WMError SetKeepScreenOn(bool keepScreenOn) = 0;
    virtual bool IsKeepScreenOn() const = 0;
    virtual WMError SetTurnScreenOn(bool turnScreenOn) = 0;
    virtual bool IsTurnScreenOn() const = 0;
    virtual WMError SetBackgroundColor(const std::string& color) = 0;
    virtual WMError SetTransparent(bool isTransparent) = 0;
    virtual bool IsTransparent() const = 0;
    virtual WMError SetBrightness(float brightness) = 0;
    virtual float GetBrightness() const = 0;
    virtual WMError SetCallingWindow(uint32_t windowId) = 0;
    virtual void SetPrivacyMode(bool isPrivacyMode) = 0;
    virtual bool IsPrivacyMode() const = 0;
    virtual void SetSystemPrivacyMode(bool isSystemPrivacyMode) = 0;
    virtual WMError BindDialogTarget(sptr<IRemoteObject> targetToken) = 0;
    virtual void SetSnapshotSkip(bool isSkip) = 0;

    // window effect
    virtual WMError SetCornerRadius(float cornerRadius) = 0;
    virtual WMError SetShadowRadius(float radius) = 0;
    virtual WMError SetShadowColor(std::string color) = 0;
    virtual void SetShadowOffsetX(float offsetX) = 0;
    virtual void SetShadowOffsetY(float offsetY) = 0;
    virtual WMError SetBlur(float radius) = 0;
    virtual WMError SetBackdropBlur(float radius) = 0;
    virtual WMError SetBackdropBlurStyle(WindowBlurStyle blurStyle) = 0;

    virtual WMError RequestFocus() const = 0;
    virtual bool IsFocused() const = 0;
    virtual WMError UpdateSurfaceNodeAfterCustomAnimation(bool isAdd) = 0;
    virtual void SetInputEventConsumer(const std::shared_ptr<IInputEventConsumer>& inputEventConsumer) = 0;
    virtual void ConsumeKeyEvent(std::shared_ptr<MMI::KeyEvent>& inputEvent) = 0;
    virtual void ConsumePointerEvent(const std::shared_ptr<MMI::PointerEvent>& inputEvent) = 0;
    virtual void RequestVsync(const std::shared_ptr<VsyncCallback>& vsyncCallback) = 0;
    virtual void UpdateConfiguration(const std::shared_ptr<AppExecFwk::Configuration>& configuration) = 0;
    /**
     * @brief register window lifecycle listener.
     *
     * @param listener
     */
    virtual bool RegisterLifeCycleListener(const sptr<IWindowLifeCycle>& listener) = 0;
    virtual bool RegisterWindowChangeListener(const sptr<IWindowChangeListener>& listener) = 0;
    virtual bool UnregisterLifeCycleListener(const sptr<IWindowLifeCycle>& listener) = 0;
    virtual bool UnregisterWindowChangeListener(const sptr<IWindowChangeListener>& listener) = 0;
    virtual bool RegisterAvoidAreaChangeListener(sptr<IAvoidAreaChangedListener>& listener) = 0;
    virtual bool UnregisterAvoidAreaChangeListener(sptr<IAvoidAreaChangedListener>& listener) = 0;
    virtual bool RegisterDragListener(const sptr<IWindowDragListener>& listener) = 0;
    virtual bool UnregisterDragListener(const sptr<IWindowDragListener>& listener) = 0;
    virtual bool RegisterDisplayMoveListener(sptr<IDisplayMoveListener>& listener) = 0;
    virtual bool UnregisterDisplayMoveListener(sptr<IDisplayMoveListener>& listener) = 0;
    virtual void RegisterWindowDestroyedListener(const NotifyNativeWinDestroyFunc& func) = 0;
    virtual bool RegisterOccupiedAreaChangeListener(const sptr<IOccupiedAreaChangeListener>& listener) = 0;
    virtual bool UnregisterOccupiedAreaChangeListener(const sptr<IOccupiedAreaChangeListener>& listener) = 0;
    virtual bool RegisterTouchOutsideListener(const sptr<ITouchOutsideListener>& listener) = 0;
    virtual bool UnregisterTouchOutsideListener(const sptr<ITouchOutsideListener>& listener) = 0;
    virtual bool RegisterAnimationTransitionController(const sptr<IAnimationTransitionController>& listener) = 0;
    virtual bool RegisterScreenshotListener(const sptr<IScreenshotListener>& listener) = 0;
    virtual bool UnregisterScreenshotListener(const sptr<IScreenshotListener>& listener) = 0;
    virtual bool RegisterDialogTargetTouchListener(const sptr<IDialogTargetTouchListener>& listener) = 0;
    virtual bool UnregisterDialogTargetTouchListener(const sptr<IDialogTargetTouchListener>& listener) = 0;
    virtual void RegisterDialogDeathRecipientListener(const sptr<IDialogDeathRecipientListener>& listener) = 0;
    virtual void UnregisterDialogDeathRecipientListener(const sptr<IDialogDeathRecipientListener>& listener) = 0;
    virtual void NotifyTouchDialogTarget() = 0;
    virtual void SetAceAbilityHandler(const sptr<IAceAbilityHandler>& handler) = 0;
    /**
     * @brief set window ui content
     *
     * @param contentInfo content info path
     * @param engine
     * @param storage
     * @param isDistributed
     * @param ability
     * @return WMError
     */
    virtual WMError SetUIContent(const std::string& contentInfo, NativeEngine* engine,
        NativeValue* storage, bool isDistributed = false, AppExecFwk::Ability* ability = nullptr) = 0;
    virtual std::string GetContentInfo() = 0;
    virtual Ace::UIContent* GetUIContent() const = 0;
    virtual void OnNewWant(const AAFwk::Want& want) = 0;
    virtual void SetRequestedOrientation(Orientation) = 0;
    virtual Orientation GetRequestedOrientation() = 0;
    virtual void SetRequestModeSupportInfo(uint32_t modeSupportInfo) = 0;
    virtual uint32_t GetRequestModeSupportInfo() const = 0;
    virtual WMError SetTouchHotAreas(const std::vector<Rect>& rects) = 0;
    virtual void GetRequestedTouchHotAreas(std::vector<Rect>& rects) const = 0;
    virtual bool IsMainHandlerAvailable() const = 0;
    virtual WMError SetAPPWindowLabel(const std::string& label) = 0;
    virtual WMError SetAPPWindowIcon(const std::shared_ptr<Media::PixelMap>& icon) = 0;

    /**
     * @brief disable main window decoration. It must be callled before loadContent.
     *
     */
    virtual void DisableAppWindowDecor() = 0;
    /**
     * @brief return window decoration is enabled. It is called by ACE
     *
     * @return true means window decoration is enabled. Otherwise disabled
     */
    virtual bool IsDecorEnable() const = 0;
    /**
     * @brief maximize the main window. It is called by ACE when maximize button is clicked.
     *
     * @return WMError
     */
    virtual WMError Maximize() = 0;
    /**
     * @brief minimize the main window. It is called by ACE when minimize button is clicked.
     *
     * @return WMError
     */
    virtual WMError Minimize() = 0;
    /**
     * @brief recovery the main window. It is called by ACE when recovery button is clicked.
     *
     * @return WMError
     */
    virtual WMError Recover() = 0;
    /**
     * @brief close the main window. It is called by ACE when close button is clicked.
     *
     * @return WMError
     */
    virtual WMError Close() = 0;
    /**
     * @brief start move main window. It is called by ACE when title is moved.
     *
     */
    virtual void StartMove() = 0;
    virtual void SetNeedRemoveWindowInputChannel(bool needRemoveWindowInputChannel) = 0;

    // colorspace, gamut
    virtual bool IsSupportWideGamut() = 0;
    virtual void SetColorSpace(ColorSpace colorSpace) = 0;
    virtual ColorSpace GetColorSpace() = 0;

    virtual void DumpInfo(const std::vector<std::string>& params, std::vector<std::string>& info) = 0;
    /**
     * @brief window snapshot
     *
     * @return std::shared_ptr<Media::PixelMap> snapshot pixel
     */
    virtual std::shared_ptr<Media::PixelMap> Snapshot() = 0;

    /**
     * @brief Handle and notify memory level.
     *
     * @param level memory level
     * @return the error code of window
     */
    virtual WMError NotifyMemoryLevel(int32_t level) const = 0;

    /**
     * @brief Update configuration for all windows
     *
     * @param configuration configuration for app
     */
    virtual bool IsAllowHaveSystemSubWindow() = 0;

    /**
     * @brief Update Limit size of window
     *
     * @param minWidth min width of window
     * @param minHeight min height of window
     * @param maxWidth max width of window
     * @param maxHeight max height of window
     */
    virtual WMError SetLimitSize(uint32_t minWidth, uint32_t minHeight,
        uint32_t maxWidth, uint32_t maxHeight) = 0;
};
}
}
#endif // OHOS_ROSEN_WINDOW_H

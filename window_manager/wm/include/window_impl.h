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

#ifndef OHOS_ROSEN_WINDOW_IMPL_H
#define OHOS_ROSEN_WINDOW_IMPL_H

#include <map>

#include <ability_context.h>
#include <i_input_event_consumer.h>
#include <key_event.h>
#include <refbase.h>
#include <string>
#include <ui_content.h>
#include <ui/rs_surface_node.h>
#include <struct_multimodal.h>

#include "input_transfer_station.h"
#include "vsync_station.h"
#include "window.h"
#include "window_property.h"
#include "window_transition_info.h"
#include "wm_common_inner.h"
#include "wm_common.h"

using OHOS::AppExecFwk::DisplayOrientation;

namespace OHOS {
namespace Rosen {
union ColorParam {
#if BIG_ENDIANNESS
    struct {
        uint8_t alpha;
        uint8_t red;
        uint8_t green;
        uint8_t blue;
    } argb;
#else
    struct {
        uint8_t blue;
        uint8_t green;
        uint8_t red;
        uint8_t alpha;
    } argb;
#endif
    uint32_t value;
};

const std::map<DisplayOrientation, Orientation> ABILITY_TO_WMS_ORIENTATION_MAP {
    {DisplayOrientation::UNSPECIFIED,                           Orientation::UNSPECIFIED                        },
    {DisplayOrientation::LANDSCAPE,                             Orientation::HORIZONTAL                         },
    {DisplayOrientation::PORTRAIT,                              Orientation::VERTICAL                           },
    {DisplayOrientation::FOLLOWRECENT,                          Orientation::UNSPECIFIED                        },
    {DisplayOrientation::LANDSCAPE_INVERTED,                    Orientation::REVERSE_HORIZONTAL                 },
    {DisplayOrientation::PORTRAIT_INVERTED,                     Orientation::REVERSE_VERTICAL                   },
    {DisplayOrientation::AUTO_ROTATION,                         Orientation::SENSOR                             },
    {DisplayOrientation::AUTO_ROTATION_LANDSCAPE,               Orientation::SENSOR_HORIZONTAL                  },
    {DisplayOrientation::AUTO_ROTATION_PORTRAIT,                Orientation::SENSOR_VERTICAL                    },
    {DisplayOrientation::AUTO_ROTATION_RESTRICTED,              Orientation::AUTO_ROTATION_RESTRICTED           },
    {DisplayOrientation::AUTO_ROTATION_LANDSCAPE_RESTRICTED,    Orientation::AUTO_ROTATION_LANDSCAPE_RESTRICTED },
    {DisplayOrientation::AUTO_ROTATION_PORTRAIT_RESTRICTED,     Orientation::AUTO_ROTATION_PORTRAIT_RESTRICTED  },
    {DisplayOrientation::LOCKED,                                Orientation::LOCKED                             },
};

class WindowImpl : public Window {
#define CALL_LIFECYCLE_LISTENER(windowLifecycleCb, listeners) \
    do {                                                      \
        for (auto& listener : (listeners)) {                  \
            if (listener.GetRefPtr() != nullptr) {            \
                listener.GetRefPtr()->windowLifecycleCb();    \
            }                                                 \
        }                                                     \
    } while (0)

#define CALL_LIFECYCLE_LISTENER_WITH_PARAM(windowLifecycleCb, listeners, param) \
    do {                                                                        \
        for (auto& listener : (listeners)) {                                    \
            if (listener.GetRefPtr() != nullptr) {                              \
                listener.GetRefPtr()->windowLifecycleCb(param);                 \
            }                                                                   \
        }                                                                       \
    } while (0)

#define CALL_UI_CONTENT(uiContentCb)                          \
    do {                                                      \
        if (uiContent_ != nullptr) {                          \
            uiContent_->uiContentCb();                        \
        }                                                     \
    } while (0)

public:
    explicit WindowImpl(const sptr<WindowOption>& option);
    ~WindowImpl();

    static sptr<Window> Find(const std::string& id);
    static sptr<Window> GetTopWindowWithContext(const std::shared_ptr<AbilityRuntime::Context>& context = nullptr);
    static sptr<Window> GetTopWindowWithId(uint32_t mainWinId);
    static std::vector<sptr<Window>> GetSubWindow(uint32_t parantId);
    static void UpdateConfigurationForAll(const std::shared_ptr<AppExecFwk::Configuration>& configuration);
    virtual std::shared_ptr<RSSurfaceNode> GetSurfaceNode() const override;
    virtual Rect GetRect() const override;
    virtual Rect GetRequestRect() const override;
    virtual WindowType GetType() const override;
    virtual WindowMode GetMode() const override;
    virtual float GetAlpha() const override;
    virtual WindowState GetWindowState() const override;
    virtual WMError SetFocusable(bool isFocusable) override;
    virtual bool GetFocusable() const override;
    virtual WMError SetTouchable(bool isTouchable) override;
    virtual bool GetTouchable() const override;
    virtual const std::string& GetWindowName() const override;
    virtual uint32_t GetWindowId() const override;
    virtual uint32_t GetWindowFlags() const override;
    uint32_t GetRequestModeSupportInfo() const override;
    bool IsMainHandlerAvailable() const override;
    inline NotifyNativeWinDestroyFunc GetNativeDestroyCallback()
    {
        return notifyNativefunc_;
    }
    virtual SystemBarProperty GetSystemBarPropertyByType(WindowType type) const override;
    virtual bool IsFullScreen() const override;
    virtual bool IsLayoutFullScreen() const override;
    virtual WMError SetWindowType(WindowType type) override;
    virtual WMError SetWindowMode(WindowMode mode) override;
    virtual void SetAlpha(float alpha) override;
    virtual void SetTransform(const Transform& trans) override;
    virtual WMError AddWindowFlag(WindowFlag flag) override;
    virtual WMError RemoveWindowFlag(WindowFlag flag) override;
    virtual WMError SetWindowFlags(uint32_t flags) override;
    virtual WMError SetSystemBarProperty(WindowType type, const SystemBarProperty& property) override;
    virtual WMError UpdateSystemBarProperty(bool status);
    virtual WMError SetLayoutFullScreen(bool status) override;
    virtual WMError SetFullScreen(bool status) override;
    virtual const Transform& GetTransform() const override;
    virtual const Transform& GetZoomTransform() const;
    virtual WMError UpdateSurfaceNodeAfterCustomAnimation(bool isAdd) override;
    inline void SetWindowState(WindowState state)
    {
        state_ = state;
    }
    virtual WMError GetAvoidAreaByType(AvoidAreaType type, AvoidArea& avoidArea) override;

    WMError Create(uint32_t parentId,
        const std::shared_ptr<AbilityRuntime::Context>& context = nullptr);
    virtual WMError Destroy() override;
    virtual WMError Show(uint32_t reason = 0, bool withAnimation = false) override;
    virtual WMError Hide(uint32_t reason = 0, bool withAnimation = false) override;
    virtual WMError MoveTo(int32_t x, int32_t y) override;
    virtual WMError Resize(uint32_t width, uint32_t height) override;
    virtual WMError SetKeepScreenOn(bool keepScreenOn) override;
    virtual bool IsKeepScreenOn() const override;
    virtual WMError SetTurnScreenOn(bool turnScreenOn) override;
    virtual bool IsTurnScreenOn() const override;
    virtual WMError SetBackgroundColor(const std::string& color) override;
    virtual WMError SetTransparent(bool isTransparent) override;
    virtual bool IsTransparent() const override;
    virtual WMError SetBrightness(float brightness) override;
    virtual float GetBrightness() const override;
    virtual WMError SetCallingWindow(uint32_t windowId) override;
    virtual void SetPrivacyMode(bool isPrivacyMode) override;
    virtual bool IsPrivacyMode() const override;
    virtual void SetSystemPrivacyMode(bool isSystemPrivacyMode) override;
    virtual void DisableAppWindowDecor() override;
    virtual WMError BindDialogTarget(sptr<IRemoteObject> targetToken) override;
    virtual void SetSnapshotSkip(bool isSkip) override;

    // window effect
    virtual WMError SetCornerRadius(float cornerRadius) override;
    virtual WMError SetShadowRadius(float radius) override;
    virtual WMError SetShadowColor(std::string color) override;
    virtual void SetShadowOffsetX(float offsetX) override;
    virtual void SetShadowOffsetY(float offsetY) override;
    virtual WMError SetBlur(float radius) override;
    virtual WMError SetBackdropBlur(float radius) override;
    virtual WMError SetBackdropBlurStyle(WindowBlurStyle blurStyle) override;

    virtual bool IsDecorEnable() const override;
    virtual WMError Maximize() override;
    virtual WMError Minimize() override;
    virtual WMError Recover() override;
    virtual WMError Close() override;
    virtual void StartMove() override;

    virtual WMError RequestFocus() const override;
    virtual void SetInputEventConsumer(const std::shared_ptr<IInputEventConsumer>& inputEventConsumer) override;

    virtual bool RegisterLifeCycleListener(const sptr<IWindowLifeCycle>& listener) override;
    virtual bool RegisterWindowChangeListener(const sptr<IWindowChangeListener>& listener) override;
    virtual bool UnregisterLifeCycleListener(const sptr<IWindowLifeCycle>& listener) override;
    virtual bool UnregisterWindowChangeListener(const sptr<IWindowChangeListener>& listener) override;
    virtual bool RegisterAvoidAreaChangeListener(sptr<IAvoidAreaChangedListener>& listener) override;
    virtual bool UnregisterAvoidAreaChangeListener(sptr<IAvoidAreaChangedListener>& listener) override;
    virtual bool RegisterDragListener(const sptr<IWindowDragListener>& listener) override;
    virtual bool UnregisterDragListener(const sptr<IWindowDragListener>& listener) override;
    virtual bool RegisterDisplayMoveListener(sptr<IDisplayMoveListener>& listener) override;
    virtual bool UnregisterDisplayMoveListener(sptr<IDisplayMoveListener>& listener) override;
    virtual void RegisterWindowDestroyedListener(const NotifyNativeWinDestroyFunc& func) override;
    virtual bool RegisterOccupiedAreaChangeListener(const sptr<IOccupiedAreaChangeListener>& listener) override;
    virtual bool UnregisterOccupiedAreaChangeListener(const sptr<IOccupiedAreaChangeListener>& listener) override;
    virtual bool RegisterTouchOutsideListener(const sptr<ITouchOutsideListener>& listener) override;
    virtual bool UnregisterTouchOutsideListener(const sptr<ITouchOutsideListener>& listener) override;
    virtual bool RegisterAnimationTransitionController(const sptr<IAnimationTransitionController>& listener) override;
    virtual bool RegisterScreenshotListener(const sptr<IScreenshotListener>& listener) override;
    virtual bool UnregisterScreenshotListener(const sptr<IScreenshotListener>& listener) override;
    virtual bool RegisterDialogTargetTouchListener(const sptr<IDialogTargetTouchListener>& listener) override;
    virtual bool UnregisterDialogTargetTouchListener(const sptr<IDialogTargetTouchListener>& listener) override;
    virtual void RegisterDialogDeathRecipientListener(const sptr<IDialogDeathRecipientListener>& listener) override;
    virtual void UnregisterDialogDeathRecipientListener(const sptr<IDialogDeathRecipientListener>& listener) override;
    virtual void SetAceAbilityHandler(const sptr<IAceAbilityHandler>& handler) override;
    virtual void SetRequestModeSupportInfo(uint32_t modeSupportInfo) override;
    void UpdateRect(const struct Rect& rect, bool decoStatus, WindowSizeChangeReason reason);
    void UpdateMode(WindowMode mode);
    void UpdateModeSupportInfo(uint32_t modeSupportInfo);
    virtual void ConsumeKeyEvent(std::shared_ptr<MMI::KeyEvent>& inputEvent) override;
    virtual void ConsumePointerEvent(const std::shared_ptr<MMI::PointerEvent>& inputEvent) override;
    virtual void RequestVsync(const std::shared_ptr<VsyncCallback>& vsyncCallback) override;
    void UpdateFocusStatus(bool focused);
    virtual bool IsFocused() const override;
    virtual void UpdateConfiguration(const std::shared_ptr<AppExecFwk::Configuration>& configuration) override;
    void UpdateAvoidArea(const sptr<AvoidArea>& avoidArea, AvoidAreaType type);
    void UpdateWindowState(WindowState state);
    sptr<WindowProperty> GetWindowProperty();
    void UpdateDragEvent(const PointInfo& point, DragEvent event);
    void UpdateDisplayId(DisplayId from, DisplayId to);
    void UpdateOccupiedAreaChangeInfo(const sptr<OccupiedAreaChangeInfo>& info);
    void UpdateActiveStatus(bool isActive);
    void NotifyTouchOutside();
    void NotifyScreenshot();
    void NotifyTouchDialogTarget() override;
    void NotifyDestroy();
    void NotifyForeground();
    void NotifyBackground();
    void UpdateZoomTransform(const Transform& trans, bool isDisplayZoomOn);

    virtual WMError SetUIContent(const std::string& contentInfo, NativeEngine* engine,
        NativeValue* storage, bool isdistributed, AppExecFwk::Ability* ability) override;
    virtual std::string GetContentInfo() override;
    virtual const std::shared_ptr<AbilityRuntime::Context> GetContext() const override;
    virtual Ace::UIContent* GetUIContent() const override;
    virtual void OnNewWant(const AAFwk::Want& want) override;
    virtual void SetRequestedOrientation(Orientation) override;
    virtual Orientation GetRequestedOrientation() override;
    virtual void SetNeedRemoveWindowInputChannel(bool needRemoveWindowInputChannel) override;
    virtual WMError SetTouchHotAreas(const std::vector<Rect>& rects) override;
    virtual void GetRequestedTouchHotAreas(std::vector<Rect>& rects) const override;
    virtual WMError SetAPPWindowLabel(const std::string& label) override;
    virtual WMError SetAPPWindowIcon(const std::shared_ptr<Media::PixelMap>& icon) override;

    // colorspace, gamut
    virtual bool IsSupportWideGamut() override;
    virtual void SetColorSpace(ColorSpace colorSpace) override;
    virtual ColorSpace GetColorSpace() override;

    virtual void DumpInfo(const std::vector<std::string>& params, std::vector<std::string>& info) override;
    virtual std::shared_ptr<Media::PixelMap> Snapshot() override;
    virtual WMError NotifyMemoryLevel(int32_t level) const override;
    virtual bool IsAllowHaveSystemSubWindow() override;
    void RestoreSplitWindowMode(uint32_t mode);
private:
    template<typename T1, typename T2, typename Ret>
    using EnableIfSame = typename std::enable_if<std::is_same_v<T1, T2>, Ret>::type;
    template<typename T> bool RegisterListener(std::vector<sptr<T>>& holder, const sptr<T>& listener);
    template<typename T> bool UnregisterListener(std::vector<sptr<T>>& holder, const sptr<T>& listener);
    template<typename T> void ClearUselessListeners(std::map<uint32_t, T>& listeners, uint32_t winId)
    {
        listeners.erase(winId);
    }
    template<typename T>
    inline EnableIfSame<T, IWindowLifeCycle, std::vector<wptr<IWindowLifeCycle>>> GetListeners()
    {
        std::vector<wptr<IWindowLifeCycle>> lifecycleListeners;
        {
            std::lock_guard<std::recursive_mutex> lock(globalMutex_);
            for (auto& listener : lifecycleListeners_[GetWindowId()]) {
                lifecycleListeners.push_back(listener);
            }
        }
        return lifecycleListeners;
    }
    template<typename T>
    inline EnableIfSame<T, IWindowChangeListener, std::vector<wptr<IWindowChangeListener>>> GetListeners()
    {
        std::vector<wptr<IWindowChangeListener>> windowChangeListeners;
        {
            std::lock_guard<std::recursive_mutex> lock(globalMutex_);
            for (auto& listener : windowChangeListeners_[GetWindowId()]) {
                windowChangeListeners.push_back(listener);
            }
        }
        return windowChangeListeners;
    }
    template<typename T>
    inline EnableIfSame<T, IAvoidAreaChangedListener, std::vector<wptr<IAvoidAreaChangedListener>>> GetListeners()
    {
        std::vector<wptr<IAvoidAreaChangedListener>> avoidAreaChangeListeners;
        {
            std::lock_guard<std::recursive_mutex> lock(globalMutex_);
            for (auto& listener : avoidAreaChangeListeners_[GetWindowId()]) {
                avoidAreaChangeListeners.push_back(listener);
            }
        }
        return avoidAreaChangeListeners;
    }
    template<typename T>
    inline EnableIfSame<T, IDisplayMoveListener, std::vector<wptr<IDisplayMoveListener>>> GetListeners()
    {
        std::vector<wptr<IDisplayMoveListener>> displayMoveListeners;
        {
            std::lock_guard<std::recursive_mutex> lock(mutex_);
            for (auto& listener : displayMoveListeners_) {
                displayMoveListeners.push_back(listener);
            }
        }
        return displayMoveListeners;
    }
    template<typename T>
    inline EnableIfSame<T, IScreenshotListener, std::vector<wptr<IScreenshotListener>>> GetListeners()
    {
        std::vector<wptr<IScreenshotListener>> screenshotListeners;
        {
            std::lock_guard<std::recursive_mutex> lock(globalMutex_);
            for (auto& listener : screenshotListeners_[GetWindowId()]) {
                screenshotListeners.push_back(listener);
            }
        }
        return screenshotListeners;
    }
    template<typename T>
    inline EnableIfSame<T, ITouchOutsideListener, std::vector<wptr<ITouchOutsideListener>>> GetListeners()
    {
        std::vector<wptr<ITouchOutsideListener>> touchOutsideListeners;
        {
            std::lock_guard<std::recursive_mutex> lock(globalMutex_);
            for (auto& listener : touchOutsideListeners_[GetWindowId()]) {
                touchOutsideListeners.push_back(listener);
            }
        }
        return touchOutsideListeners;
    }
    template<typename T>
    inline EnableIfSame<T, IDialogTargetTouchListener, std::vector<wptr<IDialogTargetTouchListener>>> GetListeners()
    {
        std::vector<wptr<IDialogTargetTouchListener>> dialogTargetTouchListeners;
        {
            std::lock_guard<std::recursive_mutex> lock(globalMutex_);
            for (auto& listener : dialogTargetTouchListeners_[GetWindowId()]) {
                dialogTargetTouchListeners.push_back(listener);
            }
        }
        return dialogTargetTouchListeners;
    }
    template<typename T>
    inline EnableIfSame<T, IWindowDragListener, std::vector<wptr<IWindowDragListener>>> GetListeners()
    {
        std::vector<wptr<IWindowDragListener>> windowDragListeners;
        {
            std::lock_guard<std::recursive_mutex> lock(mutex_);
            for (auto& listener : windowDragListeners_) {
                windowDragListeners.push_back(listener);
            }
        }
        return windowDragListeners;
    }
    template<typename T>
    inline EnableIfSame<T, IOccupiedAreaChangeListener, std::vector<wptr<IOccupiedAreaChangeListener>>> GetListeners()
    {
        std::vector<wptr<IOccupiedAreaChangeListener>> occupiedAreaChangeListeners;
        {
            std::lock_guard<std::recursive_mutex> lock(globalMutex_);
            for (auto& listener : occupiedAreaChangeListeners_[GetWindowId()]) {
                occupiedAreaChangeListeners.push_back(listener);
            }
        }
        return occupiedAreaChangeListeners;
    }
    template<typename T>
    inline EnableIfSame<T, IDialogDeathRecipientListener, wptr<IDialogDeathRecipientListener>> GetListener()
    {
        std::lock_guard<std::recursive_mutex> lock(globalMutex_);
        return dialogDeathRecipientListener_[GetWindowId()];
    }
    inline void NotifyAfterForeground(bool needNotifyUiContent = true)
    {
        auto lifecycleListeners = GetListeners<IWindowLifeCycle>();
        CALL_LIFECYCLE_LISTENER(AfterForeground, lifecycleListeners);
        if (needNotifyUiContent) {
            CALL_UI_CONTENT(Foreground);
        }
    }
    inline void NotifyAfterBackground()
    {
        auto lifecycleListeners = GetListeners<IWindowLifeCycle>();
        CALL_LIFECYCLE_LISTENER(AfterBackground, lifecycleListeners);
        CALL_UI_CONTENT(Background);
    }
    inline void NotifyAfterFocused()
    {
        auto lifecycleListeners = GetListeners<IWindowLifeCycle>();
        CALL_LIFECYCLE_LISTENER(AfterFocused, lifecycleListeners);
        CALL_UI_CONTENT(Focus);
    }
    inline void NotifyAfterUnfocused(bool needNotifyUiContent = true)
    {
        auto lifecycleListeners = GetListeners<IWindowLifeCycle>();
        // use needNotifyUinContent to separate ui content callbacks
        CALL_LIFECYCLE_LISTENER(AfterUnfocused, lifecycleListeners);
        if (needNotifyUiContent) {
            CALL_UI_CONTENT(UnFocus);
        }
    }
    inline void NotifyBeforeDestroy(std::string windowName)
    {
        std::lock_guard<std::recursive_mutex> lock(mutex_);
        if (uiContent_ != nullptr) {
            auto uiContent = std::move(uiContent_);
            uiContent_ = nullptr;
            uiContent->Destroy();
        }
        if (notifyNativefunc_) {
            notifyNativefunc_(windowName);
        }
    }
    inline void NotifyBeforeSubWindowDestroy(sptr<WindowImpl> window)
    {
        auto uiContent = window->GetUIContent();
        if (uiContent != nullptr) {
            uiContent->Destroy();
        }
        if (window->GetNativeDestroyCallback()) {
            window->GetNativeDestroyCallback()(window->GetWindowName());
        }
    }
    inline void NotifyAfterActive()
    {
        auto lifecycleListeners = GetListeners<IWindowLifeCycle>();
        CALL_LIFECYCLE_LISTENER(AfterActive, lifecycleListeners);
    }
    inline void NotifyAfterInactive()
    {
        auto lifecycleListeners = GetListeners<IWindowLifeCycle>();
        CALL_LIFECYCLE_LISTENER(AfterInactive, lifecycleListeners);
    }
    inline void NotifyForegroundFailed(WMError ret)
    {
        auto lifecycleListeners = GetListeners<IWindowLifeCycle>();
        CALL_LIFECYCLE_LISTENER_WITH_PARAM(ForegroundFailed, lifecycleListeners, static_cast<int32_t>(ret));
    }
    inline bool IsStretchableReason(WindowSizeChangeReason reason)
    {
        return reason == WindowSizeChangeReason::DRAG || reason == WindowSizeChangeReason::DRAG_END ||
            reason == WindowSizeChangeReason::DRAG_START || reason == WindowSizeChangeReason::RECOVER ||
            reason == WindowSizeChangeReason::MOVE || reason == WindowSizeChangeReason::UNDEFINED;
    }
    void ClearListenersById(uint32_t winId);
    void NotifySizeChange(Rect rect, WindowSizeChangeReason reason);
    void NotifyAvoidAreaChange(const sptr<AvoidArea>& avoidArea, AvoidAreaType type);
    void NotifyDisplayMoveChange(DisplayId from, DisplayId to);
    void NotifyOccupiedAreaChange(const sptr<OccupiedAreaChangeInfo>& info);
    void NotifyModeChange(WindowMode mode);
    void NotifyDragEvent(const PointInfo& point, DragEvent event);
    void DestroyDialogWindow();
    void DestroyFloatingWindow();
    void DestroySubWindow();
    void SetDefaultOption(); // for api7
    bool IsWindowValid() const;
    static sptr<Window> FindTopWindow(uint32_t topWinId);
    void TransferPointerEvent(const std::shared_ptr<MMI::PointerEvent>& pointerEvent);
    void ConsumeMoveOrDragEvent(const std::shared_ptr<MMI::PointerEvent>& pointerEvent);
    void ReadyToMoveOrDragWindow(const std::shared_ptr<MMI::PointerEvent>& pointerEvent,
        const MMI::PointerEvent::PointerItem& pointerItem);
    void EndMoveOrDragWindow(int32_t posX, int32_t posY, int32_t pointId, int32_t sourceType);
    void ResetMoveOrDragState();
    bool IsPointerEventConsumed();
    bool IsPointInDragHotZone(int32_t startPointPosX, int32_t startPointPosY);
    void AdjustWindowAnimationFlag(bool withAnimation = false);
    void MapFloatingWindowToAppIfNeeded();
    void MapDialogWindowToAppIfNeeded();
    WMError UpdateProperty(PropertyChangeAction action);
    WMError Destroy(bool needNotifyServer, bool needClearListener = true);
    WMError SetBackgroundColor(uint32_t color);
    uint32_t GetBackgroundColor() const;
    void InitAbilityInfo();
    std::shared_ptr<AppExecFwk::AbilityInfo> GetOriginalAbilityInfo() const;
    void RecordLifeCycleExceptionEvent(LifeCycleEvent event, WMError errCode) const;
    std::string TransferLifeCycleEventToString(LifeCycleEvent type) const;
    Rect GetSystemAlarmWindowDefaultSize(Rect defaultRect);
    void HandleModeChangeHotZones(int32_t posX, int32_t posY);
    WMError NotifyWindowTransition(TransitionReason reason);
    void UpdatePointerEventForStretchableWindow(const std::shared_ptr<MMI::PointerEvent>& pointerEvent);
    void UpdateDragType(int32_t startPointPosX, int32_t startPointPosY);
    void HandleBackKeyPressedEvent(const std::shared_ptr<MMI::KeyEvent>& keyEvent);
    bool CheckCameraFloatingWindowMultiCreated(WindowType type);
    void GetConfigurationFromAbilityInfo();
    void UpdateTitleButtonVisibility();
    void SetModeSupportInfo(uint32_t modeSupportInfo);
    uint32_t GetModeSupportInfo() const;
    WMError PreProcessShow(uint32_t reason, bool withAnimation);
    bool NeedToStopShowing();
    void CalculateStartRectExceptHotZone(float virtualPixelRatio);
    void SetSystemConfig();
    void TransformSurfaceNode(const Transform& trans);
    bool IsAppMainOrSunOrFloatingWindow();
    void SetWindowCornerRadiusAccordingToSystemConfig();
    bool IsAppMainOrSubOrFloatingWindow();
    void UpdateWindowShadowAccordingToSystemConfig();
    bool WindowCreateCheck(uint32_t parentId);
    uint32_t CalculatePointerDirection(int32_t pointerX, int32_t pointerY);
    void HandlePointerStyle(const std::shared_ptr<MMI::PointerEvent>& pointerEvent);
    RSSurfaceNode::SharedPtr CreateSurfaceNode(std::string name, WindowType type);
    void UpdateWindowStateUnfrozen();
    void UpdateViewportConfig(const Rect& rect, const sptr<class Display>& display, WindowSizeChangeReason reason);

    // colorspace, gamut
    using ColorSpaceConvertMap = struct {
        ColorSpace colorSpace;
        ColorGamut surfaceColorGamut;
    };
    static const ColorSpaceConvertMap colorSpaceConvertMap[];
    static ColorSpace GetColorSpaceFromSurfaceGamut(ColorGamut ColorGamut);
    static ColorGamut GetSurfaceGamutFromColorSpace(ColorSpace colorSpace);

    static std::map<std::string, std::pair<uint32_t, sptr<Window>>> windowMap_;
    static std::map<uint32_t, std::vector<sptr<WindowImpl>>> subWindowMap_;
    static std::map<uint32_t, std::vector<sptr<WindowImpl>>> appFloatingWindowMap_;
    static std::map<uint32_t, std::vector<sptr<WindowImpl>>> appDialogWindowMap_;
    sptr<WindowProperty> property_;
    WindowState state_ { WindowState::STATE_INITIAL };
    WindowTag windowTag_;
    sptr<IAceAbilityHandler> aceAbilityHandler_;
    static std::map<uint32_t, std::vector<sptr<IScreenshotListener>>> screenshotListeners_;
    static std::map<uint32_t, std::vector<sptr<ITouchOutsideListener>>> touchOutsideListeners_;
    static std::map<uint32_t, std::vector<sptr<IDialogTargetTouchListener>>> dialogTargetTouchListeners_;
    static std::map<uint32_t, std::vector<sptr<IWindowLifeCycle>>> lifecycleListeners_;
    static std::map<uint32_t, std::vector<sptr<IWindowChangeListener>>> windowChangeListeners_;
    static std::map<uint32_t, std::vector<sptr<IAvoidAreaChangedListener>>> avoidAreaChangeListeners_;
    std::vector<sptr<IWindowDragListener>> windowDragListeners_;
    std::vector<sptr<IDisplayMoveListener>> displayMoveListeners_;
    static std::map<uint32_t, std::vector<sptr<IOccupiedAreaChangeListener>>> occupiedAreaChangeListeners_;
    static std::map<uint32_t, sptr<IDialogDeathRecipientListener>> dialogDeathRecipientListener_;
    std::shared_ptr<IInputEventConsumer> inputEventConsumer_;
    sptr<IAnimationTransitionController> animationTransitionController_;
    NotifyNativeWinDestroyFunc notifyNativefunc_;
    std::shared_ptr<RSSurfaceNode> surfaceNode_;
    std::string name_;
    std::unique_ptr<Ace::UIContent> uiContent_;
    std::shared_ptr<AbilityRuntime::Context> context_;
    std::recursive_mutex mutex_;
    static std::recursive_mutex globalMutex_;
    const float SYSTEM_ALARM_WINDOW_WIDTH_RATIO = 0.8;
    const float SYSTEM_ALARM_WINDOW_HEIGHT_RATIO = 0.3;
    WindowSizeChangeReason lastSizeChangeReason_ = WindowSizeChangeReason::END;

    sptr<MoveDragProperty> moveDragProperty_;
    bool isAppDecorEnable_ = true;
    SystemConfig windowSystemConfig_ ;
    bool isOriginRectSet_ = false;
    bool needRemoveWindowInputChannel_ = false;
    bool isMainHandlerAvailable_ = true;
    bool isAppFloatingWindow_ = false;
    bool isFocused_ = false;
    uint32_t mouseStyleID_ = 0;
    bool isPointerStyleChanged_ = false;
    const std::map<DragType, uint32_t> STYLEID_MAP = {
        {DragType::DRAG_UNDEFINED, MMI::MOUSE_ICON::DEFAULT},
        {DragType::DRAG_BOTTOM_OR_TOP, MMI::MOUSE_ICON::NORTH_SOUTH},
        {DragType::DRAG_LEFT_OR_RIGHT, MMI::MOUSE_ICON::WEST_EAST},
        {DragType::DRAG_LEFT_TOP_CORNER, MMI::MOUSE_ICON::NORTH_WEST_SOUTH_EAST},
        {DragType::DRAG_RIGHT_TOP_CORNER, MMI::MOUSE_ICON::NORTH_EAST_SOUTH_WEST}
    };
    float virtualPixelRatio_ = 0;
    bool dragHotZoneNone_ = false;
};
} // namespace Rosen
} // namespace OHOS
#endif // OHOS_ROSEN_WINDOW_IMPL_H

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

#include "window_impl.h"
#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
std::map<std::string, std::pair<uint32_t, sptr<Window>>> WindowImpl::windowMap_;
std::map<uint32_t, std::vector<sptr<WindowImpl>>> WindowImpl::subWindowMap_;
static int constructorCnt = 0;
static int deConstructorCnt = 0;
WindowImpl::WindowImpl(const sptr<WindowOption>& option)
{
    property_ = new (std::nothrow) WindowProperty();
    property_->SetWindowName(option->GetWindowName());
    property_->SetRequestRect(option->GetWindowRect());
    property_->SetWindowType(option->GetWindowType());
    property_->SetWindowMode(option->GetWindowMode());
    property_->SetFullScreen(option->GetWindowMode() == WindowMode::WINDOW_MODE_FULLSCREEN);
    property_->SetFocusable(option->GetFocusable());
    property_->SetTouchable(option->GetTouchable());
    property_->SetDisplayId(option->GetDisplayId());
    property_->SetCallingWindow(option->GetCallingWindow());
    property_->SetWindowFlags(option->GetWindowFlags());
    property_->SetHitOffset(option->GetHitOffset());
    property_->SetRequestedOrientation(option->GetRequestedOrientation());
    property_->SetTurnScreenOn(option->IsTurnScreenOn());
    property_->SetKeepScreenOn(option->IsKeepScreenOn());
    property_->SetBrightness(option->GetBrightness());
    auto& sysBarPropMap = option->GetSystemBarProperty();
    for (auto it : sysBarPropMap) {
        property_->SetSystemBarProperty(it.first, it.second);
    }
    name_ = option->GetWindowName();

    WLOGFI("WindowImpl constructorCnt: %{public}d name: %{public}s",
        ++constructorCnt, property_->GetWindowName().c_str());
}

WindowImpl::~WindowImpl()
{
    WLOGFI("windowName: %{public}s, windowId: %{public}d, deConstructorCnt: %{public}d",
        GetWindowName().c_str(), GetWindowId(), ++deConstructorCnt);
    Destroy();
}

sptr<Window> WindowImpl::Find(const std::string& name)
{
    auto iter = windowMap_.find(name);
    if (iter == windowMap_.end()) {
        return nullptr;
    }
    return iter->second.second;
}

const std::shared_ptr<AbilityRuntime::Context> WindowImpl::GetContext() const
{
    return nullptr;
}

sptr<Window> WindowImpl::GetTopWindowWithId(uint32_t mainWinId)
{
    if (windowMap_.empty()) {
        WLOGFE("Please create mainWindow First!");
        return nullptr;
    }
    for (auto iter = windowMap_.begin(); iter != windowMap_.end(); iter++) {
        if (mainWinId == iter->second.first) {
            WLOGFI("FindTopWindow id: %{public}u", mainWinId);
            return iter->second.second;
        }
    }
    WLOGFE("Cannot find topWindow!");
}

sptr<Window> WindowImpl::GetTopWindowWithContext(const std::shared_ptr<AbilityRuntime::Context>& context)
{
    if (windowMap_.empty()) {
        WLOGFE("Please create mainWindow First!");
        return nullptr;
    }
    uint32_t mainWinId = INVALID_WINDOW_ID;
    WLOGFI("GetTopWindowfinal MainWinId:%{public}u!", mainWinId);
    if (mainWinId == INVALID_WINDOW_ID) {
        WLOGFE("Cannot find topWindow!");
        return nullptr;
    }
    return GetTopWindowWithId(mainWinId);
}

std::vector<sptr<Window>> WindowImpl::GetSubWindow(uint32_t parentId)
{
    if (subWindowMap_.find(parentId) == subWindowMap_.end()) {
        WLOGFE("Cannot parentWindow with id: %{public}u!", parentId);
        return std::vector<sptr<Window>>();
    }
    return std::vector<sptr<Window>>(subWindowMap_[parentId].begin(), subWindowMap_[parentId].end());
}

void WindowImpl::UpdateConfigurationForAll(const std::shared_ptr<AppExecFwk::Configuration>& configuration)
{
    for (const auto& winPair : windowMap_) {
        auto window = winPair.second.second;
        window->UpdateConfiguration(configuration);
    }
}

std::shared_ptr<RSSurfaceNode> WindowImpl::GetSurfaceNode() const
{
    return nullptr;
}

Rect WindowImpl::GetRect() const
{
    return property_->GetWindowRect();
}

Rect WindowImpl::GetRequestRect() const
{
    return property_->GetRequestRect();
}

WindowType WindowImpl::GetType() const
{
    return property_->GetWindowType();
}

WindowMode WindowImpl::GetMode() const
{
    return property_->GetWindowMode();
}

float WindowImpl::GetAlpha() const
{
    return property_->GetAlpha();
}

WindowState WindowImpl::GetWindowState() const
{
    return state_;
}

WMError WindowImpl::SetFocusable(bool isFocusable)
{
    return WMError::WM_OK;
}

bool WindowImpl::GetFocusable() const
{
    return property_->GetFocusable();
}

WMError WindowImpl::SetTouchable(bool isTouchable)
{
    return WMError::WM_OK;
}

bool WindowImpl::GetTouchable() const
{
    return property_->GetTouchable();
}

const std::string& WindowImpl::GetWindowName() const
{
    return name_;
}

uint32_t WindowImpl::GetWindowId() const
{
    return property_->GetWindowId();
}

uint32_t WindowImpl::GetWindowFlags() const
{
    return property_->GetWindowFlags();
}

uint32_t WindowImpl::GetRequestModeSupportInfo() const
{
    return property_->GetRequestModeSupportInfo();
}

bool WindowImpl::IsMainHandlerAvailable() const
{
    return true;
}

SystemBarProperty WindowImpl::GetSystemBarPropertyByType(WindowType type) const
{
    auto curProperties = property_->GetSystemBarProperty();
    return curProperties[type];
}

WMError WindowImpl::GetAvoidAreaByType(AvoidAreaType type, AvoidArea& avoidArea)
{
    return WMError::WM_OK;
}

WMError WindowImpl::SetWindowType(WindowType type)
{
    return WMError::WM_OK;
}

WMError WindowImpl::SetWindowMode(WindowMode mode)
{
    return WMError::WM_OK;
}

void WindowImpl::SetAlpha(float alpha)
{
    return;
}

void WindowImpl::SetTransform(const Transform& trans)
{
    return;
}

const Transform& WindowImpl::GetTransform() const
{
    return property_->GetTransform();
}

WMError WindowImpl::AddWindowFlag(WindowFlag flag)
{
    return WMError::WM_OK;
}

WMError WindowImpl::RemoveWindowFlag(WindowFlag flag)
{
    return WMError::WM_OK;
}

WMError WindowImpl::SetWindowFlags(uint32_t flags)
{
    return WMError::WM_OK;
}

void WindowImpl::OnNewWant(const AAFwk::Want& want)
{
    return;
}

WMError WindowImpl::SetUIContent(const std::string& contentInfo,
    NativeEngine* engine, NativeValue* storage, bool isdistributed, AppExecFwk::Ability* ability)
{
    return WMError::WM_OK;
}

Ace::UIContent* WindowImpl::GetUIContent() const
{
    return uiContent_.get();
}

std::string WindowImpl::GetContentInfo()
{
    return "";
}

bool WindowImpl::IsSupportWideGamut()
{
    return true;
}

void WindowImpl::SetColorSpace(ColorSpace colorSpace)
{
    return;
}

ColorSpace WindowImpl::GetColorSpace()
{
    return ColorSpace::COLOR_SPACE_DEFAULT;
}

std::shared_ptr<Media::PixelMap> WindowImpl::Snapshot()
{
    return nullptr;
}

void WindowImpl::DumpInfo(const std::vector<std::string>& params, std::vector<std::string>& info)
{
    return;
}

WMError WindowImpl::SetSystemBarProperty(WindowType type, const SystemBarProperty& property)
{
    return WMError::WM_OK;
}

WMError WindowImpl::SetLayoutFullScreen(bool status)
{
    return WMError::WM_OK;
}

WMError WindowImpl::SetFullScreen(bool status)
{
    return WMError::WM_OK;
}

WMError WindowImpl::Create(uint32_t parentId, const std::shared_ptr<AbilityRuntime::Context>& context)
{
    WLOGFI("[Client] Window [name:%{public}s] Create", name_.c_str());
    // check window name, same window names are forbidden
    if (windowMap_.find(name_) != windowMap_.end()) {
        WLOGFE("WindowName(%{public}s) already exists.", name_.c_str());
        return WMError::WM_ERROR_INVALID_PARAM;
    }
    // check parent id, if create sub window and there is not exist parent Window, then return
    if (parentId != INVALID_WINDOW_ID) {
        for (const auto& winPair : windowMap_) {
            if (winPair.second.first == parentId) {
                property_->SetParentId(parentId);
                break;
            }
        }
        if (property_->GetParentId() != parentId) {
            WLOGFE("ParentId is empty or valid. ParentId is %{public}u", parentId);
            return WMError::WM_ERROR_INVALID_PARAM;
        }
    }

    static std::atomic<uint32_t> tempWindowId = 0;
    uint32_t windowId = tempWindowId++;
    property_->SetWindowId(windowId);
    windowMap_.insert(std::make_pair(name_, std::pair<uint32_t, sptr<Window>>(windowId, this)));

    state_ = WindowState::STATE_CREATED;

    return WMError::WM_OK;
}

WMError WindowImpl::BindDialogTarget(sptr<IRemoteObject> targetToken)
{
    return WMError::WM_OK;
}

WMError WindowImpl::Destroy()
{
    return WMError::WM_OK;
}

WMError WindowImpl::UpdateSurfaceNodeAfterCustomAnimation(bool isAdd)
{
    return WMError::WM_OK;
}

WMError WindowImpl::Show(uint32_t reason, bool withAnimation)
{
    return WMError::WM_OK;
}

WMError WindowImpl::Hide(uint32_t reason, bool withAnimation)
{
    return WMError::WM_OK;
}

WMError WindowImpl::MoveTo(int32_t x, int32_t y)
{
    return WMError::WM_OK;
}

WMError WindowImpl::Resize(uint32_t width, uint32_t height)
{
    return WMError::WM_OK;
}

WMError WindowImpl::SetKeepScreenOn(bool keepScreenOn)
{
    return WMError::WM_OK;
}

bool WindowImpl::IsKeepScreenOn() const
{
    return property_->IsKeepScreenOn();
}

WMError WindowImpl::SetTurnScreenOn(bool turnScreenOn)
{
    return WMError::WM_OK;
}

bool WindowImpl::IsTurnScreenOn() const
{
    return property_->IsTurnScreenOn();
}

WMError WindowImpl::SetBackgroundColor(const std::string& color)
{
    return WMError::WM_OK;
}

WMError WindowImpl::SetTransparent(bool isTransparent)
{
    return WMError::WM_OK;
}

bool WindowImpl::IsTransparent() const
{
    return true;
}

WMError WindowImpl::SetBrightness(float brightness)
{
    return WMError::WM_OK;
}

float WindowImpl::GetBrightness() const
{
    return property_->GetBrightness();
}

WMError WindowImpl::SetCallingWindow(uint32_t windowId)
{
    return WMError::WM_OK;
}

void WindowImpl::SetPrivacyMode(bool isPrivacyMode)
{
    return;
}

bool WindowImpl::IsPrivacyMode() const
{
    return property_->GetPrivacyMode();
}

void WindowImpl::SetSystemPrivacyMode(bool isSystemPrivacyMode)
{
    return;
}

void WindowImpl::SetSnapshotSkip(bool isSkip)
{
    return;
}

void WindowImpl::DisableAppWindowDecor()
{
    return;
}

bool WindowImpl::IsDecorEnable() const
{
    return property_->GetDecorEnable();
}

WMError WindowImpl::Maximize()
{
    return WMError::WM_OK;
}

WMError WindowImpl::Minimize()
{
    return WMError::WM_OK;
}

WMError WindowImpl::Recover()
{
    return WMError::WM_OK;
}

WMError WindowImpl::Close()
{
    return WMError::WM_OK;
}

void WindowImpl::StartMove()
{
    return;
}

WMError WindowImpl::RequestFocus() const
{
    return WMError::WM_OK;
}

bool WindowImpl::IsFocused() const
{
    return true;
}

void WindowImpl::SetInputEventConsumer(const std::shared_ptr<IInputEventConsumer>& inputEventConsumer)
{
    return;
}

bool WindowImpl::RegisterLifeCycleListener(const sptr<IWindowLifeCycle>& listener)
{
    return true;
}

bool WindowImpl::RegisterWindowChangeListener(const sptr<IWindowChangeListener>& listener)
{
    return true;
}

bool WindowImpl::UnregisterLifeCycleListener(const sptr<IWindowLifeCycle>& listener)
{
    return true;
}

bool WindowImpl::UnregisterWindowChangeListener(const sptr<IWindowChangeListener>& listener)
{
    return true;
}

bool WindowImpl::RegisterAvoidAreaChangeListener(sptr<IAvoidAreaChangedListener>& listener)
{
    return true;
}

bool WindowImpl::UnregisterAvoidAreaChangeListener(sptr<IAvoidAreaChangedListener>& listener)
{
    return true;
}

bool WindowImpl::RegisterDragListener(const sptr<IWindowDragListener>& listener)
{
    return true;
}

bool WindowImpl::UnregisterDragListener(const sptr<IWindowDragListener>& listener)
{
    return true;
}

bool WindowImpl::RegisterDisplayMoveListener(sptr<IDisplayMoveListener>& listener)
{
    return true;
}

bool WindowImpl::UnregisterDisplayMoveListener(sptr<IDisplayMoveListener>& listener)
{
    return true;
}

void WindowImpl::RegisterWindowDestroyedListener(const NotifyNativeWinDestroyFunc& func)
{
    return;
}

bool WindowImpl::RegisterOccupiedAreaChangeListener(const sptr<IOccupiedAreaChangeListener>& listener)
{
    return true;
}

bool WindowImpl::UnregisterOccupiedAreaChangeListener(const sptr<IOccupiedAreaChangeListener>& listener)
{
    return true;
}

bool WindowImpl::RegisterTouchOutsideListener(const sptr<ITouchOutsideListener>& listener)
{
    return true;
}

bool WindowImpl::UnregisterTouchOutsideListener(const sptr<ITouchOutsideListener>& listener)
{
    return true;
}

bool WindowImpl::RegisterAnimationTransitionController(const sptr<IAnimationTransitionController>& listener)
{
    return true;
}

bool WindowImpl::RegisterScreenshotListener(const sptr<IScreenshotListener>& listener)
{
    return true;
}

bool WindowImpl::UnregisterScreenshotListener(const sptr<IScreenshotListener>& listener)
{
    return true;
}

bool WindowImpl::RegisterDialogTargetTouchListener(const sptr<IDialogTargetTouchListener>& listener)
{
    return true;
}

bool WindowImpl::UnregisterDialogTargetTouchListener(const sptr<IDialogTargetTouchListener>& listener)
{
    return true;
}

void WindowImpl::RegisterDialogDeathRecipientListener(const sptr<IDialogDeathRecipientListener>& listener)
{
    return;
}

void WindowImpl::UnregisterDialogDeathRecipientListener(const sptr<IDialogDeathRecipientListener>& listener)
{
    return;
}

void WindowImpl::SetAceAbilityHandler(const sptr<IAceAbilityHandler>& handler)
{
    return;
}

void WindowImpl::SetRequestModeSupportInfo(uint32_t modeSupportInfo)
{
    return;
}

void WindowImpl::ConsumeKeyEvent(std::shared_ptr<MMI::KeyEvent>& keyEvent)
{
    return;
}

void WindowImpl::ConsumePointerEvent(const std::shared_ptr<MMI::PointerEvent>& pointerEvent)
{
    return;
}

void WindowImpl::RequestVsync(const std::shared_ptr<VsyncCallback>& vsyncCallback)
{
    return;
}

void WindowImpl::UpdateConfiguration(const std::shared_ptr<AppExecFwk::Configuration>& configuration)
{
    if (uiContent_ != nullptr) {
        WLOGFD("notify ace winId:%{public}u", GetWindowId());
        uiContent_->UpdateConfiguration(configuration);
    }
    if (subWindowMap_.count(GetWindowId()) == 0) {
        return;
    }
    for (auto& subWindow : subWindowMap_.at(GetWindowId())) {
        subWindow->UpdateConfiguration(configuration);
    }
}

void WindowImpl::NotifyTouchDialogTarget()
{
    return;
}

void WindowImpl::SetNeedRemoveWindowInputChannel(bool needRemoveWindowInputChannel)
{
    return;
}

bool WindowImpl::IsLayoutFullScreen() const
{
    return true;
}

bool WindowImpl::IsFullScreen() const
{
    return true;
}

void WindowImpl::SetRequestedOrientation(Orientation orientation)
{
    return;
}

Orientation WindowImpl::GetRequestedOrientation()
{
    return property_->GetRequestedOrientation();
}

WMError WindowImpl::SetTouchHotAreas(const std::vector<Rect>& rects)
{
    return WMError::WM_OK;
}
void WindowImpl::GetRequestedTouchHotAreas(std::vector<Rect>& rects) const
{
    property_->GetTouchHotAreas(rects);
}

WMError WindowImpl::SetAPPWindowLabel(const std::string& label)
{
    if (uiContent_ == nullptr) {
        WLOGFE("uicontent is empty");
        return WMError::WM_ERROR_NULLPTR;
    }
    uiContent_->SetAppWindowTitle(label);
    return WMError::WM_OK;
}

WMError WindowImpl::SetAPPWindowIcon(const std::shared_ptr<Media::PixelMap>& icon)
{
    if (icon == nullptr) {
        WLOGFE("window icon is empty");
        return WMError::WM_ERROR_NULLPTR;
    }
    if (uiContent_ == nullptr) {
        WLOGFE("uicontent is empty");
        return WMError::WM_ERROR_NULLPTR;
    }
    uiContent_->SetAppWindowIcon(icon);
    return WMError::WM_OK;
}

WMError WindowImpl::SetCornerRadius(float cornerRadius)
{
    return WMError::WM_OK;
}

WMError WindowImpl::SetShadowRadius(float radius)
{
    return WMError::WM_OK;
}

WMError WindowImpl::SetShadowColor(std::string color)
{
    return WMError::WM_OK;
}

void WindowImpl::SetShadowOffsetX(float offsetX)
{
    return;
}

void WindowImpl::SetShadowOffsetY(float offsetY)
{
    return;
}

WMError WindowImpl::SetBlur(float radius)
{
    return WMError::WM_OK;
}

WMError WindowImpl::SetBackdropBlur(float radius)
{
    return WMError::WM_OK;
}

WMError WindowImpl::SetBackdropBlurStyle(WindowBlurStyle blurStyle)
{
    return WMError::WM_OK;
}

WMError WindowImpl::NotifyMemoryLevel(int32_t level) const
{
    return WMError::WM_OK;
}

bool WindowImpl::IsAllowHaveSystemSubWindow()
{
    return true;
}
} // namespace Rosen
} // namespace OHOS

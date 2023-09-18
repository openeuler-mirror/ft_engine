/*
 * Copyright (c) 2023 Huawei Technologies Co., Ltd.
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

#include <linux/input.h>
#include "wayland_surface.h"

#include "wayland_objects_pool.h"
#include "ui/rs_surface_extractor.h"
#include "wayland_region.h"
#include "wayland_seat.h"
#include "input_manager.h"

namespace FT {
namespace Wayland {
namespace {
    constexpr HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WAYLAND, "WaylandSurface"};
}

class InputEventConsumer : public OHOS::Rosen::IInputEventConsumer
{
public:
    InputEventConsumer(OHOS::sptr<WaylandSurface> wlSurface)
    {
        wlSurface_ = wlSurface;
    }

    bool OnInputEvent(const std::shared_ptr<OHOS::MMI::KeyEvent>& keyEvent) const override;
    bool OnInputEvent(const std::shared_ptr<OHOS::MMI::AxisEvent>& axisEvent) const override;
    bool OnInputEvent(const std::shared_ptr<OHOS::MMI::PointerEvent>& pointerEvent) const override;
private:
    OHOS::sptr<WaylandSurface> wlSurface_ = nullptr;
    int32_t MapPointerActionButton(int32_t PointerActionButtonType) const;
    int32_t MapKeyAction(int32_t keyAction) const;
    const std::map<uint32_t, int32_t> ptrActionMap_ = {
        {OHOS::MMI::PointerEvent::MOUSE_BUTTON_LEFT,   BTN_LEFT},
        {OHOS::MMI::PointerEvent::MOUSE_BUTTON_RIGHT,  BTN_RIGHT},
    };
    const std::map<uint32_t, int32_t> keyActionMap_ = {
        {OHOS::MMI::KeyEvent::KEY_ACTION_UP, WL_KEYBOARD_KEY_STATE_RELEASED},
        {OHOS::MMI::KeyEvent::KEY_ACTION_DOWN, WL_KEYBOARD_KEY_STATE_PRESSED},
    };
    const int32_t INVALID_KEYACTION = -1;
};

int32_t InputEventConsumer::MapKeyAction(int32_t keyAction) const
{
    auto it = keyActionMap_.find(keyAction);
    if (it == keyActionMap_.end()) {
        return INVALID_KEYACTION;
    }
    return it->second;
}

int32_t InputEventConsumer::MapPointerActionButton(int32_t PointerActionButtonType) const
{
    auto it = ptrActionMap_.find(PointerActionButtonType);
    if (it == ptrActionMap_.end()) {
        return OHOS::MMI::PointerEvent::BUTTON_NONE;
    }
    return it->second;
}

bool InputEventConsumer::OnInputEvent(const std::shared_ptr<OHOS::MMI::KeyEvent>& keyEvent) const
{
    OHOS::sptr<WaylandSeat> wlSeat = WaylandSeat::GetWaylandSeatGlobal();
    if (wlSeat == nullptr) {
        return false;
    }
    auto keyboard = wlSeat->GetKeyboardResource(wlSurface_->WlClient());
    if (keyboard == nullptr) {
        LOG_WARN("GetKeyboardResource fail");
        return false;
    }

    int32_t keyAction = MapKeyAction(keyEvent->GetKeyAction());
    if (keyAction == INVALID_KEYACTION) {
        return false;
    }

    keyboard->OnKeyboardKey(keyEvent->GetKeyCode(), keyAction, keyEvent->GetActionTime());
    keyEvent->MarkProcessed();
    return true;
}

bool InputEventConsumer::OnInputEvent(const std::shared_ptr<OHOS::MMI::AxisEvent>& axisEvent) const
{
    axisEvent->MarkProcessed();
    return true;
}

bool InputEventConsumer::OnInputEvent(const std::shared_ptr<OHOS::MMI::PointerEvent>& pointerEvent) const
{
    OHOS::sptr<WaylandSeat> wlSeat = WaylandSeat::GetWaylandSeatGlobal();
    if (wlSeat == nullptr) {
        return false;
    }
    pointerEvent->MarkProcessed();
    auto pointer = wlSeat->GetPointerResource(wlSurface_->WlClient());
    if (pointer == nullptr) {
        LOG_WARN("GetPointerResource fail");
        return false;
    }

    auto keyboard = wlSeat->GetKeyboardResource(wlSurface_->WlClient());
    if (keyboard == nullptr) {
        LOG_WARN("GetKeyboardResource fail");
        return false;
    }

    OHOS::MMI::PointerEvent::PointerItem pointerItem;
    int32_t pointId = pointerEvent->GetPointerId();
    if (!pointerEvent->GetPointerItem(pointId, pointerItem)) {
        LOG_WARN("GetPointerItem fail");
        return false;
    }

    if (pointerEvent->GetPointerAction() ==  OHOS::MMI::PointerEvent::POINTER_ACTION_ENTER_WINDOW) {
        pointer->OnPointerEnter(pointerItem.GetWindowX(), pointerItem.GetWindowY(), wlSurface_->WlResource());
        keyboard->OnKeyboardEnter(wlSurface_->WlResource());
    } else if (pointerEvent->GetPointerAction() ==  OHOS::MMI::PointerEvent::POINTER_ACTION_LEAVE_WINDOW) {
        pointer->OnPointerLeave(wlSurface_->WlResource());
        keyboard->OnKeyboardLeave(wlSurface_->WlResource());
    } else if (pointerEvent->GetPointerAction() == OHOS::MMI::PointerEvent::POINTER_ACTION_BUTTON_DOWN ||
        pointerEvent->GetPointerAction() == OHOS::MMI::PointerEvent::POINTER_ACTION_BUTTON_UP) {
        int32_t buttonId = MapPointerActionButton(pointerEvent->GetButtonId());
        if (buttonId != OHOS::MMI::PointerEvent::BUTTON_NONE) {
            pointer->OnPointerButton(pointerEvent->GetActionTime(), buttonId, pointerItem.IsPressed());
        }
    } else if (pointerEvent->GetPointerAction() == OHOS::MMI::PointerEvent::POINTER_ACTION_MOVE) {
        pointer->OnPointerMotionAbsolute(
            pointerEvent->GetActionTime(), pointerItem.GetWindowX(), pointerItem.GetWindowY());
    }
    return true;
}


struct wl_surface_interface IWaylandSurface::impl_ = {
    .destroy = &WaylandResourceObject::DefaultDestroyResource,
    .attach = Attach,
    .damage = Damage,
    .frame = Frame,
    .set_opaque_region = SetOpaqueRegion,
    .set_input_region = SetInputRegion,
    .commit = Commit,
    .set_buffer_transform = SetBufferTransform,
    .set_buffer_scale = SetBufferScale,
    .damage_buffer = DamageBuffer,
    .offset = Offset};

void IWaylandSurface::Attach(struct wl_client *client, struct wl_resource *surfaceResource,
    struct wl_resource *bufferResource, int32_t x, int32_t y)
{
    CAST_OBJECT_AND_CALL_FUNC(WaylandSurface, surfaceResource,
        "IWaylandSurface::Attach: failed to find object.", Attach, bufferResource, x, y);
}

void IWaylandSurface::Damage(struct wl_client *client, struct wl_resource *resource,
    int32_t x, int32_t y, int32_t width, int32_t height)
{
    CAST_OBJECT_AND_CALL_FUNC(WaylandSurface, resource,
        "IWaylandSurface::Damage: failed to find object.", Damage, x, y, width, height);
}

void IWaylandSurface::Frame(struct wl_client *client, struct wl_resource *resource, uint32_t callback)
{
    CAST_OBJECT_AND_CALL_FUNC(WaylandSurface, resource,
        "IWaylandSurface::Frame: failed to find object.", Frame, callback);
}

void IWaylandSurface::SetOpaqueRegion(struct wl_client *client,
    struct wl_resource *surfaceResource, struct wl_resource *regionResource)
{
    CAST_OBJECT_AND_CALL_FUNC(WaylandSurface, surfaceResource,
        "IWaylandSurface::SetOpaqueRegion: failed to find object.", SetOpaqueRegion, regionResource);
}

void IWaylandSurface::SetInputRegion(struct wl_client *client,
    struct wl_resource *surfaceResource, struct wl_resource *regionResource)
{
    CAST_OBJECT_AND_CALL_FUNC(WaylandSurface, surfaceResource,
        "IWaylandSurface::SetInputRegion: failed to find object.", SetInputRegion, regionResource);
}

void IWaylandSurface::Commit(struct wl_client *client, struct wl_resource *resource)
{
    CAST_OBJECT_AND_CALL_FUNC(WaylandSurface, resource, "IWaylandSurface::Commit: failed to find object.", Commit);
}

void IWaylandSurface::SetBufferTransform(struct wl_client *client, struct wl_resource *resource, int32_t transform)
{
    CAST_OBJECT_AND_CALL_FUNC(WaylandSurface, resource,
        "IWaylandSurface::SetBufferTransform: failed to find object.", SetBufferTransform, transform);
}

void IWaylandSurface::SetBufferScale(struct wl_client *client, struct wl_resource *resource, int32_t scale)
{
    CAST_OBJECT_AND_CALL_FUNC(WaylandSurface, resource,
        "IWaylandSurface::SetBufferScale: failed to find object.", SetBufferScale, scale);
}

void IWaylandSurface::DamageBuffer(struct wl_client *client, struct wl_resource *resource,
    int32_t x, int32_t y, int32_t width, int32_t height)
{
    CAST_OBJECT_AND_CALL_FUNC(WaylandSurface, resource,
        "IWaylandSurface::DamageBuffer: failed to find object.", DamageBuffer, x, y, width, height);
}

void IWaylandSurface::Offset(struct wl_client *client, struct wl_resource *resource, int32_t x, int32_t y)
{
    CAST_OBJECT_AND_CALL_FUNC(WaylandSurface, resource,
        "IWaylandSurface::Offset: failed to find object.", Offset, x, y);
}

OHOS::sptr<WaylandSurface> WaylandSurface::Create(struct wl_client *client,
    struct wl_resource *parent, uint32_t version, uint32_t id)
{
    if (client == nullptr) {
        return nullptr;
    }

    auto surface = OHOS::sptr<WaylandSurface>(new WaylandSurface(client, parent, version, id));
    WaylandObjectsPool::GetInstance().AddObject(ObjectId(surface->WlClient(), surface->Id()), surface);
    return surface;
}

class WaylandWindowListener : public OHOS::Rosen::IWindowChangeListener {
public:
    WaylandWindowListener(OHOS::sptr<WaylandSurface> wlSurface) : wlSurface_(wlSurface) {}
    ~WaylandWindowListener() = default;
    void OnSizeChange(OHOS::Rosen::Rect rect, OHOS::Rosen::WindowSizeChangeReason reason) override;
    void OnModeChange(OHOS::Rosen::WindowMode mode) override;

private:
    OHOS::sptr<WaylandSurface> wlSurface_ = nullptr;
};

void WaylandWindowListener::OnSizeChange(OHOS::Rosen::Rect rect, OHOS::Rosen::WindowSizeChangeReason reason)
{
    if (wlSurface_ != nullptr) {
        wlSurface_->OnSizeChange(rect, reason);
    }
}

void WaylandWindowListener::OnModeChange(OHOS::Rosen::WindowMode mode)
{
    if (wlSurface_ != nullptr) {
        wlSurface_->OnModeChange(mode);
    }
}

WaylandSurface::WaylandSurface(struct wl_client *client, struct wl_resource *parent, uint32_t version, uint32_t id)
    : WaylandResourceObject(client, &wl_surface_interface, version, id, &IWaylandSurface::impl_),
      parent_(parent) {}

WaylandSurface::~WaylandSurface() noexcept {}

void WaylandSurface::AddCommitCallback(SurfaceCommitCallback callback)
{
    commitCallbacks_.push_back(std::move(callback));
}

void WaylandSurface::AddRectCallback(SurfaceRectCallback callback)
{
    rectCallbacks_.push_back(std::move(callback));
}

void WaylandSurface::Attach(struct wl_resource *bufferResource, int32_t x, int32_t y)
{
    if (new_.buffer != nullptr) {
        wl_callback_send_done(new_.buffer, 0);
    }

    new_.buffer = bufferResource;
    new_.offsetX = x;
    new_.offsetY = y;
}

void WaylandSurface::Damage(int32_t x, int32_t y, int32_t width, int32_t height)
{
    new_.damage.x = x;
    new_.damage.y = y;
    new_.damage.width = static_cast<uint32_t>(width);
    new_.damage.height = static_cast<uint32_t>(height);
}

void WaylandSurface::Frame(uint32_t callback)
{
    if (new_.cb != nullptr) {
        LOG_WARN("duplicate frame request");
        return;
    }

    auto cb = FrameCallback::Create(WlClient(), WAYLAND_VERSION_MAJOR, callback);
    if (cb == nullptr) {
        LOG_ERROR("no memory");
        return;
    }

    WaylandObjectsPool::GetInstance().AddObject(ObjectId(cb->WlClient(), cb->Id()), cb);
    new_.cb = cb;
}

void WaylandSurface::SetOpaqueRegion(struct wl_resource *regionResource)
{
    LOG_DEBUG("WaylandSurface::SetOpaqueRegion, ignore");
}

void WaylandSurface::SetInputRegion(struct wl_resource *regionResource)
{
    if (regionResource == nullptr) {
        return;
    }

    auto region = CastFromResource<WaylandRegion>(regionResource);
    if (region == nullptr) {
        LOG_ERROR("failed to cast WaylandRegion from regionResource, maybe resource is not valid.");
        return;
    }

    // input
    Rect rect = region->GetRect();
    LOG_DEBUG("SetInputRegion, rect: x %{public}d, y %{public}d, width %{public}d, height %{public}d.",
        rect.x, rect.y, rect.width, rect.height);
}

void WaylandSurface::StartMove()
{
    if (window_ != nullptr) {
        window_->StartMove();
    }
}

void WaylandSurface::SetWindowMode(OHOS::Rosen::WindowMode mode)
{
    mode_ = mode;
}

void WaylandSurface::SetWindowType(OHOS::Rosen::WindowType type)
{
    type_ = type;
}

void WaylandSurface::Commit()
{
    if (window_ == nullptr) {
        CreateWindow();
    } else {
        HandleCommit();
    }

    for (auto &cb : commitCallbacks_) {
        cb();
    }
}

void WaylandSurface::SetBufferTransform(int32_t transform)
{
    new_.transform = static_cast<wl_output_transform>(transform);
}

void WaylandSurface::SetBufferScale(int32_t scale)
{
    new_.scale = scale;
}

void WaylandSurface::DamageBuffer(int32_t x, int32_t y, int32_t width, int32_t height)
{
    new_.damageBuffer.x = x;
    new_.damageBuffer.y = y;
    new_.damageBuffer.width = static_cast<uint32_t>(width);
    new_.damageBuffer.height = static_cast<uint32_t>(height);
}

void WaylandSurface::Offset(int32_t x, int32_t y)
{
    new_.offsetX = x;
    new_.offsetY = y;
}

void WaylandSurface::HandleCommit() {
    if (new_.buffer != nullptr) {
        wl_shm_buffer *shm = wl_shm_buffer_get(new_.buffer);
        if (shm == nullptr) {
            LOG_ERROR("wl_shm_buffer_get fail");
            wl_callback_send_done(new_.buffer, 0);
            return;
        }

        wl_shm_buffer_begin_access(shm);
        CopyBuffer(shm);
        wl_shm_buffer_end_access(shm);

        wl_callback_send_done(new_.buffer, 0);
        new_.buffer = nullptr;
    }

    if (new_.cb != nullptr) {
        wl_callback_send_done(new_.cb->WlResource(), 0);
        wl_resource_destroy(new_.cb->WlResource());
        new_.cb = nullptr;
    }

    old_ = new_;
    new_.Reset();
}

void WaylandSurface::CreateWindow()
{
    OHOS::sptr<OHOS::Rosen::WindowOption> option(new OHOS::Rosen::WindowOption());
    option->SetWindowType(type_);
    option->SetWindowMode(mode_);
    option->SetMainHandlerAvailable(false);

    static int count = 0;
    std::string windowName = "WaylandWindow" + std::to_string(count++);
    window_ = OHOS::Rosen::Window::Create(windowName, option);
    if (window_ == nullptr) {
        LOG_ERROR("Window::Create failed");
        return;
    }
    LOG_DEBUG("Window::Create success");
    auto listener = std::make_shared<InputEventConsumer>(this);
    window_->SetInputEventConsumer(listener);
    window_->Show();

    OHOS::sptr<WaylandWindowListener> waylandWindowListener = new WaylandWindowListener(this);
    window_->RegisterWindowChangeListener(waylandWindowListener);

    surfaceNode_ = window_->GetSurfaceNode();
    if (surfaceNode_ == nullptr) {
        LOG_ERROR("GetSurfaceNode failed");
        return;
    }

    rsSurface_ = OHOS::Rosen::RSSurfaceExtractor::ExtractRSSurface(surfaceNode_);
    if (rsSurface_ == nullptr) {
        LOG_ERROR("ExtractRSSurface failed");
        return;
    }

#ifdef ENABLE_GPU
    renderContext_ = std::make_unique<OHOS::Rosen::RenderContext>();
    renderContext_->InitializeEglContext();
    rsSurface_->SetRenderContext(renderContext_.get());
#endif

    OHOS::Rosen::Rect rect = window_->GetRect();
    rect_.width = rect.width_;
    rect_.height = rect.height_;

    for (auto &cb : rectCallbacks_) {
        cb(rect_);
    }
}

void WaylandSurface::CopyBuffer(struct wl_shm_buffer *shm)
{
    if (rsSurface_ == nullptr) {
        LOG_ERROR("rsSurface_ is nullptr");
        return;
    }

    SkColorType format = ShmFormatToSkia(wl_shm_buffer_get_format(shm));
    if (format == SkColorType::kUnknown_SkColorType) {
        LOG_ERROR("unsupported format %{public}d", wl_shm_buffer_get_format(shm));
        return;
    }

    int32_t stride = wl_shm_buffer_get_stride(shm);
    int32_t width = wl_shm_buffer_get_width(shm);
    int32_t height = wl_shm_buffer_get_height(shm);
    if (stride <= 0 || width <= 0 || height <= 0) {
        LOG_ERROR("invalid, stride:%{public}d width:%{public}d height:%{public}d", stride, width, height);
        return;
    }

    void *data = wl_shm_buffer_get_data(shm);
    if (data == nullptr) {
        LOG_ERROR("wl_shm_buffer_get_data fail");
        return;
    }

    auto framePtr = rsSurface_->RequestFrame(width, height);
    if (framePtr == nullptr) {
        LOG_ERROR("RequestFrame failed");
        return;
    }

    auto canvas = framePtr->GetCanvas();
    if (canvas == nullptr) {
        LOG_ERROR("GetCanvas failed");
        return;
    }
    canvas->clear(SK_ColorTRANSPARENT);

    SkImageInfo imageInfo = SkImageInfo::Make(width, height, format, kUnpremul_SkAlphaType);
    SkPixmap srcPixmap(imageInfo, data, stride);
    SkBitmap srcBitmap;
    srcBitmap.installPixels(srcPixmap);
    canvas->drawBitmap(srcBitmap, 0, 0);
    rsSurface_->FlushFrame(framePtr);
}

void WaylandSurface::OnSizeChange(const OHOS::Rosen::Rect& rect, OHOS::Rosen::WindowSizeChangeReason reason)
{
    rect_.x = rect.posX_;
    rect_.y = rect.posY_;
    rect_.width = rect.width_;
    rect_.height = rect.height_;
    for (auto &cb : rectCallbacks_) {
        cb(rect_);
    }
}

void WaylandSurface::OnModeChange(OHOS::Rosen::WindowMode mode)
{
    LOG_DEBUG("OnModeChange, window mode is %{public}d, ignore", mode);
}

void WaylandSurface::SetTitle(const char *title)
{
    LOG_DEBUG("Window %{public}s, set Title %{public}s.", windowTitle_.c_str(), title);
    windowTitle_ = title;
    if (window_ == nullptr) {
        LOG_ERROR("window_ is nullptr");
        return;
    }
    window_->SetAPPWindowLabel(title);
}

void WaylandSurface::Resize(uint32_t serial, uint32_t edges)
{
    LOG_DEBUG("Window %{public}s.", windowTitle_.c_str());
    if (window_ == nullptr) {
        LOG_ERROR("window_ is nullptr");
        return;
    }
    // window_->Resize(uint32_t width, uint32_t height);
}

void WaylandSurface::SetMaxSize(int32_t width, int32_t height)
{
    LOG_DEBUG("Window %{public}s.", windowTitle_.c_str());
    if (window_ == nullptr) {
        LOG_ERROR("window_ is nullptr");
        return;
    }
}

void WaylandSurface::SetMinSize(int32_t width, int32_t height)
{
    LOG_DEBUG("Window %{public}s.", windowTitle_.c_str());
    if (window_ == nullptr) {
        LOG_ERROR("window_ is nullptr");
        return;
    }
}

void WaylandSurface::SetMaximized()
{
    LOG_DEBUG("Window %{public}s.", windowTitle_.c_str());
    if (window_ == nullptr) {
        LOG_ERROR("window_ is nullptr");
        return;
    }
    window_->SetFullScreen(true);
}

void WaylandSurface::UnSetMaximized()
{
    LOG_DEBUG("Window %{public}s.", windowTitle_.c_str());
    if (window_ == nullptr) {
        LOG_ERROR("window_ is nullptr");
        return;
    }
    window_->SetFullScreen(false);
}

void WaylandSurface::SetFullscreen()
{
    LOG_DEBUG("Window %{public}s.", windowTitle_.c_str());
    if (window_ == nullptr) {
        LOG_ERROR("window_ is nullptr");
        return;
    }
    window_->SetFullScreen(true);
}

void WaylandSurface::UnSetFullscreen()
{
    LOG_DEBUG("Window %{public}s.", windowTitle_.c_str());
    if (window_ == nullptr) {
        LOG_ERROR("window_ is nullptr");
        return;
    }
    window_->SetFullScreen(false);
}

void WaylandSurface::SetMinimized()
{
    LOG_DEBUG("Window %{public}s.", windowTitle_.c_str());
    if (window_ == nullptr) {
        LOG_ERROR("window_ is nullptr");
        return;
    }
    window_->Minimize();
}

void WaylandSurface::Close()
{
    LOG_DEBUG("Window %{public}s.", windowTitle_.c_str());
    if (window_ == nullptr) {
        LOG_ERROR("window_ is nullptr");
        return;
    }
    window_->Close();
}
} // namespace Wayland
} // namespace FT

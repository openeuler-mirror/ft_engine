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

#include "wayland_xdg_positioner.h"

#include "wayland_objects_pool.h"
#include "wayland_xdg_surface.h"

namespace FT {
namespace Wayland {
namespace {
    constexpr HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WAYLAND, "WaylandXdgPositioner"};
}
struct xdg_positioner_interface WaylandXdgPositionerInterface::impl_ = {
    .destroy = WaylandResourceObject::DefaultDestroyResource,
    .set_size = WaylandXdgPositionerInterface::SetSize,
    .set_anchor_rect = WaylandXdgPositionerInterface::SetAnchorRect,
    .set_anchor = WaylandXdgPositionerInterface::SetAnchor,
    .set_gravity = WaylandXdgPositionerInterface::SetGravity,
    .set_constraint_adjustment = WaylandXdgPositionerInterface::SetConstraintAdjustment,
    .set_offset = WaylandXdgPositionerInterface::SetOffset,
    .set_reactive = WaylandXdgPositionerInterface::SetReactive,
    .set_parent_size = WaylandXdgPositionerInterface::SetParentSize,
    .set_parent_configure = WaylandXdgPositionerInterface::SetParentConfigure};

void WaylandXdgPositionerInterface::SetSize(
    struct wl_client *client,
    struct wl_resource *resource,
    int32_t width,
    int32_t height)
{
    LOG_DEBUG("Request: xdg_positioner-->set_size.");
    UNUSED(client);
    CAST_OBJECT_AND_CALL_FUNC(
        WaylandXdgPositioner, resource, "WaylandXdgPositioner::SetSize: failed to find object.", SetSize, width, height);
}

void WaylandXdgPositionerInterface::SetAnchorRect(
    struct wl_client *client,
    struct wl_resource *resource,
    int32_t x,
    int32_t y,
    int32_t width,
    int32_t height)
{
    LOG_DEBUG("Request: xdg_positioner-->set_anchor_rect.");
    UNUSED(client);
    CAST_OBJECT_AND_CALL_FUNC(
        WaylandXdgPositioner,
        resource,
        "WaylandXdgPositioner::SetAnchorRect: failed to find object.",
        SetAnchorRect,
        x,
        y,
        width,
        height);
}

void WaylandXdgPositionerInterface::SetAnchor(struct wl_client *client, struct wl_resource *resource, uint32_t anchor)
{
    LOG_DEBUG("Request: xdg_positioner-->set_anchor.");
    UNUSED(client);
    CAST_OBJECT_AND_CALL_FUNC(
        WaylandXdgPositioner, resource, "WaylandXdgPositioner::SetAnchor: failed to find object.", SetAnchor, anchor);
}

void WaylandXdgPositionerInterface::SetGravity(struct wl_client *client, struct wl_resource *resource, uint32_t gravity)
{
    LOG_DEBUG("Request: xdg_positioner-->set_gravity.");
    UNUSED(client);
    CAST_OBJECT_AND_CALL_FUNC(
        WaylandXdgPositioner, resource, "WaylandXdgPositioner::SetGravity: failed to find object.", SetGravity, gravity);
}

void WaylandXdgPositionerInterface::SetConstraintAdjustment(
    struct wl_client *client,
    struct wl_resource *resource,
    uint32_t constraintAdjustment)
{
    LOG_DEBUG("Request: xdg_positioner-->set_constraint_adjustment.");
    UNUSED(client);
    CAST_OBJECT_AND_CALL_FUNC(
        WaylandXdgPositioner,
        resource,
        "WaylandXdgPositioner::SetConstraintAdjustment: failed to find object.",
        SetConstraintAdjustment,
        constraintAdjustment);
}

void WaylandXdgPositionerInterface::SetOffset(struct wl_client *client, struct wl_resource *resource, int32_t x, int32_t y)
{
    LOG_DEBUG("Request: xdg_positioner-->set_offset.");
    UNUSED(client);
    CAST_OBJECT_AND_CALL_FUNC(
        WaylandXdgPositioner, resource, "WaylandXdgPositioner::SetOffset: failed to find object.", SetOffset, x, y);
}

void WaylandXdgPositionerInterface::SetReactive(struct wl_client *client, struct wl_resource *resource)
{
    LOG_DEBUG("Request: xdg_positioner-->set_reactive.");
    UNUSED(client);
    CAST_OBJECT_AND_CALL_FUNC(
        WaylandXdgPositioner, resource, "WaylandXdgPositioner::SetReactive: failed to find object.", SetReactive);
}

void WaylandXdgPositionerInterface::SetParentSize(
    struct wl_client *client,
    struct wl_resource *resource,
    int32_t parentWidth,
    int32_t parentHeight)
{
    LOG_DEBUG("Request: xdg_positioner-->set_parent_size.");
    UNUSED(client);
    CAST_OBJECT_AND_CALL_FUNC(
        WaylandXdgPositioner,
        resource,
        "WaylandXdgPositioner::SetParentSize: failed to find object.",
        SetParentSize,
        parentWidth,
        parentHeight);
}

void WaylandXdgPositionerInterface::SetParentConfigure(
    struct wl_client *client,
    struct wl_resource *resource,
    uint32_t serial)
{
    LOG_DEBUG("Request: xdg_positioner-->set_parent_configure.");
    UNUSED(client);
    CAST_OBJECT_AND_CALL_FUNC(
        WaylandXdgPositioner,
        resource,
        "WaylandXdgPositioner::SetParentConfigure: failed to find object.",
        SetParentConfigure,
        serial);
}

OHOS::sptr<WaylandXdgPositioner> WaylandXdgPositioner::Create(struct wl_client *client, uint32_t version, uint32_t id)
{
    // check
    if (client == nullptr) {
        LOG_WARN("WaylandXdgPositioner::Create failed: client is nullptr.");
        return nullptr;
    }

    auto xdgPositioner = OHOS::sptr<WaylandXdgPositioner>(new WaylandXdgPositioner(client, version, id));
    WaylandObjectsPool::GetInstance().AddObject(ObjectId(xdgPositioner->WlClient(), xdgPositioner->Id()), xdgPositioner);
    return xdgPositioner;
}

WaylandXdgPositioner::WaylandXdgPositioner(struct wl_client *client, uint32_t version, uint32_t id)
    : WaylandResourceObject(client, &xdg_positioner_interface, version, id, &WaylandXdgPositionerInterface::impl_)
{
    LOG_DEBUG("WaylandXdgPositioner  ctor.");
}

WaylandXdgPositioner::~WaylandXdgPositioner() noexcept
{
    LOG_DEBUG("WaylandXdgPositioner  dtor.");
}

void WaylandXdgPositioner::SetSize(int32_t width, int32_t height)
{
    if (width < 1 || height < 1) {
        wl_resource_post_error(
            WlResource(), XDG_POSITIONER_ERROR_INVALID_INPUT, "width and height must be positives and non-zero");
        return;
    }

    size_.width = width;
    size_.height = height;
}

void WaylandXdgPositioner::SetAnchorRect(int32_t x, int32_t y, int32_t width, int32_t height)
{
    if (width < 0 || height < 0) {
        wl_resource_post_error(
            WlResource(), XDG_POSITIONER_ERROR_INVALID_INPUT, "width and height must be non-negative");
        return;
    }

    anchorRect_.x = x;
    anchorRect_.y = y;
    anchorRect_.width = width;
    anchorRect_.height = height;
}

void WaylandXdgPositioner::SetAnchor(uint32_t anchor)
{
    auto newAnchor = static_cast<enum xdg_positioner_anchor>(anchor);
    switch (newAnchor) {
        case XDG_POSITIONER_ANCHOR_NONE:
        case XDG_POSITIONER_ANCHOR_TOP:
        case XDG_POSITIONER_ANCHOR_BOTTOM:
        case XDG_POSITIONER_ANCHOR_LEFT:
        case XDG_POSITIONER_ANCHOR_RIGHT:
        case XDG_POSITIONER_ANCHOR_TOP_LEFT:
        case XDG_POSITIONER_ANCHOR_BOTTOM_LEFT:
        case XDG_POSITIONER_ANCHOR_TOP_RIGHT:
        case XDG_POSITIONER_ANCHOR_BOTTOM_RIGHT:
            anchor_ = newAnchor;
            break;
        default:
            break;
    }
}

void WaylandXdgPositioner::SetGravity(uint32_t gravity)
{
    auto newGravity = static_cast<enum xdg_positioner_gravity>(gravity);
    switch (newGravity) {
        case XDG_POSITIONER_GRAVITY_NONE:
        case XDG_POSITIONER_GRAVITY_TOP:
        case XDG_POSITIONER_GRAVITY_BOTTOM:
        case XDG_POSITIONER_GRAVITY_LEFT:
        case XDG_POSITIONER_GRAVITY_RIGHT:
        case XDG_POSITIONER_GRAVITY_TOP_LEFT:
        case XDG_POSITIONER_GRAVITY_BOTTOM_LEFT:
        case XDG_POSITIONER_GRAVITY_TOP_RIGHT:
        case XDG_POSITIONER_GRAVITY_BOTTOM_RIGHT:
            gravity_ = newGravity;
            break;
        default:
            break;
    }
}

void WaylandXdgPositioner::SetConstraintAdjustment(uint32_t constraintAdjustment)
{
    auto newConstraintAdjustment = static_cast<enum xdg_positioner_constraint_adjustment>(constraintAdjustment);
    switch (newConstraintAdjustment) {
        case XDG_POSITIONER_CONSTRAINT_ADJUSTMENT_NONE:
        case XDG_POSITIONER_CONSTRAINT_ADJUSTMENT_SLIDE_X:
        case XDG_POSITIONER_CONSTRAINT_ADJUSTMENT_SLIDE_Y:
        case XDG_POSITIONER_CONSTRAINT_ADJUSTMENT_FLIP_X:
        case XDG_POSITIONER_CONSTRAINT_ADJUSTMENT_FLIP_Y:
        case XDG_POSITIONER_CONSTRAINT_ADJUSTMENT_RESIZE_X:
        case XDG_POSITIONER_CONSTRAINT_ADJUSTMENT_RESIZE_Y:
            constraintAdjustment_ = newConstraintAdjustment;
            break;
        default:
            break;
    }
}

void WaylandXdgPositioner::SetOffset(int32_t x, int32_t y)
{
    offset_.x = x;
    offset_.y = y;
}

void WaylandXdgPositioner::SetReactive()
{
    reactive_ = true;
}

void WaylandXdgPositioner::SetParentSize(int32_t parentWidth, int32_t parentHeight)
{
    if (parentWidth < 1 || parentHeight < 1) {
        wl_resource_post_error(
            WlResource(),
            XDG_POSITIONER_ERROR_INVALID_INPUT,
            "positioner parent width and height must be positives and non-zero");
        return;
    }

    parentSize_.width = parentWidth;
    parentSize_.height = parentHeight;
}

void WaylandXdgPositioner::SetParentConfigure(uint32_t serial)
{
    parentConfigureSerial_ = serial;
}
} // namespace Wayland
} // namespace FT

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

#pragma once

#include <xdg-shell-server-protocol.h>
#include "wayland_resource_object.h"
#include "wayland_xdg_positioner.h"
#include "wayalnd_utils.h"

namespace FT {
namespace Wayland {
class WaylandXdgSurface;

struct WaylandXdgPopupInterface {
    /**
     * make the popup take an explicit grab
     *
     * This request makes the created popup take an explicit grab. An
     * explicit grab will be dismissed when the user dismisses the
     * popup, or when the client destroys the xdg_popup. This can be
     * done by the user clicking outside the surface, using the
     * keyboard, or even locking the screen through closing the lid or
     * a timeout.
     *
     * If the compositor denies the grab, the popup will be immediately
     * dismissed.
     *
     * This request must be used in response to some sort of user
     * action like a button press, key press, or touch down event. The
     * serial number of the event should be passed as 'serial'.
     *
     * The parent of a grabbing popup must either be an xdg_toplevel
     * surface or another xdg_popup with an explicit grab. If the
     * parent is another xdg_popup it means that the popups are nested,
     * with this popup now being the topmost popup.
     *
     * Nested popups must be destroyed in the reverse order they were
     * created in, e.g. the only popup you are allowed to destroy at
     * all times is the topmost one.
     *
     * When compositors choose to dismiss a popup, they may dismiss
     * every nested grabbing popup as well. When a compositor dismisses
     * popups, it will follow the same dismissing order as required
     * from the client.
     *
     * The parent of a grabbing popup must either be another xdg_popup
     * with an active explicit grab, or an xdg_popup or xdg_toplevel,
     * if there are no explicit grabs already taken.
     *
     * If the topmost grabbing popup is destroyed, the grab will be
     * returned to the parent of the popup, if that parent previously
     * had an explicit grab.
     *
     * If the parent is a grabbing popup which has already been
     * dismissed, this popup will be immediately dismissed. If the
     * parent is a popup that did not take an explicit grab, an error
     * will be raised.
     *
     * During a popup grab, the client owning the grab will receive
     * pointer and touch events for all their surfaces as normal
     * (similar to an "owner-events" grab in X11 parlance), while the
     * top most grabbing popup will always have keyboard focus.
     * @param seat the wl_seat of the user event
     * @param serial the serial of the user event
     */
    static void Grab(struct wl_client *client, struct wl_resource *resource, struct wl_resource *seat, uint32_t serial);
    /**
     * recalculate the popup's location
     *
     * Reposition an already-mapped popup. The popup will be placed
     * given the details in the passed xdg_positioner object, and a
     * xdg_popup.repositioned followed by xdg_popup.configure and
     * xdg_surface.configure will be emitted in response. Any
     * parameters set by the previous positioner will be discarded.
     *
     * The passed token will be sent in the corresponding
     * xdg_popup.repositioned event. The new popup position will not
     * take effect until the corresponding configure event is
     * acknowledged by the client. See xdg_popup.repositioned for
     * details. The token itself is opaque, and has no other special
     * meaning.
     *
     * If multiple reposition requests are sent, the compositor may
     * skip all but the last one.
     *
     * If the popup is repositioned in response to a configure event
     * for its parent, the client should send an
     * xdg_positioner.set_parent_configure and possibly an
     * xdg_positioner.set_parent_size request to allow the compositor
     * to properly constrain the popup.
     *
     * If the popup is repositioned together with a parent that is
     * being resized, but not in response to a configure event, the
     * client should send an xdg_positioner.set_parent_size request.
     * @param token reposition request token
     * @since 3
     */
    static void Reposition(
        struct wl_client *client,
        struct wl_resource *resource,
        struct wl_resource *positioner,
        uint32_t token);

    static struct xdg_popup_interface impl_;
};

class WaylandXdgPopup : public WaylandResourceObject {
public:
    static OHOS::sptr<WaylandXdgPopup> Create(
        const OHOS::sptr<WaylandXdgSurface> &xdgSurface,
        const OHOS::sptr<WaylandXdgSurface> &parentXdgSurface,
        const OHOS::sptr<WaylandXdgPositioner> &positioner,
        uint32_t id);
    ~WaylandXdgPopup() noexcept;
    void OnSurfaceCommitted();
    void UpdateSize(int32_t x, int32_t y, uint32_t width, uint32_t height);
    void SetGeometry(int32_t x, int32_t y, uint32_t width, uint32_t height);

private:
    WaylandXdgPopup(
        const OHOS::sptr<WaylandXdgSurface> &xdgSurface,
        const OHOS::sptr<WaylandXdgSurface> &parentXdgSurface,
        const OHOS::sptr<WaylandXdgPositioner> &positioner,
        uint32_t id);

    friend struct WaylandXdgPopupInterface;

    void Grab(struct wl_client *client, struct wl_resource *resource, struct wl_resource *seat, uint32_t serial);

    void Reposition(
        struct wl_client *client,
        struct wl_resource *resource,
        struct wl_resource *positioner,
        uint32_t token);

private:
    OHOS::wptr<WaylandXdgSurface> xdgSurface_;
    OHOS::wptr<WaylandXdgSurface> parentXdgSurface_;
};
} // namespace Wayland
} // namespace FT

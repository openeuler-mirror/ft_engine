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
#include "wayalnd_utils.h"

namespace FT {
namespace Wayland {
struct WaylandXdgPositionerInterface {
    /**
     * set the size of the to-be positioned rectangle
     *
     * Set the size of the surface that is to be positioned with the
     * positioner object. The size is in surface-local coordinates and
     * corresponds to the window geometry. See
     * xdg_surface.set_window_geometry.
     *
     * If a zero or negative size is set the invalid_input error is
     * raised.
     * @param width width of positioned rectangle
     * @param height height of positioned rectangle
     */
    static void SetSize(struct wl_client *client, struct wl_resource *resource, int32_t width, int32_t height);
    /**
     * set the anchor rectangle within the parent surface
     *
     * Specify the anchor rectangle within the parent surface that
     * the child surface will be placed relative to. The rectangle is
     * relative to the window geometry as defined by
     * xdg_surface.set_window_geometry of the parent surface.
     *
     * When the xdg_positioner object is used to position a child
     * surface, the anchor rectangle may not extend outside the window
     * geometry of the positioned child's parent surface.
     *
     * If a negative size is set the invalid_input error is raised.
     * @param x x position of anchor rectangle
     * @param y y position of anchor rectangle
     * @param width width of anchor rectangle
     * @param height height of anchor rectangle
     */
    static void SetAnchorRect(
        struct wl_client *client,
        struct wl_resource *resource,
        int32_t x,
        int32_t y,
        int32_t width,
        int32_t height);
    /**
     * set anchor rectangle anchor
     *
     * Defines the anchor point for the anchor rectangle. The
     * specified anchor is used derive an anchor point that the child
     * surface will be positioned relative to. If a corner anchor is
     * set (e.g. 'top_left' or 'bottom_right'), the anchor point will
     * be at the specified corner; otherwise, the derived anchor point
     * will be centered on the specified edge, or in the center of the
     * anchor rectangle if no edge is specified.
     * @param anchor anchor
     */
    static void SetAnchor(struct wl_client *client, struct wl_resource *resource, uint32_t anchor);
    /**
     * set child surface gravity
     *
     * Defines in what direction a surface should be positioned,
     * relative to the anchor point of the parent surface. If a corner
     * gravity is specified (e.g. 'bottom_right' or 'top_left'), then
     * the child surface will be placed towards the specified gravity;
     * otherwise, the child surface will be centered over the anchor
     * point on any axis that had no gravity specified.
     * @param gravity gravity direction
     */
    static void SetGravity(struct wl_client *client, struct wl_resource *resource, uint32_t gravity);
    /**
     * set the adjustment to be done when constrained
     *
     * Specify how the window should be positioned if the originally
     * intended position caused the surface to be constrained, meaning
     * at least partially outside positioning boundaries set by the
     * compositor. The adjustment is set by constructing a bitmask
     * describing the adjustment to be made when the surface is
     * constrained on that axis.
     *
     * If no bit for one axis is set, the compositor will assume that
     * the child surface should not change its position on that axis
     * when constrained.
     *
     * If more than one bit for one axis is set, the order of how
     * adjustments are applied is specified in the corresponding
     * adjustment descriptions.
     *
     * The default adjustment is none.
     * @param constraintAdjustment bit mask of constraint adjustments
     */
    static void SetConstraintAdjustment(
        struct wl_client *client,
        struct wl_resource *resource,
        uint32_t constraintAdjustment);
    /**
     * set surface position offset
     *
     * Specify the surface position offset relative to the position
     * of the anchor on the anchor rectangle and the anchor on the
     * surface. For example if the anchor of the anchor rectangle is at
     * (x, y), the surface has the gravity bottom|right, and the offset
     * is (ox, oy), the calculated surface position will be (x + ox, y
     * + oy). The offset position of the surface is the one used for
     * constraint testing. See set_constraint_adjustment.
     *
     * An example use case is placing a popup menu on top of a user
     * interface element, while aligning the user interface element of
     * the parent surface with some user interface element placed
     * somewhere in the popup surface.
     * @param x surface position x offset
     * @param y surface position y offset
     */
    static void SetOffset(struct wl_client *client, struct wl_resource *resource, int32_t x, int32_t y);
    /**
     * continuously reconstrain the surface
     *
     * When set reactive, the surface is reconstrained if the
     * conditions used for constraining changed, e.g. the parent window
     * moved.
     *
     * If the conditions changed and the popup was reconstrained, an
     * xdg_popup.configure event is sent with updated geometry,
     * followed by an xdg_surface.configure event.
     * @since 3
     */
    static void SetReactive(struct wl_client *client, struct wl_resource *resource);
    /**
     *
     *
     * Set the parent window geometry the compositor should use when
     * positioning the popup. The compositor may use this information
     * to determine the future state the popup should be constrained
     * using. If this doesn't match the dimension of the parent the
     * popup is eventually positioned against, the behavior is
     * undefined.
     *
     * The arguments are given in the surface-local coordinate space.
     * @param parentWidth future window geometry width of parent
     * @param parentHeight future window geometry height of parent
     * @since 3
     */
    static void SetParentSize(
        struct wl_client *client,
        struct wl_resource *resource,
        int32_t parentWidth,
        int32_t parentHeight);
    /**
     * set parent configure this is a response to
     *
     * Set the serial of an xdg_surface.configure event this
     * positioner will be used in response to. The compositor may use
     * this information together with set_parent_size to determine what
     * future state the popup should be constrained using.
     * @param serial serial of parent configure event
     * @since 3
     */
    static void SetParentConfigure(struct wl_client *client, struct wl_resource *resource, uint32_t serial);

    static struct xdg_positioner_interface impl_;
};

class WaylandXdgPositioner final : public WaylandResourceObject {
public:
    /**
     * @brief Create XdgOutput resource by XdgOutputManager
     *
     * @param client
     * @param manager
     * @param version
     * @param id
     * @param output
     * @return OHOS::sptr<WaylandXdgOutput>
     */
    static OHOS::sptr<WaylandXdgPositioner> Create(struct wl_client *client, uint32_t version, uint32_t id);
    ~WaylandXdgPositioner() noexcept override;

    Size GetSize() const
    {
        return size_;
    }
    Rect GetAnchorRect() const
    {
        return anchorRect_;
    }
    enum xdg_positioner_anchor GetAnchor() const
    {
        return anchor_;
    }
    enum xdg_positioner_gravity GetGravity() const
    {
        return gravity_;
    }
    enum xdg_positioner_constraint_adjustment GetConstraintAdjustment() const
    {
        return constraintAdjustment_;
    }
    Offset GetOffset() const
    {
        return offset_;
    }
    bool GetReactive() const
    {
        return reactive_;
    }
    Size GetParentSize() const
    {
        return parentSize_;
    }
    uint32_t GetParentConfigureSerial() const
    {
        return parentConfigureSerial_;
    }

private:
    WaylandXdgPositioner(struct wl_client *client, uint32_t version, uint32_t id);

private:
    friend struct WaylandXdgPositionerInterface;
    // WaylandXdgPositionerInterface will call these member functions.
    void SetSize(int32_t width, int32_t height);
    void SetAnchorRect(int32_t x, int32_t y, int32_t width, int32_t height);
    // enum xdg_positioner_anchor {
    //     XDG_POSITIONER_ANCHOR_NONE = 0,
    //     XDG_POSITIONER_ANCHOR_TOP = 1,
    //     XDG_POSITIONER_ANCHOR_BOTTOM = 2,
    //     XDG_POSITIONER_ANCHOR_LEFT = 3,
    //     XDG_POSITIONER_ANCHOR_RIGHT = 4,
    //     XDG_POSITIONER_ANCHOR_TOP_LEFT = 5,
    //     XDG_POSITIONER_ANCHOR_BOTTOM_LEFT = 6,
    //     XDG_POSITIONER_ANCHOR_TOP_RIGHT = 7,
    //     XDG_POSITIONER_ANCHOR_BOTTOM_RIGHT = 8,
    // };
    void SetAnchor(uint32_t anchor);
    // enum xdg_positioner_gravity {
    //     XDG_POSITIONER_GRAVITY_NONE = 0,
    //     XDG_POSITIONER_GRAVITY_TOP = 1,
    //     XDG_POSITIONER_GRAVITY_BOTTOM = 2,
    //     XDG_POSITIONER_GRAVITY_LEFT = 3,
    //     XDG_POSITIONER_GRAVITY_RIGHT = 4,
    //     XDG_POSITIONER_GRAVITY_TOP_LEFT = 5,
    //     XDG_POSITIONER_GRAVITY_BOTTOM_LEFT = 6,
    //     XDG_POSITIONER_GRAVITY_TOP_RIGHT = 7,
    //     XDG_POSITIONER_GRAVITY_BOTTOM_RIGHT = 8,
    // };
    void SetGravity(uint32_t gravity);
    // enum xdg_positioner_constraint_adjustment {
    //     XDG_POSITIONER_CONSTRAINT_ADJUSTMENT_NONE = 0,
    //     XDG_POSITIONER_CONSTRAINT_ADJUSTMENT_SLIDE_X = 1,
    //     XDG_POSITIONER_CONSTRAINT_ADJUSTMENT_SLIDE_Y = 2,
    //     XDG_POSITIONER_CONSTRAINT_ADJUSTMENT_FLIP_X = 4,
    //     XDG_POSITIONER_CONSTRAINT_ADJUSTMENT_FLIP_Y = 8,
    //     XDG_POSITIONER_CONSTRAINT_ADJUSTMENT_RESIZE_X = 16,
    //     XDG_POSITIONER_CONSTRAINT_ADJUSTMENT_RESIZE_Y = 32,
    // };
    void SetConstraintAdjustment(uint32_t constraintAdjustment);
    void SetOffset(int32_t x, int32_t y);
    void SetReactive();
    void SetParentSize(int32_t parentWidth, int32_t parentHeight);
    void SetParentConfigure(uint32_t serial);

private:
    Size size_;
    Rect anchorRect_;
    enum xdg_positioner_anchor anchor_ = XDG_POSITIONER_ANCHOR_NONE;
    enum xdg_positioner_gravity gravity_ = XDG_POSITIONER_GRAVITY_NONE;
    enum xdg_positioner_constraint_adjustment constraintAdjustment_ = XDG_POSITIONER_CONSTRAINT_ADJUSTMENT_NONE;
    Offset offset_;
    bool reactive_ = false;
    Size parentSize_;
    uint32_t parentConfigureSerial_ = 0;
};
} // namespace Wayland
} // namespace FT

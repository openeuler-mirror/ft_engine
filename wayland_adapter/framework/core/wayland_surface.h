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

#include <list>
#include <vector>
#include <map>
#include <mutex>
#include <wayland-server-protocol.h>
#include "wayland_resource_object.h"
#include "wayalnd_utils.h"

#include "ui/rs_surface_node.h"
#include "render_context/render_context.h"

namespace FT {
namespace Wayland {
struct IWaylandSurface {
    static void Attach(struct wl_client *client, struct wl_resource *surfaceResource,
        struct wl_resource *bufferResource, int32_t x, int32_t y);
    static void Damage(struct wl_client *client, struct wl_resource *resource,
        int32_t x, int32_t y, int32_t width, int32_t height);
    static void Frame(struct wl_client *client, struct wl_resource *resource, uint32_t callback);
    static void SetOpaqueRegion(struct wl_client *client, struct wl_resource *surfaceResource,
        struct wl_resource *regionResource);
    static void SetInputRegion(struct wl_client *client, struct wl_resource *surfaceResource,
        struct wl_resource *regionResource);
    static void Commit(struct wl_client *client, struct wl_resource *resource);
    static void SetBufferTransform(struct wl_client *client, struct wl_resource *resource, int32_t transform);
    static void SetBufferScale(struct wl_client *client, struct wl_resource *resource, int32_t scale);
    static void DamageBuffer(struct wl_client *client, struct wl_resource *resource,
        int32_t x, int32_t y, int32_t width, int32_t height);
    static void Offset(struct wl_client *client, struct wl_resource *resource, int32_t x, int32_t y);
    static struct wl_surface_interface impl_;
};


class WaylandSurface final : public WaylandResourceObject {
    friend struct IWaylandSurface;

public:
    static OHOS::sptr<WaylandSurface> Create(struct wl_client *client, struct wl_resource *parent,
        uint32_t version, uint32_t id);
    ~WaylandSurface() noexcept override;

    void AddCommitCallback(SurfaceCommitCallback callback);
    void AddRectCallback(SurfaceRectCallback callback);
    void AddWindowCreateCallback(WindowCreateCallback callback);
    void OnSizeChange(const OHOS::Rosen::Rect& rect, OHOS::Rosen::WindowSizeChangeReason reason);
    void OnModeChange(OHOS::Rosen::WindowMode mode);

    // form xdgsruface
    void SetWindowGeometry(Rect rect);
    Rect GetWindowGeometry();

    void WithTopLevel(bool toplevel);
    void AddChild(struct wl_resource *child, int32_t x, int32_t y);
    void AddParent(struct wl_resource *parent);
    void ProcessSrcBitmap(SkCanvas* canvas, int32_t x, int32_t y);
    void TriggerInnerCompose();
    OHOS::sptr<OHOS::Rosen::WindowOption> GetWindowOption()
    {
        return windowOption_;
    }
    std::shared_ptr<WindowOptionExt> GetWindowOptionExt()
    {
        return windowOptionExt_;
    }

private:
    WaylandSurface(struct wl_client *client, struct wl_resource *parent, uint32_t version, uint32_t id);

    void Attach(struct wl_resource *bufferResource, int32_t x, int32_t y);
    void Damage(int32_t x, int32_t y, int32_t width, int32_t height);
    void Frame(uint32_t callback);
    void SetOpaqueRegion(struct wl_resource *regionResource);
    void SetInputRegion(struct wl_resource *regionResource);
    void Commit();
    void SetBufferTransform(int32_t transform);
    void SetBufferScale(int32_t scale);
    void DamageBuffer(int32_t x, int32_t y, int32_t width, int32_t height);
    void Offset(int32_t x, int32_t y);
    void HandleCommit();
    void CreateWindow();
    void CopyBuffer(struct wl_shm_buffer *shm);
    void CheckIsPointerSurface();
    bool WindowValid();

    struct wl_resource *parent_ = nullptr;
    std::list<SurfaceCommitCallback> commitCallbacks_;
    std::list<SurfaceRectCallback> rectCallbacks_;
    std::list<WindowCreateCallback> windowCreatebacks_;
    Rect rect_;
    Rect geometryRect_ = {0};
    SurfaceState old_;
    SurfaceState new_;
    bool isPointerSurface_ = false;

#ifdef ENABLE_GPU
    std::unique_ptr<OHOS::Rosen::RenderContext> renderContext_;
#endif
    OHOS::sptr<OHOS::Rosen::Window> window_;
    OHOS::sptr<OHOS::Rosen::WindowOption> windowOption_;
    std::shared_ptr<WindowOptionExt> windowOptionExt_;
    std::shared_ptr<OHOS::Rosen::RSSurfaceNode> surfaceNode_;
    std::shared_ptr<OHOS::Rosen::RSSurface> rsSurface_;
    std::string windowTitle_;
    bool withTopLevel_ = false;
    std::map<wl_resource *, struct SubSurfaceData> childs_;
    struct wl_resource *parentSurfaceRes_ = nullptr;
    std::mutex bitmapMutex_;
    SkBitmap srcBitmap_;
    std::vector<OHOS::sptr<FrameCallback>> pengindCb_;
};
} // namespace Wayland
} // namespace FT

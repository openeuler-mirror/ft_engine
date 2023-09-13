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

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <signal.h>
#include <errno.h>
#include <linux/input-event-codes.h>

#include <securec.h>
#include <wayland-client.h>
#include "timestamp.h"
#include "types.h"
#include "core/SkPaint.h"
#include "core/SkPath.h"
#include "xdg-shell-client-protocol.h"

#include "core/SkBitmap.h"
#include "core/SkCanvas.h"
#include "core/SkColor.h"
#include "core/SkFont.h"
#include "core/SkImageEncoder.h"
#include "core/SkPoint.h"
#include "core/SkRect.h"
#include "core/SkScalar.h"
#include "core/SkStream.h"
#include "core/SkTextBlob.h"
#include "core/SkTypeface.h"

constexpr int32_t WINDOW_WIDTH = 500;
constexpr int32_t WINDOW_HEIGHT = 400;
struct window;

struct display {
    struct wl_display *display;
    struct wl_registry *registry;
    struct wl_compositor *compositor;
    struct xdg_wm_base *wm_base;
    struct wl_shm *shm;
    struct wl_seat *seat;
    struct wl_pointer *pointer;
    struct window *window;
    bool has_rgba8888 = false;
};

struct buffer {
    struct wl_buffer *buffer;
    void *shm_data;
    bool busy;
    int32_t width, height;
};

struct window {
    struct display *display;
    int32_t width, height;
    struct wl_surface *surface;
    struct wl_region *region;
    struct xdg_surface *xdg_surface;
    struct xdg_toplevel *xdg_toplevel;
    struct buffer buffers[2];
    struct buffer *prev_buffer;
    struct wl_callback *callback;
    bool wait_for_configure = true;
};

bool g_running = true;

static void BufferRelease(void *data, struct wl_buffer *buffer)
{
    struct buffer *mybuf = (struct buffer *)data;
    mybuf->busy = false;
}
struct wl_buffer_listener g_bufferListener = {BufferRelease};

static void Redraw(void *data, struct wl_callback *callback, uint32_t time);
struct wl_callback_listener g_frameListener = {Redraw};

static int32_t CloseFd(int32_t fd)
{
    if (fd == -1) {
        close(fd);
        return -1;
    }

    auto flags = fcntl(fd, F_GETFD);
    if (flags == -1) {
        close(fd);
        return -1;
    }

    if (fcntl(fd, F_SETFD, flags | FD_CLOEXEC) == -1) {
        close(fd);
        return -1;
    }

    return fd;
}

static int32_t CreateTmpFile(char *tmpname)
{
    int32_t fd = mkstemp(tmpname);
    if (fd >= 0) {
        fd = CloseFd(fd);
        unlink(tmpname);
    }

    return fd;
}

static int32_t CreateFile(off_t size)
{
    static const char suffix[] = "/weston-shared-XXXXXX";
    const char *path = getenv("XDG_RUNTIME_DIR");
    if (!path) {
        errno = ENOENT;
        return -1;
    }

    char *name = static_cast<char *>(malloc(strlen(path) + sizeof(suffix)));
    if (!name) {
        return -1;
    }

    strcpy(name, path);
    strcat(name, suffix);
    int32_t fd = CreateTmpFile(name);
    free(name);

    if (fd < 0) {
        return -1;
    }

    int32_t ret = 0;
    do {
        ret = ftruncate(fd, size);
    } while (ret < 0 && errno == EINTR);

    if (ret < 0) {
        close(fd);
        return -1;
    }

    return fd;
}

static int32_t CreateShmBuffer(struct display *display, struct buffer *buffer, int32_t width, int32_t height, uint32_t format)
{
    int32_t stride = width * 4;
    int32_t size = stride * height;
    int32_t fd = CreateFile(size);
    if (fd < 0) {
        fprintf(stderr, "creating a buffer file for %d B failed: %s\n", size, strerror(errno));
        return -1;
    }

    void *data = mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (data == MAP_FAILED) {
        fprintf(stderr, "mmap failed: %s\n", strerror(errno));
        close(fd);
        return -1;
    }

    struct wl_shm_pool *pool = wl_shm_create_pool(display->shm, fd, size);
    buffer->buffer = wl_shm_pool_create_buffer(pool, 0, width, height, stride, format);
    buffer->width = width;
    buffer->height = height;
    wl_buffer_add_listener(buffer->buffer, &g_bufferListener, buffer);
    wl_shm_pool_destroy(pool);
    close(fd);
    buffer->shm_data = data;

    return 0;
}

static void HandleXdgSurfaceConfigure(void *data, struct xdg_surface *surface, uint32_t serial)
{
    struct window *window = static_cast<struct window *>(data);

    xdg_surface_ack_configure(surface, serial);

    if (window->wait_for_configure) {
        Redraw(window, nullptr, 0);
        window->wait_for_configure = false;
    }
}
struct xdg_surface_listener g_xdgSurfaceListener = {HandleXdgSurfaceConfigure};

static void HandleXdgToplevelConfigure(void *data, struct xdg_toplevel *xdg_toplevel,
    int32_t width, int32_t height, struct wl_array *state) {

    struct window *window = static_cast<struct window *>(data);
    if (window->width != width || window->height != height) {
        fprintf(stderr, "HandleXdgToplevelConfigure, width:%d, height:%d\n", width, height);    
        window->width = width;
        window->height = height;
    }
}
static void HandleXdgToplevelClose(void *data, struct xdg_toplevel *xdg_toplevel)
{
    g_running = false;
}
struct xdg_toplevel_listener g_xdgToplevelListener = {HandleXdgToplevelConfigure, HandleXdgToplevelClose};

static struct window *CreateWindow(struct display *display, int32_t width, int32_t height)
{
    struct window *pWindow = static_cast<struct window *>(malloc(sizeof(*pWindow)));
    if (!pWindow) {
        return nullptr;
    }
    memset_s(pWindow, sizeof(*pWindow), 0, sizeof(*pWindow));

    pWindow->callback = nullptr;
    pWindow->display = display;
    pWindow->width = width;
    pWindow->height = height;
    pWindow->surface = wl_compositor_create_surface(display->compositor);

    if (display->wm_base) {
        pWindow->xdg_surface = xdg_wm_base_get_xdg_surface(display->wm_base, pWindow->surface);
        assert(pWindow->xdg_surface);
        xdg_surface_add_listener(pWindow->xdg_surface, &g_xdgSurfaceListener, pWindow);

        pWindow->xdg_toplevel = xdg_surface_get_toplevel(pWindow->xdg_surface);
        assert(pWindow->xdg_toplevel);
        xdg_toplevel_add_listener(pWindow->xdg_toplevel, &g_xdgToplevelListener, pWindow);

        xdg_toplevel_set_title(pWindow->xdg_toplevel, "simple-shm");
        xdg_toplevel_set_app_id(pWindow->xdg_toplevel, "org.freedesktop.weston.simple-shm");

        wl_surface_commit(pWindow->surface);
        pWindow->wait_for_configure = true;
    } else {
        assert(0);
    }

    // region test
    pWindow->region = wl_compositor_create_region(display->compositor);
    wl_region_add(pWindow->region, 0, 0, width, height);
    wl_surface_set_opaque_region(pWindow->surface, pWindow->region);
    wl_surface_set_input_region(pWindow->surface, pWindow->region);
    wl_region_destroy(pWindow->region);
    display->window = pWindow;
    return pWindow;
}

static void DestroyWindow(struct window *window)
{
    if (window->callback) {
        wl_callback_destroy(window->callback);
    }

    if (window->buffers[0].buffer) {
        wl_buffer_destroy(window->buffers[0].buffer);
    }

    if (window->buffers[1].buffer) {
        wl_buffer_destroy(window->buffers[1].buffer);
    }

    if (window->xdg_toplevel) {
        xdg_toplevel_destroy(window->xdg_toplevel);
    }

    if (window->xdg_surface) {
        xdg_surface_destroy(window->xdg_surface);
    }

    wl_surface_destroy(window->surface);
    free(window);
}

static struct buffer *WindowNextBuffer(struct window *window)
{
    struct buffer *buffer;
    if (!window->buffers[0].busy) {
        buffer = &window->buffers[0];
    } else if (!window->buffers[1].busy) {
        buffer = &window->buffers[1];
    } else {
        return nullptr;
    }

    if (buffer->buffer && (buffer->width != window->width || buffer->height != window->height)) {
        wl_buffer_destroy(buffer->buffer);
        buffer->buffer = nullptr;
    }

    if (!buffer->buffer) {
        int32_t ret = CreateShmBuffer(window->display, buffer, window->width, window->height, WL_SHM_FORMAT_RGBA8888);
        if (ret < 0) {
            return nullptr;
        }
        memset(buffer->shm_data, 0xff, window->width * window->height * 4);
    }

    return buffer;
}

struct DrawClock {
public:
    DrawClock(SkCanvas *canvas, uint32_t width, uint32_t height)
        : canvas_(canvas),
          width_(width),
          height_(height),
          now_(FT::TimeStamp::Now()),
          nowSeconds_(now_.Seconds()),
          nowTm_(::localtime(&nowSeconds_))
    {
        center_.set(width_ * 1.0 / 2, height_ * 1.0 / 2);
    }
    ~DrawClock() noexcept = default;

    void Render()
    {
        if (canvas_ == nullptr || width_ == 0 || height_ == 0) {
            return;
        }

        canvas_->clear(SK_ColorGRAY);

        {
            // draw circle
            SkScalar radius = std::min(width_, height_) * 0.45;
            DrawCircle(radius);
        }

        {
            // draw center
            SkScalar radius = std::min(width_, height_) * 0.02;
            DrawCenter(radius);
        }

        {
            // draw date
            DrawDate(24.0);
        }

        {
            // draw time points
            DrawTimePoints(24.0);
        }

        {
            // draw hands
            DrawHands();
        }
        canvas_->flush();
    }

private:
    void DrawCircle(SkScalar radius)
    {
        canvas_->save();
        SkPaint paint;
        paint.setAntiAlias(true);
        paint.setColor(SK_ColorWHITE);
        canvas_->drawCircle(center_, radius, paint);
        canvas_->restore();

        paint.setColor(SK_ColorBLACK);
        paint.setStrokeWidth(3.0);
        canvas_->drawCircle(center_, radius, paint);

        paint.setColor(SK_ColorRED);
        paint.setStrokeWidth(2.0);
        canvas_->drawCircle(center_, radius * 0.98, paint);
    }

    void DrawCenter(SkScalar radius)
    {
        canvas_->save();
        SkPaint paint;
        paint.setAntiAlias(true);
        paint.setColor(SK_ColorBLACK);
        paint.setStrokeWidth(2.0);
        canvas_->drawCircle(center_, radius, paint);
        canvas_->restore();
    }

    void DrawDate(SkScalar fontSize)
    {
        canvas_->save();
        canvas_->translate(center_.x(), center_.y());

        auto timeStampString = now_.ToFormattedString(FT::TimePrecision::SECOND);
        std::string dateText = timeStampString.substr(0, 10);
        auto textBlob = SkTextBlob::MakeFromString(dateText.c_str(), SkFont(nullptr, fontSize, 1.0f, 0.0f));
        auto dateTextBounds = textBlob->bounds();
        canvas_->translate(-1.0 * dateTextBounds.width() / 2.5, dateTextBounds.height() * 1.5);
        SkPaint rectPaint;
        rectPaint.setColor(0xffefefef);
        canvas_->drawRect(dateTextBounds, rectPaint);

        SkPaint texPaint;
        texPaint.setColor(SK_ColorBLACK);
        texPaint.setAntiAlias(true);
        texPaint.setStrokeWidth(3.0);
        canvas_->drawTextBlob(textBlob, 0.0, 0.0, texPaint);
        canvas_->restore();
    }

    static std::string RomeDigit(int32_t clock)
    {
        if (clock < 0 || clock >= 12) {
            return {};
        }

        static std::vector<std::string> romeDigits = {
            "XII", "I", "II", "III", "IV", "V", "VI", "VII", "VIII", "IX", "X", "XI"};
        return romeDigits[clock];
    }

    void DrawTimePoints(SkScalar fontSize)
    {
        SkScalar radius = std::min(width_, height_) * 0.36;
        canvas_->save();
        SkPaint paint;
        paint.setColor(SK_ColorBLACK);
        paint.setAntiAlias(true);
        paint.setStrokeWidth(3.0);
        canvas_->translate(center_.x(), center_.y());
        for (auto i = 0; i != 12; ++i) {
            canvas_->save();
            auto angle = PI / 6.0 * (i - 3);
            std::string text = RomeDigit(i);
            auto textBlob = SkTextBlob::MakeFromString(text.c_str(), SkFont(nullptr, fontSize, 1.0f, 0.0f));
            auto offsetX = std::cos(angle) * radius - fontSize / 2 + 2.0;
            auto offsetY = std::sin(angle) * radius + fontSize / 2 - 2.0;
            canvas_->translate(offsetX, offsetY);
            canvas_->drawTextBlob(textBlob, 0.0, 0.0, paint);
            canvas_->restore();
        }
        canvas_->restore();

        for (auto i = 0; i < 60; ++i) {
            if (i % 5 == 0) {
                paint.setStrokeWidth(3.0);
            } else {
                paint.setStrokeWidth(1.0);
            }

            canvas_->save();
            auto angle = PI * 2.0 * i / 60 - PI / 2.0;
            SkScalar r1 = std::min(width_, height_) * 0.40;
            SkScalar r2 = std::min(width_, height_) * 0.43;

            SkPoint start;
            SkPoint end;
            start.set(center_.x() + std::cos(angle) * r1, center_.y() + std::sin(angle) * r1);
            end.set(center_.x() + std::cos(angle) * r2, center_.y() + std::sin(angle) * r2);
            canvas_->drawLine(start, end, paint);
            canvas_->restore();
        }
    }

    void DrawHand(double val, SkScalar radius, SkColor color, SkScalar strokeWidth)
    {
        canvas_->save();
        auto angle = PI * 2.0 * val / 60 - PI / 2.0;
        SkPaint paint;
        paint.setColor(color);
        paint.setAntiAlias(true);
        paint.setStrokeWidth(strokeWidth);

        SkPoint start = center_;
        SkPoint end;
        end.set(center_.x() + std::cos(angle) * radius, center_.y() + std::sin(angle) * radius);

        canvas_->drawLine(start, end, paint);
        canvas_->restore();
    }

    void DrawHands()
    {
        FT::TimeType seconds = now_.Get() / FT::MICRO_SECS_PER_SECOND;
        auto hour = nowTm_->tm_hour;
        auto minute = nowTm_->tm_min;
        auto sec = nowTm_->tm_sec;
        DrawHand(sec, std::min(width_, height_) * 0.34, SK_ColorBLACK, 2.0);
        DrawHand(minute + sec * 1.0 / 60, std::min(width_, height_) * 0.26, SK_ColorBLACK, 6.0);
        DrawHand(hour * 5 + minute * 1.0 / 60 * 5, std::min(width_, height_) * 0.2, SK_ColorBLACK, 8.0);
    }

private:
    static constexpr double PI = 3.141592653;
    SkCanvas *canvas_ = nullptr;
    uint32_t width_ = 0;
    uint32_t height_ = 0;
    SkPoint center_;
    FT::TimeStamp now_;
    FT::TimeType nowSeconds_;
    struct tm *nowTm_;
};

static void PaintPixels(void *data, int32_t padding, int32_t width, int32_t height, uint32_t time)
{
    SkImageInfo info = SkImageInfo::Make(width, height, kRGBA_8888_SkColorType, kUnpremul_SkAlphaType);
    SkBitmap bitmap;
    bitmap.installPixels(info, data, info.minRowBytes());
    SkCanvas canvas(bitmap);

    DrawClock drawClock(&canvas, width, height);
    drawClock.Render();
}

static void Redraw(void *data, struct wl_callback *callback, uint32_t time)
{
    struct window *window = static_cast<struct window *>(data);
    struct buffer *buffer = WindowNextBuffer(window);
    if (!buffer) {
        fprintf(stderr, !callback ? "Failed to create the first buffer.\n" : "Both buffers busy at Redraw()\n");
        abort();
    }

    PaintPixels(buffer->shm_data, 20, window->width, window->height, time);

    wl_surface_attach(window->surface, buffer->buffer, 0, 0);
    wl_surface_damage(window->surface, 20, 20, window->width - 40, window->height - 40);

    if (callback) {
        wl_callback_destroy(callback);
    }

    window->callback = wl_surface_frame(window->surface);
    wl_callback_add_listener(window->callback, &g_frameListener, window);
    wl_surface_commit(window->surface);
    buffer->busy = true;
}

static void ShmFormat(void *data, struct wl_shm *wl_shm, uint32_t format)
{
    struct display *d = static_cast<struct display *>(data);

    if (format == WL_SHM_FORMAT_RGBA8888) {
        d->has_rgba8888 = true;
    }
}
struct wl_shm_listener shm_listener = {ShmFormat};

static void XdgWmBasePing(void *data, struct xdg_wm_base *shell, uint32_t serial)
{
    xdg_wm_base_pong(shell, serial);
}

static const struct xdg_wm_base_listener xdg_wm_base_listener = {XdgWmBasePing};

void pointer_enter(void *data, struct wl_pointer *wl_pointer, uint32_t serial, struct wl_surface *surface, wl_fixed_t surface_x, wl_fixed_t surface_y)
{
    fprintf(stderr, "pointer_enter in, surface_x=%d, surface_y=%d\n", wl_fixed_to_int(surface_x), wl_fixed_to_int(surface_y));
}

void pointer_leave(void *data, struct wl_pointer *wl_pointer, uint32_t serial, struct wl_surface *surface)
{
    fprintf(stderr, "pointer_leave in\n");
}

void pointer_motion(void *data, struct wl_pointer *wl_pointer, uint32_t time, wl_fixed_t surface_x, wl_fixed_t surface_y)
{
    fprintf(stderr, "pointer_motion in, surface_x=%d, surface_y=%d\n", wl_fixed_to_int(surface_x), wl_fixed_to_int(surface_y));
}

void pointer_button(void *data, struct wl_pointer *wl_pointer, uint32_t serial, uint32_t time, uint32_t button, uint32_t state)
{
    struct display *d = static_cast<struct display *>(data);
    fprintf(stderr, "pointer_button in, button=%d, state=%d\n", button, state);
    if (button == BTN_LEFT && state == WL_POINTER_BUTTON_STATE_PRESSED) {
        xdg_toplevel_move(d->window->xdg_toplevel, d->seat, serial);
    }
}

void pointer_axis(void *data, struct wl_pointer *wl_pointer, uint32_t time, uint32_t axis, wl_fixed_t value)
{
    fprintf(stderr, "pointer_axis in\n");
}

void pointer_frame(void *data, struct wl_pointer *wl_pointer)
{
    fprintf(stderr, "pointer_frame in\n");
}

void pointer_axis_source(void *data, struct wl_pointer *wl_pointer, uint32_t axis_source)
{
    fprintf(stderr, "pointer_axis_source in\n");
}

void pointer_axis_stop(void *data, struct wl_pointer *wl_pointer, uint32_t time, uint32_t axis)
{
    fprintf(stderr, "pointer_axis_stop in\n");
}

void pointer_axis_discrete(void *data, struct wl_pointer *wl_pointer, uint32_t axis, int32_t discrete)
{
    fprintf(stderr, "pointer_axis_discrete in\n");
}

struct wl_pointer_listener pointer_listener = {
    .enter = pointer_enter,
    .leave = pointer_leave,
    .motion = pointer_motion,
    .button = pointer_button,
    .axis = pointer_axis,
    .frame = pointer_frame,
    .axis_source = pointer_axis_source,
    .axis_stop = pointer_axis_stop,
    .axis_discrete = pointer_axis_discrete,
};

void wl_seat_capabilities(void *data, struct wl_seat *wl_seat, uint32_t capabilities)
{
    struct display *d = static_cast<struct display *>(data);
    if (capabilities & WL_SEAT_CAPABILITY_POINTER) {
        fprintf(stderr, "fangtian has a pointer, get it!\n");
        d->pointer = wl_seat_get_pointer(d->seat);
        wl_pointer_add_listener(d->pointer, &pointer_listener, d);
    }
    if (capabilities & WL_SEAT_CAPABILITY_KEYBOARD) {
        fprintf(stderr, "fangtian has a keyboard\n");
    }
}

void wl_seat_name(void *data, struct wl_seat *wl_seat, const char *name) {}

struct wl_seat_listener seat_listener = {
    wl_seat_capabilities,
    wl_seat_name,
};

static void RegistryGlobal(void *data, struct wl_registry *registry,
    uint32_t id, const char *interface, uint32_t version)
{
    struct display *d = static_cast<struct display *>(data);

    if (strcmp(interface, "wl_compositor") == 0) {
        d->compositor = (struct wl_compositor *)wl_registry_bind(registry, id, &wl_compositor_interface, 1);
    } else if (strcmp(interface, "xdg_wm_base") == 0) {
        d->wm_base = (struct xdg_wm_base *)wl_registry_bind(registry, id, &xdg_wm_base_interface, 1);
        xdg_wm_base_add_listener(d->wm_base, &xdg_wm_base_listener, d);
    } else if (strcmp(interface, "wl_shm") == 0) {
        d->shm = (struct wl_shm *)wl_registry_bind(registry, id, &wl_shm_interface, 1);
        wl_shm_add_listener(d->shm, &shm_listener, d);
    } else if (strcmp(interface, "wl_seat") == 0) {
        d->seat = (struct wl_seat *)wl_registry_bind(registry, id, &wl_seat_interface, 1);
        wl_seat_add_listener(d->seat, &seat_listener, d);
    }
}

static void RegistryHandleGlobalRemove(void *data, struct wl_registry *registry, uint32_t name) {}
struct wl_registry_listener g_registryListener = {RegistryGlobal, RegistryHandleGlobalRemove};

static struct display *CreateDisplay()
{
    struct display *pDisplay = (struct display *)malloc(sizeof(*pDisplay));
    if (pDisplay == nullptr) {
        fprintf(stderr, "out of memory\n");
        exit(1);
    }
    memset_s(pDisplay, sizeof(*pDisplay), 0, sizeof(*pDisplay));
    pDisplay->display = wl_display_connect(nullptr);
    assert(pDisplay->display);

    pDisplay->has_rgba8888 = false;
    pDisplay->registry = wl_display_get_registry(pDisplay->display);
    wl_registry_add_listener(pDisplay->registry, &g_registryListener, pDisplay);

    wl_display_roundtrip(pDisplay->display);

    if (pDisplay->shm == nullptr) {
        fprintf(stderr, "No wl_shm global\n");
        exit(1);
    }

    wl_display_roundtrip(pDisplay->display);

    if (!pDisplay->has_rgba8888) {
        fprintf(stderr, "rgba8888 not available\n");
        exit(1);
    }

    return pDisplay;
}

static void DestroyDisplay(struct display *display)
{
    display->window = nullptr;
    if (display->shm) {
        wl_shm_destroy(display->shm);
    }

    if (display->wm_base) {
        xdg_wm_base_destroy(display->wm_base);
    }

    if (display->compositor) {
        wl_compositor_destroy(display->compositor);
    }

    wl_registry_destroy(display->registry);
    wl_display_flush(display->display);
    wl_display_disconnect(display->display);
    free(display);
}

static void SignalInt(int32_t signum)
{
    g_running = false;
}

int32_t main(int32_t argc, char **argv)
{
    fprintf(stderr, "window width: %d, height: %d\n", WINDOW_WIDTH, WINDOW_HEIGHT);

    struct display *display = CreateDisplay();
    struct window *window = CreateWindow(display, WINDOW_WIDTH, WINDOW_HEIGHT);
    if (!window) {
        return 1;
    }

    struct sigaction sigint;
    sigint.sa_handler = SignalInt;
    sigemptyset(&sigint.sa_mask);
    sigint.sa_flags = SA_RESETHAND;
    sigaction(SIGINT, &sigint, nullptr);

    wl_surface_damage(window->surface, 0, 0, window->width, window->height);

    if (!window->wait_for_configure) {
        Redraw(window, nullptr, 0);
    }

    int32_t ret = 0;
    while (g_running && ret != -1) {
        ret = wl_display_dispatch(display->display);
    }

    fprintf(stderr, "simple-shm exiting\n");

    DestroyWindow(window);
    DestroyDisplay(display);

    return 0;
}
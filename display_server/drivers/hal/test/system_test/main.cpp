/*
 * Copyright (c) 2022-2023 Huawei Technologies Co., Ltd.
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

#include <csignal>

#include "display_device.h"
#include "event_loop/event_loop.h"
#include "hdi_display.h"
#include "hdi_session.h"
#include "sync_fence.h"
#include "allocator_controller.h"

FT::HDI::DISPLAY::HdiSession& g_session = FT::HDI::DISPLAY::HdiSession::GetInstance();
FT::HDI::DISPLAY::AllocatorController& g_alloc_controller = FT::HDI::DISPLAY::AllocatorController::GetInstance();
FT::EventLoop g_mainLoop = FT::EventLoop();

bool DestoryBufferHandle(BufferHandle **handle);

struct FrameBuffer {
    BufferHandle *handle;
};

class Screen {
public:
    ~Screen() noexcept {
        if (fb[0]) {
            DestoryBufferHandle(&fb[0]->handle);
        }
        if (fb[1]) {
            DestoryBufferHandle(&fb[1]->handle);
        }
    }
    static void OnVsync(uint32_t sequence, uint64_t timestamp, void *data);

public:
    uint32_t devId = ~0x0;
    bool firstFrame = false;
    uint32_t fbIdx = 0;
    std::shared_ptr<FrameBuffer> fb[2];
};
std::unordered_map<uint32_t, Screen*> g_screens;

bool CreateBuffer(uint32_t devId, BufferHandle **handle)
{
    printf("CreateBuffer: start\n");

    /* Get Display modes */
    std::vector<DisplayModeInfo> displayModeInfos = {};
    uint32_t num = 0;

    int32_t ret = g_session.CallDisplayFunction(
        devId,
        &FT::HDI::DISPLAY::HdiDisplay::GetDisplaySupportedModes,
        &num,
        (DisplayModeInfo*)nullptr
    );
    if (ret != DISPLAY_SUCCESS) {
        printf("CreateBuffer: Failed to get display supported modes, ret=%d\n", ret);
        return false;
    }
    if (num <= 0) {
        printf("CreateBuffer: display modes is %d, exited.\n", num);
        return false;
    }
    printf("CreateBuffer: display modes num=%d\n", num);

    displayModeInfos.resize(num);
    ret = g_session.CallDisplayFunction(
        devId,
        &FT::HDI::DISPLAY::HdiDisplay::GetDisplaySupportedModes,
        &num,
        displayModeInfos.data()
    );
    if (ret != DISPLAY_SUCCESS) {
        printf("CreateBuffer: Failed to get screen supported modes, ret=%d\n", ret);
        return false;
    }

    /* Alloc buffer for framebuffer */
    // Choose mode
    static const uint32_t DEFAULT_MODE_INDEX = 0;
    uint32_t width = displayModeInfos[DEFAULT_MODE_INDEX].width;
    uint32_t height = displayModeInfos[DEFAULT_MODE_INDEX].height;
    ret = g_session.CallDisplayFunction(
        devId,
        &FT::HDI::DISPLAY::HdiDisplay::SetDisplayMode,
        DEFAULT_MODE_INDEX);
    if (ret != DISPLAY_SUCCESS) {
        printf("Draw: Failed to set display mode, ret=%d\n", ret);
        return false;
    }
    printf("CreateBuffer: choose display mode 0 to create fb: width=%d, height=%d.\n", width, height);

    // Get allocator
    AllocInfo info = {
        .width = width,
        .height = height,
        .usage = HBM_USE_MEM_DMA | HBM_USE_CPU_READ | HBM_USE_MEM_FB | HBM_USE_CPU_WRITE, // allocate dumb buffer with "HBM_USE_CPU_WRITE"
        .format = PIXEL_FMT_BGRA_8888}; // TODO: format
    auto allocator = g_alloc_controller.GetAllocator(info.usage);
    if (allocator == nullptr) {
        printf("CreateBuffer: Failed to get buffer allocator.\n");
        return false;
    }

    // Do allocate memory
    ret = allocator->AllocMem(info, handle);
    if (*handle == nullptr || ret != DISPLAY_SUCCESS) {
        printf("CreateBuffer: Failed to alloc fb.\n");
        return false;
    }

    /* Mmap buffer */
    void* addr = allocator->Mmap(**handle);
    if (addr == nullptr) {
        printf("CreateBuffer: Failed to mmap fb.\n");
        return false;
    }

    printf("CreateBuffer: end. handle fd: %i.\n", (*handle)->fd);
    return true;
}

void DrawBaseColor(void *image, uint32_t width, uint32_t height)
{
    uint32_t *pixels = static_cast<uint32_t *>(image);
    static std::vector<uint32_t> colors = {0xff0000ff, 0xffff00ff, 0xaa00ff00, 0xff00ffaa, 0xff0f0f00};

    static uint32_t index = 0;
    if (index++ > 4) {
        index = 0;
    }

    for (uint32_t x = 0; x < width; x++) {
        for (uint32_t y = 0; y < height; y++) {
            *pixels++ = colors[index];
        }
    }
}

void SignalHandler(int signum) {
    printf("Interrupt signal (%d) received.\n", signum);

    g_mainLoop.Stop();
    printf("Stop main loop done.\n");

    for (auto& screen : g_screens) {
        if (screen.second) {
            delete screen.second;
        }
    }

    exit(signum);
}

bool DestoryBufferHandle(BufferHandle **handle)
{
    printf("DestoryBufferHandle.\n");

    if (handle == nullptr) {
        return false;
    }

    auto allocator = g_alloc_controller.GetAllocator((*handle)->usage);
    if (allocator == nullptr) {
        printf("DestoryBufferHandle: Failed to get buffer allocator.\n");
        return false;
    }

    // Unmap buffer & Close fd
    allocator->FreeMem(*handle);
    printf("DestoryBufferHandle: free buffer done.\n");
    *handle = nullptr;
    return true;
}

void Screen::OnVsync(uint32_t sequence, uint64_t timestamp, void *data)
{
    Screen *screen = static_cast<Screen *>(data);
    if (screen == nullptr) {
        printf("OnVSync: screen is null\n");
        return;
    }

    // Print first frames
    static int i = 0;
    if (i < 3) {
        printf("OnVSync: screen devId=%d, sequence=%u, timestamp=%lu\n", screen->devId, sequence, timestamp);
        LOG_DEBUG("DRM Backend Test: OnVSync: screen devId=%{public}d, sequence=%{public}u, timestamp=%{public}lu",
            screen->devId, sequence, timestamp);
        ++i;
    }

    // Do draw in main loop
    g_mainLoop.RunInLoop([screen]() {
        uint32_t devId = screen->devId;
        int32_t fenceFd = -1;
        int32_t ret;

        // Create two framebuffers on first frame
        if (screen->firstFrame) {
            if (screen->fb[0] == nullptr) {
                screen->fb[0] = std::make_shared<FrameBuffer>();
                CreateBuffer(devId, &(screen->fb[0]->handle));
            }
            if (screen->fb[1] == nullptr) {
                screen->fb[1] = std::make_shared<FrameBuffer>();
                CreateBuffer(devId, &(screen->fb[1]->handle));
            }

            // // Fill fb with plain color
            // DrawBaseColor(
            //     screen->fb[screen->fbIdx]->handle->virAddr,
            //     screen->fb[screen->fbIdx]->handle->width,
            //     screen->fb[screen->fbIdx]->handle->height);

            // // Set fb as screen's current buffer
            // ret = g_session.CallDisplayFunction(
            //     devId,
            //     &FT::HDI::DISPLAY::HdiDisplay::SetDisplayClientBuffer,
            //     static_cast<const BufferHandle*>(screen->fb[screen->fbIdx]->handle),
            //     fenceFd);
            // if (ret != DISPLAY_SUCCESS) {
            //     printf("Draw: Failed to set display client buffer, ret=%d\n", ret);
            //     return;
            // }
        }
        screen->firstFrame = false;

        // Fill fb with plain color
        DrawBaseColor(
            screen->fb[screen->fbIdx]->handle->virAddr,
            screen->fb[screen->fbIdx]->handle->width,
            screen->fb[screen->fbIdx]->handle->height);

        // Set fb as screen's current buffer
        ret = g_session.CallDisplayFunction(
            devId,
            &FT::HDI::DISPLAY::HdiDisplay::SetDisplayClientBuffer,
            static_cast<const BufferHandle*>(screen->fb[screen->fbIdx]->handle),
            fenceFd);
        if (ret != DISPLAY_SUCCESS) {
            printf("Draw: Failed to set display client buffer, ret=%d\n", ret);
            return;
        }

        // Commit
        ret = g_session.CallDisplayFunction(
            devId,
            &FT::HDI::DISPLAY::HdiDisplay::Commit,
            &fenceFd);
        if (fenceFd >= 0) {
            auto fence = OHOS::SyncFence(fenceFd);
        } else {
            auto fence = OHOS::SyncFence(-1);
        }

        screen->fbIdx ^= 1;
    });
}

static void OnHotPlug(uint32_t devId, bool connected, void *data)
{
    printf("OnHotPlug: screen devId=%d, connected=%s\n", devId, connected ? "True" : "False");
    LOG_DEBUG("DRM Backend Test: OnHotPlug: screen devId=%d, connected=%s", devId, connected ? "True" : "False");

    // Store screen
    Screen* screen = new Screen();
    screen->devId = devId;
    screen->firstFrame = true;
    g_screens[devId] = screen;

    // Register VSync callback
    int32_t ret = g_session.CallDisplayFunction(
        devId,
        &FT::HDI::DISPLAY::HdiDisplay::RegDisplayVBlankCallback,
        Screen::OnVsync,
        static_cast<void *>(g_screens.at(devId))
    );
    if (ret != DISPLAY_SUCCESS) {
        printf("OnHotPlug: Failed to Register VSync callback, ret=%d\n", ret);
        return;
    }
}

int main()
{
    signal(SIGINT, SignalHandler);

    printf("session pointer in main: %p, displays size : %lu.", &g_session, g_session.GetDisplayDevice()->GetDisplays().size());
    g_alloc_controller.Init();

    // Register HotPlug callback
    g_session.RegHotPlugCallback(OnHotPlug, nullptr);

    // Start main loop
    g_mainLoop.Start();

    return 0;
}

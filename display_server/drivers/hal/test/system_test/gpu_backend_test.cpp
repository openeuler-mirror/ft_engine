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

#include <csignal>

#include "display_device.h"
#include "event_loop/event_loop.h"
#include "hdi_display.h"
#include "hdi_session.h"
#include "sync_fence.h"
#include "allocator_controller.h"
#include <EGL/egl.h>
#include <gbm.h>
#include <GLES2/gl2.h>

oewm::HDI::DISPLAY::HdiSession& g_session = oewm::HDI::DISPLAY::HdiSession::GetInstance();
oewm::HDI::DISPLAY::AllocatorController& g_alloc_controller = oewm::HDI::DISPLAY::AllocatorController::GetInstance();
oewm::EventLoop g_mainLoop = oewm::EventLoop();

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
    }
    static void OnVsync(uint32_t sequence, uint64_t timestamp, void *data);

public:
    uint32_t devId = ~0x0;
    bool firstFrame = false;
    uint32_t fbIdx = 0;
    std::shared_ptr<FrameBuffer> fb[2];
};

std::unordered_map<uint32_t, Screen*> g_screens;
static EGLDisplay defaultDisplay;
static struct gbm_device *gbm_device;
static EGLContext context;
static struct gbm_surface *gbm_surface;
static EGLSurface egl_surface;

EGLConfig GetEGLConfig()
{
	EGLint egl_config_attribs[] = {
		EGL_BUFFER_SIZE,	32,   //EGL_BUFFER_SIZE：指定缓冲区的位数为32位。
		EGL_DEPTH_SIZE,		EGL_DONT_CARE,  //EGL_DEPTH_SIZE：深度缓冲区大小，使用EGL_DONT_CARE表示不关心具体大小。
		EGL_STENCIL_SIZE,	EGL_DONT_CARE,  //EGL_STENCIL_SIZE：模板缓冲区大小
		EGL_RENDERABLE_TYPE,	EGL_OPENGL_ES2_BIT, //EGL_RENDERABLE_TYPE：指定可渲染的类型为OpenGL ES 2.0
		EGL_SURFACE_TYPE,	EGL_WINDOW_BIT, //EGL_SURFACE_TYPE：指定表面类型为窗口。
		EGL_NONE,  //EGL_NONE：属性数组的结束标志。
	};

	EGLint num_configs;   //调用eglGetConfigs函数获取系统中可用的EGL配置数量，将结果保存在num_configs变量中
	assert(eglGetConfigs(defaultDisplay, NULL, 0, &num_configs) == EGL_TRUE);

	EGLConfig *configs = (EGLConfig *)malloc(num_configs * sizeof(EGLConfig));

	assert(eglChooseConfig(defaultDisplay, egl_config_attribs,
			       configs, num_configs, &num_configs) == EGL_TRUE);
	assert(num_configs);      //确保至少存在一个满足条件的配置
	printf("num config %d\n", num_configs);

	for (int i = 0; i < num_configs; ++i) {
		EGLint gbm_format;

		assert(eglGetConfigAttrib(defaultDisplay, configs[i],
					  EGL_NATIVE_VISUAL_ID, &gbm_format) == EGL_TRUE);
		printf("gbm format %x\n", gbm_format);
		/*如果找到与目标GBM格式（GBM_FORMAT_ARGB8888）匹配的配置，
		即gbm_format等于目标格式，就释放configs的内存并返回该配置。*/
		if (gbm_format == GBM_FORMAT_ARGB8888) {
			EGLConfig ret = configs[i];
			free(configs);
			return ret;
		}
	}
	// 未找到匹配的配置，调用abort函数终止程序。
	abort();
}

void SwapBuffer(BufferHandle *handle) {
	//交换EGL表面的前后缓冲区。
	eglSwapBuffers (defaultDisplay, egl_surface);
	//锁定GBM表面的前端缓冲区，以便进行后续操作。
	struct gbm_bo *bo = gbm_surface_lock_front_buffer (gbm_surface);
	//获取前端缓冲区的句柄
	handle->key = gbm_bo_get_handle(bo).u32;
}

bool InitEGL(uint32_t devId, BufferHandle **handle)
{
    printf("InitEGL\n");

    /* Get Display modes */
    std::vector<DisplayModeInfo> displayModeInfos = {};
    uint32_t num = 0;

    int32_t ret = g_session.CallDisplayFunction(
        devId, 
        &oewm::HDI::DISPLAY::HdiDisplay::GetDisplaySupportedModes, 
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
        &oewm::HDI::DISPLAY::HdiDisplay::GetDisplaySupportedModes, 
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
        &oewm::HDI::DISPLAY::HdiDisplay::SetDisplayMode,
        DEFAULT_MODE_INDEX);
    if (ret != DISPLAY_SUCCESS) {
        printf("Draw: Failed to set display mode, ret=%d\n", ret);
        return false;
    }
    printf("CreateBuffer: choose display mode 0 to create fb: width=%d, height=%d.\n", width, height);

    /*
    // Get allocator
    AllocInfo info = {
        .width = width,
        .height = height,
        .usage = HBM_USE_MEM_DMA | HBM_USE_MEM_FB , // allocate gbm buffer
        .format = PIXEL_FMT_BGRA_8888};
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
    printf("CreateBuffer: end. handle fd: %i.\n", (*handle)->fd);
    */
    gbm_device = g_session.GetDisplayDevice()->GetGbmDevice();
	defaultDisplay = eglGetDisplay(gbm_device); 
	int major, minor;
	eglInitialize(defaultDisplay, &major, &minor);
	printf("%d %d", major, minor);

    eglBindAPI(EGL_OPENGL_ES2_BIT);
    EGLConfig config = GetEGLConfig();
    context = eglCreateContext (defaultDisplay, config, EGL_NO_CONTEXT, NULL);

	// create the GBM and EGL surface
	gbm_surface = gbm_surface_create (gbm_device, width, height, GBM_BO_FORMAT_XRGB8888, GBM_BO_USE_LINEAR|GBM_BO_USE_SCANOUT|GBM_BO_USE_RENDERING);
	//创建EGL窗口表面对象，将GBM表面与EGL绑定
	egl_surface = eglCreateWindowSurface (defaultDisplay, config, gbm_surface, NULL);
	//将OpenGL上下文与EGL表面进行绑定，使其成为当前上下文。
	eglMakeCurrent (defaultDisplay, egl_surface, egl_surface, context);

    static BufferHandle tmp; 
    *handle = &tmp;
    tmp.virAddr = nullptr;
    tmp.stride= width;
    tmp.usage = HBM_USE_MEM_DMA | HBM_USE_MEM_FB;
    tmp.format = GBM_BO_FORMAT_XRGB8888;
    tmp.height= height;
    tmp.width = width;
    tmp.size = tmp.height * tmp.stride; 

    return true;
}

void DrawBaseColor(float progress)
{
    glClearColor (1.0f-progress, progress, 0.0, 1.0);
	glClear (GL_COLOR_BUFFER_BIT);
    return;
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
    eglDestroySurface (defaultDisplay, egl_surface);
	gbm_surface_destroy (gbm_surface);
	eglDestroyContext (defaultDisplay, context);
	eglTerminate (defaultDisplay);
	gbm_device_destroy (gbm_device);

    /*
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
    */
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
                InitEGL(devId, &(screen->fb[0]->handle));
            }
        }
        screen->firstFrame = false;

        // Fill fb with plain color
        if (screen->fbIdx == 100) {
            screen->fbIdx = 0;
        }
        DrawBaseColor(screen->fbIdx / 100.0);
        screen->fbIdx++;

        // swap buffer
        SwapBuffer(screen->fb[0]->handle);

        // Set fb as screen's current buffer
        ret = g_session.CallDisplayFunction(
            devId, 
            &oewm::HDI::DISPLAY::HdiDisplay::SetDisplayClientBuffer, 
            static_cast<const BufferHandle*>(screen->fb[0]->handle), 
            fenceFd);
        if (ret != DISPLAY_SUCCESS) {
            printf("Draw: Failed to set display client buffer, ret=%d\n", ret);
            return;
        }
        
        // Commit
        ret = g_session.CallDisplayFunction(
            devId, 
            &oewm::HDI::DISPLAY::HdiDisplay::Commit,
            &fenceFd);
        if (fenceFd >= 0) {
            auto fence = OHOS::SyncFence(fenceFd);
        } else {
            auto fence = OHOS::SyncFence(-1);
        }
        
        screen->fbIdx++;
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
        &oewm::HDI::DISPLAY::HdiDisplay::RegDisplayVBlankCallback, 
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

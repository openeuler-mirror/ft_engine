/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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
#include <iostream>
#include <chrono>
#include <thread>
#include <unistd.h>
#include <iservice_registry.h>
#include <securec.h>
#include <native_window.h>
#include <assert.h>
#include <hilog/log.h>
#include <sys/wait.h>

#include "external_window.h"

using namespace OHOS::HiviewDFX;

namespace OHOS::Rosen {
class NativeWindowBufferTest : public IBufferConsumerListenerClazz {
public:
    static void SetUpTestCase();
    void OnBufferAvailable() override;
    int32_t SetData(NativeWindowBuffer *nativeWindowBuffer, NativeWindow *nativeWindow);
    bool GetData(sptr<SurfaceBuffer> &buffer);
    pid_t ChildProcessMain();

    void DoTest();

    static inline sptr<OHOS::Surface> cSurface = nullptr;
    static inline int32_t pipeFd[2] = {};
    static inline int32_t ipcSystemAbilityID = 34156;
};

void NativeWindowBufferTest::SetUpTestCase() {}

void NativeWindowBufferTest::OnBufferAvailable() {}

int32_t NativeWindowBufferTest::SetData(NativeWindowBuffer *nativeWindowBuffer, NativeWindow *nativeWindow)
{
    nativeWindowBuffer->sfbuffer->GetExtraData()->ExtraSet("123", 0x123);
    nativeWindowBuffer->sfbuffer->GetExtraData()->ExtraSet("345", (int64_t)0x345);
    nativeWindowBuffer->sfbuffer->GetExtraData()->ExtraSet("567", "567");

    // alloc OHExtDataHandle
    uint32_t reserveInts = 1;
    size_t handleSize = sizeof(OHExtDataHandle) + (sizeof(int32_t) * reserveInts);
    OHExtDataHandle *handle = static_cast<OHExtDataHandle *>(malloc(handleSize));
    if (handle == nullptr) {
        return -1;
    }
    int32_t ret = memset_s(handle, handleSize, 0, handleSize);
    if (ret != EOK) {
        return -1;
    }
    handle->fd = -1;
    handle->reserveInts = reserveInts;
    for (uint32_t i = 0; i < reserveInts; i++) {
        handle->reserve[i] = 1;
    }

    ret = OH_NativeWindow_NativeWindowSetTunnelHandle(nativeWindow, handle);
    // free OHExtDataHandle
    if (handle->fd >= 0) {
        close(handle->fd);
        handle->fd = -1;
    }
    free(handle);
    handle = nullptr;
    return ret;
}

bool NativeWindowBufferTest::GetData(sptr<SurfaceBuffer> &buffer)
{
    int32_t int32;
    int64_t int64;
    std::string str;
    buffer->GetExtraData()->ExtraGet("123", int32);
    buffer->GetExtraData()->ExtraGet("345", int64);
    buffer->GetExtraData()->ExtraGet("567", str);
    if ((int32 != 0x123) || (int64 != 0x345) || (str != "567")) {
        return false;
    }

    sptr<SurfaceTunnelHandle> handleGet = nullptr;
    handleGet = cSurface->GetTunnelHandle();
    if ((handleGet == nullptr) || (handleGet->GetHandle()->fd != -1) ||
        (handleGet->GetHandle()->reserveInts != 1) || (handleGet->GetHandle()->reserve[0] != 1)) {
            return false;
    }
    return true;
}

pid_t NativeWindowBufferTest::ChildProcessMain()
{
    pipe(pipeFd);
    pid_t pid = fork();
    if (pid != 0) {
        return pid;
    }

    int64_t data;
    read(pipeFd[0], &data, sizeof(data));

    sptr<IRemoteObject> robj = nullptr;
    while (true) {
        auto sam = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
        robj = sam->GetSystemAbility(ipcSystemAbilityID);
        if (robj != nullptr) {
            break;
        }
        sleep(0);
    }

    auto producer = iface_cast<IBufferProducer>(robj);
    sptr<Surface> pSurface = Surface::CreateSurfaceAsProducer(producer);

    struct NativeWindow *nativeWindow = OH_NativeWindow_CreateNativeWindow(&pSurface);
    struct NativeWindowBuffer *nativeWindowBuffer = nullptr;

    int code = SET_USAGE;
    uint64_t usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE;
    OH_NativeWindow_NativeWindowHandleOpt(nativeWindow, code, usage);

    code = SET_BUFFER_GEOMETRY;
    int32_t height = 0x100;
    int32_t width = 0x100;
    OH_NativeWindow_NativeWindowHandleOpt(nativeWindow, code, height, width);

    code = SET_FORMAT;
    int32_t format = GRAPHIC_PIXEL_FMT_RGBA_8888;
    OH_NativeWindow_NativeWindowHandleOpt(nativeWindow, code, format);

    code = SET_STRIDE;
    int32_t stride = 0x8;
    OH_NativeWindow_NativeWindowHandleOpt(nativeWindow, code, stride);

    int32_t fenceFd = -1;
    auto ret = OH_NativeWindow_NativeWindowRequestBuffer(nativeWindow, &nativeWindowBuffer, &fenceFd);
    if (ret != OHOS::GSERROR_OK) {
        data = ret;
        write(pipeFd[1], &data, sizeof(data));
        exit(0);
        return -1;
    }
    ret = SetData(nativeWindowBuffer, nativeWindow);
    if (ret != OHOS::GSERROR_OK) {
        data = ret;
        write(pipeFd[1], &data, sizeof(data));
        exit(0);
        return -1;
    }

    struct OHNativeWindowRegion *region = new OHNativeWindowRegion();
    struct OHNativeWindowRegion::Rect *rect = new OHNativeWindowRegion::Rect();
    rect->w = 0x100;
    rect->h = 0x100;
    region->rects = rect;
    ret = OH_NativeWindow_NativeWindowFlushBuffer(nativeWindow, nativeWindowBuffer, -1, *region);
    if (ret != OHOS::GSERROR_OK) {
        data = ret;
        write(pipeFd[1], &data, sizeof(data));
        exit(0);
        return -1;
    }
    data = ret;
    write(pipeFd[1], &data, sizeof(data));
    usleep(1000); // sleep 1000 microseconds (equals 1 milliseconds)
    read(pipeFd[0], &data, sizeof(data));
    close(pipeFd[0]);
    close(pipeFd[1]);
    exit(0);
    return 0;
}

/*
* Function: produce and consumer surface of nativewindow
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. produce surface by nativewindow interface, fill buffer
*                  2. consume surface and check buffer
* @tc.require: issueI5GMZN issueI5IWHW
 */
void NativeWindowBufferTest::DoTest()
{
    auto pid = ChildProcessMain();
    assert(pid > 0);

    std::cout << "[-] Create Consumer Surface." << std::endl;
    HiviewDFX::HiLog::Error({ LOG_CORE, 0xC01400, "SurfaceTest" }, "Create Consumer Surface");
    cSurface = Surface::CreateSurfaceAsConsumer("test");
    cSurface->RegisterConsumerListener(this);
    auto producer = cSurface->GetProducer();
    std::cout << "[-] Create Consumer Surface done." << std::endl;

    std::cout << "[-] Add SA." << std::endl;
    HiviewDFX::HiLog::Error({ LOG_CORE, 0xC01400, "SurfaceTest" }, "Add SA.");
    auto sam = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    sam->AddSystemAbility(ipcSystemAbilityID, producer->AsObject());
    std::cout << "[-] Add SA done." << std::endl;

    int64_t data = 0;
    write(pipeFd[1], &data, sizeof(data));
    usleep(1000); // sleep 1000 microseconds (equals 1 milliseconds)
    read(pipeFd[0], &data, sizeof(data));
    assert(data == OHOS::GSERROR_OK);

    std::cout << "[-] Acquire Buffer." << std::endl;
    HiviewDFX::HiLog::Info({ LOG_CORE, 0xC01400, "SurfaceTest" }, "Acquire Buffer");
    OHOS::sptr<SurfaceBuffer> buffer = nullptr;
    int32_t fence = -1;
    int64_t timestamp;
    Rect damage;
    auto ret = cSurface->AcquireBuffer(buffer, fence, timestamp, damage);
    assert(ret == OHOS::GSERROR_OK);
    assert(buffer != nullptr);
    assert(GetData(buffer) == true);
    std::cout << "[-] Acquire Buffer done." << std::endl;

    std::cout << "[-] Release Buffer." << std::endl;
    HiviewDFX::HiLog::Info({ LOG_CORE, 0xC01400, "SurfaceTest" }, "Release Buffer");
    ret = cSurface->ReleaseBuffer(buffer, -1);
    assert(ret == OHOS::GSERROR_OK);
    std::cout << "[-] Release Buffer done." << std::endl;

    write(pipeFd[1], &data, sizeof(data));
    close(pipeFd[0]);
    close(pipeFd[1]);

    std::cout << "[-] Remove SA." << std::endl;
    HiviewDFX::HiLog::Info({ LOG_CORE, 0xC01400, "SurfaceTest" }, "Remove SA.");
    sam->RemoveSystemAbility(ipcSystemAbilityID);
    waitpid(pid, nullptr, 0);
    std::cout << "[-] Remove SA done." << std::endl;
}
}

int main() {
    std::cout << "[-] Test Start." << std::endl;
    OHOS::HiviewDFX::HiLog::Info({ LOG_CORE, 0xC01400, "SurfaceTest" }, "Test Start.");

    auto tester = OHOS::Rosen::NativeWindowBufferTest();
    tester.DoTest();

    std::cout << "[-] Test Done." << std::endl;
    OHOS::HiviewDFX::HiLog::Info({ LOG_CORE, 0xC01400, "SurfaceTest" }, "Test Done.");
}

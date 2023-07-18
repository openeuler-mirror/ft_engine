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
#include <gtest/gtest.h>
#include <iservice_registry.h>
#include <native_window.h>
#include <securec.h>
#include <surface_type.h>
#include "buffer_log.h"
#include "external_window.h"

namespace OHOS::Rosen {
class BufferConsumerListener : public IBufferConsumerListener {
public:
    void OnBufferAvailable() override
    {
    }
};

static OHExtDataHandle *AllocOHExtDataHandle(uint32_t reserveInts)
{
    size_t handleSize = sizeof(OHExtDataHandle) + (sizeof(int32_t) * reserveInts);
    OHExtDataHandle *handle = static_cast<OHExtDataHandle *>(malloc(handleSize));
    if (handle == nullptr) {
        BLOGE("AllocOHExtDataHandle malloc %zu failed", handleSize);
        return nullptr;
    }
    auto ret = memset_s(handle, handleSize, 0, handleSize);
    if (ret != EOK) {
        BLOGE("AllocOHExtDataHandle memset_s failed");
        return nullptr;
    }
    handle->fd = -1;
    handle->reserveInts = reserveInts;
    for (uint32_t i = 0; i < reserveInts; i++) {
        handle->reserve[i] = -1;
    }
    return handle;
}

static void FreeOHExtDataHandle(OHExtDataHandle *handle)
{
    if (handle == nullptr) {
        BLOGW("FreeOHExtDataHandle with nullptr handle");
        return ;
    }
    if (handle->fd >= 0) {
        close(handle->fd);
        handle->fd = -1;
    }
    free(handle);
}

class NativeWindowTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();

    static inline BufferRequestConfig requestConfig = {};
    static inline BufferFlushConfig flushConfig = {};
    static inline sptr<OHOS::Surface> cSurface = nullptr;
    static inline sptr<OHOS::IBufferProducer> producer = nullptr;
    static inline sptr<OHOS::Surface> pSurface = nullptr;
    static inline sptr<OHOS::SurfaceBuffer> sBuffer = nullptr;
    static inline NativeWindow* nativeWindow = nullptr;
    static inline NativeWindowBuffer* nativeWindowBuffer = nullptr;
};

void NativeWindowTest::SetUpTestCase()
{
    requestConfig = {
        .width = 0x100,  // small
        .height = 0x100, // small
        .strideAlignment = 0x8,
        .format = GRAPHIC_PIXEL_FMT_RGBA_8888,
#ifdef RS_ENABLE_GL
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_MEM_DMA,
#else
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE,
#endif
        .timeout = 0,
    };

    cSurface = Surface::CreateSurfaceAsConsumer();
    sptr<IBufferConsumerListener> listener = new BufferConsumerListener();
    cSurface->RegisterConsumerListener(listener);
    producer = cSurface->GetProducer();
    pSurface = Surface::CreateSurfaceAsProducer(producer);
    int32_t fence;
    pSurface->RequestBuffer(sBuffer, fence, requestConfig);
}

void NativeWindowTest::TearDownTestCase()
{
    flushConfig = { .damage = {
        .w = 0x100,
        .h = 0x100,
    } };
    pSurface->FlushBuffer(sBuffer, -1, flushConfig);
    sBuffer = nullptr;
    cSurface = nullptr;
    producer = nullptr;
    pSurface = nullptr;
    nativeWindow = nullptr;
    nativeWindowBuffer = nullptr;
}

/*
* Function: OH_NativeWindow_CreateNativeWindow
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call OH_NativeWindow_CreateNativeWindow by abnormal input
*                  2. check ret
 */
TEST_F(NativeWindowTest, CreateNativeWindow001)
{
    ASSERT_EQ(OH_NativeWindow_CreateNativeWindow(nullptr), nullptr);
}

/*
* Function: OH_NativeWindow_CreateNativeWindow
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call OH_NativeWindow_CreateNativeWindow
*                  2. check ret
 */
TEST_F(NativeWindowTest, CreateNativeWindow002)
{
    nativeWindow = OH_NativeWindow_CreateNativeWindow(&pSurface);
    ASSERT_NE(nativeWindow, nullptr);
}

/*
* Function: OH_NativeWindow_NativeWindowHandleOpt
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call OH_NativeWindow_NativeWindowHandleOpt by abnormal input
*                  2. check ret
 */
TEST_F(NativeWindowTest, HandleOpt001)
{
    int code = SET_USAGE;
    uint64_t usage = BUFFER_USAGE_CPU_READ;
    ASSERT_EQ(OH_NativeWindow_NativeWindowHandleOpt(nullptr, code, usage), OHOS::GSERROR_INVALID_ARGUMENTS);
}

/*
* Function: OH_NativeWindow_NativeWindowHandleOpt
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call OH_NativeWindow_NativeWindowHandleOpt by different param
*                  2. check ret
 */
TEST_F(NativeWindowTest, HandleOpt002)
{
    int code = SET_USAGE;
    uint64_t usageSet = BUFFER_USAGE_CPU_READ;
    ASSERT_EQ(OH_NativeWindow_NativeWindowHandleOpt(nativeWindow, code, usageSet), OHOS::GSERROR_OK);

    code = GET_USAGE;
    uint64_t usageGet = BUFFER_USAGE_CPU_WRITE;
    ASSERT_EQ(OH_NativeWindow_NativeWindowHandleOpt(nativeWindow, code, &usageGet), OHOS::GSERROR_OK);
    ASSERT_EQ(usageSet, usageGet);
}

/*
* Function: OH_NativeWindow_NativeWindowHandleOpt
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call OH_NativeWindow_NativeWindowHandleOpt by different param
*                  2. check ret
 */
TEST_F(NativeWindowTest, HandleOpt003)
{
    int code = SET_BUFFER_GEOMETRY;
    int32_t heightSet = 0x100;
    int32_t widthSet = 0x100;
    ASSERT_EQ(OH_NativeWindow_NativeWindowHandleOpt(nativeWindow, code, heightSet, widthSet), OHOS::GSERROR_OK);

    code = GET_BUFFER_GEOMETRY;
    int32_t heightGet = 0;
    int32_t widthGet = 0;
    ASSERT_EQ(OH_NativeWindow_NativeWindowHandleOpt(nativeWindow, code, &heightGet, &widthGet), OHOS::GSERROR_OK);
    ASSERT_EQ(heightSet, heightGet);
    ASSERT_EQ(widthSet, widthGet);
}

/*
* Function: OH_NativeWindow_NativeWindowHandleOpt
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call OH_NativeWindow_NativeWindowHandleOpt by different param
*                  2. check ret
 */
TEST_F(NativeWindowTest, HandleOpt004)
{
    int code = SET_FORMAT;
    int32_t formatSet = GRAPHIC_PIXEL_FMT_RGBA_8888;
    ASSERT_EQ(OH_NativeWindow_NativeWindowHandleOpt(nativeWindow, code, formatSet), OHOS::GSERROR_OK);

    code = GET_FORMAT;
    int32_t formatGet = GRAPHIC_PIXEL_FMT_CLUT8;
    ASSERT_EQ(OH_NativeWindow_NativeWindowHandleOpt(nativeWindow, code, &formatGet), OHOS::GSERROR_OK);
    ASSERT_EQ(formatSet, formatGet);
}

/*
* Function: OH_NativeWindow_NativeWindowHandleOpt
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call OH_NativeWindow_NativeWindowHandleOpt by different param
*                  2. check ret
 */
TEST_F(NativeWindowTest, HandleOpt005)
{
    int code = SET_STRIDE;
    int32_t strideSet = 0x8;
    ASSERT_EQ(OH_NativeWindow_NativeWindowHandleOpt(nativeWindow, code, strideSet), OHOS::GSERROR_OK);

    code = GET_STRIDE;
    int32_t strideGet = 0;
    ASSERT_EQ(OH_NativeWindow_NativeWindowHandleOpt(nativeWindow, code, &strideGet), OHOS::GSERROR_OK);
    ASSERT_EQ(strideSet, strideGet);
}

/*
* Function: OH_NativeWindow_NativeWindowHandleOpt
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call OH_NativeWindow_NativeWindowHandleOpt by different param
*                  2. check ret
 */
TEST_F(NativeWindowTest, HandleOpt006)
{
    int code = SET_COLOR_GAMUT;
    int32_t colorGamutSet = static_cast<int32_t>(GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DCI_P3);
    ASSERT_EQ(OH_NativeWindow_NativeWindowHandleOpt(nativeWindow, code, colorGamutSet), OHOS::GSERROR_OK);

    code = GET_COLOR_GAMUT;
    int32_t colorGamutGet = 0;
    ASSERT_EQ(OH_NativeWindow_NativeWindowHandleOpt(nativeWindow, code, &colorGamutGet), OHOS::GSERROR_OK);
    ASSERT_EQ(colorGamutSet, colorGamutGet);
}

/*
* Function: OH_NativeWindow_NativeWindowHandleOpt
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call OH_NativeWindow_NativeWindowHandleOpt by different param
*                  2. check ret
 */
TEST_F(NativeWindowTest, HandleOpt007)
{
    int code = SET_TIMEOUT;
    int32_t timeoutSet = 10;  // 10: for test
    ASSERT_EQ(OH_NativeWindow_NativeWindowHandleOpt(nativeWindow, code, timeoutSet), OHOS::GSERROR_OK);

    code = GET_TIMEOUT;
    int32_t timeoutGet = 0;
    ASSERT_EQ(OH_NativeWindow_NativeWindowHandleOpt(nativeWindow, code, &timeoutGet), OHOS::GSERROR_OK);
    ASSERT_EQ(timeoutSet, timeoutGet);
}

/*
* Function: OH_NativeWindow_CreateNativeWindowBufferFromSurfaceBuffer
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call OH_NativeWindow_CreateNativeWindowBufferFromSurfaceBuffer by abnormal input
*                  2. check ret
 */
TEST_F(NativeWindowTest, CreateNativeWindowBuffer001)
{
    ASSERT_EQ(OH_NativeWindow_CreateNativeWindowBufferFromSurfaceBuffer(nullptr), nullptr);
}

/*
* Function: OH_NativeWindow_CreateNativeWindowBufferFromSurfaceBuffer
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call OH_NativeWindow_CreateNativeWindowBufferFromSurfaceBuffer
*                  2. check ret
 */
TEST_F(NativeWindowTest, CreateNativeWindowBuffer002)
{
    nativeWindowBuffer = OH_NativeWindow_CreateNativeWindowBufferFromSurfaceBuffer(&sBuffer);
    ASSERT_NE(nativeWindowBuffer, nullptr);
}

/*
* Function: OH_NativeWindow_NativeWindowRequestBuffer
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call OH_NativeWindow_NativeWindowRequestBuffer by abnormal input
*                  2. check ret
 */
TEST_F(NativeWindowTest, RequestBuffer001)
{
    ASSERT_EQ(OH_NativeWindow_NativeWindowRequestBuffer(nullptr, &nativeWindowBuffer, nullptr),
              OHOS::GSERROR_INVALID_ARGUMENTS);
}

/*
* Function: OH_NativeWindow_NativeWindowRequestBuffer
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call OH_NativeWindow_NativeWindowRequestBuffer by abnormal input
*                  2. check ret
 */
TEST_F(NativeWindowTest, RequestBuffer002)
{
    ASSERT_EQ(OH_NativeWindow_NativeWindowRequestBuffer(nativeWindow, nullptr, nullptr),
              OHOS::GSERROR_INVALID_ARGUMENTS);
}

/*
* Function: OH_NativeWindow_GetBufferHandleFromNative
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call OH_NativeWindow_GetBufferHandleFromNative by abnormal input
*                  2. check ret
 */
TEST_F(NativeWindowTest, GetBufferHandle001)
{
    ASSERT_EQ(OH_NativeWindow_GetBufferHandleFromNative(nullptr), nullptr);
}

/*
* Function: OH_NativeWindow_GetBufferHandleFromNative
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call OH_NativeWindow_GetBufferHandleFromNative
*                  2. check ret
 */
TEST_F(NativeWindowTest, GetBufferHandle002)
{
    struct NativeWindowBuffer *buffer = new NativeWindowBuffer();
    buffer->sfbuffer = sBuffer;
    ASSERT_NE(OH_NativeWindow_GetBufferHandleFromNative(nativeWindowBuffer), nullptr);
    delete buffer;
}

/*
* Function: OH_NativeWindow_NativeWindowFlushBuffer
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call OH_NativeWindow_NativeWindowFlushBuffer by abnormal input
*                  2. check ret
 */
TEST_F(NativeWindowTest, FlushBuffer001)
{
    int fenceFd = -1;
    struct OHNativeWindowRegion *region = new OHNativeWindowRegion();
    struct OHNativeWindowRegion::Rect * rect = new OHNativeWindowRegion::Rect();
    rect->x = 0x100;
    rect->y = 0x100;
    rect->w = 0x100;
    rect->h = 0x100;
    region->rects = rect;

    ASSERT_EQ(OH_NativeWindow_NativeWindowFlushBuffer(nullptr, nullptr, fenceFd, *region),
              OHOS::GSERROR_INVALID_ARGUMENTS);
    delete region;
}

/*
* Function: OH_NativeWindow_NativeWindowFlushBuffer
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call OH_NativeWindow_NativeWindowFlushBuffer by abnormal input
*                  2. check ret
 */
TEST_F(NativeWindowTest, FlushBuffer002)
{
    int fenceFd = -1;
    struct OHNativeWindowRegion *region = new OHNativeWindowRegion();
    struct OHNativeWindowRegion::Rect * rect = new OHNativeWindowRegion::Rect();
    rect->x = 0x100;
    rect->y = 0x100;
    rect->w = 0x100;
    rect->h = 0x100;
    region->rects = rect;

    ASSERT_EQ(OH_NativeWindow_NativeWindowFlushBuffer(nativeWindow, nullptr, fenceFd, *region),
              OHOS::GSERROR_INVALID_ARGUMENTS);
    delete region;
}

/*
* Function: OH_NativeWindow_NativeWindowFlushBuffer
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call OH_NativeWindow_NativeWindowFlushBuffer
*                  2. check ret
 */
TEST_F(NativeWindowTest, FlushBuffer003)
{
    int fenceFd = -1;
    struct OHNativeWindowRegion *region = new OHNativeWindowRegion();
    struct OHNativeWindowRegion::Rect * rect = new OHNativeWindowRegion::Rect();
    rect->x = 0x100;
    rect->y = 0x100;
    rect->w = 0x100;
    rect->h = 0x100;
    region->rects = rect;

    ASSERT_EQ(OH_NativeWindow_NativeWindowFlushBuffer(nativeWindow, nativeWindowBuffer, fenceFd, *region),
              OHOS::GSERROR_OK);
    delete region;
}

/*
* Function: OH_NativeWindow_NativeWindowAbortBuffer
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call OH_NativeWindow_NativeWindowAbortBuffer by abnormal input
*                  2. check ret
 */
TEST_F(NativeWindowTest, CancelBuffer001)
{
    ASSERT_EQ(OH_NativeWindow_NativeWindowAbortBuffer(nullptr, nullptr), OHOS::GSERROR_INVALID_ARGUMENTS);
}

/*
* Function: OH_NativeWindow_NativeWindowAbortBuffer
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call OH_NativeWindow_NativeWindowAbortBuffer by abnormal input
*                  2. check ret
 */
TEST_F(NativeWindowTest, CancelBuffer002)
{
    ASSERT_EQ(OH_NativeWindow_NativeWindowAbortBuffer(nativeWindow, nullptr), OHOS::GSERROR_INVALID_ARGUMENTS);
}

/*
* Function: OH_NativeWindow_NativeWindowAbortBuffer
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call OH_NativeWindow_NativeWindowAbortBuffer
*                  2. check ret
 */
TEST_F(NativeWindowTest, CancelBuffer003)
{
    ASSERT_EQ(OH_NativeWindow_NativeWindowAbortBuffer(nativeWindow, nativeWindowBuffer), OHOS::GSERROR_OK);
}

/*
* Function: OH_NativeWindow_NativeObjectReference
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call OH_NativeWindow_NativeObjectReference
*                  2. check ret
 */
TEST_F(NativeWindowTest, Reference001)
{
    struct NativeWindowBuffer *buffer = new NativeWindowBuffer();
    buffer->sfbuffer = sBuffer;
    ASSERT_EQ(OH_NativeWindow_NativeObjectReference(reinterpret_cast<void *>(buffer)), OHOS::GSERROR_OK);
    delete buffer;
}

/*
* Function: OH_NativeWindow_NativeObjectUnreference
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call OH_NativeWindow_NativeObjectUnreference
*                  2. check ret
 */
TEST_F(NativeWindowTest, Unreference001)
{
    struct NativeWindowBuffer *buffer = new NativeWindowBuffer();
    buffer->sfbuffer = sBuffer;
    ASSERT_EQ(OH_NativeWindow_NativeObjectUnreference(reinterpret_cast<void *>(buffer)), OHOS::GSERROR_OK);
    delete buffer;
}

/*
* Function: OH_NativeWindow_DestroyNativeWindow
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call OH_NativeWindow_DestroyNativeWindow by abnormal input
*                  2. check ret
 */
TEST_F(NativeWindowTest, DestroyNativeWindow001)
{
    OH_NativeWindow_DestroyNativeWindow(nullptr);
}

/*
* Function: OH_NativeWindow_DestroyNativeWindow
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call OH_NativeWindow_DestroyNativeWindow
*                  2. check ret
 */
TEST_F(NativeWindowTest, DestroyNativeWindow002)
{
    OH_NativeWindow_DestroyNativeWindow(nativeWindow);
}

/*
* Function: OH_NativeWindow_DestroyNativeWindowBuffer
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call OH_NativeWindow_DestroyNativeWindowBuffer by abnormal input
*                  2. check ret
 */
TEST_F(NativeWindowTest, OH_NativeWindow_DestroyNativeWindowBuffer001)
{
    OH_NativeWindow_DestroyNativeWindowBuffer(nullptr);
}

/*
* Function: OH_NativeWindow_DestroyNativeWindowBuffer
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call OH_NativeWindow_DestroyNativeWindowBuffer again
*                  2. check ret
 */
TEST_F(NativeWindowTest, OH_NativeWindow_DestroyNativeWindowBuffer002)
{
    OH_NativeWindow_DestroyNativeWindowBuffer(nativeWindowBuffer);
}

/*
* Function: OH_NativeWindow_NativeWindowSetScalingMode
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call OH_NativeWindow_NativeWindowSetScalingMode with abnormal parameters and check ret
 */
TEST_F(NativeWindowTest, SetScalingMode001)
{
    OHScalingMode scalingMode = OHScalingMode::OH_SCALING_MODE_SCALE_TO_WINDOW;
    ASSERT_EQ(OH_NativeWindow_NativeWindowSetScalingMode(nullptr, -1, scalingMode), OHOS::GSERROR_INVALID_ARGUMENTS);
}

/*
* Function: OH_NativeWindow_NativeWindowSetScalingMode
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call OH_NativeWindow_NativeWindowSetScalingMode with abnormal parameters and check ret
 */
TEST_F(NativeWindowTest, SetScalingMode002)
{
    OHScalingMode scalingMode = OHScalingMode::OH_SCALING_MODE_SCALE_TO_WINDOW;
    ASSERT_EQ(OH_NativeWindow_NativeWindowSetScalingMode(nativeWindow, -1, scalingMode), OHOS::GSERROR_NO_ENTRY);
}

/*
* Function: OH_NativeWindow_NativeWindowSetScalingMode
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call OH_NativeWindow_NativeWindowSetScalingMode with abnormal parameters and check ret
 */
TEST_F(NativeWindowTest, SetScalingMode003)
{
    int32_t sequence = 0;
    ASSERT_EQ(OH_NativeWindow_NativeWindowSetScalingMode(nativeWindow, sequence,
                                         static_cast<OHScalingMode>(OHScalingMode::OH_SCALING_MODE_NO_SCALE_CROP + 1)),
                                         OHOS::GSERROR_INVALID_ARGUMENTS);
}

/*
* Function: OH_NativeWindow_NativeWindowSetScalingMode
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. call OH_NativeWindow_NativeWindowSetScalingMode with abnormal parameters and check ret
*                  2. call OH_NativeWindow_NativeWindowSetScalingMode with normal parameters and check ret
 */
TEST_F(NativeWindowTest, SetScalingMode004)
{
    int32_t sequence = 0;
    OHScalingMode scalingMode = OHScalingMode::OH_SCALING_MODE_SCALE_TO_WINDOW;
    ASSERT_EQ(OH_NativeWindow_NativeWindowSetScalingMode(nativeWindow, sequence, scalingMode), OHOS::GSERROR_OK);
}

/*
* Function: OH_NativeWindow_NativeWindowSetMetaData
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call OH_NativeWindow_NativeWindowSetMetaData with abnormal parameters and check ret
 */
TEST_F(NativeWindowTest, SetMetaData001)
{
    ASSERT_EQ(OH_NativeWindow_NativeWindowSetMetaData(nullptr, -1, 0, nullptr), OHOS::GSERROR_INVALID_ARGUMENTS);
}

/*
* Function: OH_NativeWindow_NativeWindowSetMetaData
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call OH_NativeWindow_NativeWindowSetMetaData with abnormal parameters and check ret
 */
TEST_F(NativeWindowTest, SetMetaData002)
{
    ASSERT_EQ(OH_NativeWindow_NativeWindowSetMetaData(nativeWindow, -1, 0, nullptr), OHOS::GSERROR_INVALID_ARGUMENTS);
}

/*
* Function: OH_NativeWindow_NativeWindowSetMetaData
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call OH_NativeWindow_NativeWindowSetMetaData with abnormal parameters and check ret
*                  2. call OH_NativeWindow_NativeWindowSetMetaData with normal parameters and check ret
 */
TEST_F(NativeWindowTest, SetMetaData003)
{
    int32_t sequence = 0;
    ASSERT_EQ(OH_NativeWindow_NativeWindowSetMetaData(nativeWindow, sequence, 0, nullptr),
              OHOS::GSERROR_INVALID_ARGUMENTS);
}

/*
* Function: OH_NativeWindow_NativeWindowSetMetaData
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call OH_NativeWindow_NativeWindowSetMetaData with abnormal parameters and check ret
 */
TEST_F(NativeWindowTest, SetMetaData004)
{
    int32_t sequence = 0;
    int32_t size = 1;
    ASSERT_EQ(OH_NativeWindow_NativeWindowSetMetaData(nativeWindow, sequence, size, nullptr),
              OHOS::GSERROR_INVALID_ARGUMENTS);
}

/*
* Function: OH_NativeWindow_NativeWindowSetMetaData
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call OH_NativeWindow_NativeWindowSetMetaData with abnormal parameters and check ret
 */
TEST_F(NativeWindowTest, SetMetaData005)
{
    int32_t size = 1;
    const OHHDRMetaData metaData[] = {{OH_METAKEY_RED_PRIMARY_X, 0}};
    ASSERT_EQ(OH_NativeWindow_NativeWindowSetMetaData(nativeWindow, -1, size, metaData), OHOS::GSERROR_NO_ENTRY);
}

/*
* Function: OH_NativeWindow_NativeWindowSetMetaData
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. call OH_NativeWindow_NativeWindowSetMetaData with normal parameters and check ret
 */
TEST_F(NativeWindowTest, SetMetaData006)
{
    int32_t sequence = 0;
    int32_t size = 1;
    const OHHDRMetaData metaData[] = {{OH_METAKEY_RED_PRIMARY_X, 0}};
    ASSERT_EQ(OH_NativeWindow_NativeWindowSetMetaData(nativeWindow, sequence, size, metaData), OHOS::GSERROR_OK);
}

/*
* Function: OH_NativeWindow_NativeWindowSetMetaDataSet
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call OH_NativeWindow_NativeWindowSetMetaDataSet with abnormal parameters and check ret
 */
TEST_F(NativeWindowTest, SetMetaDataSet001)
{
    OHHDRMetadataKey key = OHHDRMetadataKey::OH_METAKEY_HDR10_PLUS;
    ASSERT_EQ(OH_NativeWindow_NativeWindowSetMetaDataSet(nullptr, -1, key, 0, nullptr),
              OHOS::GSERROR_INVALID_ARGUMENTS);
}

/*
* Function: OH_NativeWindow_NativeWindowSetMetaDataSet
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call OH_NativeWindow_NativeWindowSetMetaDataSet with abnormal parameters and check ret
 */
TEST_F(NativeWindowTest, SetMetaDataSet002)
{
    OHHDRMetadataKey key = OHHDRMetadataKey::OH_METAKEY_HDR10_PLUS;
    ASSERT_EQ(OH_NativeWindow_NativeWindowSetMetaDataSet(nativeWindow, -1, key, 0, nullptr),
              OHOS::GSERROR_INVALID_ARGUMENTS);
}

/*
* Function: OH_NativeWindow_NativeWindowSetMetaDataSet
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call OH_NativeWindow_NativeWindowSetMetaDataSet with abnormal parameters and check ret
 */
TEST_F(NativeWindowTest, SetMetaDataSet003)
{
    int32_t sequence = 0;
    OHHDRMetadataKey key = OHHDRMetadataKey::OH_METAKEY_HDR10_PLUS;
    ASSERT_EQ(OH_NativeWindow_NativeWindowSetMetaDataSet(nativeWindow, sequence, key, 0, nullptr),
              OHOS::GSERROR_INVALID_ARGUMENTS);
}

/*
* Function: OH_NativeWindow_NativeWindowSetMetaDataSet
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call OH_NativeWindow_NativeWindowSetMetaDataSet with abnormal parameters and check ret
 */
TEST_F(NativeWindowTest, SetMetaDataSet004)
{
    int32_t sequence = 0;
    int32_t size = 1;
    OHHDRMetadataKey key = OHHDRMetadataKey::OH_METAKEY_HDR10_PLUS;
    ASSERT_EQ(OH_NativeWindow_NativeWindowSetMetaDataSet(nativeWindow, sequence, key, size, nullptr),
              OHOS::GSERROR_INVALID_ARGUMENTS);
}

/*
* Function: OH_NativeWindow_NativeWindowSetMetaDataSet
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call OH_NativeWindow_NativeWindowSetMetaDataSet with abnormal parameters and check ret
 */
TEST_F(NativeWindowTest, SetMetaDataSet005)
{
    int32_t size = 1;
    OHHDRMetadataKey key = OHHDRMetadataKey::OH_METAKEY_HDR10_PLUS;
    const uint8_t metaData[] = {0};
    ASSERT_EQ(OH_NativeWindow_NativeWindowSetMetaDataSet(nativeWindow, -1, key, size, metaData),
              OHOS::GSERROR_NO_ENTRY);
}

/*
* Function: OH_NativeWindow_NativeWindowSetMetaDataSet
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. call OH_NativeWindow_NativeWindowSetMetaDataSet with normal parameters and check ret
 */
TEST_F(NativeWindowTest, SetMetaDataSet006)
{
    int32_t sequence = 0;
    int32_t size = 1;
    OHHDRMetadataKey key = OHHDRMetadataKey::OH_METAKEY_HDR10_PLUS;
    const uint8_t metaData[] = {0};
    ASSERT_EQ(OH_NativeWindow_NativeWindowSetMetaDataSet(nativeWindow, sequence, key, size, metaData),
              OHOS::GSERROR_OK);
}

/*
* Function: OH_NativeWindow_NativeWindowSetTunnelHandle
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call OH_NativeWindow_NativeWindowSetTunnelHandle with abnormal parameters and check ret
 */
TEST_F(NativeWindowTest, SetTunnelHandle001)
{
    ASSERT_EQ(OH_NativeWindow_NativeWindowSetTunnelHandle(nullptr, nullptr), OHOS::GSERROR_INVALID_ARGUMENTS);
}

/*
* Function: OH_NativeWindow_NativeWindowSetTunnelHandle
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call OH_NativeWindow_NativeWindowSetTunnelHandle with abnormal parameters and check ret
 */
TEST_F(NativeWindowTest, SetTunnelHandle002)
{
    ASSERT_EQ(OH_NativeWindow_NativeWindowSetTunnelHandle(nativeWindow, nullptr), OHOS::GSERROR_INVALID_ARGUMENTS);
}

/*
* Function: OH_NativeWindow_NativeWindowSetTunnelHandle
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call OH_NativeWindow_NativeWindowSetTunnelHandle with normal parameters and check ret
 */
TEST_F(NativeWindowTest, SetTunnelHandle003)
{
    uint32_t reserveInts = 1;
    OHExtDataHandle *handle = AllocOHExtDataHandle(reserveInts);
    ASSERT_EQ(OH_NativeWindow_NativeWindowSetTunnelHandle(nativeWindow, handle), OHOS::GSERROR_OK);
    FreeOHExtDataHandle(handle);
}

/*
* Function: OH_NativeWindow_NativeWindowSetTunnelHandle
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. call OH_NativeWindow_NativeWindowSetTunnelHandle with normal parameters and check ret
* @tc.require: issueI5GMZN issueI5IWHW
 */
TEST_F(NativeWindowTest, SetTunnelHandle004)
{
    uint32_t reserveInts = 2;
    OHExtDataHandle *handle = AllocOHExtDataHandle(reserveInts);
    nativeWindow = OH_NativeWindow_CreateNativeWindow(&pSurface);
    ASSERT_NE(nativeWindow, nullptr);
    ASSERT_EQ(OH_NativeWindow_NativeWindowSetTunnelHandle(nativeWindow, handle), OHOS::GSERROR_OK);
    ASSERT_EQ(OH_NativeWindow_NativeWindowSetTunnelHandle(nativeWindow, handle), OHOS::GSERROR_NO_ENTRY);
    FreeOHExtDataHandle(handle);
}
}

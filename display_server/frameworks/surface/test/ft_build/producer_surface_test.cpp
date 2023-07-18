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
#include <securec.h>
#include <gtest/gtest.h>
#include <surface.h>
#include <consumer_surface.h>
#include "buffer_consumer_listener.h"

namespace OHOS::Rosen {
class ProducerSurfaceTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();

    static inline BufferRequestConfig requestConfig = {
        .width = 0x100,
        .height = 0x100,
        .strideAlignment = 0x8,
        .format = GRAPHIC_PIXEL_FMT_RGBA_8888,
#ifdef RS_ENABLE_GL
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_MEM_DMA,
#else
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE,
#endif
        .timeout = 0,
    };
    static inline BufferFlushConfig flushConfig = {
        .damage = {
            .w = 0x100,
            .h = 0x100,
        },
    };
    static inline int64_t timestamp = 0;
    static inline Rect damage = {};
    static inline sptr<Surface> csurf = nullptr;
    static inline sptr<IBufferProducer> producer = nullptr;
    static inline sptr<Surface> pSurface = nullptr;
};

void ProducerSurfaceTest::SetUpTestCase()
{
    csurf = Surface::CreateSurfaceAsConsumer();
    sptr<IBufferConsumerListener> listener = new BufferConsumerListener();
    csurf->RegisterConsumerListener(listener);
    producer = csurf->GetProducer();
    pSurface = Surface::CreateSurfaceAsProducer(producer);
}

void ProducerSurfaceTest::TearDownTestCase()
{
    csurf = nullptr;
    producer = nullptr;
    pSurface = nullptr;
}

/*
* Function: ProducerSurface
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. check pSurface
 */
TEST_F(ProducerSurfaceTest, ProducerSurface001)
{
    ASSERT_NE(pSurface, nullptr);
}

/*
* Function: SetQueueSize and GetQueueSize
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call GetQueueSize and get default value
*                  2. call SetQueueSize
*                  3. call SetQueueSize again with abnormal value
*                  4. call GetQueueSize
*                  5. check ret
 */
TEST_F(ProducerSurfaceTest, QueueSize001)
{
    ASSERT_EQ(pSurface->GetQueueSize(), (uint32_t)SURFACE_DEFAULT_QUEUE_SIZE);
    GSError ret = pSurface->SetQueueSize(2);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

    ret = pSurface->SetQueueSize(SURFACE_MAX_QUEUE_SIZE + 1);
    ASSERT_NE(ret, OHOS::GSERROR_OK);

    ASSERT_EQ(pSurface->GetQueueSize(), 2u);
}

/*
* Function: RequestBuffer and FlushBuffer
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call RequestBuffer
*                  2. call FlushBuffer
*                  3. check ret
 */
TEST_F(ProducerSurfaceTest, ReqCanFluAcqRel001)
{
    sptr<SurfaceBuffer> buffer;

    int releaseFence = -1;
    GSError ret = pSurface->RequestBuffer(buffer, releaseFence, requestConfig);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);
    ASSERT_NE(buffer, nullptr);

    ret = pSurface->FlushBuffer(buffer, -1, flushConfig);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);
}

/*
* Function: RequestBuffer and FlushBuffer
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call RequestBuffer
*                  2. call FlushBuffer 2 times
*                  3. check ret
 */
TEST_F(ProducerSurfaceTest, ReqCanFluAcqRel002)
{
    sptr<SurfaceBuffer> buffer;
    int releaseFence = -1;

    GSError ret = pSurface->RequestBuffer(buffer, releaseFence, requestConfig);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);
    ASSERT_NE(buffer, nullptr);

    ret = pSurface->FlushBuffer(buffer, -1, flushConfig);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

    ret = pSurface->FlushBuffer(buffer, -1, flushConfig);
    ASSERT_NE(ret, OHOS::GSERROR_OK);
}

/*
* Function: AcquireBuffer and ReleaseBuffer
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call AcquireBuffer and ReleaseBuffer many times
*                  2. check ret
 */
TEST_F(ProducerSurfaceTest, ReqCanFluAcqRel003)
{
    sptr<SurfaceBuffer> buffer;
    int32_t flushFence;

    GSError ret = pSurface->AcquireBuffer(buffer, flushFence, timestamp, damage);
    ASSERT_NE(ret, OHOS::GSERROR_OK);

    ret = pSurface->ReleaseBuffer(buffer, -1);
    ASSERT_NE(ret, OHOS::GSERROR_OK);

    ret = csurf->AcquireBuffer(buffer, flushFence, timestamp, damage);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

    ret = csurf->ReleaseBuffer(buffer, -1);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

    ret = csurf->AcquireBuffer(buffer, flushFence, timestamp, damage);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

    ret = csurf->ReleaseBuffer(buffer, -1);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);
}

/*
* Function: RequestBuffer and CancelBuffer
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call RequestBuffer
*                  2. call CancelBuffer
*                  3. check ret
 */
TEST_F(ProducerSurfaceTest, ReqCanFluAcqRel004)
{
    sptr<SurfaceBuffer> buffer;

    int releaseFence = -1;
    GSError ret = pSurface->RequestBuffer(buffer, releaseFence, requestConfig);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

    ret = pSurface->CancelBuffer(buffer);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);
}

/*
* Function: RequestBuffer and CancelBuffer
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call RequestBuffer
*                  2. call CancelBuffer 2 times
*                  3. check ret
 */
TEST_F(ProducerSurfaceTest, ReqCanFluAcqRel005)
{
    sptr<SurfaceBuffer> buffer;

    int releaseFence = -1;
    GSError ret = pSurface->RequestBuffer(buffer, releaseFence, requestConfig);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

    ret = pSurface->CancelBuffer(buffer);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

    ret = pSurface->CancelBuffer(buffer);
    ASSERT_NE(ret, OHOS::GSERROR_OK);
}

/*
* Function: RequestBuffer and CancelBuffer
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call RequestBuffer and CancelBuffer many times
*                  2. check ret
 */
TEST_F(ProducerSurfaceTest, ReqCanFluAcqRel006)
{
    sptr<SurfaceBuffer> buffer;
    sptr<SurfaceBuffer> buffer1;
    sptr<SurfaceBuffer> buffer2;
    int releaseFence = -1;

    GSError ret = pSurface->RequestBuffer(buffer, releaseFence, requestConfig);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

    ret = pSurface->RequestBuffer(buffer1, releaseFence, requestConfig);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

    ret = pSurface->RequestBuffer(buffer2, releaseFence, requestConfig);
    ASSERT_NE(ret, OHOS::GSERROR_OK);

    ret = pSurface->CancelBuffer(buffer);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

    ret = pSurface->CancelBuffer(buffer1);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

    ret = pSurface->CancelBuffer(buffer2);
    ASSERT_NE(ret, OHOS::GSERROR_OK);
}

/*
* Function: GetQueueSize and SetQueueSize
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call GetQeueSize
*                  2. call SetQueueSize 2 times
*                  3. check ret
 */
TEST_F(ProducerSurfaceTest, SetQueueSizeDeleting001)
{
    sptr<ConsumerSurface> cs = static_cast<ConsumerSurface*>(csurf.GetRefPtr());
    sptr<BufferQueueProducer> bqp = static_cast<BufferQueueProducer*>(cs->GetProducer().GetRefPtr());
    ASSERT_EQ(bqp->GetQueueSize(), 2u);

    GSError ret = pSurface->SetQueueSize(1);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

    ret = pSurface->SetQueueSize(2);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);
}

/*
* Function: RequestBuffer, ReleaseBuffer and CancelBuffer
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call RequestBuffer
*                  2. call ReleaseBuffer
*                  3. call CancelBuffer
*                  4. check ret
 */
TEST_F(ProducerSurfaceTest, ReqCanFluAcqRel007)
{
    sptr<SurfaceBuffer> buffer;

    int releaseFence = -1;
    GSError ret = pSurface->RequestBuffer(buffer, releaseFence, requestConfig);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

    ret = pSurface->ReleaseBuffer(buffer, -1);
    ASSERT_NE(ret, OHOS::GSERROR_OK);

    ret = pSurface->CancelBuffer(buffer);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);
}

/*
* Function: SetUserData and GetUserData
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call SetUserData and GetUserData many times
*                  2. check ret
 */
TEST_F(ProducerSurfaceTest, UserData001)
{
    GSError ret;

    std::string strs[SURFACE_MAX_USER_DATA_COUNT];
    constexpr int32_t stringLengthMax = 32;
    char str[stringLengthMax] = {};
    for (int i = 0; i < SURFACE_MAX_USER_DATA_COUNT; i++) {
        auto secRet = snprintf_s(str, sizeof(str), sizeof(str) - 1, "%d", i);
        ASSERT_GT(secRet, 0);

        strs[i] = str;
        ret = pSurface->SetUserData(strs[i], "magic");
        ASSERT_EQ(ret, OHOS::GSERROR_OK);
    }

    ret = pSurface->SetUserData("-1", "error");
    ASSERT_NE(ret, OHOS::GSERROR_OK);

    std::string retStr;
    for (int i = 0; i < SURFACE_MAX_USER_DATA_COUNT; i++) {
        retStr = pSurface->GetUserData(strs[i]);
        ASSERT_EQ(retStr, "magic");
    }
}

/*
* Function: RegisterConsumerListener
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call RegisterConsumerListener
*                  2. check ret
 */
TEST_F(ProducerSurfaceTest, RegisterConsumerListener001)
{
    sptr<IBufferConsumerListener> listener = new BufferConsumerListener();
    GSError ret = pSurface->RegisterConsumerListener(listener);
    ASSERT_NE(ret, OHOS::GSERROR_OK);
}

/*
* Function: GetUniqueId
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call GetUniqueId
*                  2. check ret
 */
TEST_F(ProducerSurfaceTest, UniqueId001)
{
    uint64_t uniqueId = pSurface->GetUniqueId();
    ASSERT_NE(uniqueId, 0);
}

/*
* Function: SetTransform and GetTransform
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call GetTransform with default and check ret
 */
TEST_F(ProducerSurfaceTest, transform001)
{
    ASSERT_EQ(pSurface->GetTransform(), GraphicTransformType::GRAPHIC_ROTATE_BUTT);
    GSError ret = pSurface->SetTransform(GraphicTransformType::GRAPHIC_ROTATE_NONE);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);
}

/*
* Function: SetTransform and GetTransform
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. call SetTransform with other parameters and check ret
 */
TEST_F(ProducerSurfaceTest, transform002)
{
    GSError ret = pSurface->SetTransform(GraphicTransformType::GRAPHIC_ROTATE_90);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);
}

/*
* Function: SetTransform and GetTransform
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. call SetTransform with other parameters and check ret
 */
TEST_F(ProducerSurfaceTest, transform003)
{
    GSError ret = pSurface->SetTransform(GraphicTransformType::GRAPHIC_ROTATE_180);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);
}

/*
* Function: SetTransform and GetTransform
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. call SetTransform with other parameters and check ret
 */
TEST_F(ProducerSurfaceTest, transform004)
{
    GSError ret = pSurface->SetTransform(GraphicTransformType::GRAPHIC_ROTATE_270);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);
}

/*
* Function: IsSupportedAlloc
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call IsSupportedAlloc with abnormal parameters and check ret
 */
TEST_F(ProducerSurfaceTest, isSupportedAlloc001)
{
    std::vector<BufferVerifyAllocInfo> infos;
    std::vector<bool> supporteds;
    GSError ret = pSurface->IsSupportedAlloc(infos, supporteds);
    ASSERT_EQ(ret, OHOS::GSERROR_INVALID_ARGUMENTS);
}

/*
* Function: IsSupportedAlloc
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call IsSupportedAlloc with abnormal parameters and check ret
 */
TEST_F(ProducerSurfaceTest, isSupportedAlloc002)
{
    std::vector<BufferVerifyAllocInfo> infos;
    std::vector<bool> supporteds;
    GSError ret = pSurface->IsSupportedAlloc(infos, supporteds);
    ASSERT_EQ(ret, OHOS::GSERROR_INVALID_ARGUMENTS);

    BufferVerifyAllocInfo info = {
        .width = 0x100,
        .height = 0x100,
#ifdef RS_ENABLE_GL
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_MEM_DMA,
#else
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE,
#endif
        .format = GRAPHIC_PIXEL_FMT_RGBA_8888,
    };
    infos.push_back(info);
    info.format = GRAPHIC_PIXEL_FMT_YCRCB_420_SP;
    infos.push_back(info);
    info.format = GRAPHIC_PIXEL_FMT_YUV_422_I;
    infos.push_back(info);

    ret = pSurface->IsSupportedAlloc(infos, supporteds);
    ASSERT_EQ(ret, OHOS::GSERROR_INVALID_ARGUMENTS);
}

/*
* Function: IsSupportedAlloc
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. call IsSupportedAlloc with normal parameters and check ret
 */
TEST_F(ProducerSurfaceTest, isSupportedAlloc003)
{
    std::vector<BufferVerifyAllocInfo> infos;
    std::vector<bool> supporteds;
    BufferVerifyAllocInfo info = {
        .width = 0x100,
        .height = 0x100,
#ifdef RS_ENABLE_GL
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_MEM_DMA,
#else
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE,
#endif
        .format = GRAPHIC_PIXEL_FMT_RGBA_8888,
    };
    infos.push_back(info);
    info.format = GRAPHIC_PIXEL_FMT_YCRCB_420_SP;
    infos.push_back(info);
    info.format = GRAPHIC_PIXEL_FMT_YUV_422_I;
    infos.push_back(info);

    supporteds.push_back(false);
    supporteds.push_back(false);
    supporteds.push_back(false);

    GSError ret = pSurface->IsSupportedAlloc(infos, supporteds);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);  // mock data result
    ASSERT_EQ(supporteds[0], true);  // mock data result
    ASSERT_EQ(supporteds[1], true);  // mock data result
    ASSERT_EQ(supporteds[2], false);  // mock data result
}

/*
* Function: SetScalingMode and GetScalingMode
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call SetScalingMode with abnormal parameters and check ret
 */
TEST_F(ProducerSurfaceTest, scalingMode001)
{
    ScalingMode scalingMode = ScalingMode::SCALING_MODE_SCALE_TO_WINDOW;
    GSError ret = pSurface->SetScalingMode(-1, scalingMode);
    ASSERT_EQ(ret, OHOS::GSERROR_NO_ENTRY);
}

/*
* Function: SetScalingMode and GetScalingMode
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. call SetScalingMode with normal parameters and check ret
*                  2. call GetScalingMode and check ret
 */
TEST_F(ProducerSurfaceTest, scalingMode002)
{
    ScalingMode scalingMode = ScalingMode::SCALING_MODE_SCALE_TO_WINDOW;
    sptr<SurfaceBuffer> buffer;
    int releaseFence = -1;
    GSError ret = pSurface->RequestBuffer(buffer, releaseFence, requestConfig);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);
    ASSERT_NE(buffer, nullptr);

    uint32_t sequence = buffer->GetSeqNum();
    ret = pSurface->SetScalingMode(sequence, scalingMode);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

    ret = pSurface->GetScalingMode(sequence, scalingMode);
    ASSERT_EQ(ret, OHOS::GSERROR_NOT_SUPPORT);

    ret = pSurface->CancelBuffer(buffer);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);
}

/*
* Function: SetMetaData and GetMetaData
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call SetMetaData with abnormal parameters and check ret
 */
TEST_F(ProducerSurfaceTest, metaData001)
{
    uint32_t sequence = 0;
    std::vector<GraphicHDRMetaData> metaData;
    GSError ret = pSurface->SetMetaData(sequence, metaData);
    ASSERT_EQ(ret, OHOS::GSERROR_INVALID_ARGUMENTS);
}

/*
* Function: SetMetaData and GetMetaData
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call SetMetaData with abnormal parameters and check ret
 */
TEST_F(ProducerSurfaceTest, metaData002)
{
    std::vector<GraphicHDRMetaData> metaData;
    GraphicHDRMetaData data = {
        .key = GraphicHDRMetadataKey::GRAPHIC_MATAKEY_RED_PRIMARY_X,
        .value = 100,  // for test
    };
    metaData.push_back(data);
    GSError ret = pSurface->SetMetaData(-1, metaData);
    ASSERT_EQ(ret, OHOS::GSERROR_NO_ENTRY);
}

/*
* Function: SetMetaData and GetMetaData
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. call SetMetaData with normal parameters and check ret
*                  2. call GetMetaData and check ret
 */
TEST_F(ProducerSurfaceTest, metaData003)
{
    uint32_t sequence = 0;
    std::vector<GraphicHDRMetaData> metaData;
    GraphicHDRMetaData data = {
        .key = GraphicHDRMetadataKey::GRAPHIC_MATAKEY_RED_PRIMARY_X,
        .value = 100,  // for test
    };
    metaData.push_back(data);
    sptr<SurfaceBuffer> buffer;
    int releaseFence = -1;
    GSError ret = pSurface->RequestBuffer(buffer, releaseFence, requestConfig);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);
    ASSERT_NE(buffer, nullptr);

    sequence = buffer->GetSeqNum();
    ret = pSurface->SetMetaData(sequence, metaData);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

    ret = pSurface->GetMetaData(sequence, metaData);
    ASSERT_EQ(ret, OHOS::GSERROR_NOT_SUPPORT);

    ret = pSurface->CancelBuffer(buffer);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);
}

/*
* Function: SetMetaDataSet and GetMetaDataSet
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call SetMetaDataSet with abnormal parameters and check ret
 */
TEST_F(ProducerSurfaceTest, metaDataSet001)
{
    GraphicHDRMetadataKey key = GraphicHDRMetadataKey::GRAPHIC_MATAKEY_HDR10_PLUS;
    std::vector<uint8_t> metaData;

    uint32_t sequence = 0;
    GSError ret = pSurface->SetMetaDataSet(sequence, key, metaData);
    ASSERT_EQ(ret, OHOS::GSERROR_INVALID_ARGUMENTS);
}

/*
* Function: SetMetaDataSet and GetMetaDataSet
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call SetMetaDataSet with abnormal parameters and check ret
 */
TEST_F(ProducerSurfaceTest, metaDataSet002)
{
    GraphicHDRMetadataKey key = GraphicHDRMetadataKey::GRAPHIC_MATAKEY_HDR10_PLUS;
    std::vector<uint8_t> metaData;

    uint8_t data = 10;  // for test
    metaData.push_back(data);
    GSError ret = pSurface->SetMetaDataSet(-1, key, metaData);
    ASSERT_EQ(ret, OHOS::GSERROR_NO_ENTRY);
}

/*
* Function: SetMetaDataSet and GetMetaDataSet
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. call SetMetaDataSet with normal parameters and check ret
*                  2. call GetMetaDataSet and check ret
 */
TEST_F(ProducerSurfaceTest, metaDataSet003)
{
    GraphicHDRMetadataKey key = GraphicHDRMetadataKey::GRAPHIC_MATAKEY_HDR10_PLUS;
    std::vector<uint8_t> metaData;
    uint32_t sequence = 0;
    uint8_t data = 10;  // for test
    metaData.push_back(data);

    sptr<SurfaceBuffer> buffer;
    int releaseFence = -1;
    GSError ret = pSurface->RequestBuffer(buffer, releaseFence, requestConfig);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);
    ASSERT_NE(buffer, nullptr);

    sequence = buffer->GetSeqNum();
    ret = pSurface->SetMetaDataSet(sequence, key, metaData);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

    ret = pSurface->GetMetaDataSet(sequence, key, metaData);
    ASSERT_EQ(ret, OHOS::GSERROR_NOT_SUPPORT);

    ret = pSurface->CancelBuffer(buffer);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);
}

/*
* Function: QueryMetaDataType
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. call QueryMetaDataType and check ret
 */
TEST_F(ProducerSurfaceTest, QueryMetaDataType001)
{
    uint32_t sequence = 0;
    HDRMetaDataType type = HDRMetaDataType::HDR_NOT_USED;
    GSError ret = pSurface->QueryMetaDataType(sequence, type);
    ASSERT_EQ(ret, OHOS::GSERROR_NOT_SUPPORT);
}

/*
* Function: SetTunnelHandle and GetTunnelHandle
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call GetTunnelhandle and check ret
* @tc.require: issueI5GMZN issueI5IWHW
 */
TEST_F(ProducerSurfaceTest, tunnelHandle001)
{
    ASSERT_EQ(pSurface->GetTunnelHandle(), nullptr);
}

/*
* Function: SetTunnelHandle and GetTunnelHandle
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call SetTunnelhandle with normal parameters and check ret
 */
TEST_F(ProducerSurfaceTest, tunnelHandle002)
{
    GraphicExtDataHandle *handle = nullptr;
    handle = static_cast<GraphicExtDataHandle *>(malloc(sizeof(GraphicExtDataHandle) + sizeof(int32_t) * 1));
    handle->fd = -1;
    handle->reserveInts = 1;
    handle->reserve[0] = 0;
    GSError ret = pSurface->SetTunnelHandle(handle);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

    ret = pSurface->SetTunnelHandle(handle);
    ASSERT_EQ(ret, OHOS::GSERROR_NO_ENTRY);
    free(handle);
}

/*
* Function: disconnect
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. call Disconnect and check ret
 */
TEST_F(ProducerSurfaceTest, disconnect001)
{
    GSError ret = pSurface->Disconnect();
    ASSERT_EQ(ret, OHOS::GSERROR_OK);
}

/*
* Function: SetPresentTimestamp and GetPresentTimestamp
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call SetPresentTimestamp and check ret
* @tc.require: issueI5I57K
 */
TEST_F(ProducerSurfaceTest, presentTimestamp001)
{
    uint32_t sequence = 0;
    GraphicPresentTimestamp timestamp = {GRAPHIC_DISPLAY_PTS_UNSUPPORTED, 0};

    GSError ret = pSurface->SetPresentTimestamp(sequence, timestamp);
    ASSERT_EQ(ret, OHOS::GSERROR_NOT_SUPPORT);
}

/*
* Function: SetPresentTimestamp and GetPresentTimestamp
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call GetPresentTimestamp with normal parameters and check ret
* @tc.require: issueI5I57K
 */
TEST_F(ProducerSurfaceTest, presentTimestamp002)
{
    uint32_t sequence = 0;
    GraphicPresentTimestampType type = GraphicPresentTimestampType::GRAPHIC_DISPLAY_PTS_UNSUPPORTED;
    int64_t time = 0;

    GSError ret = pSurface->GetPresentTimestamp(sequence, type, time);
    ASSERT_EQ(ret, OHOS::GSERROR_INVALID_ARGUMENTS);
}

/*
* Function: SetPresentTimestamp and GetPresentTimestamp
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call SetPresentTimestamp with normal parameters and check ret
* @tc.require: issueI5I57K
 */
TEST_F(ProducerSurfaceTest, presentTimestamp003)
{
    GraphicPresentTimestampType type = GraphicPresentTimestampType::GRAPHIC_DISPLAY_PTS_DELAY;
    int64_t time = 0;
    GSError ret = pSurface->GetPresentTimestamp(-1, type, time);
    ASSERT_EQ(ret, OHOS::GSERROR_NO_ENTRY);
}

/*
* Function: SetPresentTimestamp and GetPresentTimestamp
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. call SetPresentTimestamp and check ret
* @tc.require: issueI5I57K
 */
TEST_F(ProducerSurfaceTest, presentTimestamp004)
{
    sptr<SurfaceBuffer> buffer;
    int releaseFence = -1;
    GSError ret = pSurface->RequestBuffer(buffer, releaseFence, requestConfig);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);
    ASSERT_NE(buffer, nullptr);

    uint32_t sequence = buffer->GetSeqNum();
    GraphicPresentTimestampType type = GraphicPresentTimestampType::GRAPHIC_DISPLAY_PTS_DELAY;
    int64_t time = 0;
    ret = pSurface->GetPresentTimestamp(sequence, type, time);
    ASSERT_EQ(ret, OHOS::GSERROR_NO_ENTRY);

    ret = pSurface->CancelBuffer(buffer);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);
}
}

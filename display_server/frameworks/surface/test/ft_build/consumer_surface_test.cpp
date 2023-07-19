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
#include <buffer_queue_producer.h>
#include <consumer_surface.h>
#include "buffer_consumer_listener.h"

namespace OHOS::Rosen {
class ConsumerSurfaceTest : public testing::Test {
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
    static inline sptr<Surface> cs = nullptr;
    static inline sptr<Surface> ps = nullptr;
};

void ConsumerSurfaceTest::SetUpTestCase()
{
    cs = Surface::CreateSurfaceAsConsumer();
    sptr<IBufferConsumerListener> listener = new BufferConsumerListener();
    cs->RegisterConsumerListener(listener);
    auto p = cs->GetProducer();
    ps = Surface::CreateSurfaceAsProducer(p);
}

void ConsumerSurfaceTest::TearDownTestCase()
{
    cs = nullptr;
}

/*
* Function: GetProducer
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. get ConsumerSurface and GetProducer
*                  2. check ret
 */
TEST_F(ConsumerSurfaceTest, ConsumerSurface001)
{
    ASSERT_NE(cs, nullptr);

    sptr<ConsumerSurface> qs = static_cast<ConsumerSurface*>(cs.GetRefPtr());
    ASSERT_NE(qs, nullptr);
    ASSERT_NE(qs->GetProducer(), nullptr);
}

/*
* Function: SetQueueSize and GetQueueSize
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call GetQueueSize and get default value
*                  2. call SetQueueSize
*                  3. call SetQueueSize again with abnormal value
*                  4. call GetQueueSize for BufferQueueProducer and BufferQueue
*                  5. check ret
 */
TEST_F(ConsumerSurfaceTest, QueueSize001)
{
    ASSERT_EQ(cs->GetQueueSize(), (uint32_t)SURFACE_DEFAULT_QUEUE_SIZE);
    GSError ret = cs->SetQueueSize(2);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

    ret = cs->SetQueueSize(SURFACE_MAX_QUEUE_SIZE + 1);
    ASSERT_NE(ret, OHOS::GSERROR_OK);

    ASSERT_EQ(cs->GetQueueSize(), 2u);
}

/*
* Function: SetQueueSize and GetQueueSize
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call GetQueueSize
*                  2. call SetQueueSize 2 times
*                  3. check ret
 */
TEST_F(ConsumerSurfaceTest, QueueSize002)
{
    sptr<ConsumerSurface> qs = static_cast<ConsumerSurface*>(cs.GetRefPtr());
    sptr<BufferQueueProducer> bqp = static_cast<BufferQueueProducer*>(qs->GetProducer().GetRefPtr());
    ASSERT_EQ(bqp->GetQueueSize(), 2u);

    GSError ret = cs->SetQueueSize(1);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

    ret = cs->SetQueueSize(2);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);
}

/*
* Function: RequestBuffer and FlushBuffer
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call RequestBuffer by cs and ps
*                  2. call FlushBuffer both
*                  3. check ret
 */
TEST_F(ConsumerSurfaceTest, ReqCanFluAcqRel001)
{
    sptr<SurfaceBuffer> buffer;
    int releaseFence = -1;
    GSError ret = cs->RequestBuffer(buffer, releaseFence, requestConfig);
    ASSERT_NE(ret, OHOS::GSERROR_OK);

    ret = ps->RequestBuffer(buffer, releaseFence, requestConfig);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);
    ASSERT_NE(buffer, nullptr);

    ret = cs->FlushBuffer(buffer, -1, flushConfig);
    ASSERT_NE(ret, OHOS::GSERROR_OK);

    ret = ps->FlushBuffer(buffer, -1, flushConfig);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

    ret = ps->RequestBuffer(buffer, releaseFence, requestConfig);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);
    ret = ps->FlushBuffer(buffer, -1, flushConfig);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);
}

/*
* Function: AcquireBuffer and ReleaseBuffer
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call AcquireBuffer
*                  2. call ReleaseBuffer
*                  3. check ret
 */
TEST_F(ConsumerSurfaceTest, ReqCanFluAcqRel002)
{
    sptr<SurfaceBuffer> buffer;
    int32_t flushFence;

    GSError ret = cs->AcquireBuffer(buffer, flushFence, timestamp, damage);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);
    ASSERT_NE(buffer, nullptr);

    ret = cs->ReleaseBuffer(buffer, -1);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);
}

/*
* Function: AcquireBuffer and ReleaseBuffer
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call AcquireBuffer
*                  2. call ReleaseBuffer 2 times
*                  3. check ret
 */
TEST_F(ConsumerSurfaceTest, ReqCanFluAcqRel003)
{
    sptr<SurfaceBuffer> buffer;
    int32_t flushFence;

    GSError ret = cs->AcquireBuffer(buffer, flushFence, timestamp, damage);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);
    ASSERT_NE(buffer, nullptr);

    ret = cs->ReleaseBuffer(buffer, -1);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

    ret = cs->ReleaseBuffer(buffer, -1);
    ASSERT_NE(ret, OHOS::GSERROR_OK);
}

/*
* Function: RequestBuffer and CancelBuffer
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call RequestBuffer by cs and ps
*                  2. call CancelBuffer both
*                  3. check ret
 */
TEST_F(ConsumerSurfaceTest, ReqCanFluAcqRel004)
{
    sptr<SurfaceBuffer> buffer;
    int releaseFence = -1;
    GSError ret = cs->RequestBuffer(buffer, releaseFence, requestConfig);
    ASSERT_NE(ret, OHOS::GSERROR_OK);

    ret = ps->RequestBuffer(buffer, releaseFence, requestConfig);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

    ret = cs->CancelBuffer(buffer);
    ASSERT_NE(ret, OHOS::GSERROR_OK);

    ret = ps->CancelBuffer(buffer);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);
}

/*
* Function: SetUserData
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call SetUserData many times
*                  2. check ret
 */
TEST_F(ConsumerSurfaceTest, UserData001)
{
    GSError ret;

    std::string strs[SURFACE_MAX_USER_DATA_COUNT];
    constexpr int32_t stringLengthMax = 32;
    char str[stringLengthMax] = {};
    for (int i = 0; i < SURFACE_MAX_USER_DATA_COUNT; i++) {
        auto secRet = snprintf_s(str, sizeof(str), sizeof(str) - 1, "%d", i);
        ASSERT_GT(secRet, 0);

        strs[i] = str;
        ret = cs->SetUserData(strs[i], "magic");
        ASSERT_EQ(ret, OHOS::GSERROR_OK);
    }

    ret = cs->SetUserData("-1", "error");
    ASSERT_NE(ret, OHOS::GSERROR_OK);

    std::string retStr;
    for (int i = 0; i < SURFACE_MAX_USER_DATA_COUNT; i++) {
        retStr = cs->GetUserData(strs[i]);
        ASSERT_EQ(retStr, "magic");
    }
}

/*
* Function: SetUserData
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call SetUserData many times
*                  2. check ret
 */
TEST_F(ConsumerSurfaceTest, RegisterConsumerListener001)
{
    class TestConsumerListener : public IBufferConsumerListener {
    public:
        void OnBufferAvailable() override
        {
            sptr<SurfaceBuffer> buffer;
            int32_t flushFence;

            cs->AcquireBuffer(buffer, flushFence, timestamp, damage);
            int32_t *p = (int32_t*)buffer->GetVirAddr();
            if (p != nullptr) {
                for (int32_t i = 0; i < 128; i++) {
                    ASSERT_EQ(p[i], i);
                }
            }

            cs->ReleaseBuffer(buffer, -1);
        }
    };
    sptr<IBufferConsumerListener> listener = new TestConsumerListener();
    GSError ret = cs->RegisterConsumerListener(listener);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

    sptr<SurfaceBuffer> buffer;
    int releaseFence = -1;
    ret = ps->RequestBuffer(buffer, releaseFence, requestConfig);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);
    ASSERT_NE(buffer, nullptr);

    int32_t *p = (int32_t*)buffer->GetVirAddr();
    if (p != nullptr) {
        for (int32_t i = 0; i < 128; i++) {
            p[i] = i;
        }
    }

    ret = ps->FlushBuffer(buffer, -1, flushConfig);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);
}

/*
* Function: RegisterConsumerListener, RequestBuffer and FlushBuffer
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call RegisterConsumerListener
*                  2. call RequestBuffer
*                  3. call FlushBuffer
*                  4. check ret
 */
TEST_F(ConsumerSurfaceTest, RegisterConsumerListener002)
{
    sptr<IBufferConsumerListener> listener = new BufferConsumerListener();
    GSError ret = cs->RegisterConsumerListener(listener);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

    sptr<SurfaceBuffer> buffer;
    int releaseFence = -1;
    ret = ps->RequestBuffer(buffer, releaseFence, requestConfig);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);
    ASSERT_NE(buffer, nullptr);

    int32_t *p = (int32_t*)buffer->GetVirAddr();
    if (p != nullptr) {
        for (int32_t i = 0; i < requestConfig.width * requestConfig.height; i++) {
            p[i] = i;
        }
    }

    ret = ps->FlushBuffer(buffer, -1, flushConfig);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);
}

/*
* Function: SetTransform and GetTransform
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call GetTransform by default
 */
TEST_F(ConsumerSurfaceTest, transform001)
{
    ASSERT_EQ(cs->GetTransform(), GraphicTransformType::GRAPHIC_ROTATE_NONE);
}

/*
* Function: SetTransform and GetTransform
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. call SetTransform with different paramaters and call GetTransform
*                  2. check ret
 */
TEST_F(ConsumerSurfaceTest, transform002)
{
    GraphicTransformType transform = GraphicTransformType::GRAPHIC_ROTATE_90;
    GSError ret = ps->SetTransform(transform);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);
    ASSERT_EQ(cs->GetTransform(), GraphicTransformType::GRAPHIC_ROTATE_90);
}

/*
* Function: SetTransform and GetTransform
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. call SetTransform with different paramaters and call GetTransform
*                  2. check ret
 */
TEST_F(ConsumerSurfaceTest, transform003)
{
    GraphicTransformType transform = GraphicTransformType::GRAPHIC_ROTATE_180;
    GSError ret = ps->SetTransform(transform);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);
    ASSERT_EQ(cs->GetTransform(), GraphicTransformType::GRAPHIC_ROTATE_180);
}

/*
* Function: SetTransform and GetTransform
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. call SetTransform with different paramaters and call GetTransform
*                  2. check ret
 */
TEST_F(ConsumerSurfaceTest, transform004)
{
    GraphicTransformType transform = GraphicTransformType::GRAPHIC_ROTATE_270;
    GSError ret = ps->SetTransform(transform);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);
    ASSERT_EQ(cs->GetTransform(), GraphicTransformType::GRAPHIC_ROTATE_270);
}

/*
* Function: SetTransform and GetTransform
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. call SetTransform with different paramaters and call GetTransform
*                  2. check ret
 */
TEST_F(ConsumerSurfaceTest, transform005)
{
    GraphicTransformType transform = GraphicTransformType::GRAPHIC_ROTATE_NONE;
    GSError ret = ps->SetTransform(transform);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);
    ASSERT_EQ(cs->GetTransform(), GraphicTransformType::GRAPHIC_ROTATE_NONE);
}

/*
* Function: IsSupportedAlloc
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call IsSupportedAlloc
*                  2. check ret
 */
TEST_F(ConsumerSurfaceTest, isSupportedAlloc001)
{
    std::vector<BufferVerifyAllocInfo> infos;
    std::vector<bool> supporteds;
    GSError ret = cs->IsSupportedAlloc(infos, supporteds);
    ASSERT_EQ(ret, OHOS::GSERROR_NOT_SUPPORT);
}

/*
* Function: SetScalingMode and GetScalingMode
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call SetScalingMode with abnormal parameters and check ret
 */
TEST_F(ConsumerSurfaceTest, scalingMode001)
{
    ScalingMode scalingMode = ScalingMode::SCALING_MODE_SCALE_TO_WINDOW;
    GSError ret = cs->SetScalingMode(-1, scalingMode);
    ASSERT_EQ(ret, OHOS::GSERROR_NO_ENTRY);
    ret = cs->GetScalingMode(-1, scalingMode);
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
TEST_F(ConsumerSurfaceTest, scalingMode002)
{
    uint32_t sequence = 0;
    ScalingMode scalingMode = ScalingMode::SCALING_MODE_SCALE_TO_WINDOW;
    sptr<SurfaceBuffer> buffer;
    int releaseFence = -1;
    GSError ret = ps->RequestBuffer(buffer, releaseFence, requestConfig);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);
    ASSERT_NE(buffer, nullptr);

    sequence = buffer->GetSeqNum();
    ret = cs->SetScalingMode(sequence, scalingMode);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

    ScalingMode scalingModeGet = ScalingMode::SCALING_MODE_FREEZE;
    ret = cs->GetScalingMode(sequence, scalingModeGet);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);
    ASSERT_EQ(scalingMode, scalingModeGet);

    ret = ps->CancelBuffer(buffer);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);
}

/*
* Function: QueryMetaDataType
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. call QueryMetaDataType and check ret
 */
TEST_F(ConsumerSurfaceTest, QueryMetaDataType001)
{
    uint32_t sequence = 0;
    HDRMetaDataType type = HDRMetaDataType::HDR_META_DATA;
    GSError ret = cs->QueryMetaDataType(sequence, type);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);
    ASSERT_EQ(type, HDRMetaDataType::HDR_NOT_USED);
}

/*
* Function: QueryMetaDataType
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. call SetMetaData with normal parameters and check ret
*                  2. call QueryMetaDataType and check ret
 */
TEST_F(ConsumerSurfaceTest, QueryMetaDataType002)
{
    uint32_t sequence = 0;
    std::vector<GraphicHDRMetaData> metaData;
    GraphicHDRMetaData data = {
        .key = GraphicHDRMetadataKey::GRAPHIC_MATAKEY_RED_PRIMARY_X,
        .value = 1,
    };
    metaData.push_back(data);
    GSError ret = cs->SetMetaData(sequence, metaData);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

    HDRMetaDataType type = HDRMetaDataType::HDR_NOT_USED;
    ret = cs->QueryMetaDataType(sequence, type);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);
    ASSERT_EQ(type, HDRMetaDataType::HDR_META_DATA);
}

/*
* Function: QueryMetaDataType
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. call SetMetaDataSet with normal parameters and check ret
*                  2. call QueryMetaDataType and check ret
 */
TEST_F(ConsumerSurfaceTest, QueryMetaDataType003)
{
    uint32_t sequence = 0;
    GraphicHDRMetadataKey key = GraphicHDRMetadataKey::GRAPHIC_MATAKEY_HDR10_PLUS;
    std::vector<uint8_t> metaData;
    uint8_t data = 1;
    metaData.push_back(data);
    GSError ret = cs->SetMetaDataSet(sequence, key, metaData);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

    HDRMetaDataType type = HDRMetaDataType::HDR_NOT_USED;
    ret = cs->QueryMetaDataType(sequence, type);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);
    ASSERT_EQ(type, HDRMetaDataType::HDR_META_DATA_SET);
}

/*
* Function: SetMetaData and GetMetaData
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call SetMetaData with abnormal parameters and check ret
 */
TEST_F(ConsumerSurfaceTest, metaData001)
{
    uint32_t sequence = 0;
    std::vector<GraphicHDRMetaData> metaData;
    GSError ret = cs->SetMetaData(sequence, metaData);
    ASSERT_EQ(ret, OHOS::GSERROR_INVALID_ARGUMENTS);
}

/*
* Function: SetMetaData and GetMetaData
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. call SetMetaData with normal parameters and check ret
 */
TEST_F(ConsumerSurfaceTest, metaData002)
{
    uint32_t sequence = 0;
    std::vector<GraphicHDRMetaData> metaData;
    GraphicHDRMetaData data = {
        .key = GraphicHDRMetadataKey::GRAPHIC_MATAKEY_RED_PRIMARY_X,
        .value = 100,  // for test
    };
    metaData.push_back(data);
    GSError ret = cs->SetMetaData(sequence, metaData);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);
}

/*
* Function: SetMetaData and GetMetaData
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. call SetMetaData with normal parameters and check ret
*                  2. call GetMetaData and check ret
 */
TEST_F(ConsumerSurfaceTest, metaData003)
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
    GSError ret = ps->RequestBuffer(buffer, releaseFence, requestConfig);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);
    ASSERT_NE(buffer, nullptr);

    sequence = buffer->GetSeqNum();
    ret = cs->SetMetaData(sequence, metaData);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

    std::vector<GraphicHDRMetaData> metaDataGet;
    ret = cs->GetMetaData(sequence, metaDataGet);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);
    ASSERT_EQ(metaData[0].key, metaDataGet[0].key);
    ASSERT_EQ(metaData[0].value, metaDataGet[0].value);

    ret = cs->GetMetaData(sequence + 1, metaDataGet);
    ASSERT_EQ(ret, OHOS::GSERROR_NO_ENTRY);

    ret = ps->CancelBuffer(buffer);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);
}

/*
* Function: SetMetaDataSet and GetMetaDataSet
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call SetMetaDataSet with abnormal parameters and check ret
 */
TEST_F(ConsumerSurfaceTest, metaDataSet001)
{
    uint32_t sequence = 0;
    GraphicHDRMetadataKey key = GraphicHDRMetadataKey::GRAPHIC_MATAKEY_HDR10_PLUS;
    std::vector<uint8_t> metaData;
    GSError ret = cs->SetMetaDataSet(sequence, key, metaData);
    ASSERT_EQ(ret, OHOS::GSERROR_INVALID_ARGUMENTS);
}

/*
* Function: SetMetaDataSet and GetMetaDataSet
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. call SetMetaDataSet with normal parameters and check ret
 */
TEST_F(ConsumerSurfaceTest, metaDataSet002)
{
    uint32_t sequence = 0;
    GraphicHDRMetadataKey key = GraphicHDRMetadataKey::GRAPHIC_MATAKEY_HDR10_PLUS;
    std::vector<uint8_t> metaData;
    uint8_t data = 10;  // for test
    metaData.push_back(data);
    GSError ret = cs->SetMetaDataSet(sequence, key, metaData);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);
}

/*
* Function: SetMetaDataSet and GetMetaDataSet
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. call SetMetaDataSet with normal parameters and check ret
*                  2. call GetMetaDataSet and check ret
 */
TEST_F(ConsumerSurfaceTest, metaDataSet003)
{
    uint32_t sequence = 0;
    GraphicHDRMetadataKey key = GraphicHDRMetadataKey::GRAPHIC_MATAKEY_HDR10_PLUS;
    std::vector<uint8_t> metaData;
    uint8_t data = 10;  // for test
    metaData.push_back(data);

    sptr<SurfaceBuffer> buffer;
    int releaseFence = -1;
    GSError ret = ps->RequestBuffer(buffer, releaseFence, requestConfig);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);
    ASSERT_NE(buffer, nullptr);

    sequence = buffer->GetSeqNum();
    ret = cs->SetMetaDataSet(sequence, key, metaData);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

    GraphicHDRMetadataKey keyGet = GraphicHDRMetadataKey::GRAPHIC_MATAKEY_RED_PRIMARY_X;
    std::vector<uint8_t> metaDataGet;
    ret = cs->GetMetaDataSet(sequence, keyGet, metaDataGet);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);
    ASSERT_EQ(key, keyGet);
    ASSERT_EQ(metaData[0], metaDataGet[0]);

    ret = cs->GetMetaDataSet(sequence + 1, keyGet, metaDataGet);
    ASSERT_EQ(ret, OHOS::GSERROR_NO_ENTRY);

    ret = ps->CancelBuffer(buffer);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);
}

/*
* Function: SetTunnelHandle and GetTunnelHandle
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call SetTunnelHandle with abnormal parameters and check ret
 */
TEST_F(ConsumerSurfaceTest, TunnelHandle001)
{
    GraphicExtDataHandle *handle = nullptr;
    GSError ret = cs->SetTunnelHandle(handle);
    ASSERT_EQ(ret, OHOS::GSERROR_INVALID_ARGUMENTS);
}

/*
* Function: SetTunnelHandle and GetTunnelHandle
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call SetTunnelHandle with abnormal parameters and check ret
 */
TEST_F(ConsumerSurfaceTest, TunnelHandle002)
{
    GraphicExtDataHandle *handle = nullptr;
    handle = new GraphicExtDataHandle();
    handle->fd = -1;
    handle->reserveInts = 0;
    GSError ret = cs->SetTunnelHandle(handle);
    ASSERT_EQ(ret, OHOS::GSERROR_INVALID_ARGUMENTS);
}

/*
* Function: SetTunnelHandle and GetTunnelHandle
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. call SetTunnelHandle with normal parameters and check ret
*                  2. call GetTunnelHandle and check ret
* @tc.require: issueI5GMZN issueI5IWHW
 */
TEST_F(ConsumerSurfaceTest, TunnelHandle003)
{
    size_t size = sizeof(GraphicExtDataHandle) + sizeof(int32_t) * 1;
    GraphicExtDataHandle *handle = static_cast<GraphicExtDataHandle *>(malloc(size));
    handle->fd = -1;
    handle->reserveInts = 1;
    handle->reserve[0] = 0;
    GSError ret = cs->SetTunnelHandle(handle);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

    ret = cs->SetTunnelHandle(handle);
    ASSERT_EQ(ret, OHOS::GSERROR_NO_ENTRY);

    sptr<SurfaceTunnelHandle> handleGet = nullptr;
    handleGet = cs->GetTunnelHandle();
    ASSERT_NE(handleGet, nullptr);
    ASSERT_EQ(handle->fd, handleGet->GetHandle()->fd);
    ASSERT_EQ(handle->reserveInts, handleGet->GetHandle()->reserveInts);
    ASSERT_EQ(handle->reserve[0], handleGet->GetHandle()->reserve[0]);
    free(handle);
}

/*
* Function: disconnect
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. call Disconnect and check ret
 */
TEST_F(ConsumerSurfaceTest, disconnect001)
{
    GSError ret = cs->Disconnect();
    ASSERT_EQ(ret, OHOS::GSERROR_NOT_SUPPORT);
}

/*
* Function: SetPresentTimestamp and GetPresentTimestamp
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call GetPresentTimestamp and check ret
* @tc.require: issueI5I57K
 */
TEST_F(ConsumerSurfaceTest, presentTimestamp001)
{
    uint32_t sequence = 0;
    GraphicPresentTimestampType type = GraphicPresentTimestampType::GRAPHIC_DISPLAY_PTS_UNSUPPORTED;
    int64_t time = 0;
    GSError ret = cs->GetPresentTimestamp(sequence, type, time);
    ASSERT_EQ(ret, OHOS::GSERROR_NOT_SUPPORT);
}

/*
* Function: SetPresentTimestamp and GetPresentTimestamp
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call SetPresentTimestamp with abnormal parameters and check ret
* @tc.require: issueI5I57K
 */
TEST_F(ConsumerSurfaceTest, presentTimestamp002)
{
    GraphicPresentTimestamp timestamp = {GRAPHIC_DISPLAY_PTS_UNSUPPORTED, 0};
    GSError ret = cs->SetPresentTimestamp(-1, timestamp);
    ASSERT_EQ(ret, OHOS::GSERROR_INVALID_ARGUMENTS);
}

/*
* Function: SetPresentTimestamp and GetPresentTimestamp
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call SetPresentTimestamp with abnormal parameters and check ret
* @tc.require: issueI5I57K
 */
TEST_F(ConsumerSurfaceTest, presentTimestamp003)
{
    GraphicPresentTimestamp timestamp = {GRAPHIC_DISPLAY_PTS_DELAY, 0};
    GSError ret = cs->SetPresentTimestamp(-1, timestamp);
    ASSERT_EQ(ret, OHOS::GSERROR_NO_ENTRY);
}

/*
* Function: SetPresentTimestamp and GetPresentTimestamp
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. call SetPresentTimestamp with normal parameters and check ret
* @tc.require: issueI5I57K
 */
TEST_F(ConsumerSurfaceTest, presentTimestamp004)
{
    uint32_t sequence = 0;
    GraphicPresentTimestamp timestamp = {GRAPHIC_DISPLAY_PTS_DELAY, 0};
    GSError ret = cs->SetPresentTimestamp(sequence, timestamp);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);
}

/*
* Function: SetPresentTimestamp and GetPresentTimestamp
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. call SetPresentTimestamp with normal parameters and check ret
* @tc.require: issueI5I57K
 */
TEST_F(ConsumerSurfaceTest, presentTimestamp005)
{
    uint32_t sequence = 0;
    GraphicPresentTimestamp timestamp = {GRAPHIC_DISPLAY_PTS_TIMESTAMP, 0};
    GSError ret = cs->SetPresentTimestamp(sequence, timestamp);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);
}
}

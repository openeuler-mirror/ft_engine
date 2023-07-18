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
#include <chrono>
#include <thread>
#include <vector>
#include <sys/wait.h>
#include <unistd.h>
#include <gtest/gtest.h>

#include <iservice_registry.h>
#include <surface.h>
#include <buffer_extra_data_impl.h>
#include <buffer_client_producer.h>
#include <buffer_queue_producer.h>
#include "buffer_consumer_listener.h"
#include "sync_fence.h"

namespace OHOS::Rosen {
class BufferClientProducerRemoteTest : public testing::Test {
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
    static inline sptr<IRemoteObject> robj = nullptr;
    static inline sptr<IBufferProducer> bp = nullptr;
    static inline std::vector<int32_t> deletingBuffers;
    static inline pid_t pid = 0;
    static inline int pipeFd[2] = {};
    static inline int pipe1Fd[2] = {};
    static inline int32_t systemAbilityID = 345135;
    static inline sptr<BufferExtraData> bedata = new BufferExtraDataImpl;
};

void BufferClientProducerRemoteTest::SetUpTestCase()
{
    if (pipe(pipeFd) < 0) {
        exit(1);
    }
    if (pipe(pipe1Fd) < 0) {
        exit(0);
    }
    pid = fork();
    if (pid < 0) {
        exit(1);
    }
    if (pid == 0) {
        sptr<BufferQueue> bq = new BufferQueue("test");
        ASSERT_NE(bq, nullptr);
        sptr<BufferQueueProducer> bqp = new BufferQueueProducer(bq);
        ASSERT_NE(bqp, nullptr);
        bq->Init();
        sptr<IBufferConsumerListener> listener = new BufferConsumerListener();
        bq->RegisterConsumerListener(listener);
        auto sam = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
        sam->AddSystemAbility(systemAbilityID, bqp);
        close(pipeFd[1]);
        close(pipe1Fd[0]);
        char buf[10] = "start";
        write(pipe1Fd[1], buf, sizeof(buf));
        sleep(0);
        read(pipeFd[0], buf, sizeof(buf));
        sam->RemoveSystemAbility(systemAbilityID);
        close(pipeFd[0]);
        close(pipe1Fd[1]);
        exit(0);
    } else {
        close(pipeFd[0]);
        close(pipe1Fd[1]);
        char buf[10];
        read(pipe1Fd[0], buf, sizeof(buf));
        auto sam = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
        robj = sam->GetSystemAbility(systemAbilityID);
        bp = iface_cast<IBufferProducer>(robj);
    }
}

void BufferClientProducerRemoteTest::TearDownTestCase()
{
    bp = nullptr;
    robj = nullptr;

    char buf[10] = "over";
    write(pipeFd[1], buf, sizeof(buf));
    close(pipeFd[1]);
    close(pipe1Fd[0]);

    int32_t ret = 0;
    do {
        waitpid(pid, nullptr, 0);
    } while (ret == -1 && errno == EINTR);
}

/*
* Function: IsProxyObject
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. check ret for IsProxyObject func
 */
TEST_F(BufferClientProducerRemoteTest, IsProxy001)
{
    ASSERT_TRUE(robj->IsProxyObject());
}

/*
* Function: SetQueueSize and GetQueueSize
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call GetQueueSize for default
*                  2. call SetQueueSize and check the ret of GetQueueSize
 */
TEST_F(BufferClientProducerRemoteTest, QueueSize001)
{
    ASSERT_EQ(bp->GetQueueSize(), (uint32_t)SURFACE_DEFAULT_QUEUE_SIZE);

    GSError ret = bp->SetQueueSize(2);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

    ret = bp->SetQueueSize(SURFACE_MAX_QUEUE_SIZE + 1);
    ASSERT_NE(ret, OHOS::GSERROR_OK);

    ASSERT_EQ(bp->GetQueueSize(), 2u);
}

/*
* Function: SetQueueSize and GetQueueSize
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call GetQueueSize for default
*                  2. call SetQueueSize and check the ret of GetQueueSize
 */
TEST_F(BufferClientProducerRemoteTest, ReqCan001)
{
    IBufferProducer::RequestBufferReturnValue retval;
    GSError ret = bp->RequestBuffer(requestConfig, bedata, retval);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);
    ASSERT_NE(retval.buffer, nullptr);

    ret = bp->CancelBuffer(retval.sequence, bedata);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);
}

/*
* Function: RequestBuffer and CancelBuffer
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call RequestBuffer
*                  2. call CancelBuffer 2 times
 */
TEST_F(BufferClientProducerRemoteTest, ReqCan002)
{
    IBufferProducer::RequestBufferReturnValue retval;
    GSError ret = bp->RequestBuffer(requestConfig, bedata, retval);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);
    ASSERT_EQ(retval.buffer, nullptr);

    ret = bp->CancelBuffer(retval.sequence, bedata);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

    ret = bp->CancelBuffer(retval.sequence, bedata);
    ASSERT_NE(ret, OHOS::GSERROR_OK);
}

/*
* Function: RequestBuffer and CancelBuffer
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call RequestBuffer and CancelBuffer 3 times
 */
TEST_F(BufferClientProducerRemoteTest, ReqCan003)
{
    IBufferProducer::RequestBufferReturnValue retval1;
    IBufferProducer::RequestBufferReturnValue retval2;
    IBufferProducer::RequestBufferReturnValue retval3;
    GSError ret;

    ret = bp->RequestBuffer(requestConfig, bedata, retval1);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);
    ASSERT_EQ(retval1.buffer, nullptr);

    ret = bp->RequestBuffer(requestConfig, bedata, retval2);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);
    ASSERT_NE(retval2.buffer, nullptr);

    ret = bp->RequestBuffer(requestConfig, bedata, retval3);
    ASSERT_NE(ret, OHOS::GSERROR_OK);
    ASSERT_EQ(retval3.buffer, nullptr);

    ret = bp->CancelBuffer(retval1.sequence, bedata);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

    ret = bp->CancelBuffer(retval2.sequence, bedata);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

    ret = bp->CancelBuffer(retval3.sequence, bedata);
    ASSERT_NE(ret, OHOS::GSERROR_OK);
}

/*
* Function: SetQueueSize, RequestBuffer and CancelBuffer
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call SetQueueSize
*                  2. call RequestBuffer and CancelBuffer
*                  3. call SetQueueSize again
 */
TEST_F(BufferClientProducerRemoteTest, SetQueueSizeDeleting001)
{
    GSError ret = bp->SetQueueSize(1);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

    IBufferProducer::RequestBufferReturnValue retval;
    ret = bp->RequestBuffer(requestConfig, bedata, retval);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);
    ASSERT_EQ(retval.buffer, nullptr);

    ret = bp->CancelBuffer(retval.sequence, bedata);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

    ret = bp->SetQueueSize(2);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);
}

/*
* Function: RequestBuffer and FlushBuffer
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call RequestBuffer
*                  2. call FlushBuffer
 */
TEST_F(BufferClientProducerRemoteTest, ReqFlu001)
{
    IBufferProducer::RequestBufferReturnValue retval;
    GSError ret = bp->RequestBuffer(requestConfig, bedata, retval);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

    sptr<SyncFence> acquireFence = SyncFence::INVALID_FENCE;
    ret = bp->FlushBuffer(retval.sequence, bedata, acquireFence, flushConfig);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);
}

/*
* Function: RequestBuffer and FlushBuffer
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call RequestBuffer
*                  2. call FlushBuffer 2 times
 */
TEST_F(BufferClientProducerRemoteTest, ReqFlu002)
{
    IBufferProducer::RequestBufferReturnValue retval;
    GSError ret = bp->RequestBuffer(requestConfig, bedata, retval);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

    sptr<SyncFence> acquireFence = SyncFence::INVALID_FENCE;
    ret = bp->FlushBuffer(retval.sequence, bedata, acquireFence, flushConfig);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

    ret = bp->FlushBuffer(retval.sequence, bedata, acquireFence, flushConfig);
    ASSERT_NE(ret, OHOS::GSERROR_OK);
}

/*
* Function: AttachBuffer and DetachBuffer
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call AttachBuffer
*                  2. call DetachBuffer
*/
TEST_F(BufferClientProducerRemoteTest, AttachDetach001)
{
    sptr<OHOS::SurfaceBuffer> buffer = new SurfaceBufferImpl(0);
    GSError ret = bp->AttachBuffer(buffer);
    ASSERT_EQ(ret, OHOS::GSERROR_NOT_SUPPORT);

    ret = bp->DetachBuffer(buffer);
    ASSERT_EQ(ret, OHOS::GSERROR_NOT_SUPPORT);
}

/*
* Function: RegisterReleaseListener
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call RegisterReleaseListener
*/
TEST_F(BufferClientProducerRemoteTest, RegisterReleaseListener001)
{
    OnReleaseFunc onBufferRelease = nullptr;
    GSError ret = bp->RegisterReleaseListener(onBufferRelease);
    ASSERT_EQ(ret, OHOS::GSERROR_NOT_SUPPORT);
}

/*
* Function: GetName
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call GetName
*/
TEST_F(BufferClientProducerRemoteTest, GetName001)
{
    std::string name;
    GSError ret = bp->GetName(name);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);
}

/*
* Function: GetUniqueId
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call GetUniqueId
*/
TEST_F(BufferClientProducerRemoteTest, GetUniqueId001)
{
    uint64_t bpid = bp->GetUniqueId();
    ASSERT_NE(bpid, 0);
}

/*
* Function: GetDefaultUsage
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call GetDefaultUsage
*/
TEST_F(BufferClientProducerRemoteTest, GetDefaultUsage001)
{
    uint32_t usage = bp->GetDefaultUsage();
    ASSERT_EQ(usage, 0);
}

/*
* Function: SetTransform
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call SetTransform
*/
TEST_F(BufferClientProducerRemoteTest, SetTransform001)
{
    GraphicTransformType transform = GraphicTransformType::GRAPHIC_ROTATE_90;
    GSError ret = bp->SetTransform(transform);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);
}

/*
* Function: IsSupportedAlloc
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call IsSupportedAlloc with abnormal parameters and check ret
*/
TEST_F(BufferClientProducerRemoteTest, isSupportedAlloc001)
{
    std::vector<BufferVerifyAllocInfo> infos;
    std::vector<bool> supporteds;
    GSError ret = bp->IsSupportedAlloc(infos, supporteds);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);
}

/*
* Function: IsSupportedAlloc
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call IsSupportedAlloc with abnormal parameters and check ret
*/
TEST_F(BufferClientProducerRemoteTest, isSupportedAlloc002)
{
    std::vector<BufferVerifyAllocInfo> infos;
    std::vector<bool> supporteds;
    GSError ret = bp->IsSupportedAlloc(infos, supporteds);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

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

    ret = bp->IsSupportedAlloc(infos, supporteds);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);
}

/*
* Function: SetScalingMode
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call SetScalingMode with abnormal parameters and check ret
*/
TEST_F(BufferClientProducerRemoteTest, SetScalingMode001)
{
    ScalingMode scalingMode = ScalingMode::SCALING_MODE_SCALE_TO_WINDOW;
    GSError ret = bp->SetScalingMode(-1, scalingMode);
    ASSERT_EQ(ret, OHOS::GSERROR_NO_ENTRY);
}

/*
* Function: SetMetaData
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call SetMetaData with abnormal parameters and check ret
*/
TEST_F(BufferClientProducerRemoteTest, SetMetaData001)
{
    uint32_t sequence = 0;
    std::vector<GraphicHDRMetaData> metaData;
    GSError ret = bp->SetMetaData(sequence, metaData);
    ASSERT_EQ(ret, OHOS::GSERROR_INVALID_ARGUMENTS);
}

/*
* Function: SetMetaDataSet
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call SetMetaDataSet with abnormal parameters and check ret
*/
TEST_F(BufferClientProducerRemoteTest, SetMetaDataSet001)
{
    GraphicHDRMetadataKey key = GraphicHDRMetadataKey::GRAPHIC_MATAKEY_HDR10_PLUS;
    std::vector<uint8_t> metaData;

    uint32_t sequence = 0;
    GSError ret = bp->SetMetaDataSet(sequence, key, metaData);
    ASSERT_EQ(ret, OHOS::GSERROR_INVALID_ARGUMENTS);
}

/*
* Function: GoBackground
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call GoBackground
*/
TEST_F(BufferClientProducerRemoteTest, GoBackground001)
{
    GSError ret = bp->GoBackground();
    ASSERT_EQ(ret, OHOS::GSERROR_OK);
}
}

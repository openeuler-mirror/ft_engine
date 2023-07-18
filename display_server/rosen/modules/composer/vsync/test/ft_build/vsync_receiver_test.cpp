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
#include "vsync_receiver.h"
#include "vsync_distributor.h"
#include "vsync_controller.h"

namespace OHOS {
namespace Rosen {
static void OnVSync(int64_t now, void* data) {}
class vsyncReceiverTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();

    static inline sptr<VSyncController> vsyncController = nullptr;
    static inline sptr<VSyncDistributor> vsyncDistributor = nullptr;
    static inline sptr<VSyncGenerator> vsyncGenerator = nullptr;
    static inline sptr<VSyncConnection> conn = nullptr;
    static inline sptr<VSyncReceiver> vsyncReceiver = nullptr;
};

void vsyncReceiverTest::SetUpTestCase()
{
    vsyncGenerator = CreateVSyncGenerator();
    vsyncController = new VSyncController(vsyncGenerator, 0);
    vsyncDistributor = new VSyncDistributor(vsyncController, "vsyncReceiverTest");
    conn = new VSyncConnection(vsyncDistributor, "vsyncReceiverTest");
    vsyncReceiver = new VSyncReceiver(conn);
}

void vsyncReceiverTest::TearDownTestCase()
{
    vsyncReceiver = nullptr;
    vsyncController = nullptr;
    vsyncGenerator = nullptr;
    vsyncDistributor = nullptr;
    conn = nullptr;
    DestroyVSyncGenerator();
}

namespace {
/*
* Function: Init001
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call Init
 */
TEST_F(vsyncReceiverTest, Init001)
{
    ASSERT_EQ(vsyncReceiverTest::vsyncReceiver->Init(), VSYNC_ERROR_OK);
}

/*
* Function: RequestNextVSync001
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call RequestNextVSync
 */
TEST_F(vsyncReceiverTest, RequestNextVSync001)
{
    VSyncReceiver::FrameCallback fcb = {
        .userData_ = this,
        .callback_ = OnVSync,
    };
    vsyncDistributor->AddConnection(conn);
    ASSERT_EQ(vsyncReceiverTest::vsyncReceiver->RequestNextVSync(fcb), VSYNC_ERROR_OK);
    vsyncDistributor->RemoveConnection(conn);
}

/*
* Function: SetVSyncRate001
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call SetVSyncRate
 */
TEST_F(vsyncReceiverTest, SetVSyncRate001)
{
    VSyncReceiver::FrameCallback fcb = {
        .userData_ = this,
        .callback_ = OnVSync,
    };
    vsyncDistributor->AddConnection(conn);
    ASSERT_EQ(vsyncReceiverTest::vsyncReceiver->SetVSyncRate(fcb, 0), VSYNC_ERROR_INVALID_ARGUMENTS);
    vsyncDistributor->RemoveConnection(conn);
}

/*
* Function: SetVSyncRate002
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call SetVSyncRate
 */
TEST_F(vsyncReceiverTest, SetVSyncRate002)
{
    VSyncReceiver::FrameCallback fcb = {
        .userData_ = this,
        .callback_ = OnVSync,
    };
    vsyncDistributor->AddConnection(conn);
    ASSERT_EQ(vsyncReceiverTest::vsyncReceiver->SetVSyncRate(fcb, 1), VSYNC_ERROR_OK);
    vsyncDistributor->RemoveConnection(conn);
}
} // namespace
} // namespace Rosen
} // namespace OHOS

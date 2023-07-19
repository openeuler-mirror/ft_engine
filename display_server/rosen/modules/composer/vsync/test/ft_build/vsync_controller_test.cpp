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
#include <vsync_controller.h>

namespace OHOS::Rosen {
class VSyncControllerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    static inline sptr<VSyncController> vsyncController_ = nullptr;
    static inline sptr<VSyncGenerator> vsyncGenerator_ = nullptr;
};

class VSyncControllerCallback : public VSyncController::Callback {
public:
    void OnVSyncEvent(int64_t now) override;
};

void VSyncControllerTest::SetUpTestCase()
{
    vsyncGenerator_ = CreateVSyncGenerator();
    vsyncController_ = new VSyncController(vsyncGenerator_, 0);
}

void VSyncControllerTest::TearDownTestCase()
{
    vsyncController_ = nullptr;
    vsyncGenerator_ = nullptr;
    DestroyVSyncGenerator();
}

void VSyncControllerCallback::OnVSyncEvent(int64_t now) {}

/*
* Function: SetEnable
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call SetEnable
 */
TEST_F(VSyncControllerTest, SetEnable001)
{
    ASSERT_EQ(VSyncControllerTest::vsyncController_->SetEnable(true), VSYNC_ERROR_OK);
}

/*
* Function: SetCallback001
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. generate a VSyncControllerCallback obj
*                  2. call SetCallback
 */
TEST_F(VSyncControllerTest, SetCallback001)
{
    VSyncControllerCallback* callback = new VSyncControllerCallback;
    ASSERT_EQ(VSyncControllerTest::vsyncController_->SetCallback(callback), VSYNC_ERROR_OK);
    delete callback;
}

/*
* Function: SetCallback002
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call SetCallback with nullptr
 */
TEST_F(VSyncControllerTest, SetCallback002)
{
    ASSERT_EQ(VSyncControllerTest::vsyncController_->SetCallback(nullptr), VSYNC_ERROR_NULLPTR);
}

/*
* Function: SetPhaseOffset001
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call SetPhaseOffset
 */
TEST_F(VSyncControllerTest, SetPhaseOffset)
{
    ASSERT_EQ(VSyncControllerTest::vsyncController_->SetPhaseOffset(2), VSYNC_ERROR_OK);
}
}

/*
 * Copyright (c) 2022-2022 Huawei Device Co., Ltd.
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
#include "display_manager_agent_stub.h"
#include "display_manager_agent_default.h"


using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class DisplayManagerAgentStubTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    sptr<DisplayManagerAgentStub> stub_;
};

void DisplayManagerAgentStubTest::SetUpTestCase()
{
}

void DisplayManagerAgentStubTest::TearDownTestCase()
{
}

void DisplayManagerAgentStubTest::SetUp()
{
    stub_ = new DisplayManagerAgentDefault();
}

void DisplayManagerAgentStubTest::TearDown()
{
}

namespace {
/**
 * @tc.name: OnRemoteRequest01
 * @tc.desc: TRANS_ID_ON_DISPLAY_CONNECT
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAgentStubTest, OnRemoteRequest01, Function | SmallTest | Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(DisplayManagerAgentStub::GetDescriptor());
    sptr<DisplayInfo> displayInfo;
    data.WriteParcelable(displayInfo.GetRefPtr());
    uint32_t code = static_cast<uint32_t>(IDisplayManagerAgent::TRANS_ID_ON_DISPLAY_CONNECT);
    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}

/**
 * @tc.name: OnRemoteRequest02
 * @tc.desc: TRANS_ID_ON_DISPLAY_DISCONNECT
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAgentStubTest, OnRemoteRequest02, Function | SmallTest | Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(DisplayManagerAgentStub::GetDescriptor());
    DisplayId displayId = 0;
    data.WriteUint64(displayId);
    uint32_t code = static_cast<uint32_t>(IDisplayManagerAgent::TRANS_ID_ON_DISPLAY_DISCONNECT);
    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}

}
} // namespace Rosen
} // namespace OHOS
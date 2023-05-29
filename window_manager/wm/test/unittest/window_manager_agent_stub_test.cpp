/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
#include "window_manager_agent_stub.h"
#include "window_manager_agent.h"
#include "marshalling_helper.h"

using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace Rosen {
class WindowManagerAgentStubTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    sptr<WindowManagerAgentStub> stub_;
};

void WindowManagerAgentStubTest::SetUpTestCase()
{
}

void WindowManagerAgentStubTest::TearDownTestCase()
{
}

void WindowManagerAgentStubTest::SetUp()
{
    stub_ = new WindowManagerAgent();
}

void WindowManagerAgentStubTest::TearDown()
{
}

namespace {
/**
 * @tc.name: OnRemoteRequest01
 * @tc.desc: test InterfaceToken check failed
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerAgentStubTest, OnRemoteRequest01, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(u"error.GetDescriptor");

    uint32_t code = static_cast<uint32_t>(IWindowManagerAgent::WindowManagerAgentMsg::TRANS_ID_UPDATE_FOCUS);

    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, -1);
}

/**
 * @tc.name: OnRemoteRequest02
 * @tc.desc: test TRANS_ID_UPDATE_FOCUS
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerAgentStubTest, OnRemoteRequest02, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(WindowManagerAgentStub::GetDescriptor());

    sptr<FocusChangeInfo> focusChangeInfo = new FocusChangeInfo();
    data.WriteParcelable(focusChangeInfo);
    data.WriteRemoteObject(focusChangeInfo->abilityToken_);
    data.WriteBool(false);

    uint32_t code = static_cast<uint32_t>(IWindowManagerAgent::WindowManagerAgentMsg::TRANS_ID_UPDATE_FOCUS);

    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}

/**
 * @tc.name: OnRemoteRequest03
 * @tc.desc: test TRANS_ID_UPDATE_FOCUS success
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerAgentStubTest, OnRemoteRequest03, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(WindowManagerAgentStub::GetDescriptor());

    uint32_t code = static_cast<uint32_t>(IWindowManagerAgent::WindowManagerAgentMsg::TRANS_ID_UPDATE_FOCUS);

    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}

/**
 * @tc.name: OnRemoteRequest04
 * @tc.desc: test TRANS_ID_UPDATE_WINDOW_STATUS success
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerAgentStubTest, OnRemoteRequest04, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(WindowManagerAgentStub::GetDescriptor());

    sptr<AccessibilityWindowInfo> info = new AccessibilityWindowInfo();
    std::vector<sptr<AccessibilityWindowInfo>> infos;
    infos.emplace_back(info);
    MarshallingHelper::MarshallingVectorParcelableObj<AccessibilityWindowInfo>(data, infos);

    data.WriteUint32(static_cast<uint32_t>(WindowUpdateType::WINDOW_UPDATE_ADDED));

    uint32_t code = static_cast<uint32_t>(IWindowManagerAgent::WindowManagerAgentMsg::TRANS_ID_UPDATE_WINDOW_STATUS);

    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}

/**
 * @tc.name: OnRemoteRequest06
 * @tc.desc: test TRANS_ID_UPDATE_WINDOW_VISIBILITY success
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerAgentStubTest, OnRemoteRequest06, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(WindowManagerAgentStub::GetDescriptor());

    sptr<WindowVisibilityInfo> visibilityInfo = new WindowVisibilityInfo();
    std::vector<sptr<WindowVisibilityInfo>> visibilityInfos;
    visibilityInfos.emplace_back(visibilityInfo);
    data.WriteUint32(static_cast<uint32_t>(visibilityInfos.size()));
    for (auto& info : visibilityInfos) {
        data.WriteParcelable(info);
    }

    auto code = static_cast<uint32_t>(IWindowManagerAgent::WindowManagerAgentMsg::TRANS_ID_UPDATE_WINDOW_VISIBILITY);

    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}

/**
 * @tc.name: OnRemoteRequest08
 * @tc.desc: test TRANS_ID_UPDATE_SYSTEM_BAR_PROPS success
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerAgentStubTest, OnRemoteRequest08, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(WindowManagerAgentStub::GetDescriptor());

    data.WriteUint64(0);

    SystemBarRegionTints tints;
    MarshallingHelper::MarshallingVectorObj<SystemBarRegionTint>(data, tints,
        [](Parcel& parcel, const SystemBarRegionTint& tint) {
            return parcel.WriteUint32(static_cast<uint32_t>(tint.type_)) && parcel.WriteBool(tint.prop_.enable_) &&
                parcel.WriteUint32(tint.prop_.backgroundColor_) && parcel.WriteUint32(tint.prop_.contentColor_) &&
                parcel.WriteInt32(tint.region_.posX_) && parcel.WriteInt32(tint.region_.posY_) &&
                parcel.WriteInt32(tint.region_.width_) && parcel.WriteInt32(tint.region_.height_);
        }
    );

    uint32_t code = static_cast<uint32_t>(IWindowManagerAgent::WindowManagerAgentMsg::TRANS_ID_UPDATE_SYSTEM_BAR_PROPS);

    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}

}
}
}

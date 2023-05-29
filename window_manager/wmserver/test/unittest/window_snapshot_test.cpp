/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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
#include "snapshot_controller.h"
#include "wm_common.h"
#include "iremote_object_mocker.h"
#include "common_test_utils.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class WindowSnapshotTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    virtual void SetUp() override;
    virtual void TearDown() override;
};

void WindowSnapshotTest::SetUpTestCase()
{
}

void WindowSnapshotTest::TearDownTestCase()
{
}

void WindowSnapshotTest::SetUp()
{
}

void WindowSnapshotTest::TearDown()
{
}

namespace {
/**
 * @tc.name: GetSnapshot
 * @tc.desc: GetSnapshot when parameter abilityToken is nullptr
 * @tc.type: FUNC
 */
HWTEST_F(WindowSnapshotTest, GetSnapshot01, Function | SmallTest | Level3)
{
    sptr<WindowRoot> root = nullptr;
    std::shared_ptr<AppExecFwk::EventHandler> handler = nullptr;
    sptr<SnapshotController> snapshotController_ = new SnapshotController(root, handler);
    AAFwk::Snapshot snapshot_;
    ASSERT_EQ(static_cast<int32_t>(WMError::WM_ERROR_NULLPTR), snapshotController_->GetSnapshot(nullptr, snapshot_));
}

/**
 * @tc.name: GetSnapshot
 * @tc.desc: GetSnapshot when parameter abilityToken is nullptr
 * @tc.type: FUNC
 */
HWTEST_F(WindowSnapshotTest, GetSnapshot02, Function | SmallTest | Level3)
{
    sptr<WindowRoot> root = nullptr;
    std::shared_ptr<AppExecFwk::EventHandler> handler = nullptr;
    sptr<IRemoteObject> iRemoteObjectMocker = new IRemoteObjectMocker();

    sptr<SnapshotController> snapshotController_ = new SnapshotController(root, handler);
    AAFwk::Snapshot snapshot_;
    ASSERT_EQ(static_cast<int32_t>(WMError::WM_ERROR_NULLPTR),
        snapshotController_->GetSnapshot(iRemoteObjectMocker, snapshot_));
}

/**
 * @tc.name: GetSnapshot
 * @tc.desc: GetSnapshot when parameter abilityToken is nullptr
 * @tc.type: FUNC
 */
HWTEST_F(WindowSnapshotTest, GetSnapshot03, Function | SmallTest | Level3)
{
    auto runner = AppExecFwk::EventRunner::Create("TestRunner");
    auto handler = std::make_shared<AppExecFwk::EventHandler>(runner);
    sptr<WindowRoot> root = nullptr;
    sptr<IRemoteObject> iRemoteObjectMocker = new IRemoteObjectMocker();
    
    sptr<SnapshotController> snapshotController_ = new SnapshotController(root, handler);
    AAFwk::Snapshot snapshot_;
    ASSERT_EQ(static_cast<int32_t>(WMError::WM_ERROR_NULLPTR),
        snapshotController_->GetSnapshot(iRemoteObjectMocker, snapshot_));
}

/**
 * @tc.name: GetSnapshot
 * @tc.desc: GetSnapshot when parameter abilityToken is nullptr
 * @tc.type: FUNC
 */
HWTEST_F(WindowSnapshotTest, GetSnapshot04, Function | SmallTest | Level3)
{
    auto runner = AppExecFwk::EventRunner::Create("TestRunner");
    auto handler = std::make_shared<AppExecFwk::EventHandler>(runner);

    sptr<WindowRoot> root = new WindowRoot([](Event event, const sptr<IRemoteObject>& remoteObject) {});
    sptr<WindowNode> node = new WindowNode();
    root->windowNodeMap_.insert(std::make_pair(0, node));

    sptr<IRemoteObject> iRemoteObjectMocker = new IRemoteObjectMocker();
    node->abilityToken_ = iRemoteObjectMocker;

    sptr<SnapshotController> snapshotController_ = new SnapshotController(root, handler);
    AAFwk::Snapshot snapshot_;
    ASSERT_EQ(static_cast<int32_t>(WMError::WM_ERROR_NULLPTR),
        snapshotController_->GetSnapshot(iRemoteObjectMocker, snapshot_));

    sptr<IRemoteObject> iRemoteObjectMockerInvalid = new IRemoteObjectMocker();
    ASSERT_EQ(static_cast<int32_t>(WMError::WM_ERROR_NULLPTR),
        snapshotController_->GetSnapshot(iRemoteObjectMockerInvalid, snapshot_));
}

/**
 * @tc.name: GetSnapshot
 * @tc.desc: GetSnapshot when parameter abilityToken is nullptr
 * @tc.type: FUNC
 */
HWTEST_F(WindowSnapshotTest, GetSnapshot05, Function | SmallTest | Level3)
{
    auto runner = AppExecFwk::EventRunner::Create("TestRunner");
    auto handler = std::make_shared<AppExecFwk::EventHandler>(runner);

    sptr<WindowRoot> root = new WindowRoot([](Event event, const sptr<IRemoteObject>& remoteObject) {});
    sptr<WindowNode> node = new WindowNode();
    root->windowNodeMap_.insert(std::make_pair(0, node));

    sptr<IRemoteObject> iRemoteObjectMocker = new IRemoteObjectMocker();
    node->abilityToken_ = iRemoteObjectMocker;
    node->SetSnapshot(CommonTestUtils::CreatePixelMap());

    sptr<SnapshotController> snapshotController_ = new SnapshotController(root, handler);
    AAFwk::Snapshot snapshot_;
    ASSERT_EQ(static_cast<int32_t>(WMError::WM_OK), snapshotController_->GetSnapshot(iRemoteObjectMocker, snapshot_));
}
}
} // namespace Rosen
} // namespace OHOS

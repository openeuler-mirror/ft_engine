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

#include "window_extension_connection.h"
#include "wm_common.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class ExtensionCallback : public Rosen::IWindowExtensionCallback {
public:
    ExtensionCallback() = default;
    ~ExtensionCallback()  = default;
    void OnWindowReady(const std::shared_ptr<Rosen::RSSurfaceNode>& rsSurfaceNode) override;
    void OnExtensionDisconnected() override;
    void OnKeyEvent(const std::shared_ptr<MMI::KeyEvent>& event) override;
    void OnPointerEvent(const std::shared_ptr<MMI::PointerEvent>& event) override;
    void OnBackPress() override;
    bool isWindowReady_ = false;
};

void ExtensionCallback::OnWindowReady(const std::shared_ptr<Rosen::RSSurfaceNode>& rsSurfaceNode)
{
    isWindowReady_ = true;
}

void ExtensionCallback::OnExtensionDisconnected()
{
}

void ExtensionCallback::OnKeyEvent(const std::shared_ptr<MMI::KeyEvent>& event)
{
}

void ExtensionCallback::OnPointerEvent(const std::shared_ptr<MMI::PointerEvent>& event)
{
}

void ExtensionCallback::OnBackPress()
{
}

class ExtensionConnectionTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    virtual void SetUp() override;
    virtual void TearDown() override;
};

void ExtensionConnectionTest::SetUpTestCase()
{
}

void ExtensionConnectionTest::TearDownTestCase()
{
}

void ExtensionConnectionTest::SetUp()
{
}

void ExtensionConnectionTest::TearDown()
{
}

namespace {
/**
 * @tc.name: WindowExtensionConnection01
 * @tc.desc: connect window extension
 * @tc.type: FUNC
 */
HWTEST_F(ExtensionConnectionTest, WindowExtensionConnection01, Function | SmallTest | Level2)
{
    sptr<WindowExtensionConnection> connection = new(std::nothrow)WindowExtensionConnection();
    if (connection == nullptr) {
        return;
    }
    AppExecFwk::ElementName element;
    element.SetBundleName("com.test.windowextension");
    element.SetAbilityName("WindowExtAbility");
    Rosen::Rect rect {100, 100, 60, 60};
    ASSERT_TRUE(connection->ConnectExtension(element, rect, 100, INVALID_WINDOW_ID, nullptr) != ERR_OK);
    connection->Show();
    connection->RequestFocus();
    connection->SetBounds(rect);
    connection->Hide();
}
}
} // Rosen
} // OHOS
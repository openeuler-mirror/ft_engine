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
#include "window_dumper.h"
#include "window_manager_service.h"
#include "window_impl.h"
#include "window_agent.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class WindowDumperTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void WindowDumperTest::SetUpTestCase()
{
}

void WindowDumperTest::TearDownTestCase()
{
}

void WindowDumperTest::SetUp()
{
}

void WindowDumperTest::TearDown()
{
}

namespace {
/**
 * @tc.name: Dump01
 * @tc.desc: Dump
 * @tc.type: FUNC
 */
HWTEST_F(WindowDumperTest, Dump01, Function | SmallTest | Level1)
{
    sptr<WindowDumper> windowDumper;
    windowDumper = new WindowDumper(WindowManagerService::GetInstance().windowRoot_);
    int fd = 0;
    std::vector<std::u16string> args;
    WMError ret = windowDumper->Dump(fd, args);
    ASSERT_EQ(ret, WMError::WM_OK);
}

/**
 * @tc.name: Dump02
 * @tc.desc: Dump fd less 0
 * @tc.type: FUNC
 */
HWTEST_F(WindowDumperTest, Dump02, Function | SmallTest | Level1)
{
    sptr<WindowDumper> windowDumper;
    windowDumper = new WindowDumper(WindowManagerService::GetInstance().windowRoot_);
    int fd = -1;
    std::vector<std::u16string> args;
    WMError ret = windowDumper->Dump(fd, args);
    ASSERT_EQ(ret, WMError::WM_ERROR_INVALID_PARAM);
}

/**
 * @tc.name: Dump03
 * @tc.desc: Dump one param with '-h'
 * @tc.type: FUNC
 */
HWTEST_F(WindowDumperTest, Dump03, Function | SmallTest | Level1)
{
    sptr<WindowDumper> windowDumper;
    windowDumper = new WindowDumper(WindowManagerService::GetInstance().windowRoot_);
    int fd = 1;
    std::vector<std::u16string> args;
    const std::u16string DUMP_HELP = u"-h";
    args.emplace_back(DUMP_HELP);
    WMError ret = windowDumper->Dump(fd, args);
    ASSERT_EQ(ret, WMError::WM_OK);
}

/**
 * @tc.name: Dump04
 * @tc.desc: Dump one param with '-x'
 * @tc.type: FUNC
 */
HWTEST_F(WindowDumperTest, Dump04, Function | SmallTest | Level1)
{
    sptr<WindowDumper> windowDumper;
    windowDumper = new WindowDumper(WindowManagerService::GetInstance().windowRoot_);
    int fd = 2;
    std::vector<std::u16string> args;
    const std::u16string DUMP_HELP = u"-x";
    args.emplace_back(DUMP_HELP);
    WMError ret = windowDumper->Dump(fd, args);
    ASSERT_EQ(ret, WMError::WM_OK);
}

/**
 * @tc.name: Dump05
 * @tc.desc: Dump two param with '-a'
 * @tc.type: FUNC
 */
HWTEST_F(WindowDumperTest, Dump05, Function | SmallTest | Level1)
{
    sptr<WindowDumper> windowDumper;
    windowDumper = new WindowDumper(WindowManagerService::GetInstance().windowRoot_);
    int fd = 3;
    std::vector<std::u16string> args;
    const std::u16string DUMP_ALL = u"-a";
    args.emplace_back(DUMP_ALL);
    WMError ret = windowDumper->Dump(fd, args);
    ASSERT_TRUE(ret == WMError::WM_OK || ret == WMError::WM_ERROR_INVALID_OPERATION);
}

/**
 * @tc.name: Dump06
 * @tc.desc: Dump two param with '-w 1'
 * @tc.type: FUNC
 */
HWTEST_F(WindowDumperTest, Dump06, Function | SmallTest | Level1)
{
    sptr<WindowDumper> windowDumper;
    windowDumper = new WindowDumper(WindowManagerService::GetInstance().windowRoot_);
    int fd = 4;
    std::vector<std::u16string> args;
    const std::u16string DUMP_WINDOW = u"-w";
    const std::u16string DUMP_WINDOW_ID = u"3";
    args.emplace_back(DUMP_WINDOW);
    args.emplace_back(DUMP_WINDOW_ID);
    WMError ret = windowDumper->Dump(fd, args);
    ASSERT_TRUE(ret == WMError::WM_OK || ret == WMError::WM_ERROR_INVALID_OPERATION);
}

/**
 * @tc.name: Dump07
 * @tc.desc: Dump two param with '-w n'
 * @tc.type: FUNC
 */
HWTEST_F(WindowDumperTest, Dump07, Function | SmallTest | Level1)
{
    sptr<WindowDumper> windowDumper;
    windowDumper = new WindowDumper(WindowManagerService::GetInstance().windowRoot_);
    int fd = 5;
    std::vector<std::u16string> args;
    const std::u16string DUMP_WINDOW = u"-w";
    const std::u16string DUMP_WINDOW_ID = u"n";
    args.emplace_back(DUMP_WINDOW);
    args.emplace_back(DUMP_WINDOW_ID);
    WMError ret = windowDumper->Dump(fd, args);
    ASSERT_EQ(ret, WMError::WM_ERROR_INVALID_OPERATION);
}

/**
 * @tc.name: ShowAceDumpHelp02
 * @tc.desc: ShowAceDumpHelp
 * @tc.type: FUNC
 */
HWTEST_F(WindowDumperTest, ShowAceDumpHelp02, Function | SmallTest | Level1)
{
    sptr<WindowDumper> windowDumper;
    sptr<WindowNode> node = new WindowNode();
    uint32_t id = 102;
    node->property_->SetWindowId(id);
    node->property_->SetWindowType(WindowType::WINDOW_TYPE_KEYGUARD);
    WindowManagerService::GetInstance().windowRoot_->windowNodeMap_.insert(std::make_pair(id, node));
    windowDumper = new WindowDumper(WindowManagerService::GetInstance().windowRoot_);
    std::string dumpInfo;
    windowDumper->ShowAceDumpHelp(dumpInfo);
    WindowManagerService::GetInstance().windowRoot_->windowNodeMap_.clear();
    ASSERT_TRUE(dumpInfo.empty());
}
}
}
}
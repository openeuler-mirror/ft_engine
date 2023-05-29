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
#include "display_dumper.h"
#include "display_manager_service.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class DisplayDumperTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void DisplayDumperTest::SetUpTestCase()
{
}

void DisplayDumperTest::TearDownTestCase()
{
}

void DisplayDumperTest::SetUp()
{
}

void DisplayDumperTest::TearDown()
{
}

namespace {
/**
 * @tc.name: Dump01
 * @tc.desc: Dump
 * @tc.type: FUNC
 */
HWTEST_F(DisplayDumperTest, Dump01, Function | SmallTest | Level1)
{
    sptr<DisplayDumper> displayDumper;
    displayDumper = new DisplayDumper(DisplayManagerService::GetInstance().abstractDisplayController_,
        DisplayManagerService::GetInstance().abstractScreenController_,
        DisplayManagerService::GetInstance().mutex_);
    int fd = 1;
    std::vector<std::u16string> args;
    DMError ret = displayDumper->Dump(fd, args);
    ASSERT_EQ(ret, DMError::DM_OK);
}

/**
 * @tc.name: Dump02
 * @tc.desc: Dump fd less 0
 * @tc.type: FUNC
 */
HWTEST_F(DisplayDumperTest, Dump02, Function | SmallTest | Level1)
{
    sptr<DisplayDumper> displayDumper;
    displayDumper = new DisplayDumper(DisplayManagerService::GetInstance().abstractDisplayController_,
        DisplayManagerService::GetInstance().abstractScreenController_,
        DisplayManagerService::GetInstance().mutex_);
    int fd = -1;
    std::vector<std::u16string> args;
    DMError ret = displayDumper->Dump(fd, args);
    ASSERT_EQ(ret, DMError::DM_ERROR_INVALID_PARAM);
}

/**
 * @tc.name: Dump03
 * @tc.desc: Dump one param with '-h'
 * @tc.type: FUNC
 */
HWTEST_F(DisplayDumperTest, Dump03, Function | SmallTest | Level1)
{
    sptr<DisplayDumper> displayDumper;
    displayDumper = new DisplayDumper(DisplayManagerService::GetInstance().abstractDisplayController_,
        DisplayManagerService::GetInstance().abstractScreenController_,
        DisplayManagerService::GetInstance().mutex_);
    int fd = 3;
    std::vector<std::u16string> args;
    const std::u16string DUMP_HELP = u"-h";
    args.emplace_back(DUMP_HELP);
    DMError ret = displayDumper->Dump(fd, args);
    ASSERT_TRUE(ret == DMError::DM_OK || ret == DMError::DM_ERROR_UNKNOWN);
}

/**
 * @tc.name: Dump04
 * @tc.desc: Dump one param with '-x'
 * @tc.type: FUNC
 */
HWTEST_F(DisplayDumperTest, Dump04, Function | SmallTest | Level1)
{
    sptr<DisplayDumper> displayDumper;
    displayDumper = new DisplayDumper(DisplayManagerService::GetInstance().abstractDisplayController_,
        DisplayManagerService::GetInstance().abstractScreenController_,
        DisplayManagerService::GetInstance().mutex_);
    int fd = 4;
    std::vector<std::u16string> args;
    const std::u16string DUMP_HELP = u"-x";
    args.emplace_back(DUMP_HELP);
    DMError ret = displayDumper->Dump(fd, args);
    ASSERT_TRUE(ret == DMError::DM_OK || ret == DMError::DM_ERROR_UNKNOWN);
}

/**
 * @tc.name: Dump05
 * @tc.desc: Dump two param with '-s -a'
 * @tc.type: FUNC
 */
HWTEST_F(DisplayDumperTest, Dump05, Function | SmallTest | Level1)
{
    sptr<DisplayDumper> displayDumper;
    displayDumper = new DisplayDumper(DisplayManagerService::GetInstance().abstractDisplayController_,
        DisplayManagerService::GetInstance().abstractScreenController_,
        DisplayManagerService::GetInstance().mutex_);
    int fd = 5;
    std::vector<std::u16string> args;
    const std::u16string DUMP_SCREEN = u"-s";
    const std::u16string DUMP_ALL = u"-a";
    args.emplace_back(DUMP_SCREEN);
    args.emplace_back(DUMP_ALL);
    DMError ret = displayDumper->Dump(fd, args);
    ASSERT_EQ(ret, DMError::DM_ERROR_UNKNOWN);
}

/**
 * @tc.name: Dump06
 * @tc.desc: Dump two param with '-d -a'
 * @tc.type: FUNC
 */
HWTEST_F(DisplayDumperTest, Dump06, Function | SmallTest | Level1)
{
    sptr<DisplayDumper> displayDumper;
    displayDumper = new DisplayDumper(DisplayManagerService::GetInstance().abstractDisplayController_,
        DisplayManagerService::GetInstance().abstractScreenController_,
        DisplayManagerService::GetInstance().mutex_);
    int fd = 6;
    std::vector<std::u16string> args;
    const std::u16string DUMP_DISPLAY = u"-d";
    const std::u16string DUMP_ALL = u"-a";
    args.emplace_back(DUMP_DISPLAY);
    args.emplace_back(DUMP_ALL);
    DMError ret = displayDumper->Dump(fd, args);
    ASSERT_TRUE(ret == DMError::DM_OK || ret == DMError::DM_ERROR_UNKNOWN);
}

/**
 * @tc.name: Dump07
 * @tc.desc: Dump two param with '-s 1'
 * @tc.type: FUNC
 */
HWTEST_F(DisplayDumperTest, Dump071, Function | SmallTest | Level1)
{
    sptr<DisplayDumper> displayDumper;
    displayDumper = new DisplayDumper(DisplayManagerService::GetInstance().abstractDisplayController_,
        DisplayManagerService::GetInstance().abstractScreenController_,
        DisplayManagerService::GetInstance().mutex_);
    int fd = 71;
    std::vector<std::u16string> args;
    const std::u16string DUMP_SCREEN = u"-s";
    const std::u16string DUMP_NUMBER = u"0";
    args.emplace_back(DUMP_SCREEN);
    args.emplace_back(DUMP_NUMBER);
    DMError ret = displayDumper->Dump(fd, args);
    ASSERT_EQ(ret, DMError::DM_ERROR_UNKNOWN);
}

/**
 * @tc.name: Dump07
 * @tc.desc: Dump two param with '-s -1'
 * @tc.type: FUNC
 */
HWTEST_F(DisplayDumperTest, Dump07, Function | SmallTest | Level1)
{
    sptr<DisplayDumper> displayDumper;
    displayDumper = new DisplayDumper(DisplayManagerService::GetInstance().abstractDisplayController_,
        DisplayManagerService::GetInstance().abstractScreenController_,
        DisplayManagerService::GetInstance().mutex_);
    int fd = 7;
    std::vector<std::u16string> args;
    const std::u16string DUMP_SCREEN = u"-s";
    const std::u16string DUMP_NUMBER = u"-1";
    args.emplace_back(DUMP_SCREEN);
    args.emplace_back(DUMP_NUMBER);
    DMError ret = displayDumper->Dump(fd, args);
    ASSERT_EQ(ret, DMError::DM_ERROR_UNKNOWN);
}

/**
 * @tc.name: Dump08
 * @tc.desc: Dump two param with '-d 1'
 * @tc.type: FUNC
 */
HWTEST_F(DisplayDumperTest, Dump08, Function | SmallTest | Level1)
{
    sptr<DisplayDumper> displayDumper;
    displayDumper = new DisplayDumper(DisplayManagerService::GetInstance().abstractDisplayController_,
        DisplayManagerService::GetInstance().abstractScreenController_,
        DisplayManagerService::GetInstance().mutex_);
    int fd = 8;
    std::vector<std::u16string> args;
    const std::u16string DUMP_DISPLAY = u"-d";
    const std::u16string DUMP_NUMBER = u"1";
    args.emplace_back(DUMP_DISPLAY);
    args.emplace_back(DUMP_NUMBER);
    DMError ret = displayDumper->Dump(fd, args);
    ASSERT_EQ(ret, DMError::DM_ERROR_UNKNOWN);
}

/**
 * @tc.name: Dump09
 * @tc.desc: Dump two param with '-d -1'
 * @tc.type: FUNC
 */
HWTEST_F(DisplayDumperTest, Dump09, Function | SmallTest | Level1)
{
    sptr<DisplayDumper> displayDumper;
    displayDumper = new DisplayDumper(DisplayManagerService::GetInstance().abstractDisplayController_,
        DisplayManagerService::GetInstance().abstractScreenController_,
        DisplayManagerService::GetInstance().mutex_);
    int fd = 9;
    std::vector<std::u16string> args;
    const std::u16string DUMP_DISPLAY = u"-d";
    const std::u16string DUMP_NUMBER = u"-1";
    args.emplace_back(DUMP_DISPLAY);
    args.emplace_back(DUMP_NUMBER);
    DMError ret = displayDumper->Dump(fd, args);
    ASSERT_EQ(ret, DMError::DM_ERROR_UNKNOWN);
}

/**
 * @tc.name: Dump10
 * @tc.desc: Dump three param with '-d -a 1'
 * @tc.type: FUNC
 */
HWTEST_F(DisplayDumperTest, Dump10, Function | SmallTest | Level1)
{
    sptr<DisplayDumper> displayDumper;
    displayDumper = new DisplayDumper(DisplayManagerService::GetInstance().abstractDisplayController_,
        DisplayManagerService::GetInstance().abstractScreenController_,
        DisplayManagerService::GetInstance().mutex_);
    int fd = 10;
    std::vector<std::u16string> args;
    const std::u16string DUMP_DISPLAY = u"-d";
    const std::u16string DUMP_ALL = u"-a";
    const std::u16string DUMP_NUMBER = u"1";
    args.emplace_back(DUMP_DISPLAY);
    args.emplace_back(DUMP_ALL);
    args.emplace_back(DUMP_NUMBER);
    DMError ret = displayDumper->Dump(fd, args);
    ASSERT_TRUE(ret == DMError::DM_OK || ret == DMError::DM_ERROR_UNKNOWN);
}

/**
 * @tc.name: IsValidDigitString01
 * @tc.desc: IsValidDigitString "06w"
 * @tc.type: FUNC
 */
HWTEST_F(DisplayDumperTest, IsValidDigitString01, Function | SmallTest | Level1)
{
    sptr<DisplayDumper> displayDumper;
    displayDumper = new DisplayDumper(DisplayManagerService::GetInstance().abstractDisplayController_,
        DisplayManagerService::GetInstance().abstractScreenController_,
        DisplayManagerService::GetInstance().mutex_);
    std::string idStr = "06w";
    bool ret = displayDumper->IsValidDigitString(idStr);
    ASSERT_EQ(ret, false);
}

/**
 * @tc.name: IsValidDigitString02
 * @tc.desc: IsValidDigitString "96+"
 * @tc.type: FUNC
 */
HWTEST_F(DisplayDumperTest, IsValidDigitString02, Function | SmallTest | Level1)
{
    sptr<DisplayDumper> displayDumper;
    displayDumper = new DisplayDumper(DisplayManagerService::GetInstance().abstractDisplayController_,
        DisplayManagerService::GetInstance().abstractScreenController_,
        DisplayManagerService::GetInstance().mutex_);
    std::string idStr = "96+";
    bool ret = displayDumper->IsValidDigitString(idStr);
    ASSERT_EQ(ret, false);
}

/**
 * @tc.name: IsValidDigitString03
 * @tc.desc: IsValidDigitString empty
 * @tc.type: FUNC
 */
HWTEST_F(DisplayDumperTest, IsValidDigitString03, Function | SmallTest | Level1)
{
    sptr<DisplayDumper> displayDumper;
    displayDumper = new DisplayDumper(DisplayManagerService::GetInstance().abstractDisplayController_,
        DisplayManagerService::GetInstance().abstractScreenController_,
        DisplayManagerService::GetInstance().mutex_);
    std::string idStr;
    bool ret = displayDumper->IsValidDigitString(idStr);
    ASSERT_EQ(ret, false);
}

/**
 * @tc.name: DumpAllScreenInfo01
 * @tc.desc: DumpAllScreenInfo
 * @tc.type: FUNC
 */
HWTEST_F(DisplayDumperTest, DumpAllScreenInfo01, Function | SmallTest | Level1)
{
    sptr<DisplayDumper> displayDumper;
    displayDumper = new DisplayDumper(DisplayManagerService::GetInstance().abstractDisplayController_,
        DisplayManagerService::GetInstance().abstractScreenController_,
        DisplayManagerService::GetInstance().mutex_);
    std::string dumpInfo;
    displayDumper->DumpAllScreenInfo(dumpInfo);
}

/**
 * @tc.name: GetDisplayInfo01
 * @tc.desc: GetDisplayInfo
 * @tc.type: FUNC
 */
HWTEST_F(DisplayDumperTest, GetDisplayInfo01, Function | SmallTest | Level1)
{
    sptr<DisplayDumper> displayDumper;
    displayDumper = new DisplayDumper(DisplayManagerService::GetInstance().abstractDisplayController_,
        DisplayManagerService::GetInstance().abstractScreenController_,
        DisplayManagerService::GetInstance().mutex_);
    std::ostringstream oss;
    displayDumper->GetDisplayInfo(nullptr, oss);
}
}
}
}
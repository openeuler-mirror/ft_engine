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

#include "string_util.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class StringUtilTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    virtual void SetUp() override;
    virtual void TearDown() override;
};

void StringUtilTest::SetUpTestCase()
{
}

void StringUtilTest::TearDownTestCase()
{
}

void StringUtilTest::SetUp()
{
}

void StringUtilTest::TearDown()
{
}

namespace {
/**
 * @tc.name: StringUtil Trim
 * @tc.desc: test trim
 * @tc.type: FUNC
 * @tc.require: issueI5M2SK
 */
HWTEST_F(StringUtilTest, Trim, Function | SmallTest | Level1)
{
    ASSERT_EQ("", StringUtil::Trim(""));
    ASSERT_EQ("", StringUtil::Trim("  "));
    ASSERT_EQ("123", StringUtil::Trim(" 123 "));
    ASSERT_EQ("12 3", StringUtil::Trim(" 12 3 "));
}
}
} // namespace Rosen
} // namespace OHOS
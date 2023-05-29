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

#include <libxml/globals.h>
#include <libxml/xmlstring.h>

#include "display_manager_config.h"
#include "display_manager_config.cpp"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class DisplayManagerConfigTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    virtual void SetUp() override;
    virtual void TearDown() override;
};

void DisplayManagerConfigTest::SetUpTestCase()
{
}

void DisplayManagerConfigTest::TearDownTestCase()
{
}

void DisplayManagerConfigTest::SetUp()
{
}

void DisplayManagerConfigTest::TearDown()
{
}

namespace {
/**
 * @tc.name: IsNumber
 * @tc.desc: test function : IsNumber
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerConfigTest, IsNumber, Function | SmallTest | Level1)
{
    bool result = DisplayManagerConfig::IsNumber("123");
    ASSERT_EQ(true, result);
    result = DisplayManagerConfig::IsNumber("a123");
    ASSERT_EQ(false, result);
}

/**
 * @tc.name: GetConfigPath
 * @tc.desc: test function : GetConfigPath
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerConfigTest, GetConfigPath, Function | SmallTest | Level1)
{
    auto result = DisplayManagerConfig::GetConfigPath("");
    ASSERT_STRNE("/system/", result.c_str());

    result = DisplayManagerConfig::GetConfigPath("a.xml");
    ASSERT_STREQ("/system/a.xml", result.c_str());
}

/**
 * @tc.name: ReadEnableConfigInfo
 * @tc.desc: test function : ReadEnableConfigInfo
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerConfigTest, ReadEnableConfigInfo, Function | SmallTest | Level1)
{
    DisplayManagerConfig::enableConfig_.clear();

    auto configFilePath = DisplayManagerConfig::GetConfigPath("etc/window/resources/display_manager_config.xml");
    xmlDocPtr docPtr = xmlReadFile(configFilePath.c_str(), nullptr, XML_PARSE_NOBLANKS);
    if (docPtr == nullptr) {
        return;
    }

    xmlNodePtr rootPtr = xmlDocGetRootElement(docPtr);
    if (rootPtr == nullptr || rootPtr->name == nullptr ||
        xmlStrcmp(rootPtr->name, reinterpret_cast<const xmlChar*>("Configs"))) {
        xmlFreeDoc(docPtr);
        return;
    }
    for (xmlNodePtr curNodePtr = rootPtr->xmlChildrenNode; curNodePtr != nullptr; curNodePtr = curNodePtr->next) {
        if (!DisplayManagerConfig::IsValidNode(*curNodePtr)) {
            continue;
        }

        auto nodeName = curNodePtr->name;
        if (!xmlStrcmp(nodeName, reinterpret_cast<const xmlChar*>("isWaterfallDisplay"))) {
            DisplayManagerConfig::ReadEnableConfigInfo(curNodePtr);
            continue;
        }

        if (!xmlStrcmp(nodeName, reinterpret_cast<const xmlChar*>("dpi"))) {
            DisplayManagerConfig::ReadEnableConfigInfo(curNodePtr);
            continue;
        }
    }

    DisplayManagerConfig::DumpConfig();
    xmlFreeDoc(docPtr);
}

/**
 * @tc.name: ReadStringConfigInfo
 * @tc.desc: test function : ReadStringConfigInfo
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerConfigTest, ReadStringConfigInfo, Function | SmallTest | Level1)
{
    DisplayManagerConfig::enableConfig_.clear();

    auto configFilePath = DisplayManagerConfig::GetConfigPath("etc/window/resources/display_manager_config.xml");
    xmlDocPtr docPtr = xmlReadFile(configFilePath.c_str(), nullptr, XML_PARSE_NOBLANKS);
    if (docPtr == nullptr) {
        return;
    }

    xmlNodePtr rootPtr = xmlDocGetRootElement(docPtr);
    if (rootPtr == nullptr || rootPtr->name == nullptr ||
        xmlStrcmp(rootPtr->name, reinterpret_cast<const xmlChar*>("Configs"))) {
        xmlFreeDoc(docPtr);
        return;
    }
    for (xmlNodePtr curNodePtr = rootPtr->xmlChildrenNode; curNodePtr != nullptr; curNodePtr = curNodePtr->next) {
        if (!DisplayManagerConfig::IsValidNode(*curNodePtr)) {
            continue;
        }

        auto nodeName = curNodePtr->name;
        if (!xmlStrcmp(nodeName, reinterpret_cast<const xmlChar*>("defaultDisplayCutoutPath"))) {
            DisplayManagerConfig::ReadStringConfigInfo(curNodePtr);
            continue;
        }

        if (!xmlStrcmp(nodeName, reinterpret_cast<const xmlChar*>("dpi"))) {
            DisplayManagerConfig::ReadStringConfigInfo(curNodePtr);
            continue;
        }
    }

    DisplayManagerConfig::DumpConfig();
    xmlFreeDoc(docPtr);
}

}
} // namespace Rosen
} // namespace OHOS
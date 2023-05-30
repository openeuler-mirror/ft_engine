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
#include "window_manager_service.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
using ConfigItem = WindowManagerConfig::ConfigItem;
const std::string XML_STR = R"(<?xml version='1.0' encoding="utf-8"?>
<Configs>
    <!--decor enable is true means app main window show decoration-->
    <decor enable="false"></decor>
    <!--max number of main window that could be shown on display-->
    <maxAppWindowNumber>100</maxAppWindowNumber>
    <!--maximum number of windows supported by unified rendering -->
    <maxUniRenderAppWindowNumber>99</maxUniRenderAppWindowNumber>
    <!--minimizeByOther enable is true means fullscreen window will be minmized by other fullscreen window-->
    <minimizeByOther enable="true"></minimizeByOther>
    <!--window mdoe change hot zones config, fullscreen primary secondary-->
    <modeChangeHotZones>50 50 50</modeChangeHotZones>
    <!--stretchable enable is true means all window be stretchable-->
    <stretchable enable="false"></stretchable>
    <!--exit split screen mode ratio config-->
    <exitSplitRatios>0.1 0.9</exitSplitRatios>
    <!--split screen ratios config-->
    <splitRatios>0.5 0.33 0.67</splitRatios>
    <!--default window mode config-->
    <defaultWindowMode>1</defaultWindowMode>
    <!--window animation config-->
    <windowAnimation>
        <timing>
            <!--duration of animation when add/remove window, unit is ms-->
            <duration>350</duration>
            <!--timing curve of animation, config it as below:
            name=ease, easeIn, easeOut, easeInOut, default, linear,
            spring, interactiveSpring, cubic(float float float float)-->
            <curve name="easeOut"></curve>
        </timing>
        <!--scaleX and scaleY of animation start state-->
        <scale>0.7 0.7</scale>
        <!--rotation of animation start state, 4 numbers is axis and angle-->
        <rotation>0 0 1 0</rotation>
        <!--translateX and translateY of animation start state-->
        <translate>0 0</translate>
        <!--opacity of animation start state-->
        <opacity>0</opacity>
    </windowAnimation>
    <!--keyboard animation config-->
    <keyboardAnimation>
        <timing>
            <!--duration of animation when add keyboard, unit is ms-->
            <durationIn>500</durationIn>
            <!--duration of animation when remove keyboard, unit is ms-->
            <durationOut>300</durationOut>
            <!--friction curve-->
            <curve name="cubic">0.2 0.0 0.2 1.0</curve>
        </timing>
    </keyboardAnimation>
    <!--enable/disable remote animation-->
    <remoteAnimation enable="true"></remoteAnimation>
    <!--window effect config-->
    <windowEffect>
        <appWindows>
            <cornerRadius>
                <!--off: no corner, defaultCornerRadiusXS: 4vp, defaultCornerRadiusS: 8vp-->
                <!--defaultCornerRadiusM: 12vp, defaultCornerRadiusL: 16vp, defaultCornerRadiusXL: 24vp-->
                <fullScreen>off</fullScreen>
                <split>off</split>
                <float>off</float>
            </cornerRadius>
            <shadow>
                <focused>
                    <elevation>0</elevation>
                    <color>#000000</color>
                    <offsetX>0</offsetX>
                    <offsetY>0</offsetY>
                    <alpha>0</alpha>
                </focused>
                <unfocused>
                    <elevation>0</elevation>
                    <color>#000000</color>
                    <offsetX>0</offsetX>
                    <offsetY>0</offsetY>
                    <alpha>0</alpha>
                </unfocused>
            </shadow>
        </appWindows>
    </windowEffect>
</Configs>
)";

class WindowManagerConfigTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    virtual void SetUp() override;
    virtual void TearDown() override;
    ConfigItem ReadConfig(const std::string& xmlStr);
};

void WindowManagerConfigTest::SetUpTestCase()
{
}

void WindowManagerConfigTest::TearDownTestCase()
{
}

void WindowManagerConfigTest::SetUp()
{
}

void WindowManagerConfigTest::TearDown()
{
}

ConfigItem WindowManagerConfigTest::ReadConfig(const std::string& xmlStr)
{
    ConfigItem config;
    xmlDocPtr docPtr = xmlParseMemory(xmlStr.c_str(), xmlStr.length() + 1);
    if (docPtr == nullptr) {
        return config;
    }

    xmlNodePtr rootPtr = xmlDocGetRootElement(docPtr);
    if (rootPtr == nullptr || rootPtr->name == nullptr ||
        xmlStrcmp(rootPtr->name, reinterpret_cast<const xmlChar*>("Configs"))) {
        xmlFreeDoc(docPtr);
        return config;
    }

    std::map<std::string, ConfigItem> configMap;
    config.SetValue(configMap);
    WindowManagerConfig::ReadConfig(rootPtr, *config.mapValue_);
    xmlFreeDoc(docPtr);
    return config;
}
namespace {
/**
 * @tc.name: AnimationConfig
 * @tc.desc: animation config test
 * @tc.type: FUNC
 * @tc.require issueI5N26H
 */
HWTEST_F(WindowManagerConfigTest, AnimationConfig, Function | SmallTest | Level2)
{
    WindowManagerConfig::config_ = ReadConfig(XML_STR);
    WindowManagerService::GetInstance().ConfigureWindowManagerService();
    ConfigItem item = WindowManagerConfig::config_["windowAnimation"];
    ASSERT_EQ(true, item.IsMap());
    item = WindowManagerConfig::config_["windowAnimation"]["timing"]["duration"];
    ASSERT_EQ(true, item.IsInts());
    auto value = *item.intsValue_;
    ASSERT_EQ(true, value.size() == 1);
    ASSERT_EQ(350, value[0]);
    item = WindowManagerConfig::config_["windowAnimation"]["timing"]["curve"].GetProp("name");
    ASSERT_EQ(true, item.IsString());
    ASSERT_EQ("easeOut", item.stringValue_);
    item = WindowManagerConfig::config_["windowAnimation"]["scale"];
    ASSERT_EQ(true, item.IsFloats());
    ASSERT_EQ(true, item.floatsValue_->size() == 2);
    item = WindowManagerConfig::config_["windowAnimation"]["rotation"];
    ASSERT_EQ(true, item.IsFloats());
    ASSERT_EQ(true, item.floatsValue_->size() == 4);
    item = WindowManagerConfig::config_["windowAnimation"]["translate"];
    ASSERT_EQ(true, item.IsFloats());
    ASSERT_EQ(true, item.floatsValue_->size() == 2);
    item = WindowManagerConfig::config_["windowAnimation"]["opacity"];
    ASSERT_EQ(true, item.IsFloats());
    ASSERT_EQ(true, item.floatsValue_->size() == 1);
}

/**
 * @tc.name: maxAppWindowNumber
 * @tc.desc: maxAppWindowNumber test
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerConfigTest, MaxAppWindowNumber, Function | SmallTest | Level2)
{
    std::string xmlStr = "<?xml version='1.0' encoding=\"utf-8\"?>"
        "<Configs>"
        "<maxAppWindowNumber>0</maxAppWindowNumber>"
        "</Configs>";
    WindowManagerConfig::config_ = ReadConfig(xmlStr);
    WindowManagerService::GetInstance().ConfigureWindowManagerService();
    ASSERT_EQ(true, WindowManagerService::GetInstance().windowRoot_->maxAppWindowNumber_ == 100);

    xmlStr = "<?xml version='1.0' encoding=\"utf-8\"?>"
        "<Configs>"
        "<maxAppWindowNumber>-2</maxAppWindowNumber>"
        "</Configs>";
    WindowManagerConfig::config_ = ReadConfig(xmlStr);
    WindowManagerService::GetInstance().ConfigureWindowManagerService();
    ASSERT_EQ(true, WindowManagerService::GetInstance().windowRoot_->maxAppWindowNumber_ == 100);

    xmlStr = "<?xml version='1.0' encoding=\"utf-8\"?>"
        "<Configs>"
        "<maxAppWindowNumber>4</maxAppWindowNumber>"
        "</Configs>";
    WindowManagerConfig::config_ = ReadConfig(xmlStr);
    WindowManagerService::GetInstance().ConfigureWindowManagerService();
    ASSERT_EQ(true, WindowManagerService::GetInstance().windowRoot_->maxAppWindowNumber_ == 4);

    xmlStr = "<?xml version='1.0' encoding=\"utf-8\"?>"
        "<Configs>"
        "<maxAppWindowNumber>1000</maxAppWindowNumber>"
        "</Configs>";
    WindowManagerConfig::config_ = ReadConfig(xmlStr);
    WindowManagerService::GetInstance().ConfigureWindowManagerService();
    ASSERT_EQ(true, WindowManagerService::GetInstance().windowRoot_->maxAppWindowNumber_ == 1000);
}

/**
 * @tc.name: decor
 * @tc.desc: set decor true and false.
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerConfigTest, Decor, Function | SmallTest | Level2)
{
    std::string xmlStr = "<?xml version='1.0' encoding=\"utf-8\"?>"
        "<Configs>"
        "<decor enable=\"true\"/>"
        "</Configs>";
    WindowManagerConfig::config_ = ReadConfig(xmlStr);
    WindowManagerService::GetInstance().ConfigureWindowManagerService();
    ASSERT_EQ(true, WindowManagerService::GetInstance().systemConfig_.isSystemDecorEnable_);

    xmlStr = "<?xml version='1.0' encoding=\"utf-8\"?>"
        "<Configs>"
        "<decor enable=\"false\"/>"
        "</Configs>";
    WindowManagerConfig::config_ = ReadConfig(xmlStr);
    WindowManagerService::GetInstance().ConfigureWindowManagerService();
    ASSERT_EQ(false, WindowManagerService::GetInstance().systemConfig_.isSystemDecorEnable_);
}
} // namespace
} // namespace Rosen
} // namespace OHOS
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
#include "display_manager_config.h"

#include <climits>
#include <cstdint>
#include <cstdlib>
#include <libxml/globals.h>
#include <libxml/xmlstring.h>
#include <map>
#include <cstring>
#include <string>
#include <utility>
#include <vector>

#include "config_policy_utils.h"
#include "window_manager_hilog.h"


namespace OHOS::Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_DISPLAY, "DisplayManagerConfig"};
}

std::map<std::string, bool> DisplayManagerConfig::enableConfig_;
std::map<std::string, std::vector<int>> DisplayManagerConfig::intNumbersConfig_;
std::map<std::string, std::string> DisplayManagerConfig::stringConfig_;

std::vector<std::string> DisplayManagerConfig::Split(std::string str, std::string pattern)
{
    std::vector<std::string> result;
    str += pattern;
    int32_t length = static_cast<int32_t>(str.size());
    for (int32_t i = 0; i < length; i++) {
        int32_t position = static_cast<int32_t>(str.find(pattern, i));
        if (position < length) {
            std::string tmp = str.substr(i, position - i);
            result.push_back(tmp);
            i = position + static_cast<int32_t>(pattern.size()) - 1;
        }
    }
    return result;
}

bool inline DisplayManagerConfig::IsNumber(std::string str)
{
    for (int32_t i = 0; i < static_cast<int32_t>(str.size()); i++) {
        if (str.at(i) < '0' || str.at(i) > '9') {
            return false;
        }
    }
    return true;
}

std::string DisplayManagerConfig::GetConfigPath(const std::string& configFileName)
{
    char buf[PATH_MAX + 1];
    char* configPath = GetOneCfgFile(configFileName.c_str(), buf, PATH_MAX + 1);
    char tmpPath[PATH_MAX + 1] = { 0 };
    if (!configPath || strlen(configPath) == 0 || strlen(configPath) > PATH_MAX || !realpath(configPath, tmpPath)) {
        WLOGFI("[DmConfig] can not get customization config file");
        return "/usr/share/ft/window_manager/" + configFileName;
    }
    return std::string(tmpPath);
}

bool DisplayManagerConfig::LoadConfigXml()
{
    auto configFilePath = GetConfigPath("etc/window/resources/display_manager_config.xml");
    xmlDocPtr docPtr = nullptr;
    {
        std::lock_guard<std::recursive_mutex> lock(mutex_);
        docPtr = xmlReadFile(configFilePath.c_str(), nullptr, XML_PARSE_NOBLANKS);
    }
    WLOGFI("[DmConfig] filePath: %{public}s", configFilePath.c_str());
    if (docPtr == nullptr) {
        WLOGFE("[DmConfig] load xml error!");
        return false;
    }

    xmlNodePtr rootPtr = xmlDocGetRootElement(docPtr);
    if (rootPtr == nullptr || rootPtr->name == nullptr ||
        xmlStrcmp(rootPtr->name, reinterpret_cast<const xmlChar*>("Configs"))) {
        WLOGFE("[DmConfig] get root element failed!");
        xmlFreeDoc(docPtr);
        return false;
    }

    for (xmlNodePtr curNodePtr = rootPtr->xmlChildrenNode; curNodePtr != nullptr; curNodePtr = curNodePtr->next) {
        if (!IsValidNode(*curNodePtr)) {
            WLOGFE("DmConfig]: invalid node!");
            continue;
        }

        auto nodeName = curNodePtr->name;
        if (!xmlStrcmp(nodeName, reinterpret_cast<const xmlChar*>("isWaterfallDisplay")) ||
            !xmlStrcmp(nodeName, reinterpret_cast<const xmlChar*>("isWaterfallAreaCompressionEnableWhenHorizontal"))) {
            ReadEnableConfigInfo(curNodePtr);
            continue;
        }
        if (!xmlStrcmp(nodeName, reinterpret_cast<const xmlChar*>("dpi")) ||
            !xmlStrcmp(nodeName, reinterpret_cast<const xmlChar*>("defaultDeviceRotationOffset")) ||
            !xmlStrcmp(nodeName, reinterpret_cast<const xmlChar*>("cutoutArea")) ||
            !xmlStrcmp(nodeName, reinterpret_cast<const xmlChar*>("curvedScreenBoundary")) ||
            !xmlStrcmp(nodeName, reinterpret_cast<const xmlChar*>("waterfallAreaCompressionSizeWhenHorzontal")) ||
            !xmlStrcmp(nodeName, reinterpret_cast<const xmlChar*>("buildInDefaultOrientation"))) {
            ReadIntNumbersConfigInfo(curNodePtr);
            continue;
        }
        if (!xmlStrcmp(nodeName, reinterpret_cast<const xmlChar*>("defaultDisplayCutoutPath"))) {
            ReadStringConfigInfo(curNodePtr);
            continue;
        }
    }
    xmlFreeDoc(docPtr);
    return true;
}

bool DisplayManagerConfig::IsValidNode(const xmlNode& currNode)
{
    if (currNode.name == nullptr || currNode.type == XML_COMMENT_NODE) {
        return false;
    }
    return true;
}

void DisplayManagerConfig::ReadIntNumbersConfigInfo(const xmlNodePtr& currNode)
{
    xmlChar* context = xmlNodeGetContent(currNode);
    if (context == nullptr) {
        WLOGFE("[DmConfig] read xml node error: nodeName:(%{public}s)", currNode->name);
        return;
    }

    std::vector<int> numbersVec;
    std::string numbersStr = reinterpret_cast<const char*>(context);
    if (numbersStr.empty()) {
        xmlFree(context);
        return;
    }
    auto numbers = Split(numbersStr, " ");
    for (auto& num : numbers) {
        if (!IsNumber(num)) {
            WLOGFE("[DmConfig] read number error: nodeName:(%{public}s)", currNode->name);
            xmlFree(context);
            return;
        }
        numbersVec.emplace_back(std::stoi(num));
    }

    std::string nodeName = reinterpret_cast<const char *>(currNode->name);
    intNumbersConfig_[nodeName] = numbersVec;
    xmlFree(context);
}

void DisplayManagerConfig::ReadEnableConfigInfo(const xmlNodePtr& currNode)
{
    xmlChar* enable = xmlGetProp(currNode, reinterpret_cast<const xmlChar*>("enable"));
    if (enable == nullptr) {
        WLOGFE("[DmConfig] read xml node error: nodeName:(%{public}s)", currNode->name);
        return;
    }

    std::string nodeName = reinterpret_cast<const char *>(currNode->name);
    if (!xmlStrcmp(enable, reinterpret_cast<const xmlChar*>("true"))) {
        enableConfig_[nodeName] = true;
    } else {
        enableConfig_[nodeName] = false;
    }
    xmlFree(enable);
}

void DisplayManagerConfig::ReadStringConfigInfo(const xmlNodePtr& currNode)
{
    xmlChar* context = xmlNodeGetContent(currNode);
    if (context == nullptr) {
        WLOGFE("[DmConfig] read xml node error: nodeName:(%{public}s)", currNode->name);
        return;
    }

    std::string inputString = reinterpret_cast<const char*>(context);
    std::string nodeName = reinterpret_cast<const char*>(currNode->name);
    stringConfig_[nodeName] = inputString;
    xmlFree(context);
}

const std::map<std::string, bool>& DisplayManagerConfig::GetEnableConfig()
{
    return enableConfig_;
}

const std::map<std::string, std::vector<int>>& DisplayManagerConfig::GetIntNumbersConfig()
{
    return intNumbersConfig_;
}

const std::map<std::string, std::string>& DisplayManagerConfig::GetStringConfig()
{
    return stringConfig_;
}

void DisplayManagerConfig::DumpConfig()
{
    for (auto& enable : enableConfig_) {
        WLOGFI("[DmConfig] Enable: %{public}s %{public}u", enable.first.c_str(), enable.second);
    }
    for (auto& numbers : intNumbersConfig_) {
        WLOGFI("[DmConfig] Numbers: %{public}s %{public}zu", numbers.first.c_str(), numbers.second.size());
        for (auto& num : numbers.second) {
            WLOGFI("[DmConfig] Num: %{public}d", num);
        }
    }
    for (auto& string : stringConfig_) {
        WLOGFI("[DmConfig] String: %{public}s", string.first.c_str());
    }
}
} // namespace OHOS::Rosen

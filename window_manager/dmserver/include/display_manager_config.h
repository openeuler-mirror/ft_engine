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

#ifndef OHOS_ROSEN_DISPLAY_MANAGER_CONFIG_H
#define OHOS_ROSEN_DISPLAY_MANAGER_CONFIG_H

#include <refbase.h>
#include "xml_config_base.h"
#include "libxml/parser.h"

namespace OHOS::Rosen {
class DisplayManagerConfig : public RefBase, public XmlConfigBase {
public:
    DisplayManagerConfig() = delete;
    ~DisplayManagerConfig() = default;

    static bool LoadConfigXml();
    static const std::map<std::string, bool>& GetEnableConfig();
    static const std::map<std::string, std::vector<int>>& GetIntNumbersConfig();
    static const std::map<std::string, std::string>& GetStringConfig();
    static void DumpConfig();

private:
    static std::map<std::string, bool> enableConfig_;
    static std::map<std::string, std::vector<int>> intNumbersConfig_;
    static std::map<std::string, std::string> stringConfig_;

    static bool IsValidNode(const xmlNode& currNode);
    static void ReadEnableConfigInfo(const xmlNodePtr& currNode);
    static void ReadIntNumbersConfigInfo(const xmlNodePtr& currNode);
    static void ReadStringConfigInfo(const xmlNodePtr& currNode);
    static std::string GetConfigPath(const std::string& configFileName);

    static std::vector<std::string> Split(std::string str, std::string pattern);
    static inline bool IsNumber(std::string str);
};
} // namespace OHOS::Rosen
#endif // OHOS_ROSEN_DISPLAY_MANAGER_CONFIG_H

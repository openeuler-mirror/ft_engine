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

#ifndef OHOS_ROSEN_WINDOW_MANAGER_CONFIG_H
#define OHOS_ROSEN_WINDOW_MANAGER_CONFIG_H

#include <refbase.h>
#include "xml_config_base.h"
#include "libxml/parser.h"
#include "libxml/tree.h"

namespace OHOS {
namespace Rosen {
class WindowManagerConfig : public RefBase, public XmlConfigBase {
public:
    WindowManagerConfig() = delete;
    ~WindowManagerConfig() = default;

    static bool LoadConfigXml();
    static const ConfigItem& GetConfig()
    {
        return config_;
    }
    static void DumpConfig(const std::map<std::string, ConfigItem>& config);

private:
    static ConfigItem config_;
    static const std::map<std::string, ValueType> configItemTypeMap_;

    static bool IsValidNode(const xmlNode& currNode);
    static void ReadProperty(const xmlNodePtr& currNode, std::map<std::string, ConfigItem>& property);
    static void ReadIntNumbersConfigInfo(const xmlNodePtr& currNode, std::vector<int>& intsValue);
    static void ReadFloatNumbersConfigInfo(const xmlNodePtr& currNode, std::vector<float>& floatsValue, bool allowNeg);
    static void ReadStringConfigInfo(const xmlNodePtr& currNode, std::string& stringValue);
    static void ReadConfig(const xmlNodePtr& rootPtr, std::map<std::string, ConfigItem>& mapValue);
    static std::string GetConfigPath(const std::string& configFileName);
    static std::vector<std::string> ReadNumberStrings(const xmlNodePtr& node);
};
} // namespace Rosen
} // namespace OHOS
#endif // OHOS_ROSEN_WINDOW_MANAGER_CONFIG_H

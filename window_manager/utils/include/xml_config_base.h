/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef OHOS_ROSEN_XML_CONFIG_BASE_H
#define OHOS_ROSEN_XML_CONFIG_BASE_H

#include <map>
#include <mutex>
#include <string>
#include <vector>

namespace OHOS {
namespace Rosen {
class XmlConfigBase {
public:
    enum class ValueType {
        UNDIFINED,
        MAP,
        BOOL,
        STRING,
        INTS,
        FLOATS,
        POSITIVE_FLOATS,
    };
    struct ConfigItem {
        std::map<std::string, ConfigItem>* property_ = nullptr;
        ValueType type_ = ValueType::UNDIFINED;
        union {
            std::map<std::string, ConfigItem>* mapValue_ = nullptr;
            bool boolValue_;
            std::string stringValue_;
            std::vector<int>* intsValue_;
            std::vector<float>* floatsValue_;
        };
        ConfigItem() {}
        ~ConfigItem()
        {
            Destroy();
        };
        void Destroy()
        {
            ClearValue();
            if (property_) {
                delete property_;
                property_ = nullptr;
            }
        }
        void ClearValue()
        {
            switch (type_) {
                case ValueType::MAP:
                    delete mapValue_;
                    mapValue_ = nullptr;
                    break;
                case ValueType::STRING:
                    stringValue_.~basic_string();
                    break;
                case ValueType::INTS:
                    delete intsValue_;
                    intsValue_ = nullptr;
                    break;
                case ValueType::FLOATS:
                    delete floatsValue_;
                    floatsValue_ = nullptr;
                    break;
                default:
                    break;
            }
        }
        ConfigItem(const ConfigItem& value)
        {
            *this = value;
        }
        ConfigItem& operator=(const ConfigItem& value)
        {
            Destroy();
            switch (value.type_) {
                case ValueType::MAP:
                    mapValue_ = new std::map<std::string, ConfigItem>(*value.mapValue_);
                    break;
                case ValueType::BOOL:
                    boolValue_ = value.boolValue_;
                    break;
                case ValueType::STRING:
                    new(&stringValue_)std::string(value.stringValue_);
                    break;
                case ValueType::INTS:
                    intsValue_ = new std::vector<int>(*value.intsValue_);
                    break;
                case ValueType::FLOATS:
                    floatsValue_ = new std::vector<float>(*value.floatsValue_);
                    break;
                default:
                    break;
            }
            type_ = value.type_;
            if (value.property_) {
                property_ = new std::map<std::string, ConfigItem>(*value.property_);
            }
            return *this;
        }
        ConfigItem(ConfigItem&& value) noexcept
        {
            *this = std::move(value);
        }
        ConfigItem& operator=(ConfigItem&& value) noexcept
        {
            Destroy();
            switch (value.type_) {
                case ValueType::MAP:
                    mapValue_ = value.mapValue_;
                    value.mapValue_ = nullptr;
                    break;
                case ValueType::BOOL:
                    boolValue_ = value.boolValue_;
                    break;
                case ValueType::STRING:
                    new(&stringValue_)std::string(std::move(value.stringValue_));
                    break;
                case ValueType::INTS:
                    intsValue_ = value.intsValue_;
                    value.intsValue_ = nullptr;
                    break;
                case ValueType::FLOATS:
                    floatsValue_ = value.floatsValue_;
                    value.floatsValue_ = nullptr;
                    break;
                default:
                    break;
            }
            type_ = value.type_;
            property_ = value.property_;
            value.type_ = ValueType::UNDIFINED;
            value.property_ = nullptr;
            return *this;
        }
        void SetProperty(const std::map<std::string, ConfigItem>& prop)
        {
            if (property_) {
                delete property_;
            }
            property_ = new std::map<std::string, ConfigItem>(prop);
        }
        // set map value
        void SetValue(const std::map<std::string, ConfigItem>& value)
        {
            ClearValue();
            type_ = ValueType::MAP;
            mapValue_ = new std::map<std::string, ConfigItem>(value);
        }
        // set bool value
        void SetValue(bool value)
        {
            ClearValue();
            type_ = ValueType::BOOL;
            boolValue_ = value;
        }
        // set string value
        void SetValue(const std::string& value)
        {
            ClearValue();
            type_ = ValueType::STRING;
            new(&stringValue_)std::string(value);
        }
        // set ints value
        void SetValue(const std::vector<int>& value)
        {
            ClearValue();
            type_ = ValueType::INTS;
            intsValue_ = new std::vector<int>(value);
        }
        // set floats value
        void SetValue(const std::vector<float>& value)
        {
            ClearValue();
            type_ = ValueType::FLOATS;
            floatsValue_ = new std::vector<float>(value);
        }
        bool IsInts() const
        {
            return type_ == ValueType::INTS;
        }
        bool IsFloats() const
        {
            return type_ == ValueType::FLOATS;
        }
        bool IsString() const
        {
            return type_ == ValueType::STRING;
        }
        bool IsBool() const
        {
            return type_ == ValueType::BOOL;
        }
        bool IsMap() const
        {
            return type_ == ValueType::MAP;
        }
        const ConfigItem& operator[](const std::string& key) const
        {
            if (type_ != ValueType::MAP) {
                return DEFAULT;
            }
            if (mapValue_->count(key) == 0) {
                return DEFAULT;
            }
            return mapValue_->at(key);
        }
        const ConfigItem& GetProp(const std::string& key) const
        {
            if (!property_) {
                return DEFAULT;
            }
            if (property_->count(key) == 0) {
                return DEFAULT;
            }
            return property_->at(key);
        }
        static const ConfigItem DEFAULT;
    };
    static std::recursive_mutex mutex_;
};
} // namespace Rosen
} // namespace OHOS

#endif // OHOS_ROSEN_XML_CONFIG_BASE_H
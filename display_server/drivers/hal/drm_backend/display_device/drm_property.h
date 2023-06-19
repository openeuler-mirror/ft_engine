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

#pragma once

#include <memory>
#include <unordered_map>
#include <vector>

#include "drm_common.h"

namespace oewm {
namespace drm {
enum class DrmPropertyType {
    DRM_PROPERTY_TYPE_RANGE,
    DRM_PROPERTY_TYPE_ENUM,
    DRM_PROPERTY_TYPE_OBJECT,
    DRM_PROPERTY_TYPE_BLOB,
    DRM_PROPERTY_TYPE_BITMASK,
    DRM_PROPERTY_TYPE_INVALID,
};

// copyable
class DrmPropertyEnum {
public:
    explicit DrmPropertyEnum(const drm_mode_property_enum &e) : value(e.value), name(e.name) {}
    ~DrmPropertyEnum() noexcept {};

    uint64_t value;
    std::string name;
};

class DrmProperty : NonCopyable {
public:
    DrmProperty(int drmFd, uint32_t propId, uint64_t propValue);
    ~DrmProperty() noexcept;
    bool operator!() const
    {
        return prop_ == nullptr;
    }
    bool operator==(std::nullptr_t) const
    {
        return prop_ == nullptr;
    }
    bool operator!=(std::nullptr_t) const
    {
        return prop_ != nullptr;
    }
    uint32_t Id() const
    {
        return propId_;
    }
    uint64_t Value() const
    {
        return value_;
    }
    uint32_t Flags() const
    {
        return flags_;
    }
    const std::string &Name() const
    {
        return name_;
    }
    const std::vector<uint64_t> &GetValues() const
    {
        return values_;
    }
    const std::vector<DrmPropertyEnum> &GetEnums() const
    {
        return enums_;
    }
    const std::vector<uint32_t> &GetBlobIds() const
    {
        return blobIds_;
    }

private:
    void ParseProp();

    drmModePropertyPtr prop_ = nullptr;

    uint32_t propId_;
    uint64_t value_;
    uint32_t flags_;
    std::string name_;
    std::vector<uint64_t> values_;
    std::vector<DrmPropertyEnum> enums_;
    std::vector<uint32_t> blobIds_;
};

class DrmObjectPropertyFetcher : NonCopyable {
public:
    DrmObjectPropertyFetcher(int drmFd, uint32_t objId, uint32_t objType);
    ~DrmObjectPropertyFetcher() noexcept;

    bool operator!() const
    {
        return props_ == nullptr;
    }
    bool operator==(std::nullptr_t) const
    {
        return props_ == nullptr;
    }
    bool operator!=(std::nullptr_t) const
    {
        return props_ != nullptr;
    }

    uint32_t GetPropId(const std::string &name) const;
    uint64_t GetPropValue(const std::string &name) const;

private:
    void BuildPropsMap();
    drmModeObjectPropertiesPtr props_ = nullptr;
    int drmFd_;
    uint32_t objId_;
    uint32_t objType_;
    std::string objInfo_;
    std::unordered_map<std::string, std::unique_ptr<DrmProperty>> propsMap_;
};
} // namespace drm
} // namespace oewm

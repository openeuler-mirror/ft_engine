/*
 * Copyright (c) 2022 Huawei Technologies Co., Ltd.
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

#include "drm_property.h"

#include "log.h"

namespace FT {
namespace drm {
DrmProperty::DrmProperty(int drmFd, uint32_t propId, uint64_t propValue)
    : prop_(::drmModeGetProperty(drmFd, propId)), propId_(propId), value_(propValue)
{
    if (prop_ == nullptr) {
        LOG_ERROR("Failed to get property of propId: %{public}" PRIu32 ", err: %{public}s", propId_, ErrnoToString(errno).c_str());
        return;
    }

    ParseProp();
}

DrmProperty::~DrmProperty() noexcept
{
    if (prop_ != nullptr) {
        ::drmModeFreeProperty(prop_);
    }
}

void DrmProperty::ParseProp()
{
    flags_ = prop_->flags;
    name_ = prop_->name;
    for (int i = 0; i < prop_->count_values; ++i) {
        values_.push_back(prop_->values[i]);
    }
    for (int i = 0; i < prop_->count_enums; ++i) {
        enums_.push_back(DrmPropertyEnum(prop_->enums[i]));
    }
    for (int i = 0; i < prop_->count_blobs; ++i) {
        blobIds_.push_back(prop_->blob_ids[i]);
    }
}

DrmObjectPropertyFetcher::DrmObjectPropertyFetcher(int drmFd, uint32_t objId, uint32_t objType)
    : props_(::drmModeObjectGetProperties(drmFd, objId, objType)),
      drmFd_(drmFd),
      objId_(objId),
      objType_(objType),
      objInfo_(ObjInfoDump(objId_, objType_))
{
    if (props_ == nullptr) {
        LOG_ERROR("Failed to get properties for %{public}s", objInfo_.c_str());
        return;
    }

    BuildPropsMap();
}

DrmObjectPropertyFetcher::~DrmObjectPropertyFetcher() noexcept
{
    if (props_ != nullptr) {
        ::drmModeFreeObjectProperties(props_);
    }
}

void DrmObjectPropertyFetcher::BuildPropsMap()
{
    for (uint32_t i = 0; i != props_->count_props; ++i) {
        auto propId = props_->props[i];
        auto propValue = props_->prop_values[i];
        auto prop = std::make_unique<DrmProperty>(drmFd_, propId, propValue);
        if (prop == nullptr) {
            continue;
        }
        propsMap_[prop->Name()] = std::move(prop);
    }
}

uint32_t DrmObjectPropertyFetcher::GetPropId(const std::string &name) const
{
    if (propsMap_.count(name) == 0) {
        LOG_WARN("Failed to get propId of name: %{public}s for %{public}s", name.c_str(), objInfo_.c_str());
        return DRM_INVALID_PROP_ID;
    }

    return propsMap_.at(name)->Id();
}

uint64_t DrmObjectPropertyFetcher::GetPropValue(const std::string &name) const
{
    if (propsMap_.count(name) == 0) {
        LOG_WARN("Failed to get propValue of name: %{public}s for %{public}s", name.c_str(), objInfo_.c_str());
        return DRM_INVLIAD_VALUE;
    }

    return propsMap_.at(name)->Value();
}
} // namespace drm
} // namespace FT

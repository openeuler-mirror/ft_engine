/*
 * Copyright (c) 2023 Huawei Technologies Co., Ltd.
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

#include "wayland_objects_pool.h"

namespace FT {
namespace Wayland {
namespace {
    constexpr HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WAYLAND, "WaylandObjectsPool"};
}

OHOS::sptr<OEObjectsPoolCallback> WaylandObjectsPool::cb_ = nullptr;
void WaylandObjectsPool::SetCallback(OHOS::sptr<OEObjectsPoolCallback> cb)
{
    cb_ = cb;
}

void WaylandObjectsPool::AddObject(ObjectId id, const OHOS::sptr<OEResourceObject> &object)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (objects_.count(id) > 0) {
        LOG_WARN("object already exists");
    }

    objects_[id] = object;
}

void WaylandObjectsPool::RemoveObject(ObjectId id, const OHOS::sptr<OEResourceObject> &object)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (objects_.count(id) == 0) {
        LOG_WARN("object already removed");
        return;
    }

    const auto &objInPool = objects_.at(id);
    if (objInPool != object) {
        LOG_ERROR("invalid id");
        return;
    }

    objects_.erase(id);

    if (cb_ != nullptr) {
        cb_->OnDestroy(id);
    }
}

OHOS::sptr<OEResourceObject> WaylandObjectsPool::GetObject(ObjectId id) const
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (objects_.count(id) == 0) {
        LOG_WARN("object does not exist");
        return nullptr;
    }

    return objects_.at(id);
}
} // namespace Wayland
} // namespace FT

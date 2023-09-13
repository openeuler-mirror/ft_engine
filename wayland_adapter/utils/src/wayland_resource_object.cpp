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

#include "wayland_resource_object.h"

#include "wayland_objects_pool.h"

namespace FT {
namespace Wayland {
namespace {
    constexpr HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WAYLAND, "WaylandResourceObject"};
}

WaylandResourceObject::WaylandResourceObject(struct wl_client *client, const struct wl_interface *interface,
    uint32_t version, uint32_t id, void *implementation)
    : client_(client),
      display_(wl_client_get_display(client_)),
      interface_(interface),
      version_(version),
      implementation_(implementation)
{
    resource_ = wl_resource_create(client_, interface_, version_, id);
    if (resource_ == nullptr) {
        LOG_ERROR("no memory");
        wl_client_post_no_memory(client_);
        return;
    }

    id_ = wl_resource_get_id(resource_);
    wl_resource_set_implementation(resource_, implementation_, this, &WaylandResourceObject::OnDestroy);
    name_ = std::string(interface_->name) + "_" + std::to_string(version_) + "_" + std::to_string(id_);
    LOG_DEBUG("create WaylandResourceObject, name=%{public}s, id=%{public}d", name_.c_str(), id_);
}

WaylandResourceObject::~WaylandResourceObject() noexcept
{
    if (resource_ != nullptr) {
        wl_resource_destroy(resource_);
    }
}

void WaylandResourceObject::DefaultDestroyResource(struct wl_client *client, struct wl_resource *resource)
{
    auto object = CastFromResource<WaylandResourceObject>(resource);
    if (object == nullptr) {
        LOG_WARN("object is nullptr");
        return;
    }

    object->resource_ = nullptr;
    wl_resource_destroy(resource);
}

void WaylandResourceObject::OnDestroy(struct wl_resource *resource)
{
    auto object = CastFromResource<WaylandResourceObject>(resource);
    if (object == nullptr) {
        LOG_WARN("object is nullptr");
        return;
    }

    object->OnResourceDestroy();
    object->resource_ = nullptr;
    WaylandObjectsPool::GetInstance().RemoveObject(ObjectId(object->WlClient(), object->Id()), object);
}

bool WaylandResourceObject::CheckIfObjectIsValid(const OHOS::sptr<WaylandResourceObject> &object)
{
    if (object == nullptr) {
        LOG_ERROR("object is nullptr");
        return false;
    }

    auto objId = ObjectId(object->WlClient(), object->Id());
    auto objInPool = WaylandObjectsPool::GetInstance().GetObject(objId);
    if (objInPool != object) {
        LOG_ERROR("CheckIfObjectIsValid failed");
        return false;
    }

    return true;
}
} // namespace Wayland
} // namespace FT

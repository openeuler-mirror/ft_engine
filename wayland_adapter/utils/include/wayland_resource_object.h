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

#pragma once

#include <string>
#include "wayland-server-core.h"
#include "wayland_adapter_hilog.h"
#include "noncopyable_hal.h"
#include "refbase.h"
#include "types.h"

namespace FT {
namespace Wayland {
struct ObjectId {
    ObjectId(struct wl_client *client, uint32_t id) : client(client), id(id) {}
    ~ObjectId() noexcept = default;

    ObjectId(const ObjectId &other) = default;
    ObjectId &operator=(const ObjectId &other) = default;

    ObjectId(ObjectId &&other) noexcept : client(other.client), id(other.id)
    {
        other.client = nullptr;
        other.id = 0;
    }
    ObjectId &operator=(ObjectId &&other) noexcept
    {
        client = other.client;
        id = other.id;
        other.client = nullptr;
        other.id = 0;
        return *this;
    }
    bool operator<(const ObjectId &other) const
    {
        if (client != other.client) {
            return client < other.client;
        }
        return id < other.id;
    }
    bool operator==(const ObjectId &other) const
    {
        return client == other.client && id == other.id;
    }
    bool operator!=(const ObjectId &other) const
    {
        return !(*this == other);
    }

    struct wl_client *client = nullptr;
    uint32_t id = 0;
};

template <typename OutStream>
inline OutStream &operator<<(OutStream &os, const ObjectId &objId)
{
    os << "ObjectId(client: " << objId.client << ", id: " << objId.id << ")";
    return os;
}

class OEResourceObject : NonCopyable, virtual public OHOS::RefBase {
public:
    OEResourceObject(struct wl_client *client, const struct wl_interface *interface,
        uint32_t version, uint32_t id, void *implementation);
    virtual ~OEResourceObject() noexcept override;

    const std::string &Name() const
    {
        return name_;
    }
    uint32_t Id() const
    {
        return id_;
    }
    uint32_t Version() const
    {
        return version_;
    }
    struct wl_client *WlClient() const
    {
        return client_;
    }
    struct wl_display *WlDisplay() const
    {
        return display_;
    }
    struct wl_resource *WlResource() const
    {
        return resource_;
    }

    static bool CheckIfObjectIsValid(const OHOS::sptr<OEResourceObject> &object);
    static void DefaultDestroyResource(struct wl_client *client, struct wl_resource *resource);

protected:
    static void OnDestroy(struct wl_resource *resource);
    virtual void OnResourceDestroy() {}

    struct wl_client *client_ = nullptr;
    struct wl_display *display_ = nullptr;
    const struct wl_interface *interface_ = nullptr;
    uint32_t version_ = 0;
    uint32_t id_ = 0;
    void *implementation_ = nullptr;
    std::string name_;
    struct wl_resource *resource_ = nullptr;
};

namespace detail {
template <typename T>
struct HasFuncDefaultDestroyResource {
    template <typename U>
    static char check(decltype(&U::DefaultDestroyResource));
    template <typename U>
    static int32_t check(...);
    const static bool value = sizeof(check<T>(0)) == sizeof(char);
};
} // detail

template <typename T>
inline OHOS::sptr<T> CastFromResource(struct wl_resource *resource)
{
    if (resource == nullptr) {
        return nullptr;
    }

    static_assert(detail::HasFuncDefaultDestroyResource<T>::value,
        "Can't cast wl_resource to the type which is neither an OEResourceObject nor the derived type of it.");

    auto wptrObj = OHOS::wptr<T>(DownCast<T *>(wl_resource_get_user_data(resource)));
    return wptrObj.promote();
}

#define OBJECT_CHECK(object, errlog)                                                                                   \
    if (!OEResourceObject::CheckIfObjectIsValid(object)) {                                                             \
        LOG_WARN(errlog);                                                                                            \
        return;                                                                                                        \
    }

#define CAST_OBJECT_AND_CALL_FUNC(objectType, resource, errlog, func, args...)                                         \
    auto object = CastFromResource<objectType>((resource));                                                            \
    OBJECT_CHECK(object, errlog);                                                                                      \
    object->func(args);
} // namespace Wayland
} // namespace FT

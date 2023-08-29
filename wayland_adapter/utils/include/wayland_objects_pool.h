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

#include <map>
#include <functional>

#include "wayland_singleton.h"
#include "wayland_resource_object.h"

namespace FT {
namespace Wayland {
class WaylandObjectsPoolCallback : public OHOS::RefBase {
public:
    virtual void OnDestroy(ObjectId objectId) {}
};

class WaylandObjectsPool : public Singleton<WaylandObjectsPool> {
    DECLARE_SINGLETON(WaylandObjectsPool)

public:
    static void SetCallback(OHOS::sptr<WaylandObjectsPoolCallback> cb);
    void AddObject(ObjectId id, const OHOS::sptr<WaylandResourceObject> &object);
    void RemoveObject(ObjectId id, const OHOS::sptr<WaylandResourceObject> &object);
    OHOS::sptr<WaylandResourceObject> GetObject(ObjectId id) const;

private:
    WaylandObjectsPool() = default;
    ~WaylandObjectsPool() noexcept override = default;

    static OHOS::sptr<WaylandObjectsPoolCallback> cb_;
    mutable std::mutex mutex_;
    std::map<ObjectId, OHOS::sptr<WaylandResourceObject>> objects_;
};
} // namespace Wayland
} // namespace FT

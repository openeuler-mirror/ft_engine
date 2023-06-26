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

#pragma once

#include <cstddef>
#include <cstring>

#include <libudev.h>

#include "types.h"

namespace oewm {
static constexpr char defaultSeat[] = "seat0";

// RAII for udev objects to do auto unref.
template <typename T>
class UdevObject : NonCopyable {
public:
    using UdevObjectUnRefFunc = T *(*)(T *);
    using Pointer = T *;

public:
    UdevObject() = default;
    UdevObject(Pointer obj, UdevObjectUnRefFunc unrefFunc) : obj_(obj), unrefFunc_(unrefFunc) {}
    ~UdevObject() noexcept
    {
        if (OE_LIKELY(obj_ != nullptr) && OE_LIKELY(unrefFunc_ != nullptr)) {
            unrefFunc_(obj_);
        }
    }

    Pointer Get() const
    {
        return obj_;
    }
    bool operator==(std::nullptr_t) const
    {
        return obj_ == nullptr;
    }
    bool operator!=(std::nullptr_t) const
    {
        return obj_ != nullptr;
    }

private:
    Pointer obj_ = nullptr;
    UdevObjectUnRefFunc unrefFunc_ = nullptr;
};

inline bool IsBootVGADevice(struct udev_device *device)
{
    struct udev_device *pciDevice = udev_device_get_parent_with_subsystem_devtype(device, "pci", nullptr);
    if (pciDevice == nullptr) {
        return false;
    }

    const char *bootVGAId = udev_device_get_sysattr_value(pciDevice, "boot_vga");
    if (bootVGAId == nullptr) {
        return false;
    }

    return strcmp(bootVGAId, "1") == 0;
}
} // namespace oewm

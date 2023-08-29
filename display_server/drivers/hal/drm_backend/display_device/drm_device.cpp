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

#include "drm_device.h"

#include <fcntl.h>
#include <optional>

#include "drm_display.h"
#include "log.h"

namespace FT {
namespace drm {
std::shared_ptr<DrmDevice> DrmDevice::Create(std::string devicePath)
{
    std::shared_ptr<DrmDevice> device(new DrmDevice(std::move(devicePath)));
    if (!device->Init()) {
        return nullptr;
    }

    return device;
}

DrmDevice::DrmDevice(std::string devicePath)
    : devicePath_(std::move(devicePath)), fd_(::open(devicePath_.c_str(), O_RDWR | O_CLOEXEC))
{}

DrmDevice::~DrmDevice() noexcept
{
    if (drmResource_ != nullptr) {
        drmModeFreeResources(drmResource_);
    }

#ifdef DRM_BACKEND_USE_GBM
    if (gbmDevice_ != nullptr) {
        gbm_device_destroy(gbmDevice_);
    }
#endif // DRM_BACKEND_USE_GBM
}

void DrmDevice::SetupAllCrtcs()
{
    for (int i = 0; i != drmResource_->count_crtcs; ++i) {
        auto crtcId = drmResource_->crtcs[i];
        crtcs_[crtcId] = std::make_shared<DrmCrtc>(fd_, crtcId, i);
    }
}

void DrmDevice::SetupAllConnectors()
{
    for (int i = 0; i != drmResource_->count_connectors; ++i) {
        auto connectorId = drmResource_->connectors[i];
        connectors_[connectorId] = std::make_shared<DrmConnector>(fd_, connectorId);
    }
}

void DrmDevice::SetupAllEncoders()
{
    for (int i = 0; i != drmResource_->count_encoders; ++i) {
        auto encoderId = drmResource_->encoders[i];
        encoders_[encoderId] = std::make_shared<DrmEncoder>(fd_, encoderId);
    }
}

void DrmDevice::SetupAllPlanes()
{
    drmModePlaneResPtr planeRes = drmModeGetPlaneResources(fd_);
    if (planeRes == nullptr) {
        LOG_ERROR("Failed to get plane resources.");
        return;
    }
    for (uint32_t i = 0; i != planeRes->count_planes; ++i) {
        auto planeId = planeRes->planes[i];
        planes_[planeId] = std::make_shared<DrmPlane>(fd_, planeId);
    }
}

bool DrmDevice::InitKmsCaps()
{
    uint64_t cap = DRM_INVLIAD_VALUE;
    /**
     * DRM_CAP_TIMESTAMP_MONOTONIC
     * Starting from kernel version 2.6.39, the default value for this capability
     * is 1. Starting kernel version 4.15, this capability is always set to 1.
     */
    int ret = drmGetCap(fd_, DRM_CAP_TIMESTAMP_MONOTONIC, &cap);
    if (ret != 0 || cap != 1) {
        LOG_ERROR("%{public}s DRM KMS does not support DRM_CAP_TIMESTAMP_MONOTONIC!", devicePath_.c_str());
        return false;
    }

    ret = drmSetClientCap(fd_, DRM_CLIENT_CAP_UNIVERSAL_PLANES, 1);
    if (ret) {
        LOG_ERROR("%{public}s DRM KMS doesn't support universal planes! error: %{public}s",
            devicePath_.c_str(), ErrnoToString(errno).c_str());
        return false;
    }

    ret = drmGetCap(fd_, DRM_CAP_DUMB_BUFFER, &cap);
    if (ret != 0 || cap == 0) {
        LOG_ERROR("%{public}s DRM KMS doesn't support dumb buffers! error: %{public}s",
            devicePath_.c_str(), ErrnoToString(errno).c_str());
    }
    supportDumbBuffer_ = (cap != 0);

    if (::getenv("OEWM_DISABLE_ATOMIC") == nullptr) {
        ret = drmGetCap(fd_, DRM_CAP_CRTC_IN_VBLANK_EVENT, &cap);
        if (ret != 0) {
            cap = 0;
        }
        ret = drmSetClientCap(fd_, DRM_CLIENT_CAP_ATOMIC, 1);
        // we will use legacy method to present fb to screens if supportAtomicModeSet_ = false.
        supportAtomicModeSet_ = ((ret == 0) && (cap != 0));
        LOG_DEBUG("%{public}s supportAtomicModeSet: %{public}i", devicePath_.c_str(), supportAtomicModeSet_);
    }

    if (::getenv("OEWM_DISABLE_GBM_MODIFIERS") == nullptr) {
        ret = drmGetCap(fd_, DRM_CAP_ADDFB2_MODIFIERS, &cap);
        if (ret == 0) {
            supportGbmModifiers_ = (cap != 0);
        }
    }

    ret = drmGetCap(fd_, DRM_PRIME_CAP_IMPORT, &cap);
    if (ret == 0) {
        supportPrimeImport_ = (cap != 0);
    }

    ret = drmGetCap(fd_, DRM_PRIME_CAP_EXPORT, &cap);
    if (ret == 0) {
        supportPrimeExport_ = (cap != 0);
    }

    ret = drmSetClientCap(fd_, DRM_CLIENT_CAP_WRITEBACK_CONNECTORS, 1);
    if (ret != 0) {
        LOG_WARN("%{public}s DRM KMS doesn't writeback connectors. error: %{public}s", devicePath_.c_str(), ErrnoToString(errno).c_str());
    }

    ret = drmSetClientCap(fd_, DRM_CLIENT_CAP_ASPECT_RATIO, 1);
    if (ret == 0) {
        supportAspectRatio_ = true;
    }

    ret = drmGetCap(fd_, DRM_CAP_CURSOR_WIDTH, &cap);
    if (ret == 0) {
        cursorWidth_ = static_cast<int>(cap);
    }

    ret = drmGetCap(fd_, DRM_CAP_CURSOR_HEIGHT, &cap);
    if (ret == 0) {
        cursorHeight_ = static_cast<int>(cap);
    }

    return true;
}

bool DrmDevice::Init()
{
    LOG_DEBUG("DrmDevice::Init");

    if (IsInvalidFd(fd_)) {
        LOG_ERROR("DrmDevice::Init: failed to open drm_device: %{public}s", devicePath_.c_str());
        return false;
    }

#ifdef DRM_BACKEND_USE_GBM
    gbmDevice_ = gbm_create_device(fd_);
    if (gbmDevice_ == nullptr) {
        LOG_ERROR("DrmDevice::Init: failed to create gbm device.");
        return false;
    }
#endif // DRM_BACKEND_USE_GBM

    drmResource_ = drmModeGetResources(fd_);
    if (drmResource_ == nullptr) {
        return false;
    }

    if (drmResource_->count_crtcs <= 0 || drmResource_->count_connectors <= 0 || drmResource_->count_encoders <= 0) {
        LOG_ERROR("Drm device %{public}s is invalid.", devicePath_.c_str());
        return false;
    }

    if (!InitKmsCaps()) {
        return false;
    }

    SetupAllCrtcs();
    SetupAllConnectors();
    SetupAllEncoders();
    SetupAllPlanes();
    DiscoveryDisplays();

    LOG_DEBUG("DrmDevice::Init: done.");

    return true;
}

std::shared_ptr<HdiDisplay> DrmDevice::BuildHdiDisplayFromConnector(
    uint32_t connectorIndex,
    const std::shared_ptr<DrmConnector> &connector)
{
    uint32_t crtcId = 0;
    if (!connector->FindCrtcId(encoders_, crtcs_, crtcId)) {
        return nullptr;
    }

    const auto &crtc = crtcs_.at(crtcId);
    auto displayId = static_cast<HdiDisplayId>(connectorIndex);
    crtc->BindToDisplay(displayId);
    auto display = std::make_shared<DrmDisplay>(fd_, displayId, connector, crtc);
    if (!display->Init()) {
        LOG_ERROR("DrmDevice::BuildHdiDisplayFromConnector display init failed");
        return nullptr;
    }

    for (const auto &[planeId, plane] : planes_) {
        UNUSED(planeId);
        if (plane->GetPossibleCrtcs() & (1 << crtc->Pipe()) && plane->GetPlaneType() == DRM_PLANE_TYPE_PRIMARY) {
            display->SetPrimaryPlane(plane);
        }
    }

    return display;
}

void DrmDevice::DiscoveryDisplays()
{
    for (int i = 0; i != drmResource_->count_connectors; ++i) {
        auto connectorId = drmResource_->connectors[i];
        if (connectors_.count(connectorId) == 0) {
            LOG_ERROR("DrmDevice::DiscoveryDisplays: unexpected error: can not find connector for conId %{public}" PRIu32 ".",
                      connectorId);
            continue;
        }

        const auto &connector = connectors_.at(connectorId);
        if (!connector->Connected()) {
            LOG_INFO("DrmDevice::DiscoveryDisplays: ignore unused connector %{public}" PRIu32 ".",
                connectorId);
            continue;
        }

        auto display = BuildHdiDisplayFromConnector(static_cast<uint32_t>(i), connector);
        if (display != nullptr) {
            std::lock_guard<std::mutex> lock(mutex_);
            LOG_INFO("DrmDevice::DiscoveryDisplays: added display(%{public}u).", display->Id());
            displays_[display->Id()] = std::move(display);
        }
    }
}
} // namespace drm
} // namespace FT

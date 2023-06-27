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

#include <mutex>
#include <unordered_map>

#ifdef DRM_BACKEND_USE_GBM
#include <gbm.h>
#endif // DRM_BACKEND_USE_GBM

#include "hdi_display.h"
#include "unique_fd.h"
#include "drm_crtc.h"
#include "drm_connector.h"
#include "drm_encoder.h"
#include "drm_plane.h"

namespace oewm {
namespace drm {
class DrmDevice : private NonCopyable {
public:
    static std::shared_ptr<DrmDevice> Create(std::string devicePath);
    ~DrmDevice() noexcept;

    int Fd() const
    {
        return fd_;
    }

    drmModeResPtr GetModeResources() const
    {
        return drmResource_;
    }

    const std::string &DevicePath() const
    {
        return devicePath_;
    }

    bool SupportDumbBuffer() const
    {
        return supportDumbBuffer_;
    }

    bool SupportAtomicModeSet() const
    {
        return supportAtomicModeSet_;
    }

    bool SupportGbmModifiers() const
    {
        return supportGbmModifiers_;
    }

    bool SupportAspectRatio() const
    {
        return supportAspectRatio_;
    }

    bool SupportPrimeImport() const
    {
        return supportPrimeImport_;
    }

    bool SupportPrimeExport() const
    {
        return supportPrimeExport_;
    }

    // TODO: SetCursorSize
    int GetCursorWidth() const
    {
        return cursorWidth_;
    }
    int GetCursorHeight() const
    {
        return cursorHeight_;
    }

    const std::unordered_map<HdiDisplayId, std::shared_ptr<HdiDisplay>> &GetDisplays() const
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return displays_;
    }
    std::unordered_map<HdiDisplayId, std::shared_ptr<HdiDisplay>> &GetMutableDisplays()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return displays_;
    }

#ifdef DRM_BACKEND_USE_GBM
    struct gbm_device *GetGbmDevice() const
    {
        return gbmDevice_;
    }
#endif // DRM_BACKEND_USE_GBM

private:
    explicit DrmDevice(std::string devicePath);
    bool InitKmsCaps();
    void SetupAllCrtcs();
    void SetupAllConnectors();
    void SetupAllEncoders();
    void SetupAllPlanes();
    std::shared_ptr<HdiDisplay> BuildHdiDisplayFromConnector(
        uint32_t connectorIndex,
        const std::shared_ptr<DrmConnector> &connector);
    void DiscoveryDisplays();
    bool Init();

    std::string devicePath_;
    OHOS::UniqueFd fd_;

#ifdef DRM_BACKEND_USE_GBM
    struct gbm_device *gbmDevice_ = nullptr;
#endif // DRM_BACKEND_USE_GBM

    static constexpr int DEFAULT_CURSOR_WIDTH = 64;
    static constexpr int DEFAULT_CURSOR_HEIGHT = 64;
    int cursorWidth_ = DEFAULT_CURSOR_WIDTH;
    int cursorHeight_ = DEFAULT_CURSOR_HEIGHT;

    drmModeResPtr drmResource_ = nullptr;

    // drm capabilities
    bool supportAtomicModeSet_ = false;
    bool supportDumbBuffer_ = false;
    bool supportGbmModifiers_ = false;
    bool supportPrimeImport_ = false;
    bool supportPrimeExport_ = false;
    bool supportAspectRatio_ = false;

    IdMapPtr<DrmCrtc> crtcs_;
    IdMapPtr<DrmConnector> connectors_;
    IdMapPtr<DrmEncoder> encoders_;
    IdMapPtr<DrmPlane> planes_;

    mutable std::mutex mutex_;
    IdMapPtr<HdiDisplay> displays_; // guarded by mutex_;
};
} // namespace drm
} // namespace oewm

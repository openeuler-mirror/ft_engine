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

#include <memory>
#include <vector>

#include "display_type.h"
#include "drm_mode_info.h"
#include "drm_encoder.h"
#include "drm_crtc.h"

namespace FT {
namespace drm {
// prop for connector
constexpr char PROP_CRTCID[] = "CRTC_ID";
constexpr char PROP_DPMS[] = "DPMS";
constexpr char PROP_BRIGHTNESS[] = "brightness";
constexpr int DEFAULT_RESOLUTION_WIDTH = 1280;
constexpr int DEFAULT_RESOLUTION_HEIGHT = 800;

class DrmConnector : NonCopyable {
public:
    DrmConnector(int drmFd, uint32_t connectorId);
    ~DrmConnector() noexcept;

    uint32_t Id() const
    {
        return id_;
    }

    uint32_t EncoderId() const
    {
        return encoderId_;
    }

    uint32_t ConnType() const
    {
        return connType_;
    }

    uint32_t ConnTypeId() const
    {
        return connTypeId_;
    }

    bool Connected() const
    {
        return connStatus_ == DRM_MODE_CONNECTED;
    }

    // physical width in millimetres
    uint32_t PhyWidth() const
    {
        return phyWidth_;
    }

    // physical height in millimetres
    uint32_t PhyHeight() const
    {
        return phyHeight_;
    }

    uint32_t CrtcPropId() const
    {
        return crtcPropId_;
    }

    uint32_t DpmsPropId() const
    {
        return dpmsPropId_;
    }

    uint64_t Dpms() const
    {
        return dpms_;
    }

    uint32_t BrightnessPropId() const
    {
        return brightnessPropId_;
    }

    uint64_t brightness() const
    {
        return brightness_;
    }

    uint64_t BlobId() const;

    void GetDisplayCapability(DisplayCapability *cap) const;
    void GetSupportedModes(uint32_t *num, DisplayModeInfo *modes) const;
    uint32_t GetActiveModeId() const;
    bool SetActiveModeId(uint32_t modeId);
    void SetDefaultActiveMode();
    // display power manager status
    uint64_t GetDpms() const
    {
        return dpms_;
    }
    bool SetDpms(uint64_t dpms);
    uint64_t GetBrightness() const
    {
        return brightness_;
    }
    bool SetBrightness(uint64_t brightness);

    bool FindCrtcId(const IdMapPtr<DrmEncoder> &encoders, const IdMapPtr<DrmCrtc> &crtcs, uint32_t &crtcId);
    bool FindPlane(const IdMapPtr<DrmEncoder> &encoders, const IdMapPtr<DrmCrtc> &crtcs, uint32_t &crtcId);
    void GetMode(uint32_t modeId, drmModeModeInfo *mode) const
    {
        *mode = modes_[modeId]->ModeInfo();
    }

private:
    static InterfaceType ToHdiConnType(uint32_t drmConnType);
    static std::string TypeToName(uint32_t connType, uint32_t connTypeId);
    void InitModes(const drmModeConnectorPtr &conn);
    void ParseFrom(const drmModeConnectorPtr &conn);

    int drmFd_ = INVALID_FD;
    uint32_t id_ = DRM_INVALID_OBJECT_ID;
    uint32_t encoderId_ = DRM_INVALID_OBJECT_ID;
    std::vector<uint32_t> possibleEncoders_;
    uint32_t connType_ = DRM_MODE_CONNECTOR_Unknown;
    uint32_t connTypeId_ = DRM_INVALID_OBJECT_ID;
    drmModeConnection connStatus_ = DRM_MODE_CONNECTED;
    uint32_t phyWidth_ = DRM_INVLIAD_SIZE;
    uint32_t phyHeight_ = DRM_INVLIAD_SIZE;
    drmModeSubPixel subPixel_;
    uint32_t crtcPropId_ = DRM_INVALID_PROP_ID;
    uint32_t dpmsPropId_ = DRM_INVALID_PROP_ID;
    uint64_t dpms_ = DRM_INVLIAD_VALUE;
    uint32_t brightnessPropId_ = DRM_INVALID_PROP_ID;
    uint64_t brightness_ = DRM_INVLIAD_VALUE;
    std::vector<std::unique_ptr<DrmModeInfo>> modes_;
    uint32_t activeModeId_ = -1;
};
} // namespace drm
} // namespace FT

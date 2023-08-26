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

#include "drm_connector.h"

#include "display_type.h"
#include "drm_mode.h"
#include "log.h"
#include "drm_property.h"

namespace oewm {
namespace drm {
DrmConnector::DrmConnector(int drmFd, uint32_t connectorId) : drmFd_(drmFd), id_(connectorId)
{
    auto conn = drmModeGetConnector(drmFd_, id_);
    if (conn == nullptr) {
        LOG_ERROR("Failed to get drm connector for connector id: %{public}u", id_);
        return;
    }
    ParseFrom(conn);
    drmModeFreeConnector(conn);
    SetDefaultActiveMode();
}

DrmConnector::~DrmConnector() noexcept {}

bool DrmConnector::FindCrtcId(const IdMapPtr<DrmEncoder> &encoders, const IdMapPtr<DrmCrtc> &crtcs, uint32_t &crtcId)
{
    if (encoders.count(encoderId_) != 0) {
        if (encoders.at(encoderId_)->FindCrtcId(crtcs, crtcId)) {
            return true;
        }
    }

    for (auto encoderId : possibleEncoders_) {
        if (encoders.count(encoderId) != 0) {
            if (encoders.at(encoderId)->FindCrtcId(crtcs, crtcId)) {
                return true;
            }
        }
    }

    LOG_ERROR("cannot find a suitable crtc for connector %{public}u", id_);
    return false;
}

InterfaceType DrmConnector::ToHdiConnType(uint32_t drmConnType)
{
    switch (drmConnType) {
        case DRM_MODE_CONNECTOR_VGA:
            return DISP_INTF_VGA;
        case DRM_MODE_CONNECTOR_DSI:
            return DISP_INTF_MIPI;
        case DRM_MODE_CONNECTOR_HDMIA:
        case DRM_MODE_CONNECTOR_HDMIB:
            return DISP_INTF_HDMI;
        default:
            return DISP_INTF_BUTT;
    }
}

std::string DrmConnector::TypeToName(uint32_t connType, uint32_t connTypeId)
{
    static std::unordered_map<uint32_t, std::string> connTypeNames = {
        {DRM_MODE_CONNECTOR_Unknown, "Unknown"},
        {DRM_MODE_CONNECTOR_VGA, "VGA"},
        {DRM_MODE_CONNECTOR_DVII, "DVI-I"},
        {DRM_MODE_CONNECTOR_DVID, "DVI-D"},
        {DRM_MODE_CONNECTOR_DVIA, "DVI-A"},
        {DRM_MODE_CONNECTOR_Composite, "Composite"},
        {DRM_MODE_CONNECTOR_SVIDEO, "SVIDEO"},
        {DRM_MODE_CONNECTOR_LVDS, "LVDS"},
        {DRM_MODE_CONNECTOR_Component, "Component"},
        {DRM_MODE_CONNECTOR_9PinDIN, "DIN"},
        {DRM_MODE_CONNECTOR_DisplayPort, "DP"},
        {DRM_MODE_CONNECTOR_HDMIA, "HDMI-A"},
        {DRM_MODE_CONNECTOR_HDMIB, "HDMI-B"},
        {DRM_MODE_CONNECTOR_TV, "TV"},
        {DRM_MODE_CONNECTOR_eDP, "eDP"},
        {DRM_MODE_CONNECTOR_VIRTUAL, "Virtual"},
        {DRM_MODE_CONNECTOR_DSI, "DSI"},
        {DRM_MODE_CONNECTOR_DPI, "DPI"},
        {DRM_MODE_CONNECTOR_WRITEBACK, "WriteBack"}};
    // {DRM_MODE_CONNECTOR_SPI, "SPI"},
    // {DRM_MODE_CONNECTOR_USB, "USB"}};

    std::string id = std::to_string(connTypeId);
    if (connTypeNames.count(connType) == 0) {
        return "Unknown-" + id;
    }

    return connTypeNames[connType] + "-" + id;
}

void DrmConnector::InitModes(drmModeConnectorPtr const &conn)
{
    for (int i = 0; i != conn->count_modes; ++i) {
        modes_.push_back(std::make_unique<DrmModeInfo>(conn->modes[i], i, drmFd_));
    }
}

void DrmConnector::ParseFrom(const drmModeConnectorPtr &conn)
{
    encoderId_ = conn->encoder_id;
    for (int i = 0; i < conn->count_encoders; i++) {
        possibleEncoders_.push_back(conn->encoders[i]);
    }
    connType_ = conn->connector_type;
    connTypeId_ = conn->connector_type_id;
    connStatus_ = conn->connection;
    phyWidth_ = conn->mmWidth;
    phyHeight_ = conn->mmHeight;
    subPixel_ = conn->subpixel;
    InitModes(conn);
    DrmObjectPropertyFetcher connPropFetcher(drmFd_, id_, DRM_MODE_OBJECT_CONNECTOR);
    crtcPropId_ = connPropFetcher.GetPropId(PROP_CRTCID);
    dpmsPropId_ = connPropFetcher.GetPropId(PROP_DPMS);
    dpms_ = connPropFetcher.GetPropValue(PROP_DPMS);
    brightnessPropId_ = connPropFetcher.GetPropId(PROP_BRIGHTNESS);
    brightness_ = connPropFetcher.GetPropValue(PROP_BRIGHTNESS);
}

uint64_t DrmConnector::BlobId() const
{
    if (OE_UNLIKELY(activeModeId_ > modes_.size())) {
        return DRM_INVALID_OBJECT_ID;
    }

    return modes_[activeModeId_]->BlobId();
}

void DrmConnector::GetDisplayCapability(DisplayCapability *cap) const
{
    auto typeName = TypeToName(connType_, connTypeId_);
    std::move(typeName.begin(), typeName.end(), cap->name);
    cap->type = ToHdiConnType(connType_);
    cap->phyWidth = phyWidth_;
    cap->phyHeight = phyHeight_;

    // these are not supported yet.
    cap->supportLayers = 0;
    cap->virtualDispCount = 0;
    cap->propertyCount = 0;
    cap->props = nullptr;
}

void DrmConnector::GetSupportedModes(uint32_t *num, DisplayModeInfo *modes) const
{
    *num = static_cast<uint32_t>(modes_.size());

    if (modes == nullptr) {
        return;
    }
    for (uint32_t i = 0; i != *num; ++i) {
        modes[i] = modes_[i]->ToHdiModeInfo();
    }
}

bool DrmConnector::SetActiveModeId(uint32_t modeId)
{
    if (modeId >= static_cast<uint32_t>(modes_.size())) {
        LOG_ERROR("DrmConnector::SetActiveModeId invalid modeId(%{public}u) by range [0, %{public}zu]", modeId, modes_.size());
        return false;
    }
    activeModeId_ = modeId;
    auto mi = modes_[activeModeId_].get()->ToHdiModeInfo();
    LOG_INFO("set display mode resolution is %dx%d\n", mi.width, mi.height);
    return true;
}
void DrmConnector::SetDefaultActiveMode()
{
    if (activeModeId_ == -1) {
        for (uint32_t i = 0; i < modes_.size(); i++) {
            auto mi = modes_[i].get()->ToHdiModeInfo();
            if (mi.width == DEFAULT_RESOLUTION_WIDTH && mi.height == DEFAULT_RESOLUTION_HEIGHT) {
                activeModeId_ = i;
                break;
            }
        }
        if (activeModeId_ == -1) {
            activeModeId_ = 0;
        }
        if (activeModeId_ < modes_.size()) {
            auto mi = modes_[activeModeId_].get()->ToHdiModeInfo();
            LOG_INFO("current display resolution is %dx%d\n", mi.width, mi.height);
        }
    }
}

uint32_t DrmConnector::GetActiveModeId() const
{
    return activeModeId_;
}

bool DrmConnector::SetDpms(uint64_t dpms)
{
    int ret = drmModeConnectorSetProperty(drmFd_, id_, dpmsPropId_, dpms);
    if (ret != 0) {
        LOG_ERROR("DrmConnector::SetDpms drmModeConnectorSetProperty failed, err: %{public}s", ErrnoToString(errno).c_str());
        return false;
    }
    dpms_ = dpms;
    return true;
}

bool DrmConnector::SetBrightness(uint64_t brightness)
{
    int ret = drmModeConnectorSetProperty(drmFd_, id_, brightnessPropId_, brightness);
    if (ret != 0) {
        LOG_ERROR("DrmConnector::SetBrightness drmModeConnectorSetProperty failed, err: %{public}s", ErrnoToString(errno).c_str());
        return false;
    }
    brightness_ = brightness;
    return true;
}
} // namespace drm
} // namespace oewm

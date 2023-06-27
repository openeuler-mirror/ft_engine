/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "drm_display.h"

#include "sync_fence.h"
#include "drm_atomic_committer.h"
#include "log.h"

#include "hdi_session.h"

namespace oewm {
namespace drm {
DrmDisplay::DrmDisplay(
    int drmFd,
    HdiDisplayId id,
    std::shared_ptr<DrmConnector> connector,
    std::shared_ptr<DrmCrtc> crtc)
    : drmFd_(drmFd), id_(id), connector_(std::move(connector)), crtc_(std::move(crtc))
{}

DrmDisplay::~DrmDisplay() noexcept {}

bool DrmDisplay::Init()
{
    LOG_DEBUG("DrmDisplay::Init");
    return HdiDisplay::Init();
}

int32_t DrmDisplay::GetDisplayCapability(DisplayCapability *info)
{
    if (OE_UNLIKELY(connector_ == nullptr)) {
        LOG_ERROR("DrmDisplay::GetDisplayCapability: connector for display id %{public}u is null!", id_);
        return DISPLAY_NULL_PTR;
    }

    connector_->GetDisplayCapability(info);

    return DISPLAY_SUCCESS;
}

int32_t DrmDisplay::GetDisplaySupportedModes(uint32_t *num, DisplayModeInfo *modes)
{
    if (OE_UNLIKELY(connector_ == nullptr)) {
        LOG_ERROR("DrmDisplay::GetDisplaySupportedModes: connector for display id %{public}" PRIu32 " is null!", id_);
        return DISPLAY_NULL_PTR;
    }

    connector_->GetSupportedModes(num, modes);

    return DISPLAY_SUCCESS;
}

int32_t DrmDisplay::GetDisplayMode(uint32_t *modeId)
{
    if (OE_UNLIKELY(connector_ == nullptr)) {
        LOG_ERROR("DrmDisplay::GetDisplayMode: connector for display id %{public}" PRIu32 " is null!", id_);
        return DISPLAY_NULL_PTR;
    }

    *modeId = connector_->GetActiveModeId();

    return DISPLAY_SUCCESS;
}

int32_t DrmDisplay::SetDisplayMode(uint32_t modeId)
{
    if (OE_UNLIKELY(connector_ == nullptr)) {
        LOG_ERROR("DrmDisplay::SetDisplayMode: connector for display id %{public}" PRIu32 " is null!", id_);
        return DISPLAY_NULL_PTR;
    }

    if (!connector_->SetActiveModeId(modeId)) {
        return DISPLAY_PARAM_ERR;
    }

    return DISPLAY_SUCCESS;
}

int32_t DrmDisplay::GetDisplayPowerStatus(DispPowerStatus *status)
{
    if (OE_UNLIKELY(connector_ == nullptr)) {
        LOG_ERROR("DrmDisplay::GetDisplayPowerStatus: connector for display id %{public}" PRIu32 " is null!", id_);
        return DISPLAY_NULL_PTR;
    }

    *status = ToDispPowerStatus(connector_->GetDpms());

    return DISPLAY_SUCCESS;
}

int32_t DrmDisplay::SetDisplayPowerStatus(DispPowerStatus status)
{
    auto dpms = ToDpms(status);
    if (dpms == DRM_INVLIAD_VALUE) {
        LOG_ERROR("DrmDisplay::SetDisplayPowerStatus: status invalid!");
        return DISPLAY_PARAM_ERR;
    }

    if (OE_UNLIKELY(connector_ == nullptr)) {
        LOG_ERROR("DrmDisplay::SetDisplayPowerStatus: connector for display id %{public}" PRIu32 " is null!", id_);
        return DISPLAY_NULL_PTR;
    }

    if (!connector_->SetDpms(dpms)) {
        return DISPLAY_FAILURE;
    }

    return DISPLAY_SUCCESS;
}

DispPowerStatus DrmDisplay::ToDispPowerStatus(uint64_t dpms)
{
    switch (dpms) {
        case DRM_MODE_DPMS_OFF:
            return POWER_STATUS_OFF;
        case DRM_MODE_DPMS_ON:
            return POWER_STATUS_ON;
        case DRM_MODE_DPMS_STANDBY:
            return POWER_STATUS_STANDBY;
        case DRM_MODE_DPMS_SUSPEND:
            return POWER_STATUS_SUSPEND;
        default:
            return POWER_STATUS_BUTT;
    }
}

uint64_t DrmDisplay::ToDpms(DispPowerStatus status)
{
    switch (status) {
        case POWER_STATUS_OFF:
            return DRM_MODE_DPMS_OFF;
        case POWER_STATUS_ON:
            return DRM_MODE_DPMS_ON;
        case POWER_STATUS_STANDBY:
            return DRM_MODE_DPMS_STANDBY;
        case POWER_STATUS_SUSPEND:
            return DRM_MODE_DPMS_SUSPEND;
        default:
            return DRM_INVLIAD_VALUE;
    }
}

std::unique_ptr<HdiLayer> DrmDisplay::CreateHdiLayer(HDI::DISPLAY::LayerId id, LayerType type)
{
    LOG_DEBUG("DrmDisplay::CreateHdiLayer");
    return std::make_unique<DrmLayer>(id, type);
}

int32_t DrmDisplay::GetDisplayBacklight(uint32_t *value)
{
    if (OE_UNLIKELY(connector_ == nullptr)) {
        LOG_ERROR("DrmDisplay::GetDisplayBacklight: connector for display id %{public}" PRIu32 " is null!", id_);
        return DISPLAY_NULL_PTR;
    }

    *value = static_cast<uint32_t>(connector_->GetBrightness());

    return DISPLAY_SUCCESS;
}

int32_t DrmDisplay::SetDisplayBacklight(uint32_t value)
{
    if (OE_UNLIKELY(connector_ == nullptr)) {
        LOG_ERROR("DrmDisplay::SetDisplayBacklight: connector for display id %{public}" PRIu32 " is null!", id_);
        return DISPLAY_NULL_PTR;
    }

    if (!connector_->SetBrightness(static_cast<uint64_t>(value))) {
        return DISPLAY_FAILURE;
    }

    return DISPLAY_SUCCESS;
}

bool DrmDisplay::IsConnected()
{
    if (OE_UNLIKELY(connector_ == nullptr)) {
        LOG_ERROR("DrmDisplay::IsConnected: connector for display id %{public}" PRIu32 " is null!", id_);
        return false;
    }

    return connector_->Connected();
}

void DrmDisplay::InitReservedFb()
{
    const auto &displayDevice = oewm::HDI::DISPLAY::HdiSession::GetInstance().GetDisplayDevice();
    ASSERT(displayDevice != nullptr);

    auto modeId = connector_->GetActiveModeId();
    drmModeModeInfo mode{};
    connector_->GetMode(modeId, &mode);
    auto width = static_cast<uint32_t>(mode.hdisplay);
    auto height = static_cast<uint32_t>(mode.vdisplay);
    uint64_t usage = HBM_USE_CPU_READ | HBM_USE_MEM_DMA | HBM_USE_MEM_FB | HBM_USE_CPU_WRITE;

    // Use Dumb Buffer as reversed fb is enough for now.
    reservedFb_ = DrmFrameBuffer::CreateAsDumb(drmFd_, width, height, usage, true);
}

int32_t DrmDisplay::RegDisplayVBlankCallback(VBlankCallback cb, void *data)
{
    bool isFirstReg = false;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        vSyncCallBack_ = cb;
        vsyncUserData_ = data;
        if (!vSyncCbEverReged_) {
            isFirstReg = true;
            vSyncCbEverReged_ = true;
        }
    }

    // if this is the first time to register vsync callback of this dispay,
    // we should do a commit to schdule the pageflip event.
    if (isFirstReg) {
        // TODO: create from gralloc
        InitReservedFb();
        if (reservedFb_ == nullptr) {
            LOG_ERROR("DrmDisplay::RegDisplayVBlankCallback: failed to schdule the pageflip event(fb create failed).");
            return DISPLAY_FAILURE;
        }

        const auto &displayDevice = oewm::HDI::DISPLAY::HdiSession::GetInstance().GetDisplayDevice();
        ASSERT(displayDevice != nullptr);

        int fenceFd = INVALID_FD;
        if (displayDevice->SupportAtomicModeSet()) {
            CommitAtomic(&fenceFd, reservedFb_.get(), DRM_MODE_ATOMIC_TEST_ONLY | DRM_MODE_ATOMIC_ALLOW_MODESET);
#ifdef ENABLE_HARDWARE_VSYNC
            CommitAtomic(&fenceFd, reservedFb_.get(), DRM_MODE_ATOMIC_ALLOW_MODESET | DRM_MODE_PAGE_FLIP_EVENT);
#else
            CommitAtomic(&fenceFd, reservedFb_.get(), DRM_MODE_ATOMIC_ALLOW_MODESET);
#endif // ENABLE_HARDWARE_VSYNC
        } else {
            CommitLegacy(&fenceFd, reservedFb_.get());
        }

        LOG_DEBUG("First commit Fence %{public}i", fenceFd);
        OHOS::SyncFence fence(fenceFd);
        fence.Wait(3000);
    }

    return DISPLAY_SUCCESS;
}

void DrmDisplay::OnVSync(uint32_t sequence, uint64_t timeStamp)
{
    VBlankCallback cb = nullptr;
    void *data = nullptr;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        cb = vSyncCallBack_;
        data = vsyncUserData_;
    }

    if (cb != nullptr) {
        cb(sequence, timeStamp, data);
    }
}

int32_t DrmDisplay::SetDisplayVsyncEnabled(bool enabled)
{
#ifdef ENABLE_HARDWARE_VSYNC
    if (enabled) {
        commitFlag_ |= DRM_MODE_PAGE_FLIP_EVENT;
    } else {
        commitFlag_ &= (~DRM_MODE_PAGE_FLIP_EVENT);
    }
    enableVsync_ = enabled;
#endif // ENABLE_HARDWARE_VSYNC
    return DISPLAY_SUCCESS;
}

int32_t DrmDisplay::GetDisplayReleaseFence(uint32_t *num, uint32_t *layers, int32_t *fences)
{
    *num = layers_.size();
    uint32_t i = 0;
    for (auto &[layerId, layer] : layers_) {
        if (layers != nullptr && fences != nullptr) {
            *(layers + i) = layerId;
            *(fences + i) = dup(layer->GetReleaseFenceFd());
            LOG_DEBUG("DrmDisplay::GetDisplayReleaseFence layerId %{public}u, fencefd %{public}i", layerId, layer->GetReleaseFenceFd());
        }
        i++;
    }
    return DISPLAY_SUCCESS;
}

int32_t DrmDisplay::GetDisplaySupportedColorGamuts(uint32_t *num, ColorGamut *gamuts)
{
    UNUSED(num);
    UNUSED(gamuts);
    return DISPLAY_SUCCESS;
}

int32_t DrmDisplay::GetDisplayColorGamut(ColorGamut *gamut)
{
    UNUSED(gamut);
    return DISPLAY_SUCCESS;
}

int32_t DrmDisplay::SetDisplayColorGamut(ColorGamut gamut)
{
    UNUSED(gamut);
    return DISPLAY_SUCCESS;
}

int32_t DrmDisplay::GetDisplayGamutMap(GamutMap *gamutMap)
{
    UNUSED(gamutMap);
    return DISPLAY_SUCCESS;
}

int32_t DrmDisplay::SetDisplayGamutMap(GamutMap gamutMap)
{
    UNUSED(gamutMap);
    return DISPLAY_SUCCESS;
}

int32_t DrmDisplay::GetHDRCapabilityInfos(HDRCapability *info)
{
    // mock data
    info->formatCount = 1;
    std::vector<HDRFormat> formats;
    formats.resize(info->formatCount);
    formats.push_back(HDR10);
    info->formats = formats.data();
    info->maxLum = 1000;
    info->minLum = 100;
    info->maxAverageLum = 600;
    return DISPLAY_SUCCESS;
}

int32_t DrmDisplay::GetSupportedMetadataKey(uint32_t *num, HDRMetadataKey *keys)
{
    return DISPLAY_SUCCESS;
}

int32_t DrmDisplay::SetDisplayClientBuffer(const BufferHandle *buffer, int32_t fence)
{
    // LOG_DEBUG("DrmDisplay::SetDisplayClientBuffer handle fd: %{public}i", buffer->fd);
    clientLayer_->SetBuffer(buffer, fence);
    return DISPLAY_SUCCESS;
}

int32_t DrmDisplay::PrepareDisplayLayers(bool *needFlushFb)
{
    changeLayers_.clear();
    std::vector<oewm::HDI::DISPLAY::HdiLayer *> layers;
    for (auto &[layerId, layer] : layers_) {
        UNUSED(layerId);
        layers.push_back(layer.get());
    }
    sort(
        layers.begin(),
        layers.end(),
        [](const oewm::HDI::DISPLAY::HdiLayer *lhs, const oewm::HDI::DISPLAY::HdiLayer *rhs) {
            return lhs->GetZOrder() < rhs->GetZOrder();
        }); // sort by Zorder
    for (auto &layer : layers) {
        LOG_DEBUG("DrmDisplay::PrepareDisplayLayers layer id %{public}u, ZOrder %{public}u"
                  , layer->GetId(), layer->GetZOrder());
    }

    for (auto &layer : layers) {
        if (layer->GetCompositionType() != COMPOSITION_CURSOR && layer->GetCompositionType() != COMPOSITION_VIDEO &&
            layer->GetCompositionType() != COMPOSITION_TUNNEL) {
            layer->SetDeviceSelect(COMPOSITION_CLIENT);
        } else {
            layer->SetDeviceSelect(layer->GetCompositionType());
        }

        // get the change layers
        if (layer->GetDeviceSelect() != layer->GetCompositionType()) {
            LOG_DEBUG("DrmDisplay::PrepareDisplayLayers layer compositionType from %{public}i to %{public}i",
                layer->GetCompositionType(), layer->GetDeviceSelect());
            layer->SetCompositionType(layer->GetDeviceSelect());
            changeLayers_.push_back(layer);
        }
    }

    *needFlushFb = true;
    return DISPLAY_SUCCESS;
}

int32_t DrmDisplay::GetDisplayCompChange(uint32_t *num, uint32_t *layers, int32_t *type)
{
    *num = changeLayers_.size();
    for (uint32_t i = 0; i < changeLayers_.size(); i++) {
        if (layers != nullptr && type != nullptr) {
            *(layers + i) = changeLayers_[i]->GetId();
            *(type + i) = changeLayers_[i]->GetCompositionType();
            LOG_DEBUG("DrmDisplay::GetDisplayCompChange layerId %{public}u, fencefd %{public}i", 
                changeLayers_[i]->GetId(), changeLayers_[i]->GetCompositionType());
        }
    }

    return DISPLAY_SUCCESS;
}

void DrmDisplay::CommitAtomic(int32_t *fence, const DrmFrameBuffer *fb, int commitFlag)
{
    ASSERT(fb != nullptr);
    auto width = fb->GetFbWidth();
    auto height = fb->GetFbHeight();
    auto fbId = fb->GetFbId();

    LOG_DEBUG("DrmDisplay::CommitAtomic. \n"
        "Connector Id: %{public}u, "
        "CRTC Id: %{public}d, "
        "CRTC Mode Blob Id: %{public}lu, "
        "CRTC Active: 1, "
        "CRTC OutFence: %{public}lu, "
        "Plain Id: %{public}u, "
        "Plain FB Id: %{public}u, "
        "Plain SRC W: %{public}u, "
        "Plain SRC H: %{public}u, "
        "Plain CRTC W: %{public}u, "
        "Plain CRTC H: %{public}u, "
        "Commit flag: %{public}d.",
        connector_->Id(), crtc_->Id(), connector_->BlobId(), (uint64_t)fence, 
        primaryPlane_->Id(), fbId, width << 16, height << 16, width, height, commitFlag);

    DrmAtomicCommitter atomicAutoCommitter(drmFd_, commitFlag, this);

    /* set id of the CRTC id that the connector is using */
    atomicAutoCommitter.AddAtomicProperty(connector_->Id(), connector_->CrtcPropId(), crtc_->Id());

    /* set the mode id of the CRTC; this property receives the id of a blob
	 * property that holds the struct that actually contains the mode info */
    atomicAutoCommitter.AddAtomicProperty(crtc_->Id(), crtc_->ModeIdPropId(), connector_->BlobId());

    /* set the CRTC object as active */
    atomicAutoCommitter.AddAtomicProperty(crtc_->Id(), crtc_->ActivePropId(), 1);

    atomicAutoCommitter.AddAtomicProperty(crtc_->Id(), crtc_->OutFencePropId(), (uint64_t)fence);

    /* set properties of the plane related to the CRTC and the framebuffer */
    atomicAutoCommitter.AddAtomicProperty(primaryPlane_->Id(), primaryPlane_->FBPropId(), fbId);
    atomicAutoCommitter.AddAtomicProperty(primaryPlane_->Id(), primaryPlane_->CrtcPropId(), crtc_->Id());
    atomicAutoCommitter.AddAtomicProperty(primaryPlane_->Id(), primaryPlane_->SrcXPropId(), 0);
    atomicAutoCommitter.AddAtomicProperty(primaryPlane_->Id(), primaryPlane_->SrcYPropId(), 0);
    atomicAutoCommitter.AddAtomicProperty(primaryPlane_->Id(), primaryPlane_->SrcWPropId(), width << 16);
    atomicAutoCommitter.AddAtomicProperty(primaryPlane_->Id(), primaryPlane_->SrcHPropId(), height << 16);
    atomicAutoCommitter.AddAtomicProperty(primaryPlane_->Id(), primaryPlane_->CrtcXPropId(), 0);
    atomicAutoCommitter.AddAtomicProperty(primaryPlane_->Id(), primaryPlane_->CrtcYPropId(), 0);
    atomicAutoCommitter.AddAtomicProperty(primaryPlane_->Id(), primaryPlane_->CrtcWPropId(), width);
    atomicAutoCommitter.AddAtomicProperty(primaryPlane_->Id(), primaryPlane_->CrtcHPropId(), height);
    atomicAutoCommitter.Commit();

    clientLayer_->SetReleaseFence(*fence);
    LOG_DEBUG("DrmDisplay::CommitAtomic: done.");
}

void DrmDisplay::CommitLegacy(int32_t *fence, const DrmFrameBuffer *fb)
{
    ASSERT(fb != nullptr);

    auto fbId = fb->GetFbId();
    uint32_t ctrcId = crtc_->Id();
    uint32_t connId = connector_->Id();
    drmModeModeInfo mode;
    auto modeId = connector_->GetActiveModeId();
    connector_->GetMode(modeId, &mode);

    auto width = fb->GetFbWidth();
    auto height = fb->GetFbHeight();

    LOG_DEBUG("DrmDisplay::CommitLegacy connectorId: %{public}" PRIu32 ", crtcId: %{public}" PRIu32 ", modeId: %{public}" PRIu32 ", fbWidth: %{public}" PRIu32 ", fbHeight: %{public}" PRIu32 ".",
        connId, ctrcId, modeId, width, height);

    if (drmModeSetCrtc(drmFd_, ctrcId, fbId, 0, 0, &connId, 1, &mode) != 0) {
        LOG_WARN("DrmDisplay::CommitLegacy drmModeSetCrtc failed, error: %{public}s", ErrnoToString(errno).c_str());
    }
    LOG_DEBUG("DrmDisplay::CommitLegacy done");
}

int32_t DrmDisplay::Commit(int32_t *fence)
{
    DrmLayer *layer = DownCast<DrmLayer *>(clientLayer_.get());
    if (layer == nullptr) {
        LOG_ERROR("DrmDisplay::Commit: client layer nullptr.");
        return DISPLAY_NULL_PTR;
    }

    const DrmFrameBuffer *fb = layer->GetFrameBuffer(drmFd_);
    if (fb == nullptr) {
        LOG_ERROR("DrmDisplay::Commit: failed to get framebuffer, use reservedFb_ instead.");
        fb = reservedFb_.get();
    }

    if (oewm::HDI::DISPLAY::HdiSession::GetInstance().GetDisplayDevice()->SupportAtomicModeSet()) {
        CommitAtomic(fence, fb, commitFlag_);
    } else {
        // legacy
        CommitLegacy(fence, fb);
    }

    return DISPLAY_SUCCESS;
}
} // namespace drm
} // namespace oewm

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

#include <array>
#include <mutex>

#include "drm_connector.h"
#include "drm_layer.h"
#include "drm_mode.h"
#include "drm_plane.h"
#include "hdi_display.h"

namespace oewm {
namespace drm {
class DrmDisplay : public HdiDisplay {
public:
    DrmDisplay(int drmFd, HdiDisplayId id, std::shared_ptr<DrmConnector> connector, std::shared_ptr<DrmCrtc> crtc);
    ~DrmDisplay() noexcept;

    bool Init() override;
    void SetPrimaryPlane(const std::shared_ptr<DrmPlane> &primaryPlane)
    {
        primaryPlane_ = primaryPlane;
    }

    HdiDisplayId Id() const override
    {
        return id_;
    }
    bool IsConnected() override;

    int32_t GetDisplayCapability(DisplayCapability *info) override;
    int32_t GetDisplaySupportedModes(uint32_t *num, DisplayModeInfo *modes) override;
    int32_t GetDisplayMode(uint32_t *modeId) override;
    int32_t SetDisplayMode(uint32_t modeId) override;
    int32_t GetDisplayPowerStatus(DispPowerStatus *status) override;
    int32_t SetDisplayPowerStatus(DispPowerStatus status) override;
    int32_t GetDisplayBacklight(uint32_t *value) override;
    int32_t SetDisplayBacklight(uint32_t value) override;
    int32_t RegDisplayVBlankCallback(VBlankCallback cb, void *data) override;
    void OnVSync(uint32_t sequence, uint64_t timeStamp) override;
    int32_t SetDisplayVsyncEnabled(bool enabled) override;
    int32_t GetDisplayReleaseFence(uint32_t *num, uint32_t *layers, int32_t *fences) override;
    int32_t GetDisplaySupportedColorGamuts(uint32_t *num, ColorGamut *gamuts) override;
    int32_t GetDisplayColorGamut(ColorGamut *gamut) override;
    int32_t SetDisplayColorGamut(ColorGamut gamut) override;
    int32_t GetDisplayGamutMap(GamutMap *gamutMap) override;
    int32_t SetDisplayGamutMap(GamutMap gamutMap) override;
    int32_t GetHDRCapabilityInfos(HDRCapability *info) override;
    int32_t GetSupportedMetadataKey(uint32_t *num, HDRMetadataKey *keys) override;
    int32_t SetDisplayClientBuffer(const BufferHandle *buffer, int32_t fence) override;

    int32_t PrepareDisplayLayers(bool *needFlushFb) override;
    int32_t GetDisplayCompChange(uint32_t *num, uint32_t *layers, int32_t *type) override;
    int32_t Commit(int32_t *fence) override;

private:
    // convert drm DPMS(display power manager status) to hdi DispPowerStatus.
    static DispPowerStatus ToDispPowerStatus(uint64_t dpms);
    // convert hdi DispPowerStatus to drm DPMS(display power manager status).
    static uint64_t ToDpms(DispPowerStatus status);

    std::unique_ptr<HdiLayer> CreateHdiLayer(HDI::DISPLAY::LayerId id, LayerType type) override;

    void CommitAtomic(int32_t *fence, const DrmFrameBuffer *fb, int commitFlag);
    void CommitLegacy(int32_t *fence, const DrmFrameBuffer *fb);

    int drmFd_ = INVALID_FD;

    HdiDisplayId id_;
    std::shared_ptr<DrmConnector> connector_;
    std::shared_ptr<DrmCrtc> crtc_;
    std::shared_ptr<DrmPlane> primaryPlane_;

    mutable std::mutex mutex_;
    VBlankCallback vSyncCallBack_ = nullptr; // guarded by mutex_;
    void *vsyncUserData_ = nullptr;          // guarded by mutex_;
    bool vSyncCbEverReged_ = false;          // guarded by mutex_;
#ifdef ENABLE_HARDWARE_VSYNC
    // bool enableVsync_ = true;
#endif // ENABLE_HARDWARE_VSYNC

    void InitReservedFb();
    std::unique_ptr<DrmFrameBuffer> reservedFb_; // to do first commit to enable vsync.
#ifdef ENABLE_HARDWARE_VSYNC
    int commitFlag_ = DRM_MODE_ATOMIC_ALLOW_MODESET | DRM_MODE_PAGE_FLIP_EVENT | DRM_MODE_ATOMIC_NONBLOCK;
#else
    int commitFlag_ = DRM_MODE_ATOMIC_ALLOW_MODESET | DRM_MODE_ATOMIC_NONBLOCK;
#endif // ENABLE_HARDWARE_VSYNC
};
} // namespace drm
} // namespace oewm

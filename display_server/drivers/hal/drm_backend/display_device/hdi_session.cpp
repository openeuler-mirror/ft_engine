/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "hdi_session.h"

#include <libudev.h>

#include "display_layer.h"

namespace oewm {
namespace HDI {
namespace DISPLAY {
namespace detail {
constexpr char DEFAULT_DRM_DEVICE_PATH[] = "/dev/dri/card0";

#ifdef USE_LIBUDEV
std::shared_ptr<drm::DrmDevice> FindPrimaryDevice(struct udev *udevice, std::string seatId = defaultSeat)
{
    if (udevice == nullptr) {
        LOG_ERROR("Failed to initialize udev context.");
        return nullptr;
    }

    std::shared_ptr<drm::DrmDevice> drmDevice;
    UdevObject udevEnum(udev_enumerate_new(udevice), udev_enumerate_unref);

    udev_enumerate_add_match_subsystem(udevEnum.Get(), "drm");
    udev_enumerate_add_match_sysname(udevEnum.Get(), "card[0-9]*");
    udev_enumerate_scan_devices(udevEnum.Get());

    struct udev_list_entry *entry = nullptr;
    udev_list_entry_foreach(entry, udev_enumerate_get_list_entry(udevEnum.Get()))
    {
        const char *devicePath = udev_list_entry_get_name(entry);
        UdevObject device(udev_device_new_from_syspath(udevice, devicePath), udev_device_unref);
        if (device == nullptr) {
            continue;
        }

        const char *deviceSeat = udev_device_get_property_value(device.Get(), "ID_SEAT");
        if (deviceSeat == nullptr) {
            deviceSeat = defaultSeat;
        }
        if (deviceSeat != seatId) {
            continue;
        }

        if (IsBootVGADevice(device.Get())) {
            // If we create a drmDevice from this devicePath which indicate the boot vga device successfully,
            // then we can break the enumeration immediately.
            const char *filePath = udev_device_get_devnode(device.Get());
            auto tmpDrmDevice = drm::DrmDevice::Create(filePath);
            if (tmpDrmDevice != nullptr) {
                drmDevice = tmpDrmDevice;
                break;
            }
        }
    }

    return drmDevice;
}
#endif // USE_LIBUDEV
} // namespace detail

HdiSession &HdiSession::GetInstance()
{
    static HdiSession instance;
    return instance;
}

HdiSession::HdiSession()
#ifdef USE_LIBUDEV
    : udev_(udev_new(), udev_unref)
#endif // USE_LIBUDEV
{
#ifdef USE_LIBUDEV
    displayDevice_ = detail::FindPrimaryDevice(udev_.Get());
    if (displayDevice_ == nullptr) {
        displayDevice_ = drm::DrmDevice::Create(detail::DEFAULT_DRM_DEVICE_PATH);
    }
#else  // USE_LIBUDEV
    displayDevice_ = drm::DrmDevice::Create(detail::DEFAULT_DRM_DEVICE_PATH);
#endif // USE_LIBUDEV

    if (displayDevice_ == nullptr) {
        LOG_FATAL("Failed to find primary display device!");
    }

#ifdef USE_LIBUDEV
    deviceEventMonitor_ = std::make_shared<DeviceEventMonitor>(
        udev_.Get(), displayDevice_, [this](TimeStamp timeStamp, struct udev_device *event) {
            OnHotPlugEvent(timeStamp, event);
        });
#else  // USE_LIBUDEV
    deviceEventMonitor_ = std::make_shared<DeviceEventMonitor>(displayDevice_);
#endif // USE_LIBUDEV
}

HdiSession::~HdiSession() noexcept {}

int32_t HdiSession::RegHotPlugCallback(HotPlugCallback callback, void *data)
{
    if (callback == nullptr) {
        LOG_ERROR("HdiSession::RegHotPlugCallback: param callback is nullptr.");
        return DISPLAY_PARAM_ERR;
    }

    if (hotPlugCallback_ != nullptr) {
        LOG_ERROR("HdiSession::RegHotPlugCallback: callback already registered.");
        return DISPLAY_SUCCESS;
    }

    hotPlugCallback_ = callback;
    hotPlugUserData_ = data;
    const auto &displays = displayDevice_->GetDisplays();
    for (const auto &[id, display] : displays) {
        DoHotPlugCallback(id, display->IsConnected());
    }

    if (!deviceEventMonitor_->Init()) {
        LOG_ERROR("HdiSession::RegHotPlugCallback Failed to init device event monitor!");
    }
    return DISPLAY_SUCCESS;
}

void HdiSession::DoHotPlugCallback(uint32_t devId, bool connect)
{
    if (hotPlugCallback_ == nullptr) {
        LOG_ERROR("HdiSession::DoHotPlugCallback: callback is nullptr.");
        return;
    }
    hotPlugCallback_(devId, connect, hotPlugUserData_);
}

#ifdef USE_LIBUDEV
// TODO：Implement this function
void HdiSession::OnHotPlugEvent(TimeStamp timeStamp, struct udev_device *event) {}
// FIXME: ifndef USE_LIBUDEV
#endif // USE_LIBUDEV
} // namespace DISPLAY
} // namespace HDI
} // namespace oewm

using namespace oewm::HDI::DISPLAY;
static int32_t RegHotPlugCallback(HotPlugCallback callback, void *data)
{
    HdiSession::GetInstance().RegHotPlugCallback(callback, data);
    return DISPLAY_SUCCESS;
}

static int32_t GetDisplayCapability(uint32_t devId, DisplayCapability *info)
{
    if (info == nullptr) {
        LOG_ERROR("GetDisplayCapability: info is nullptr.");
        return DISPLAY_PARAM_ERR;
    }

    return HdiSession::GetInstance().CallDisplayFunction(devId, &HdiDisplay::GetDisplayCapability, info);
}

static int32_t GetDisplaySupportedModes(uint32_t devId, uint32_t *num, DisplayModeInfo *modes)
{
    if (num == nullptr) {
        LOG_ERROR("GetDisplaySupportedModes: num is nullptr.");
        return DISPLAY_PARAM_ERR;
    }

    return HdiSession::GetInstance().CallDisplayFunction(devId, &HdiDisplay::GetDisplaySupportedModes, num, modes);
}

static int32_t GetDisplayMode(uint32_t devId, uint32_t *mode)
{
    if (mode == nullptr) {
        LOG_ERROR("GetDisplayMode: mode is nullptr.");
        return DISPLAY_PARAM_ERR;
    }

    return HdiSession::GetInstance().CallDisplayFunction(devId, &HdiDisplay::GetDisplayMode, mode);
}

static int32_t SetDisplayMode(uint32_t devId, uint32_t mode)
{
    return HdiSession::GetInstance().CallDisplayFunction(devId, &HdiDisplay::SetDisplayMode, mode);
}

static int32_t GetDisplayPowerStatus(uint32_t devId, DispPowerStatus *status)
{
    if (status == nullptr) {
        LOG_ERROR("GetDisplayPowerStatus: status is nullptr.");
        return DISPLAY_PARAM_ERR;
    }

    return HdiSession::GetInstance().CallDisplayFunction(devId, &HdiDisplay::GetDisplayPowerStatus, status);
}

static int32_t SetDisplayPowerStatus(uint32_t devId, DispPowerStatus status)
{
    return HdiSession::GetInstance().CallDisplayFunction(devId, &HdiDisplay::SetDisplayPowerStatus, status);
}

static int32_t GetDisplayBacklight(uint32_t devId, uint32_t *value)
{
    if (value == nullptr) {
        LOG_ERROR("GetDisplayBacklight: value is nullptr.");
        return DISPLAY_PARAM_ERR;
    }

    return HdiSession::GetInstance().CallDisplayFunction(devId, &HdiDisplay::GetDisplayBacklight, value);
}

static int32_t SetDisplayBacklight(uint32_t devId, uint32_t value)
{
    return HdiSession::GetInstance().CallDisplayFunction(devId, &HdiDisplay::SetDisplayBacklight, value);
}

static int32_t GetDisplayProperty(uint32_t devId, uint32_t id, uint64_t *value)
{
    UNUSED(id);
    UNUSED(value);

    return DISPLAY_NOT_SUPPORT;
}

static int32_t SetDisplayProperty(uint32_t devId, uint32_t id, uint64_t value)
{
    UNUSED(id);
    UNUSED(value);
    return DISPLAY_NOT_SUPPORT;
}

static int32_t PrepareDisplayLayers(uint32_t devId, bool *needFlushFb)
{
    if (needFlushFb == nullptr) {
        LOG_ERROR("PrepareDisplayLayers: needFlushFb is nullptr.");
        return DISPLAY_PARAM_ERR;
    }

    return HdiSession::GetInstance().CallDisplayFunction(devId, &HdiDisplay::PrepareDisplayLayers, needFlushFb);
}

static int32_t GetDisplayCompChange(uint32_t devId, uint32_t *num, uint32_t *layers, int32_t *type)
{
    if (num == nullptr) {
        LOG_ERROR("GetDisplayCompChange: num is nullptr.");
        return DISPLAY_PARAM_ERR;
    }

    return HdiSession::GetInstance().CallDisplayFunction(devId, &HdiDisplay::GetDisplayCompChange, num, layers, type);
}

static int32_t SetDisplayClientCrop(uint32_t devId, IRect *rect)
{
    if (rect == nullptr) {
        LOG_ERROR("SetDisplayClientCrop: rect is nullptr.");
        return DISPLAY_PARAM_ERR;
    }

    return DISPLAY_NOT_SUPPORT;
}

static int32_t SetDisplayClientDestRect(uint32_t devId, IRect *rect)
{
    if (rect == nullptr) {
        LOG_ERROR("SetDisplayClientDestRect: rect is nullptr.");
        return DISPLAY_PARAM_ERR;
    }

    return DISPLAY_NOT_SUPPORT;
}

static int32_t SetDisplayClientBuffer(uint32_t devId, const BufferHandle *buffer, int32_t fence)
{
    return HdiSession::GetInstance().CallDisplayFunction(devId, &HdiDisplay::SetDisplayClientBuffer, buffer, fence);
}

static int32_t SetDisplayClientDamage(uint32_t devId, uint32_t num, IRect *rect)
{
    UNUSED(num);
    UNUSED(rect);
    return DISPLAY_NOT_SUPPORT;
}

static int32_t SetDisplayVsyncEnabled(uint32_t devId, bool enabled)
{
    return HdiSession::GetInstance().CallDisplayFunction(devId, &HdiDisplay::SetDisplayVsyncEnabled, enabled);
}

static int32_t RegDisplayVBlankCallback(uint32_t devId, VBlankCallback callback, void *data)
{
    return HdiSession::GetInstance().CallDisplayFunction(devId, &HdiDisplay::RegDisplayVBlankCallback, callback, data);
}

static int32_t GetDisplayReleaseFence(uint32_t devId, uint32_t *num, uint32_t *layers, int32_t *fences)
{
    if (num == nullptr) {
        LOG_ERROR("GetDisplayReleaseFence: num is nullptr.");
        return DISPLAY_PARAM_ERR;
    }
    return HdiSession::GetInstance().CallDisplayFunction(
        devId, &HdiDisplay::GetDisplayReleaseFence, num, layers, fences);
}

static int32_t GetDisplaySupportedColorGamuts(uint32_t devId, uint32_t *num, ColorGamut *gamuts)
{
    if (num == nullptr) {
        LOG_ERROR("GetDisplaySupportedColorGamuts: num is nullptr.");
        return DISPLAY_PARAM_ERR;
    }
    return HdiSession::GetInstance().CallDisplayFunction(
        devId, &HdiDisplay::GetDisplaySupportedColorGamuts, num, gamuts);
}

static int32_t GetDisplayColorGamut(uint32_t devId, ColorGamut *gamut)
{
    if (gamut == nullptr) {
        LOG_ERROR("GetDisplayColorGamut: gamut is nullptr.");
        return DISPLAY_PARAM_ERR;
    }

    return HdiSession::GetInstance().CallDisplayFunction(devId, &HdiDisplay::GetDisplayColorGamut, gamut);
}

static int32_t SetDisplayColorGamut(uint32_t devId, ColorGamut gamut)
{
    return HdiSession::GetInstance().CallDisplayFunction(devId, &HdiDisplay::SetDisplayColorGamut, gamut);
}

static int32_t GetDisplayGamutMap(uint32_t devId, GamutMap *gamutMap)
{
    if (gamutMap == nullptr) {
        LOG_ERROR("GetDisplayGamutMap: gamutMap is nullptr.");
        return DISPLAY_PARAM_ERR;
    }

    return HdiSession::GetInstance().CallDisplayFunction(devId, &HdiDisplay::GetDisplayGamutMap, gamutMap);
}

static int32_t SetDisplayGamutMap(uint32_t devId, GamutMap gamutMap)
{
    return HdiSession::GetInstance().CallDisplayFunction(devId, &HdiDisplay::SetDisplayGamutMap, gamutMap);
}

static int32_t GetHDRCapabilityInfos(uint32_t devId, HDRCapability *info)
{
    if (info == nullptr) {
        LOG_ERROR("GetHDRCapabilityInfos: info is nullptr.");
        return DISPLAY_PARAM_ERR;
    }

    return HdiSession::GetInstance().CallDisplayFunction(devId, &HdiDisplay::GetHDRCapabilityInfos, info);
}

static int32_t GetSupportedMetadataKey(uint32_t devId, uint32_t *num, HDRMetadataKey *keys)
{
    if (num == nullptr) {
        LOG_ERROR("GetSupportedMetadataKey: num is nullptr.");
        return DISPLAY_PARAM_ERR;
    }
    return HdiSession::GetInstance().CallDisplayFunction(devId, &HdiDisplay::GetSupportedMetadataKey, num, keys);
}

static int32_t Commit(uint32_t devId, int32_t *fence)
{
    if (fence == nullptr) {
        LOG_ERROR("Commit: fence is nullptr.");
        return DISPLAY_PARAM_ERR;
    }

    return HdiSession::GetInstance().CallDisplayFunction(devId, &HdiDisplay::Commit, fence);
}

static int32_t CreateVirtualDisplay(uint32_t width, uint32_t height, int32_t *format, uint32_t *devId)
{
    return DISPLAY_NOT_SUPPORT;
}
static int32_t DestroyVirtualDisplay(uint32_t devId)
{
    return DISPLAY_NOT_SUPPORT;
}
static int32_t SetVirtualDisplayBuffer(uint32_t devId, BufferHandle *buffer, int32_t releaseFence)
{
    return DISPLAY_NOT_SUPPORT;
}

// Layer function
static int32_t CreateLayer(uint32_t devId, const LayerInfo *layerInfo, uint32_t *layerId)
{
    if (layerId == nullptr) {
        LOG_ERROR("CreateLayer: layerId is nullptr.");
        return DISPLAY_PARAM_ERR;
    }

    return HdiSession::GetInstance().CallDisplayFunction(devId, &HdiDisplay::CreateLayer, layerInfo, layerId);
}

static int32_t CloseLayer(uint32_t devId, uint32_t layerId)
{
    return HdiSession::GetInstance().CallDisplayFunction(devId, &HdiDisplay::CloseLayer, layerId);
}

static int32_t SetLayerSize(uint32_t devId, uint32_t layerId, IRect *rect)
{
    if (rect == nullptr) {
        LOG_ERROR("SetLayerSize: rect is nullptr.");
        return DISPLAY_PARAM_ERR;
    }

    return HdiSession::GetInstance().CallLayerFunction(devId, layerId, &HdiLayer::SetSize, rect);
}

static int32_t SetLayerCrop(uint32_t devId, uint32_t layerId, IRect *rect)
{
    if (rect == nullptr) {
        LOG_ERROR("SetLayerCrop: rect is nullptr.");
        return DISPLAY_PARAM_ERR;
    }

    return HdiSession::GetInstance().CallLayerFunction(devId, layerId, &HdiLayer::SetCrop, rect);
}

static int32_t SetLayerZOrder(uint32_t devId, uint32_t layerId, uint32_t zOrder)
{
    return HdiSession::GetInstance().CallLayerFunction(devId, layerId, &HdiLayer::SetZOrder, zOrder);
}

static int32_t SetLayerPreMulti(uint32_t devId, uint32_t layerId, bool preMul)
{
    return HdiSession::GetInstance().CallLayerFunction(devId, layerId, &HdiLayer::SetPreMulti, preMul);
}

static int32_t SetLayerAlpha(uint32_t devId, uint32_t layerId, LayerAlpha *alpha)
{
    if (alpha == nullptr) {
        LOG_ERROR("SetLayerAlpha: alpha is nullptr.");
        return DISPLAY_PARAM_ERR;
    }

    return HdiSession::GetInstance().CallLayerFunction(devId, layerId, &HdiLayer::SetAlpha, alpha);
}

static int32_t SetTransformMode(uint32_t devId, uint32_t layerId, TransformType type)
{
    return HdiSession::GetInstance().CallLayerFunction(devId, layerId, &HdiLayer::SetTransformMode, type);
}

static int32_t SetLayerDirtyRegion(uint32_t devId, uint32_t layerId, IRect *region)
{
    if (region == nullptr) {
        LOG_ERROR("SetLayerDirtyRegion: region is nullptr.");
        return DISPLAY_PARAM_ERR;
    }

    return HdiSession::GetInstance().CallLayerFunction(devId, layerId, &HdiLayer::SetDirtyRegion, region);
}

static int32_t SetLayerVisibleRegion(uint32_t devId, uint32_t layerId, uint32_t num, IRect *rect)
{
    if (rect == nullptr) {
        LOG_ERROR("SetLayerVisibleRegion: rect is nullptr.");
        return DISPLAY_PARAM_ERR;
    }

    return HdiSession::GetInstance().CallLayerFunction(devId, layerId, &HdiLayer::SetVisibleRegion, num, rect);
}

static int32_t SetLayerBuffer(uint32_t devId, uint32_t layerId, const BufferHandle *buffer, int32_t fence)
{
    return HdiSession::GetInstance().CallLayerFunction(devId, layerId, &HdiLayer::SetBuffer, buffer, fence);
}

static int32_t SetLayerCompositionType(uint32_t devId, uint32_t layerId, CompositionType type)
{
    return HdiSession::GetInstance().CallLayerFunction(devId, layerId, &HdiLayer::SetCompositionType, type);
}

static int32_t SetLayerBlendType(uint32_t devId, uint32_t layerId, BlendType type)
{
    return HdiSession::GetInstance().CallLayerFunction(devId, layerId, &HdiLayer::SetBlendType, type);
}

static int32_t SetLayerTunnelHandle(uint32_t devId, uint32_t layerId, ExtDataHandle *handle)
{
    return HdiSession::GetInstance().CallLayerFunction(devId, layerId, &HdiLayer::SetTunnelHandle, handle);
}

#if defined(__cplusplus)
extern "C" {
#endif // __cplusplus
int32_t DeviceInitialize(DeviceFuncs **funcs)
{
    LOG_INFO("hdi device initialize begin.");
    if (funcs == nullptr) {
        LOG_ERROR("DeviceInitialize: param funcs is nullptr.");
        return DISPLAY_PARAM_ERR;
    }

    auto deviceFuncs = (DeviceFuncs *)calloc(1, sizeof(DeviceFuncs));
    if (deviceFuncs == nullptr) {
        LOG_ERROR("DeviceInitialize: deviceFuncs alloc failed, err: %{public}s", oewm::ErrnoToString(errno).c_str());
        return DISPLAY_FAILURE;
    }

    deviceFuncs->RegHotPlugCallback = RegHotPlugCallback;
    deviceFuncs->GetDisplayCapability = GetDisplayCapability;
    deviceFuncs->GetDisplaySupportedModes = GetDisplaySupportedModes;
    deviceFuncs->GetDisplayMode = GetDisplayMode;
    deviceFuncs->SetDisplayMode = SetDisplayMode;
    deviceFuncs->GetDisplayPowerStatus = GetDisplayPowerStatus;
    deviceFuncs->SetDisplayPowerStatus = SetDisplayPowerStatus;
    deviceFuncs->PrepareDisplayLayers = PrepareDisplayLayers;
    deviceFuncs->GetDisplayBacklight = GetDisplayBacklight;
    deviceFuncs->SetDisplayBacklight = SetDisplayBacklight;
    deviceFuncs->GetDisplayProperty = GetDisplayProperty;
    deviceFuncs->GetDisplayCompChange = GetDisplayCompChange;
    deviceFuncs->SetDisplayClientCrop = SetDisplayClientCrop;
    deviceFuncs->SetDisplayClientDestRect = SetDisplayClientDestRect;
    deviceFuncs->SetDisplayClientBuffer = SetDisplayClientBuffer;
    deviceFuncs->SetDisplayClientDamage = SetDisplayClientDamage;
    deviceFuncs->SetDisplayVsyncEnabled = SetDisplayVsyncEnabled;
    deviceFuncs->RegDisplayVBlankCallback = RegDisplayVBlankCallback;
    deviceFuncs->GetDisplayReleaseFence = GetDisplayReleaseFence;
    deviceFuncs->GetDisplaySupportedColorGamuts = GetDisplaySupportedColorGamuts;
    deviceFuncs->GetDisplayColorGamut = GetDisplayColorGamut;
    deviceFuncs->SetDisplayColorGamut = SetDisplayColorGamut;
    deviceFuncs->GetDisplayGamutMap = GetDisplayGamutMap;
    deviceFuncs->SetDisplayGamutMap = SetDisplayGamutMap;
    deviceFuncs->GetHDRCapabilityInfos = GetHDRCapabilityInfos;
    deviceFuncs->GetSupportedMetadataKey = GetSupportedMetadataKey;
    deviceFuncs->CreateVirtualDisplay = CreateVirtualDisplay;
    deviceFuncs->DestroyVirtualDisplay = DestroyVirtualDisplay;
    deviceFuncs->SetVirtualDisplayBuffer = SetVirtualDisplayBuffer;
    deviceFuncs->SetDisplayProperty = SetDisplayProperty;

    deviceFuncs->Commit = Commit;
    *funcs = deviceFuncs;
    LOG_INFO("hdi device initialize succeed.");
    return DISPLAY_SUCCESS;
}

int32_t DeviceUnInitialize(DeviceFuncs *funcs)
{
    if (funcs == nullptr) {
        LOG_ERROR("DeviceUnInitialize: param funcs is nullptr.");
        return DISPLAY_PARAM_ERR;
    }

    free(funcs);
    LOG_INFO("hdi device unInitialize succeed.");

    return DISPLAY_SUCCESS;
}

int32_t LayerInitialize(LayerFuncs **funcs)
{
    LOG_INFO("hdi layer initialize begin.");
    if (funcs == nullptr) {
        LOG_ERROR("LayerInitialize: param funcs is nullptr.");
        return DISPLAY_PARAM_ERR;
    }

    LayerFuncs *layerFuncs = (LayerFuncs *)calloc(1, sizeof(LayerFuncs));
    if (layerFuncs == nullptr) {
        LOG_ERROR("LayerInitialize: layerFuncs alloc failed, err: %{public}s", oewm::ErrnoToString(errno).c_str());
        return DISPLAY_FAILURE;
    }

    layerFuncs->SetLayerAlpha = SetLayerAlpha;
    layerFuncs->CreateLayer = CreateLayer;
    layerFuncs->CloseLayer = CloseLayer;
    layerFuncs->SetLayerSize = SetLayerSize;
    layerFuncs->SetLayerCrop = SetLayerCrop;
    layerFuncs->SetLayerZorder = SetLayerZOrder;
    layerFuncs->SetLayerPreMulti = SetLayerPreMulti;
    layerFuncs->SetTransformMode = SetTransformMode;
    layerFuncs->SetLayerDirtyRegion = SetLayerDirtyRegion;
    layerFuncs->SetLayerVisibleRegion = SetLayerVisibleRegion;
    layerFuncs->SetLayerBuffer = SetLayerBuffer;
    layerFuncs->SetLayerCompositionType = SetLayerCompositionType;
    layerFuncs->SetLayerBlendType = SetLayerBlendType;
    layerFuncs->SetLayerTunnelHandle = SetLayerTunnelHandle;

    *funcs = layerFuncs;
    LOG_INFO("hdi layer initialize succeed.");
    return DISPLAY_SUCCESS;
}

int32_t LayerUnInitialize(LayerFuncs *funcs)
{
    if (funcs == nullptr) {
        LOG_ERROR("LayerUnInitialize: param funcs is nullptr.");
        return DISPLAY_PARAM_ERR;
    }

    free(funcs);

    LOG_INFO("layer unInitialize succeed.");

    return DISPLAY_SUCCESS;
}
#if defined(__cplusplus)
}
#endif // __cplusplus

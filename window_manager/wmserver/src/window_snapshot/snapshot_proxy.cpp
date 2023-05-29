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

#include "snapshot_proxy.h"
#include "ipc_types.h"
#include "pixel_map.h"
#include "window_manager_hilog.h"
#include "wm_common.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_DISPLAY, "SnapshotProxy"};
}

int32_t SnapshotProxy::GetSnapshot(const sptr<IRemoteObject> &token, AAFwk::Snapshot& snapshot)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return static_cast<int32_t>(WMError::WM_ERROR_IPC_FAILED);
    }

    if (!data.WriteRemoteObject(token)) {
        WLOGFE("Write ability token failed");
        return static_cast<int32_t>(WMError::WM_ERROR_IPC_FAILED);
    }

    if (Remote()->SendRequest(TRANS_ID_GET_SNAPSHOT, data, reply, option) != ERR_NONE) {
        return static_cast<int32_t>(WMError::WM_ERROR_IPC_FAILED);
    }

    std::shared_ptr<Media::PixelMap> pixelMap(reply.ReadParcelable<Media::PixelMap>());
    snapshot.SetPixelMap(pixelMap);
    return reply.ReadInt32();
}
}
}
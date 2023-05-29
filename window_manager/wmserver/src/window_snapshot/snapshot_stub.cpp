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

#include "snapshot_stub.h"
#include <ipc_skeleton.h>
#include "window_manager_hilog.h"
#include "wm_common.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_DISPLAY, "SnapshotStub"};
}

int32_t SnapshotStub::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply,
    MessageOption &option)
{
    WLOGFI("SnapshotStub::OnRemoteRequest code is %{public}u", code);
    if (data.ReadInterfaceToken() != GetDescriptor()) {
        WLOGFE("InterfaceToken check failed!");
        return -1;
    }
    switch (code) {
        case TRANS_ID_GET_SNAPSHOT : {
            AAFwk::Snapshot snapshot_;
            sptr<IRemoteObject> abilityObject = data.ReadRemoteObject();
            int32_t ret = GetSnapshot(abilityObject, snapshot_);
            if (snapshot_.GetPixelMap() == nullptr) {
                reply.WriteParcelable(nullptr);
                reply.WriteInt32(static_cast<int32_t>(WMError::WM_ERROR_NULLPTR));
                break;
            }
            reply.WriteParcelable(snapshot_.GetPixelMap().get());
            reply.WriteInt32(ret);
            break;
        }
        default:
            WLOGFW("unknown transaction code");
            return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }
    return 0;
}
}
}

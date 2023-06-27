/*
 * Copyright (c) 2023 Huawei Technologies Co., Ltd.
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

#ifndef OHOS_ABILITY_RUNTIME_SNAPSHOT_H
#define OHOS_ABILITY_RUNTIME_SNAPSHOT_H

#include <iremote_broker.h>

#include "pixel_map.h"

namespace OHOS {
namespace AAFwk {
class Snapshot {
public:
    Snapshot() = default;
    ~Snapshot() = default;
    const std::shared_ptr<OHOS::Media::PixelMap>& GetPixelMap() const
    {
        return pixelMap_;
    }
    void SetPixelMap(const std::shared_ptr<OHOS::Media::PixelMap>& pixelMap) {}
private:
    std::shared_ptr<OHOS::Media::PixelMap> pixelMap_ = nullptr;
};

class ISnapshotHandler : public OHOS::IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.aafwk.SnapshotHandler");

    enum {
        TRANS_ID_GET_SNAPSHOT
    };

    virtual int32_t GetSnapshot(const sptr<IRemoteObject>& token, Snapshot& snapshot) = 0;
};
} // namespace AAFwk
} // namespace OHOS
#endif // OHOS_ABILITY_RUNTIME_SNAPSHOT_H

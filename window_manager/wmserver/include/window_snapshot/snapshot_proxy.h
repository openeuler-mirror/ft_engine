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

#ifndef OHOS_SNAPSHOT_PROXY_H
#define OHOS_SNAPSHOT_PROXY_H

#include <pixel_map.h>
#include <snapshot.h>
#include <iremote_proxy.h>
#include "wm_common.h"

namespace OHOS {
namespace Rosen {
class SnapshotProxy : public IRemoteProxy<AAFwk::ISnapshotHandler> {
public:
    explicit SnapshotProxy(const sptr<IRemoteObject>& impl) : IRemoteProxy<AAFwk::ISnapshotHandler>(impl) {};
    ~SnapshotProxy() = default;
    int32_t GetSnapshot(const sptr<IRemoteObject> &token, AAFwk::Snapshot& snapshot) override;

private:
    static inline BrokerDelegator<SnapshotProxy> delegator_;
};
}
}
#endif // OHOS_SNAPSHOT_PROXY_H

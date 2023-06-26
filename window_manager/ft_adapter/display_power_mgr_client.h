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

#ifndef DISPLAYMGR_DISPLAY_MGR_CLIENT_H
#define DISPLAYMGR_DISPLAY_MGR_CLIENT_H

#include <iremote_object.h>

namespace OHOS {
namespace DisplayPowerMgr {
class DisplayPowerMgrClient {
public:
    static DisplayPowerMgrClient& GetInstance() {
        static DisplayPowerMgrClient client;
        return client;
    }
    bool OverrideBrightness(uint32_t value, uint32_t displayId = 0)
    {
        return true;
    }
    bool RestoreBrightness(uint32_t displayId = 0)
    {
        return true;
    }
};
} // namespace DisplayPowerMgr
} // namespace OHOS
#endif // DISPLAYMGR_GRADUAL_ANIMATOR_H

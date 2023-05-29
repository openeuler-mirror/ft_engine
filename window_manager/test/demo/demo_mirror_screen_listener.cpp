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

#include <iostream>
#include <refbase.h>
#include "virtual_screen_group_change_listener_future.h"

using namespace OHOS;
using namespace OHOS::Rosen;

int main(int argc, char *argv[])
{
    std::cout << "===========================Start===========================" << std::endl;
    std::cout << "Please do screen mirror in 20s..." << std::endl;

    sptr<VirtualScreenGroupChangeListenerFuture> listener = new VirtualScreenGroupChangeListenerFuture();
    auto& sm = ScreenManager::GetInstance();
    sm.RegisterVirtualScreenGroupListener(listener);
    auto info = listener->mirrorChangeFuture_.GetResult(20000);

    std::cout << "mirror event=" << std::to_string(static_cast<int>(info.event))
        << ", trigger=" << info.trigger << std::endl;
    std::cout << "ids.size=" << std::to_string(info.ids.size()) << ", ";
    for (size_t i = 0; i < info.ids.size(); i++) {
        std::cout << "ids[" <<  std::to_string(i) << "]=" <<  std::to_string(info.ids[i]);
    }
    std::cout << std::endl;

    sm.UnregisterVirtualScreenGroupListener(listener);

    std::cout << "============================End============================" << std::endl;
    return 0;
}
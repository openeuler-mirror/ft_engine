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
#include "display_manager.h"

using namespace OHOS;
using namespace OHOS::Rosen;

int main(int argc, char *argv[])
{
    auto displayId = DisplayManager::GetInstance().GetDefaultDisplayId();
    std::vector<DisplayId> ids;
    ids.push_back(displayId);
    if (argc < 2) { // 2 is arg number
        std::cout << "Invalid operation." << std::endl;
        return 0;
    }
    if (atoi(argv[1]) == 1) {
        if (DisplayManager::GetInstance().Freeze(ids)) {
            std::cout << "Freeze display success." << std::endl;
            return 0;
        }
    } else if (atoi(argv[1]) == 0) {
        if (DisplayManager::GetInstance().Unfreeze(ids)) {
            std::cout << "Unfreeze display success." << std::endl;
            return 0;
        }
    } else {
        std::cout << "Invalid cmd." << std::endl;
        return 0;
    }
    std::cout << "Freeze/Unfreeze failed." << std::endl;
    return 0;
}
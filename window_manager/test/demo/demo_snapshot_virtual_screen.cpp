/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include <ctime>
#include <iostream>
#include <string>

#include "screen_manager.h"
#include "snapshot_utils.h"
#include "surface_reader.h"
#include "surface_reader_handler_impl.h"

using namespace OHOS;
using namespace OHOS::Rosen;
using namespace OHOS::Media;

namespace {
const int SLEEP_US = 10 * 1000; // 10ms
const int MAX_SNAPSHOT_COUNT = 10;
const int MAX_WAIT_COUNT = 200;
const float DEFAULT_DENSITY = 2.0;
const std::string FILE_NAME = "/data/snapshot_virtual_screen";
}

static VirtualScreenOption InitOption(ScreenId mainId, SurfaceReader& surfaceReader)
{
    auto defaultScreen = ScreenManager::GetInstance().GetScreenById(mainId);
    VirtualScreenOption option = {
        .name_ = "virtualScreen",
        .width_ = defaultScreen->GetWidth(),
        .height_ = defaultScreen->GetHeight(),
        .density_ = DEFAULT_DENSITY,
        .surface_ = surfaceReader.GetSurface(),
        .flags_ = 0,
        .isForShot_ = true,
    };
    return option;
}

int main(int argc, char *argv[])
{
    SurfaceReader surfaceReader;
    sptr<SurfaceReaderHandlerImpl> surfaceReaderHandler = new SurfaceReaderHandlerImpl();
    if (!surfaceReader.Init()) {
        std::cout << "surfaceReader init failed!" << std::endl;
        return 0;
    }
    surfaceReader.SetHandler(surfaceReaderHandler);
    ScreenId mainId = static_cast<ScreenId>(DisplayManager::GetInstance().GetDefaultDisplayId());
    VirtualScreenOption option = InitOption(mainId, surfaceReader);
    ScreenId virtualScreenId = ScreenManager::GetInstance().CreateVirtualScreen(option);
    std::vector<ScreenId> mirrorIds;
    mirrorIds.push_back(virtualScreenId);
    ScreenManager::GetInstance().MakeMirror(mainId, mirrorIds);
    int fileIndex = 1;
    auto startTime = time(nullptr);
    if (startTime < 0) {
        std::cout << "startTime error!" << std::endl;
        return 0;
    }
    while (time(nullptr) - startTime < MAX_SNAPSHOT_COUNT) {
        int waitCount = 0;
        while (!surfaceReaderHandler->IsImageOk()) {
            waitCount++;
            if (waitCount >= MAX_WAIT_COUNT) {
                std::cout << "wait image overtime" << std::endl;
                break;
            }
            usleep(SLEEP_US);
        }
        if (waitCount >= MAX_WAIT_COUNT) {
            continue;
        }
        auto pixelMap = surfaceReaderHandler->GetPixelMap();
        bool ret = SnapShotUtils::WriteToJpegWithPixelMap(FILE_NAME + std::to_string(fileIndex) + ".jpeg", *pixelMap);
        if (ret) {
            std::cout << "snapshot "<< mainId << " write to " <<
                (FILE_NAME + std::to_string(fileIndex)).c_str() << " as jpeg" << std::endl;
        } else {
            std::cout << "snapshot "<< mainId << " write to " <<
                (FILE_NAME + std::to_string(fileIndex)).c_str() << " failed!" << std::endl;
        }
        surfaceReaderHandler->ResetFlag();
        fileIndex++;
    }
    ScreenManager::GetInstance().DestroyVirtualScreen(virtualScreenId);
    std::cout << "DestroyVirtualScreen " << virtualScreenId << std::endl;
    return 0;
}

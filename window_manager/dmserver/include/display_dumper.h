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

#ifndef OHOS_ROSEN_DISPLAY_DUMPER_H
#define OHOS_ROSEN_DISPLAY_DUMPER_H

#include "dm_common.h"
#include "abstract_display_controller.h"
#include "abstract_screen_controller.h"

namespace OHOS {
namespace Rosen {
enum DumpType : uint32_t {
    DUMP_ALL_SCREEN = 0,
    DUMP_ALL_DISPLAY,
    DUMP_SCREEN,
    DUMP_DISPLAY,
    DUMP_NONE = 100,
};
class DisplayDumper : public RefBase {
public:
    DisplayDumper(const sptr<AbstractDisplayController>& abstractDisplayController,
        const sptr<AbstractScreenController>& abstractScreenController, std::recursive_mutex& mutex);
    DMError Dump(int fd, const std::vector<std::u16string>& args) const;

private:
    void ShowHelpInfo(std::string& dumpInfo) const;
    void ShowIllegalArgsInfo(std::string& dumpInfo, DMError errCode) const;

    DMError DumpInfo(const std::vector<std::string>& args, std::string& dumpInfo) const;
    DMError DumpAllScreenInfo(std::string& dumpInfo) const;
    DMError DumpScreenInfo(const sptr<AbstractScreenGroup>& screenGroup, std::string& dumpInfo) const;
    DMError DumpSpecifiedScreenInfo(ScreenId screenId, std::string& dumpInfo) const;
    DMError DumpAllDisplayInfo(std::string& dumpInfo) const;
    DMError DumpSpecifiedDisplayInfo(DisplayId displayId, std::string& dumpInfo) const;

    bool IsValidDigitString(const std::string& idStr) const;
    std::string TransferTypeToString(ScreenType type) const;
    void GetScreenInfo(const sptr<AbstractScreen>& screen, std::ostringstream& oss) const;
    void GetDisplayInfo(const sptr<AbstractDisplay>& display, std::ostringstream& oss) const;

    const sptr<AbstractDisplayController> abstractDisplayController_;
    const sptr<AbstractScreenController> abstractScreenController_;
    std::recursive_mutex& mutex_;
};
}
}
#endif // OHOS_ROSEN_DISPLAY_DUMPER_H
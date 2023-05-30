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

#ifndef FRAMEWORKS_WM_TEST_UT_MOCK_DISPLAY_MANAGER_ADAPTER_H
#define FRAMEWORKS_WM_TEST_UT_MOCK_DISPLAY_MANAGER_ADAPTER_H
#include <gmock/gmock.h>

#include "display_manager_adapter.h"

namespace OHOS {
namespace Rosen {
class MockDisplayManagerAdapter : public DisplayManagerAdapter {
public:
    MOCK_METHOD0(Clear, void());
    MOCK_METHOD2(RegisterDisplayManagerAgent, bool(const sptr<IDisplayManagerAgent>& displayManagerAgent,
        DisplayManagerAgentType type));
    MOCK_METHOD2(UnregisterDisplayManagerAgent, bool(const sptr<IDisplayManagerAgent>& displayManagerAgent,
        DisplayManagerAgentType type));
    MOCK_METHOD0(GetDefaultDisplayInfo, sptr<DisplayInfo>());
    MOCK_METHOD1(GetDisplayInfoByScreenId, sptr<DisplayInfo>(ScreenId screenId));
    MOCK_METHOD1(GetDisplaySnapshot, std::shared_ptr<Media::PixelMap>(DisplayId displayId));

    MOCK_METHOD1(WakeUpBegin, bool(PowerStateChangeReason reason));
    MOCK_METHOD0(WakeUpEnd, bool());
    MOCK_METHOD1(SuspendBegin, bool(PowerStateChangeReason reason));
    MOCK_METHOD0(SuspendEnd, bool());
    MOCK_METHOD1(SetDisplayState, bool(DisplayState state));
    MOCK_METHOD1(GetDisplayState, DisplayState(DisplayId displayId));
    MOCK_METHOD1(NotifyDisplayEvent, void(DisplayEvent event));
    MOCK_METHOD1(GetDisplayInfo, sptr<DisplayInfo>(DisplayId displayId));
    MOCK_METHOD1(GetCutoutInfo, sptr<CutoutInfo>(DisplayId displayId));
};

class MockScreenManagerAdapter : public ScreenManagerAdapter {
public:
    MOCK_METHOD0(Clear, void());
    MOCK_METHOD2(RegisterDisplayManagerAgent, bool(const sptr<IDisplayManagerAgent>& displayManagerAgent,
        DisplayManagerAgentType type));
    MOCK_METHOD2(UnregisterDisplayManagerAgent, bool(const sptr<IDisplayManagerAgent>& displayManagerAgent,
        DisplayManagerAgentType type));
    MOCK_METHOD2(RequestRotation, bool(ScreenId screenId, Rotation rotation));
    MOCK_METHOD2(CreateVirtualScreen, ScreenId(VirtualScreenOption option,
        const sptr<IDisplayManagerAgent>& displayManagerAgent));
    MOCK_METHOD1(DestroyVirtualScreen, DMError(ScreenId screenId));
    MOCK_METHOD2(SetVirtualScreenSurface, DMError(ScreenId screenId, sptr<Surface> surface));
    MOCK_METHOD1(GetScreenGroupInfoById, sptr<ScreenGroupInfo>(ScreenId screenId));
    MOCK_METHOD0(GetAllScreenInfos, std::vector<sptr<ScreenInfo>>());
    MOCK_METHOD2(MakeMirror, ScreenId(ScreenId mainScreenId, std::vector<ScreenId> mirrorScreenId));
    MOCK_METHOD2(MakeExpand, ScreenId(std::vector<ScreenId> screenId, std::vector<Point> startPoint));
    MOCK_METHOD2(SetScreenActiveMode, bool(ScreenId screenId, uint32_t modeId));
    MOCK_METHOD1(GetScreenInfo, sptr<ScreenInfo>(ScreenId screenId));
    MOCK_METHOD2(SetScreenPowerForAll, bool(ScreenPowerState state, PowerStateChangeReason reason));
    MOCK_METHOD1(GetScreenPower, ScreenPowerState(ScreenId dmsScreenId));

    MOCK_METHOD2(GetScreenSupportedColorGamuts, DMError(ScreenId screenId, std::vector<ScreenColorGamut>& colorGamuts));
    MOCK_METHOD2(GetScreenColorGamut, DMError(ScreenId screenId, ScreenColorGamut& colorGamut));
    MOCK_METHOD2(SetScreenColorGamut, DMError(ScreenId screenId, int32_t colorGamutIdx));
    MOCK_METHOD2(GetScreenGamutMap, DMError(ScreenId screenId, ScreenGamutMap& gamutMap));
    MOCK_METHOD2(SetScreenGamutMap, DMError(ScreenId screenId, ScreenGamutMap gamutMap));
    MOCK_METHOD1(SetScreenColorTransform, DMError(ScreenId screenId));

    MOCK_METHOD1(RemoveVirtualScreenFromGroup, void(std::vector<ScreenId> screens));
    MOCK_METHOD1(SetScreenRotationLocked, void(bool isLocked));
    MOCK_METHOD0(IsScreenRotationLocked, bool());
};
}
} // namespace OHOS

#endif // FRAMEWORKS_WM_TEST_UT_MOCK_DISPLAY_MANAGER_ADAPTER_H
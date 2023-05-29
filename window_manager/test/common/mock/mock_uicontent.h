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

#ifndef OHOS_ROSEN_TEST_COMMON_MOCK_IREMOTE_OBJECT_MOCKER
#define OHOS_ROSEN_TEST_COMMON_MOCK_IREMOTE_OBJECT_MOCKER

#include "ui_content.h"
#include "native_engine/native_value.h"
#include "native_engine/native_engine.h"
#include <gmock/gmock.h>
namespace OHOS {
namespace AppExecFwk {
    class Ability;
    class Configuration;
} // namespace AppExecFwk
namespace AAFwk {
    class Want;
} // namespace AAFwk
namespace MMI {
    class PointerEvent;
    class AxisEvent;
} // namespace MMI
namespace Media {
    class PixelMap;
} // namespace Media
} // namespace OHOS
namespace OHOS {
namespace Ace {
class UIContentMocker : public UIContent {
public:
    MOCK_METHOD3(Initialize, void(OHOS::Rosen::Window* window, const std::string& url, NativeValue* storage));
    MOCK_METHOD0(Foreground, void());
    MOCK_METHOD0(Background, void());
    MOCK_METHOD0(Focus, void());
    MOCK_METHOD0(UnFocus, void());
    MOCK_METHOD0(Destroy, void());
    MOCK_METHOD1(OnNewWant, void(const OHOS::AAFwk::Want& want));
    MOCK_METHOD3(Restore, void(OHOS::Rosen::Window* window, const std::string& contentInfo, NativeValue* storage));
    MOCK_CONST_METHOD0(GetContentInfo, std::string());
    MOCK_METHOD0(DestroyUIDirector, void());
    MOCK_METHOD0(ProcessBackPressed, bool());
    MOCK_METHOD1(ProcessPointerEvent, bool(const std::shared_ptr<OHOS::MMI::PointerEvent>& pointerEvent));
    MOCK_METHOD1(ProcessKeyEvent, bool(const std::shared_ptr<OHOS::MMI::KeyEvent>& keyEvent));
    MOCK_METHOD1(ProcessAxisEvent, bool(const std::shared_ptr<OHOS::MMI::AxisEvent>& axisEvent));
    MOCK_METHOD1(ProcessVsyncEvent, bool(uint64_t timeStampNanos));
    MOCK_METHOD1(UpdateConfiguration, void(const std::shared_ptr<OHOS::AppExecFwk::Configuration>& config));
    MOCK_METHOD2(UpdateViewportConfig, void(const ViewportConfig& config, OHOS::Rosen::WindowSizeChangeReason reason));
    MOCK_METHOD1(UpdateWindowMode, void(OHOS::Rosen::WindowMode mode));
    MOCK_METHOD3(HideWindowTitleButton, void(bool hideSplit, bool hideMaximize, bool hideMinimize));
    MOCK_METHOD0(GetBackgroundColor, uint32_t());
    MOCK_METHOD1(SetBackgroundColor, void(uint32_t color));
    MOCK_METHOD2(DumpInfo, void(const std::vector<std::string>& params, std::vector<std::string>& info));
    MOCK_METHOD1(SetNextFrameLayoutCallback, void(std::function<void()>&& callback));
    MOCK_METHOD1(NotifyMemoryLevel, void(int32_t level));
    MOCK_METHOD1(SetAppWindowTitle, void(const std::string& title));
    MOCK_METHOD1(SetAppWindowIcon, void(const std::shared_ptr<Media::PixelMap>& pixelMap));
    MOCK_METHOD0(GetFormRootNode, std::shared_ptr<Rosen::RSSurfaceNode>());
    MOCK_METHOD1(UpdateFormData, void(const std::string& data));
    MOCK_METHOD1(SetFormWidth, void(const float width));
    MOCK_METHOD1(SetFormHeight, void(const float height));
    MOCK_METHOD0(GetFormWidth, float());
    MOCK_METHOD0(GetFormHeight, float());
    MOCK_METHOD1(SetActionEventHandler, void(std::function<void(const std::string& action)>&& actionCallback));
    MOCK_METHOD1(SetErrorEventHandler,
        void(std::function<void(const std::string& code, const std::string& msg)>&& actionCallback));
};
} // namespace Ace
} // namespace OHOS

#endif

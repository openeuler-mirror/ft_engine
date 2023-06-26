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

#ifndef FOUNDATION_ACE_INTERFACE_INNERKITS_ACE_UI_CONTENT_H
#define FOUNDATION_ACE_INTERFACE_INNERKITS_ACE_UI_CONTENT_H

#include <string>
#include <memory>
#include <vector>
#include <window.h>

class NativeValue;

namespace OHOS::AAFwk {
class Want;
}

namespace OHOS::AppExecFwk {
class Ability;
class Configuration;
}

namespace OHOS::AppExecFwk {
class Context;
}

namespace OHOS::MMI {
class PointerEvent;
class KeyEvent;
}

namespace OHOS::Media {
    class PixelMap;
}

namespace OHOS::Ace {
class ViewportConfig {
public:
    void SetSize(int32_t width, int32_t height) {}
    void SetPosition(int32_t x, int32_t y) {}
    void SetDensity(float density) {}
};

class UIContent {
public:
    static std::unique_ptr<UIContent> Create(OHOS::AbilityRuntime::Context* context, NativeEngine* runtime)
    {
        return nullptr;
    }
    static std::unique_ptr<UIContent> Create(OHOS::AppExecFwk::Ability* ability)
    {
        return nullptr;
    }
    static void ShowDumpHelp(std::vector<std::string>& info) {}

    void Initialize(OHOS::Rosen::Window* window, const std::string& url, NativeValue* storage) {}
    void Foreground() {}
    void Background() {}
    void Focus() {}
    void UnFocus() {}
    void Destroy() {}
    void OnNewWant(const OHOS::AAFwk::Want& want) {}
    void DumpInfo(const std::vector<std::string>& params, std::vector<std::string>& info) {}
    void Restore(OHOS::Rosen::Window* window, const std::string& contentInfo, NativeValue* storage) {}
    void SetBackgroundColor(uint32_t color) {}
    void SetAppWindowTitle(const std::string& title) {}
    void SetAppWindowIcon(const std::shared_ptr<OHOS::Media::PixelMap>& pixelMap) {}
    void SetNextFrameLayoutCallback(std::function<void()>&& callback) {}
    void HideWindowTitleButton(bool hideSplit, bool hideMaximize, bool hideMinimize) {}
    void NotifyMemoryLevel(int32_t level) {}
    void UpdateWindowMode(OHOS::Rosen::WindowMode mode) {}
    void UpdateConfiguration(const std::shared_ptr<OHOS::AppExecFwk::Configuration>& config) {}
    void UpdateViewportConfig(const ViewportConfig& config, OHOS::Rosen::WindowSizeChangeReason reason) {}
    std::string GetContentInfo()
    {
        return "";
    }
    uint32_t GetBackgroundColor()
    {
        return 0;
    }
    bool ProcessPointerEvent(const std::shared_ptr<OHOS::MMI::PointerEvent>& pointerEvent)
    {
        return true;
    }
    bool ProcessKeyEvent(const std::shared_ptr<OHOS::MMI::KeyEvent>& keyEvent)
    {
        return true;
    }
    bool ProcessBackPressed()
    {
        return true;
    }
};
} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_INTERFACE_INNERKITS_ACE_UI_CONTENT_H

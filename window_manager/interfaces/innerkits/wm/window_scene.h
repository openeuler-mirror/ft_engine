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

#ifndef INTERFACES_INNERKITS_WINDOW_SCENE_H
#define INTERFACES_INNERKITS_WINDOW_SCENE_H

#include <refbase.h>
#include <iremote_object.h>

#include "window.h"
#include "window_option.h"

namespace OHOS::AppExecFwk {
    class Configuration;
}

namespace OHOS {
namespace Rosen {
class WindowScene : public RefBase {
public:
    /**
     * Default constructor used to create an empty WindowScene instance.
     */
    WindowScene() = default;

    /**
     * Default deconstructor used to deconstruct.
     *
     */
    ~WindowScene();

    /**
     * Init a WindowScene instance based on the parameters displayId, context, listener and option.
     *
     * @param displayId the id of current display
     * @param context current ability context
     * @param listener the life cycle listener of the window
     * @param option the settings for window, such as WindowType, width, height, etc
     * @return the error code of window
     */
    WMError Init(DisplayId displayId, const std::shared_ptr<AbilityRuntime::Context>& context,
        sptr<IWindowLifeCycle>& listener, sptr<WindowOption> option = nullptr);

    /**
     * Create a window instance based on the parameters windowName and option.
     *
     * @param windowName the id of this window
     * @param option the settings for window, such as WindowType, width, height, etc.
     * @return the shared pointer of window
     */
    sptr<Window> CreateWindow(const std::string& windowName, sptr<WindowOption>& option) const;

    /**
     * Get shared pointer of main window.
     *
     * @return the shared pointer of window
     */
    const sptr<Window>& GetMainWindow() const;

    /**
     * Get a set of sub window.
     *
     * @return a set of sub window
     */
    std::vector<sptr<Window>> GetSubWindow();

    /**
     * window go foreground.
     *
     * @param reason the reason of window to go to foreground, default 0.
     * @return the error code of window
     */
    WMError GoForeground(uint32_t reason = 0);

    /**
     * Window go background.
     *
     * @param reason the reason of window to go to background, default 0.
     * @return the error code of window
     */
    WMError GoBackground(uint32_t reason = 0);

    /**
     * Window go distroy.
     *
     * @return the error code of window
     */
    WMError GoDestroy();

    /**
     * Window handle new want.
     *
     * @param want ability want.
     * @return the error code of window
     */
    WMError OnNewWant(const AAFwk::Want& want);

    /**
     * Request to get the focus.
     *
     * @return the error code of window
     */
    WMError RequestFocus() const;

    /**
     * Update ability configuration.
     *
     * @param configuration the configuration of ability
     */
    void UpdateConfiguration(const std::shared_ptr<AppExecFwk::Configuration>& configuration);

    /**
     * Set main window system bar property
     *
     * @param type the type of window
     * @param property the property of system bar
     * @return the error code of window
     */
    WMError SetSystemBarProperty(WindowType type, const SystemBarProperty& property) const;

    /**
     * Get content info of main window.
     *
     * @return content info of main window
     */
    std::string GetContentInfo() const;

    /**
     * @brief Handle and notify memory.
     *
     * @param level memory level
     * @return the error code of window
     */
    WMError NotifyMemoryLevel(int32_t level) const;

public:
    static const DisplayId DEFAULT_DISPLAY_ID = 0;
    static const std::string MAIN_WINDOW_ID;

private:
    /**
     * @param context the context of a main window
     * @return the name of main window
     */
    std::string GenerateMainWindowName(const std::shared_ptr<AbilityRuntime::Context>& context) const;

private:
    sptr<Window> mainWindow_ = nullptr;
    static inline std::atomic<uint32_t> count { 0 };
    DisplayId displayId_ = DEFAULT_DISPLAY_ID;
    std::shared_ptr<AbilityRuntime::Context> context_ = nullptr;
};
} // namespace Rosen
} // namespace OHOS
#endif // INTERFACES_INNERKITS_WINDOW_SCENE_H

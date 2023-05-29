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

#include "window.h"
#include "wm_common.h"
#include "window_option.h"
#include "window_manager.h"

#include "future.h"

namespace OHOS {
namespace Rosen {
class SystemSubWindowFuture : public IVisibilityChangedListener {
using VisibleInfos = std::vector<sptr<WindowVisibilityInfo>>;
public:
    void OnWindowVisibilityChanged(const std::vector<sptr<WindowVisibilityInfo>>& windowVisibilityInfo) override
    {
        future_.SetValue(1);
    };
    RunnableFuture<int32_t> future_;
    static constexpr long WAIT_TIME = 20000;
};
}
}

using namespace OHOS;
using namespace OHOS::Rosen;

void OutputWindowInfos(const std::vector<sptr<AccessibilityWindowInfo>>& infos)
{
    std::cout << "window tree infos length :" << infos.size() << std::endl;
    std::cout << "windowId -- windowType -- displayId" << std::endl;
    for (auto info: infos) {
        std::cout << "  " << info->wid_;
        std::cout << " -- " << static_cast<int32_t>(info->type_);
        std::cout << " -- " << std::to_string(info->displayId_);
        std::cout << std::endl;
    }
}

int main(int argc, char *argv[])
{
    std::cout << "===========================Start===========================" << std::endl;
    std::cout << "Wait 20s, The Windows will close itself" << std::endl;

    std::vector<sptr<AccessibilityWindowInfo>> infos;
    WindowManager::GetInstance().GetAccessibilityWindowInfo(infos);
    std::cout << "before add window " << std::endl;
    OutputWindowInfos(infos);

    Rect baseWindowRect = { 150, 150, 400, 600 };
    sptr<WindowOption> baseOp = new WindowOption();
    baseOp->SetWindowType(WindowType::WINDOW_TYPE_FLOAT);
    baseOp->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    baseOp->SetWindowRect(baseWindowRect);

    sptr<Window> window = Window::Create("Demo_SSW_BaseWindow", baseOp, nullptr);
    window->Show();
    std::cout << "base window id = " << window->GetWindowId() << std::endl;

    Rect subWindowRect = { 200, 200, 150, 150 };
    sptr<WindowOption> subWindowOp = new WindowOption();
    subWindowOp->SetWindowType(WindowType::WINDOW_TYPE_SYSTEM_SUB_WINDOW);
    subWindowOp->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    subWindowOp->SetWindowRect(subWindowRect);
    subWindowOp->SetParentId(window->GetWindowId());

    sptr<Window> subWindow = Window::Create("Demo_SSW_SubWindow", subWindowOp, nullptr);
    subWindow->Show();
    std::cout << "sub window id = " << subWindow->GetWindowId() << std::endl;

    infos.clear();
    WindowManager::GetInstance().GetAccessibilityWindowInfo(infos);
    std::cout << "after add window:" << std::endl;
    OutputWindowInfos(infos);

    std::cout << std::endl;
    std::cout << "please check hidump to makesure the sub window node is on window tree" << std::endl;
    sptr<SystemSubWindowFuture> listener = new SystemSubWindowFuture();
    listener->future_.Reset(0);
    listener->future_.GetResult(SystemSubWindowFuture::WAIT_TIME);

    subWindow->Hide();
    window->Hide();

    subWindow->Destroy();
    window->Destroy();

    infos.clear();
    WindowManager::GetInstance().GetAccessibilityWindowInfo(infos);
    std::cout << "after destroy window:" << std::endl;
    OutputWindowInfos(infos);

    std::cout << "============================End============================" << std::endl;
    return 0;
}
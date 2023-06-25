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

#ifndef RENDER_SERVICE_CLIENT_CORE_TRANSACTION_RS_INTERFACES_H
#define RENDER_SERVICE_CLIENT_CORE_TRANSACTION_RS_INTERFACES_H

#include <memory>

class RSSurfaceNode;

namespace OHOS {
namespace AppExecFwk {
class EventHandler;
}
}

namespace OHOS {
namespace Rosen {
class VSyncReceiver;
class SurfaceCaptureCallback;

class RSInterfaces {
public:
    static RSInterfaces &GetInstance()
    {
        static RSInterfaces instance;
        return instance;
    };

    int32_t GetScreenBacklight(uint32_t id)
    {
        return 0;
    }

    void SetScreenBacklight(uint32_t id, uint32_t level)
    {
        return;
    }

    std::shared_ptr<VSyncReceiver> CreateVSyncReceiver(
        const std::string& name, const std::shared_ptr<OHOS::AppExecFwk::EventHandler> &looper = nullptr)
    {
        return nullptr;
    }

    bool TakeSurfaceCapture(std::shared_ptr<RSSurfaceNode> node,
        std::shared_ptr<SurfaceCaptureCallback> callback, float scaleX = 1.0f, float scaleY = 1.0f)
    {
        return true;
    }

private:
    RSInterfaces() = default;
    ~RSInterfaces() noexcept {};
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_TRANSACTION_RS_INTERFACES_H

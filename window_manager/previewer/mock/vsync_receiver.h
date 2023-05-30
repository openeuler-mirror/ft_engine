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

#ifndef VSYNC_VSYNC_RECEIVER_H
#define VSYNC_VSYNC_RECEIVER_H

#include <mutex>
#include <refbase.h>

namespace OHOS {
namespace Rosen {
enum VsyncError {
    GSERROR_OK = 0,
};

class VSyncCallBackListener {
public:
    using VSyncCallback = std::function<void(int64_t, void*)>;
    struct FrameCallback {
        void *userData_;
        VSyncCallback callback_;
    };
};

class VSyncReceiver : public RefBase {
public:
    using FrameCallback = VSyncCallBackListener::FrameCallback;

    VsyncError Init()
    {
        return GSERROR_OK;
    }
    VsyncError RequestNextVSync(FrameCallback callback)
    {
        return GSERROR_OK;
    }
};
} // namespace Rosen
} // namespace OHOS

#endif

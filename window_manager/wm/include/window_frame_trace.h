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

#ifndef OHOS_WINDOW_FRAME_TRACE_H
#define OHOS_WINDOW_FRAME_TRACE_H

#include <string>
namespace FRAME_TRACE {

struct TraceHandle;

class WindowFrameTrace {
public:
    WindowFrameTrace() = default;
    virtual ~WindowFrameTrace() = default;
    virtual void VsyncStartFrameTrace() {}
    virtual void VsyncStopFrameTrace() {}
};

class WindowFrameTraceImpl : public WindowFrameTrace {
public:
    WindowFrameTraceImpl() = default;
    ~WindowFrameTraceImpl() override = default;
    static WindowFrameTraceImpl* GetInstance();
#ifdef FRAME_TRACE_ENABLE
    void VsyncStartFrameTrace() override;
    void VsyncStopFrameTrace() override;

private:
    bool judgeFrameTrace_ = false;
    bool accessFrameTrace_ = false;
    struct TraceHandle* handleUI_ = nullptr;
    std::string intervalName_ = "ui";

    bool AccessFrameTrace();
#endif
};
} // namespace FRAME_TRACE
#endif // OHOS_WINDOW_FRAME_TRACE_H

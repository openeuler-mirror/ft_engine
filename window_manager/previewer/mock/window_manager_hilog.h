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

#ifndef OHOS_WM_INCLUDE_WINDOW_MANAGER_HILOG_H
#define OHOS_WM_INCLUDE_WINDOW_MANAGER_HILOG_H

#include "window_input_channel.h"
namespace OHOS {
namespace Rosen {
typedef enum {
    LOG_TYPE_MIN = 0,
    LOG_APP = 0,
    LOG_INIT = 1,
    LOG_CORE = 3,
    LOG_KMSG = 4,
    LOG_TYPE_MAX
} LogType;

namespace HiviewDFX {
using HiLogLabel = struct {
    LogType type;
    unsigned int domain;
    const char *tag;
};
} // namespace HiviewDFX

static constexpr unsigned int HILOG_DOMAIN_WINDOW = 0xD004200;
#define WLOGFD(...)
#define WLOGFI(...)
#define WLOGFW(...)
#define WLOGFE(...)
} // namespace Rosen
} // namespace OHOS
#endif // FRAMEWORKS_WM_INCLUDE_WINDOW_MANAGER_HILOG_H

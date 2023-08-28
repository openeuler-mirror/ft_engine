/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef WAYLAND_ADAPTER_HILOG_H
#define WAYLAND_ADAPTER_HILOG_H

#include "hilog/log.h"

using namespace OHOS::HiviewDFX;

namespace FT {
namespace Wayland {
static constexpr unsigned int HILOG_DOMAIN_WAYLAND = 0xD004202;

#define _W_CPRINTF(func, fmt, ...) func(LABEL, "<%{public}d>" fmt, __LINE__, ##__VA_ARGS__)
#define WLOGD(fmt, ...) _W_CPRINTF(HiLog::Debug, fmt, ##__VA_ARGS__)
#define WLOGI(fmt, ...) _W_CPRINTF(HiLog::Info, fmt, ##__VA_ARGS__)
#define WLOGW(fmt, ...) _W_CPRINTF(HiLog::Warn, fmt, ##__VA_ARGS__)
#define WLOGE(fmt, ...) _W_CPRINTF(HiLog::Error, fmt, ##__VA_ARGS__)

#define _W_FUNC __func__
#define LOG_DEBUG(fmt, ...) WLOGD("%{public}s: " fmt, _W_FUNC, ##__VA_ARGS__)
#define LOG_INFO(fmt, ...) WLOGI("%{public}s: " fmt, _W_FUNC, ##__VA_ARGS__)
#define LOG_WARN(fmt, ...) WLOGW("%{public}s: " fmt, _W_FUNC, ##__VA_ARGS__)
#define LOG_ERROR(fmt, ...) WLOGE("%{public}s: " fmt, _W_FUNC, ##__VA_ARGS__)
} // namespace Wayland
} // namespace FT
#endif // WAYLAND_ADAPTER_HILOG_H

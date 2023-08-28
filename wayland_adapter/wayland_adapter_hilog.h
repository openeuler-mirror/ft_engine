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
constexpr HiLogLabel LABEL = { LOG_CORE, 0xD004202, "WaylandAdapter" };

#define LOG_FATAL(format, ...) HiLog::Fatal(LABEL, format, ##__VA_ARGS__)
#define LOG_ERROR(format, ...) HiLog::Error(LABEL, format, ##__VA_ARGS__)
#define LOG_WARN(format, ...) HiLog::Warn(LABEL, format, ##__VA_ARGS__)
#define LOG_INFO(format, ...) HiLog::Info(LABEL, format, ##__VA_ARGS__)
#define LOG_DEBUG(format, ...) HiLog::Debug(LABEL, format, ##__VA_ARGS__)
#define LOG_TRACE(format, ...) HiLog::Trace(LABEL, format, ##__VA_ARGS__)
} // namespace Wayland
} // namespace FT
#endif // WAYLAND_ADAPTER_HILOG_H

/*
 * Copyright (c) 2022 Huawei Technologies Co., Ltd.
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

#pragma once

#include "types.h"

namespace oewm {
namespace CurrentThread {
// Thread Local Storage
struct TLS {
    ThreadId tid;
    const char *name;
    char tidString[16]; // current tid string
};
extern __thread TLS t_tls;

void CacheTid();

inline ThreadId Tid()
{
    if (OE_UNLIKELY(t_tls.tid == 0)) {
        CacheTid();
    }

    return t_tls.tid;
}

ProcessId Pid();

inline bool IsMainThread()
{
    return Tid() == Pid();
}

inline const char *Name()
{
    return t_tls.name;
}

const char *TidString();
const char *PidString();
} // namespace CurrentThread
} // namespace oewm

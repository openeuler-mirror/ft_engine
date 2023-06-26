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

#include "current_thread.h"

#include <stdlib.h>
#include <sys/syscall.h>

namespace oewm {
namespace detail {
static ProcessId pid = 0;
static char pidString[16];

inline ThreadId getThreadId()
{
    return static_cast<ThreadId>(::syscall(SYS_gettid));
}

void mainThreadInit()
{
    CurrentThread::CacheTid();
    CurrentThread::t_tls.name = "main";
    pid = CurrentThread::Pid();
    int len = ::snprintf(pidString, sizeof(pidString), "%d", pid);
    ASSERT(static_cast<std::size_t>(len) < sizeof(pidString));
}

struct MainThreadInitializer {
    MainThreadInitializer() noexcept
    {
        mainThreadInit();
    }
};

[[maybe_unused]] MainThreadInitializer mainThreadInitializer;
} // namespace detail

namespace CurrentThread {
__thread TLS t_tls = {.tid = 0, .name = "unknown"};

ProcessId Pid()
{
    return ::getpid();
}

void CacheTid()
{
    if (t_tls.tid == 0) {
        t_tls.tid = detail::getThreadId();
    }

    int len = ::snprintf(t_tls.tidString, sizeof(t_tls.tidString), "%d", t_tls.tid);
    ASSERT(static_cast<std::size_t>(len) < sizeof(t_tls.tidString));
}

const char *TidString()
{
    (void)Tid();
    return t_tls.tidString;
}

const char *PidString()
{
    return detail::pidString;
}
} // namespace CurrentThread
} // namespace oewm

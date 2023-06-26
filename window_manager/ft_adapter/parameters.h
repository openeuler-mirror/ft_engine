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

#ifndef SYSTEM_PARAMETERS_H
#define SYSTEM_PARAMETERS_H

#include <limits>
#include <string>

namespace OHOS {
namespace system {
static std::string GetParameter(const std::string& key, const std::string& def)
{
    return "";
}
static bool SetParameter(const std::string& key, const std::string& value)
{
    return true;
}
} // namespace system
} // namespace OHOS

#endif // SYSTEM_PARAMETERS_H
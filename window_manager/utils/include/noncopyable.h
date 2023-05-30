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

#ifndef OHOS_WM_INCLUDE_NONCOPYABLE_H
#define OHOS_WM_INCLUDE_NONCOPYABLE_H

#define WM_DISALLOW_COPY_AND_MOVE(className) \
    WM_DISALLOW_COPY(className);             \
    WM_DISALLOW_MOVE(className)

#define WM_DISALLOW_COPY(className)      \
    className(const className&) = delete; \
    className& operator=(const className&) = delete

#define WM_DISALLOW_MOVE(className) \
    className(className&&) = delete; \
    className& operator=(className&&) = delete

namespace OHOS::Rosen {
class NonCopyable {
protected:
    NonCopyable() = default;
    virtual ~NonCopyable() = default;

private:
    WM_DISALLOW_COPY_AND_MOVE(NonCopyable);
};
} // namespace OHOS::Rosen

#endif // OHOS_WM_INCLUDE_NONCOPYABLE_H

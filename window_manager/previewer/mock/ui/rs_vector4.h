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

#ifndef RENDER_SERVICE_CLIENT_CORE_COMMON_RS_VECTOR4_H
#define RENDER_SERVICE_CLIENT_CORE_COMMON_RS_VECTOR4_H

namespace OHOS {
namespace Rosen {
class Vector4 {
};
class Quaternion {
public:
    Quaternion(float x, float y, float z, float w) : myx(x), myy(y), myz(z), myw(w) {}
    int useXYZW()
    {
        return myx + myy + myz + myw;
    };
private:
    float myx;
    float myy;
    float myz;
    float myw;
};
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_CLIENT_CORE_COMMON_RS_VECTOR4_H
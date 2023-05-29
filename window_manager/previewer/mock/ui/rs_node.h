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
#ifndef RENDER_SERVICE_CLIENT_CORE_UI_RS_NODE_H
#define RENDER_SERVICE_CLIENT_CORE_UI_RS_NODE_H

#include "ui/rs_vector4.h"

namespace OHOS {
namespace Rosen {
class RSNode {
public:
    void SetPivotX(float pivotX);
    void SetPivotY(float pivotY);
    
    void SetScaleX(float scaleX);
    void SetScaleY(float scaleY);

    void SetRotation(float degree);
    void SetRotation(const Quaternion& quaternion);
    void SetRotationX(float degree);
    void SetRotationY(float degree);

    void SetTranslateX(float translate);
    void SetTranslateY(float translate);
    void SetTranslateZ(float translate);
};
} // Rosen
} // OHOS
#endif // RENDER_SERVICE_CLIENT_CORE_UI_RS_NODE_H
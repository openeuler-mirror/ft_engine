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

#include "windowutilmath_fuzzer.h"

#include <securec.h>

#include "display_manager.h"
#include "window.h"
#include "window_manager.h"
#include "window_scene.h"
#include "wm_math.h"

using namespace OHOS::Rosen;
using namespace OHOS::Rosen::TransformHelper;

namespace OHOS {
namespace {
    constexpr size_t DATA_MIN_SIZE = 2;
}

template<class T>
size_t GetObject(T &object, const uint8_t *data, size_t size)
{
    size_t objectSize = sizeof(object);
    if (objectSize > size) {
        return 0;
    }
    return memcpy_s(&object, objectSize, data, objectSize) == EOK ? objectSize : 0;
}

size_t InitMatrix3(OHOS::Rosen::TransformHelper::Matrix3& mat3,
    const uint8_t *data, size_t size, size_t startPos)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return 0;
    }
    const int maxRow = 3;
    const int maxCol = 3;
    for (int i = 0; i < maxRow; i++) {
        for (int j = 0; j < maxCol; j++) {
            startPos += GetObject<float>(mat3.mat_[i][j], data + startPos, size - startPos);
        }
    }
    return startPos;
}

void InitMatrix4(OHOS::Rosen::TransformHelper::Matrix4& mat4,
    const uint8_t *data, size_t size, size_t& startPos)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return;
    }
    const int maxRow = 4;
    const int maxCol = 4;
    for (int i = 0; i < maxRow; i++) {
        for (int j = 0; j < maxCol; j++) {
            startPos += GetObject<float>(mat4.mat_[i][j], data + startPos, size - startPos);
        }
    }
}

void InitVector2(OHOS::Rosen::TransformHelper::Vector2& vec2,
    const uint8_t *data, size_t size, size_t& startPos)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return;
    }
    startPos += GetObject<float>(vec2.x_, data + startPos, size - startPos);
    startPos += GetObject<float>(vec2.y_, data + startPos, size - startPos);
}

void InitVector3(OHOS::Rosen::TransformHelper::Vector3& vec3,
    const uint8_t *data, size_t size, size_t& startPos)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return;
    }
    startPos += GetObject<float>(vec3.x_, data + startPos, size - startPos);
    startPos += GetObject<float>(vec3.y_, data + startPos, size - startPos);
    startPos += GetObject<float>(vec3.y_, data + startPos, size - startPos);
}

void WindowUtilMathFuzzPart1(const uint8_t *data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return;
    }
    size_t startPos = 0;
    OHOS::Rosen::TransformHelper::Matrix3 left3;
    OHOS::Rosen::TransformHelper::Matrix3 right3;
    InitMatrix3(left3, data, size, startPos);
    InitMatrix3(right3, data, size, startPos);
    OHOS::Rosen::TransformHelper::Matrix3 result = left3 * right3;
    left3 *= right3;

    OHOS::Rosen::TransformHelper::Matrix4 left4;
    OHOS::Rosen::TransformHelper::Matrix4 right4;
    InitMatrix4(left4, data, size, startPos);
    InitMatrix4(right4, data, size, startPos);
    OHOS::Rosen::TransformHelper::Matrix4 result2 = left4 * right4;
    left4 *= right4;
    left4.Invert();
    left4.GetScale();
    left4.GetTranslation();

    float theta;
    startPos += GetObject<float>(theta, data + startPos, size - startPos);
    OHOS::Rosen::TransformHelper::CreateRotation(theta);
    OHOS::Rosen::TransformHelper::CreateRotationX(theta);
    OHOS::Rosen::TransformHelper::CreateRotationY(theta);
    OHOS::Rosen::TransformHelper::CreateRotationZ(theta);

    OHOS::Rosen::TransformHelper::Vector3 scaleVec;
    InitVector3(scaleVec, data, size, startPos);
    OHOS::Rosen::TransformHelper::CreateScale(scaleVec.x_, scaleVec.y_);
    OHOS::Rosen::TransformHelper::CreateScale(scaleVec.x_, scaleVec.y_, scaleVec.z_);

    OHOS::Rosen::TransformHelper::Vector2 vec2;
    InitVector2(vec2, data, size, startPos);
    OHOS::Rosen::TransformHelper::CreateTranslation(vec2);

    OHOS::Rosen::TransformHelper::Vector3 vec3;
    InitVector3(vec3, data, size, startPos);
    OHOS::Rosen::TransformHelper::CreateTranslation(vec3);
}

void WindowUtilMathFuzzPart2(const uint8_t *data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return;
    }
    size_t startPos = 0;

    OHOS::Rosen::TransformHelper::Vector3 eye, target, up;
    InitVector3(eye, data, size, startPos);
    InitVector3(target, data, size, startPos);
    InitVector3(up, data, size, startPos);
    OHOS::Rosen::TransformHelper::CreateLookAt(eye, target, up);

    OHOS::Rosen::TransformHelper::CreatePerspective(eye);

    OHOS::Rosen::TransformHelper::Matrix3 mat3;
    InitMatrix3(mat3, data, size, startPos);
    OHOS::Rosen::TransformHelper::Vector2 vec2;
    InitVector2(vec2, data, size, startPos);
    OHOS::Rosen::TransformHelper::Transform(vec2, mat3);

    OHOS::Rosen::TransformHelper::Matrix4 mat4;
    InitMatrix4(mat4, data, size, startPos);
    OHOS::Rosen::TransformHelper::Vector3 vec3;
    InitVector3(vec3, data, size, startPos);
    OHOS::Rosen::TransformHelper::Transform(vec3, mat4);
    float w;
    startPos += GetObject<float>(w, data + startPos, size - startPos);
    OHOS::Rosen::TransformHelper::TransformWithPerspDiv(vec3, mat4, w);
    OHOS::Rosen::TransformHelper::GetOriginScreenPoint(vec2, mat4);
}

bool DoWindowUtilMathFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }
    WindowUtilMathFuzzPart1(data, size);
    WindowUtilMathFuzzPart2(data, size);
    return true;
}
} // namespace.OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::DoWindowUtilMathFuzzTest(data, size);
    return 0;
}


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
#include "wm_math.h"
#include <cstdlib>

namespace OHOS::Rosen {
namespace TransformHelper {
const Matrix3 Matrix3::Identity = { {
    { 1, 0, 0 },
    { 0, 1, 0 },
    { 0, 0, 1 },
} };
const Matrix4 Matrix4::Identity = { {
    { 1, 0, 0, 0 },
    { 0, 1, 0, 0 },
    { 0, 0, 1, 0 },
    { 0, 0, 0, 1 },
} };

Matrix3 operator*(const Matrix3& left, const Matrix3& right)
{
    return  { {
        // row 0
        { left.mat_[0][0] * right.mat_[0][0] + left.mat_[0][1] * right.mat_[1][0] + left.mat_[0][2] * right.mat_[2][0],
        left.mat_[0][0] * right.mat_[0][1] + left.mat_[0][1] * right.mat_[1][1] + left.mat_[0][2] * right.mat_[2][1],
        left.mat_[0][0] * right.mat_[0][2] + left.mat_[0][1] * right.mat_[1][2] + left.mat_[0][2] * right.mat_[2][2] },

        // row 1
        { left.mat_[1][0] * right.mat_[0][0] + left.mat_[1][1] * right.mat_[1][0] + left.mat_[1][2] * right.mat_[2][0],
        left.mat_[1][0] * right.mat_[0][1] + left.mat_[1][1] * right.mat_[1][1] + left.mat_[1][2] * right.mat_[2][1],
        left.mat_[1][0] * right.mat_[0][2] + left.mat_[1][1] * right.mat_[1][2] + left.mat_[1][2] * right.mat_[2][2] },

        // row 2
        { left.mat_[2][0] * right.mat_[0][0] + left.mat_[2][1] * right.mat_[1][0] + left.mat_[2][2] * right.mat_[2][0],
        left.mat_[2][0] * right.mat_[0][1] + left.mat_[2][1] * right.mat_[1][1] + left.mat_[2][2] * right.mat_[2][1],
        left.mat_[2][0] * right.mat_[0][2] + left.mat_[2][1] * right.mat_[1][2] + left.mat_[2][2] * right.mat_[2][2] }
    } };
}

Matrix3& Matrix3::operator*=(const Matrix3& right)
{
    *this = *this * right;
    return *this;
}

Matrix4 operator*(const Matrix4& left, const Matrix4& right)
{
    return { {
        // row 0
        { left.mat_[0][0] * right.mat_[0][0] + left.mat_[0][1] * right.mat_[1][0] +
        left.mat_[0][2] * right.mat_[2][0] + left.mat_[0][3] * right.mat_[3][0],
        left.mat_[0][0] * right.mat_[0][1] + left.mat_[0][1] * right.mat_[1][1] +
        left.mat_[0][2] * right.mat_[2][1] + left.mat_[0][3] * right.mat_[3][1],
        left.mat_[0][0] * right.mat_[0][2] + left.mat_[0][1] * right.mat_[1][2] +
        left.mat_[0][2] * right.mat_[2][2] + left.mat_[0][3] * right.mat_[3][2],
        left.mat_[0][0] * right.mat_[0][3] + left.mat_[0][1] * right.mat_[1][3] +
        left.mat_[0][2] * right.mat_[2][3] + left.mat_[0][3] * right.mat_[3][3] },

        // row 1
        { left.mat_[1][0] * right.mat_[0][0] + left.mat_[1][1] * right.mat_[1][0] +
        left.mat_[1][2] * right.mat_[2][0] + left.mat_[1][3] * right.mat_[3][0],
        left.mat_[1][0] * right.mat_[0][1] + left.mat_[1][1] * right.mat_[1][1] +
        left.mat_[1][2] * right.mat_[2][1] + left.mat_[1][3] * right.mat_[3][1],
        left.mat_[1][0] * right.mat_[0][2] + left.mat_[1][1] * right.mat_[1][2] +
        left.mat_[1][2] * right.mat_[2][2] + left.mat_[1][3] * right.mat_[3][2],
        left.mat_[1][0] * right.mat_[0][3] + left.mat_[1][1] * right.mat_[1][3] +
        left.mat_[1][2] * right.mat_[2][3] + left.mat_[1][3] * right.mat_[3][3] },

        // row 2
        { left.mat_[2][0] * right.mat_[0][0] + left.mat_[2][1] * right.mat_[1][0] +
        left.mat_[2][2] * right.mat_[2][0] + left.mat_[2][3] * right.mat_[3][0],
        left.mat_[2][0] * right.mat_[0][1] + left.mat_[2][1] * right.mat_[1][1] +
        left.mat_[2][2] * right.mat_[2][1] + left.mat_[2][3] * right.mat_[3][1],
        left.mat_[2][0] * right.mat_[0][2] + left.mat_[2][1] * right.mat_[1][2] +
        left.mat_[2][2] * right.mat_[2][2] + left.mat_[2][3] * right.mat_[3][2],
        left.mat_[2][0] * right.mat_[0][3] + left.mat_[2][1] * right.mat_[1][3] +
        left.mat_[2][2] * right.mat_[2][3] + left.mat_[2][3] * right.mat_[3][3] },

        // row 3
        { left.mat_[3][0] * right.mat_[0][0] + left.mat_[3][1] * right.mat_[1][0] +
        left.mat_[3][2] * right.mat_[2][0] + left.mat_[3][3] * right.mat_[3][0],
        left.mat_[3][0] * right.mat_[0][1] + left.mat_[3][1] * right.mat_[1][1] +
        left.mat_[3][2] * right.mat_[2][1] + left.mat_[3][3] * right.mat_[3][1],
        left.mat_[3][0] * right.mat_[0][2] + left.mat_[3][1] * right.mat_[1][2] +
        left.mat_[3][2] * right.mat_[2][2] + left.mat_[3][3] * right.mat_[3][2],
        left.mat_[3][0] * right.mat_[0][3] + left.mat_[3][1] * right.mat_[1][3] +
        left.mat_[3][2] * right.mat_[2][3] + left.mat_[3][3] * right.mat_[3][3] }
    } };
}

Matrix4& Matrix4::operator*=(const Matrix4& right)
{
    *this = *this * right;
    return *this;
}

void Matrix4::SwapRow(int row1, int row2)
{
    float *p = mat_[row1];
    float *q = mat_[row2];
    float tmp = p[0];
    p[0] = q[0];
    q[0] = tmp;

    tmp = p[1];
    p[1] = q[1];
    q[1] = tmp;

    tmp = p[2];
    p[2] = q[2];
    q[2] = tmp;

    tmp = p[3];
    p[3] = q[3];
    q[3] = tmp;
}

void Matrix4::Invert()
{
    // Inverse matrix with Gauss-Jordan method
    Matrix4 tmp = Matrix4::Identity;
    int i, j, k;
    for (k = 0; k < MAT_SIZE; k++) {
        float t = mat_[k][k];
        if (t < MathHelper::POS_ZERO && t > MathHelper::NAG_ZERO) {
            for (i = k + 1; i < MAT_SIZE; i++) {
                if (mat_[i][k] < MathHelper::NAG_ZERO || mat_[i][k] > MathHelper::POS_ZERO) {
                    SwapRow(k, i);
                    tmp.SwapRow(k, i);
                    break;
                }
            }
            t = mat_[k][k];
        }
        for (j = 0; j <= k; j++) {
            tmp.mat_[k][j] /= t;
        }
        for (; j < MAT_SIZE; j++) {
            mat_[k][j] /= t;
            tmp.mat_[k][j] /= t;
        }
        for (i = 0; i < MAT_SIZE; i++) {
            if (i == k) {
                continue;
            }
            float u = mat_[i][k];
            for (j = 0; j <= k; j++) {
                tmp.mat_[i][j] -= tmp.mat_[k][j] * u;
            }
            for (; j < MAT_SIZE; j++) {
                mat_[i][j] -= mat_[k][j] * u;
                tmp.mat_[i][j] -= tmp.mat_[k][j] * u;
            }
        }
    }
    *this = tmp;
}

Vector3 Matrix4::GetScale() const
{
    Vector3 retVal;
    retVal.x_ = Vector3(mat_[0][0], mat_[0][1], mat_[0][2]).Length();
    retVal.y_ = Vector3(mat_[1][0], mat_[1][1], mat_[1][2]).Length();
    retVal.z_ = Vector3(mat_[2][0], mat_[2][1], mat_[2][2]).Length();
    return retVal;
}

Vector3 Matrix4::GetTranslation() const
{
    return Vector3(mat_[3][0], mat_[3][1], mat_[3][2]);
}

// Create a scale matrix with x and y scales
Matrix3 CreateScale(float xScale, float yScale)
{
    return { {
        { xScale, 0.0f, 0.0f },
        { 0.0f, yScale, 0.0f },
        { 0.0f, 0.0f, 1.0f },
    } };
}

// Create a rotation matrix about the Z axis
// theta is in radians
Matrix3 CreateRotation(float theta)
{
    return { {
        { std::cos(theta), std::sin(theta), 0.0f },
        { -std::sin(theta), std::cos(theta), 0.0f },
        { 0.0f, 0.0f, 1.0f },
    } };
}

// Create a translation matrix (on the xy-plane)
Matrix3 CreateTranslation(const Vector2& trans)
{
    return { {
        { 1.0f, 0.0f, 0.0f },
        { 0.0f, 1.0f, 0.0f },
        { trans.x_, trans.y_, 1.0f },
    } };
}

// Create a scale matrix with x, y, and z scales
Matrix4 CreateScale(float xScale, float yScale, float zScale)
{
    return { {
        { xScale, 0.0f, 0.0f, 0.0f },
        { 0.0f, yScale, 0.0f, 0.0f },
        { 0.0f, 0.0f, zScale, 0.0f },
        { 0.0f, 0.0f, 0.0f, 1.0f },
    } };
}

// Create a rotation matrix about X axis
// theta is in radians
Matrix4 CreateRotationX(float theta)
{
    return { {
        { 1.0f, 0.0f, 0.0f, 0.0f },
        { 0.0f, std::cos(theta), std::sin(theta), 0.0f },
        { 0.0f, -std::sin(theta), std::cos(theta), 0.0f },
        { 0.0f, 0.0f, 0.0f, 1.0f },
    } };
}

// Create a rotation matrix about Y axis
// theta is in radians
Matrix4 CreateRotationY(float theta)
{
    return { {
        { std::cos(theta), 0.0f, -std::sin(theta), 0.0f },
        { 0.0f, 1.0f, 0.0f, 0.0f },
        { std::sin(theta), 0.0f, std::cos(theta), 0.0f },
        { 0.0f, 0.0f, 0.0f, 1.0f },
    } };
}

// Create a rotation matrix about Z axis
// theta is in radians
Matrix4 CreateRotationZ(float theta)
{
    return { {
        { std::cos(theta), std::sin(theta), 0.0f, 0.0f },
        { -std::sin(theta), std::cos(theta), 0.0f, 0.0f },
        { 0.0f, 0.0f, 1.0f, 0.0f },
        { 0.0f, 0.0f, 0.0f, 1.0f },
    } };
}

// Create a 3D translation matrix
Matrix4 CreateTranslation(const Vector3& trans)
{
    return { {
        { 1.0f, 0.0f, 0.0f, 0.0f },
        { 0.0f, 1.0f, 0.0f, 0.0f },
        { 0.0f, 0.0f, 1.0f, 0.0f },
        { trans.x_, trans.y_, trans.z_, 1.0f },
    } };
}

Matrix4 CreateLookAt(const Vector3& eye, const Vector3& target, const Vector3& up)
{
    Vector3 zaxis = Vector3::Normalize(target - eye);
    Vector3 xaxis = Vector3::Normalize(Vector3::Cross(up, zaxis));
    Vector3 yaxis = Vector3::Normalize(Vector3::Cross(zaxis, xaxis));
    Vector3 trans;
    trans.x_ = -Vector3::Dot(xaxis, eye);
    trans.y_ = -Vector3::Dot(yaxis, eye);
    trans.z_ = -Vector3::Dot(zaxis, eye);

    return { {
        { xaxis.x_, yaxis.x_, zaxis.x_, 0.0f },
        { xaxis.y_, yaxis.y_, zaxis.y_, 0.0f },
        { xaxis.z_, yaxis.z_, zaxis.z_, 0.0f },
        { trans.x_, trans.y_, trans.z_, 1.0f }
    } };
}

Matrix4 CreatePerspective(const Vector3& camera)
{
    return { {
        { std::abs(camera.z_), 0.0f,                0.0f, 0.0f },
        { 0.0f,                std::abs(camera.z_), 0.0f, 0.0f },
        { camera.x_,           camera.y_,           0.0f, 1.0f },
        { 0.0f,                0.0f,                1.0f, 0.0f },
    } };
}

// Transform a Vector2 in xy-plane by matrix3
Vector2 Transform(const Vector2& vec, const Matrix3& mat)
{
    Vector2 retVal;
    retVal.x_ = vec.x_ * mat.mat_[0][0] + vec.y_ * mat.mat_[1][0] + mat.mat_[2][0];
    retVal.y_ = vec.x_ * mat.mat_[0][1] + vec.y_ * mat.mat_[1][1] + mat.mat_[2][1];
    return retVal;
}

// Transform a Vector3 in 3D world by matrix4
Vector3 Transform(const Vector3& vec, const Matrix4& mat)
{
    Vector3 retVal;
    retVal.x_ = vec.x_ * mat.mat_[0][0] + vec.y_ * mat.mat_[1][0] +
        vec.z_ * mat.mat_[2][0] + mat.mat_[3][0]; // 2: row2, 3: row3
    retVal.y_ = vec.x_ * mat.mat_[0][1] + vec.y_ * mat.mat_[1][1] +
        vec.z_ * mat.mat_[2][1] + mat.mat_[3][1];
    retVal.z_ = vec.x_ * mat.mat_[0][2] + vec.y_ * mat.mat_[1][2] +
        vec.z_ * mat.mat_[2][2] + mat.mat_[3][2];
    return retVal;
}

// Transform the vector and renormalize the w component
Vector3 TransformWithPerspDiv(const Vector3& vec, const Matrix4& mat, float w)
{
    Vector3 retVal;
    retVal.x_ = vec.x_ * mat.mat_[0][0] + vec.y_ * mat.mat_[1][0] +
        vec.z_ * mat.mat_[2][0] + w * mat.mat_[3][0]; // 2: row2, 3: row3
    retVal.y_ = vec.x_ * mat.mat_[0][1] + vec.y_ * mat.mat_[1][1] +
        vec.z_ * mat.mat_[2][1] + w * mat.mat_[3][1]; // 2: row2, 3: row3
    retVal.z_ = vec.x_ * mat.mat_[0][2] + vec.y_ * mat.mat_[1][2] +
        vec.z_ * mat.mat_[2][2] + w * mat.mat_[3][2]; // 2: row2, 3: row3
    float transformedW = vec.x_ * mat.mat_[0][3] + vec.y_ * mat.mat_[1][3] +
        vec.z_ * mat.mat_[2][3] + w * mat.mat_[3][3]; // 2: row2, 3: row3
    if (!MathHelper::NearZero(transformedW)) {
        transformedW = 1.0f / transformedW;
        retVal *= transformedW;
    }
    return retVal;
}

// Given a screen point, unprojects it into origin position at screen,
// based on the current transform matrix
Vector2 GetOriginScreenPoint(const Vector2& p, const Matrix4& mat)
{
    Matrix4 invertMat = mat;
    invertMat.Invert();
    // Get start point
    Vector3 screenPoint(p.x_, p.y_, 0.1f);
    Vector3 start = TransformWithPerspDiv(screenPoint, invertMat);
    // Get end point
    screenPoint.z_ = 0.9f;
    Vector3 end = TransformWithPerspDiv(screenPoint, invertMat);
    // Get the intersection point of line start-end and xy plane
    float t = end.z_ / (end.z_ - start.z_);
    return Vector2(t * start.x_ + (1 - t) * end.x_, t * start.y_ + (1 - t) * end.y_);
}
} // namespace TransformHelper
} // namespace OHOS::Rosen

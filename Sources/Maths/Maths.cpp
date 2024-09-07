#include "Maths/Maths.hpp"

#include <stdio.h>

// TODO enable this if using Vulkan, or any other API that invert some axis
// #define INVERTED_PROJECTION

namespace Maths
{

    Mat4::Mat4(f32 diagonal)
    {
        for (size_t i = 0; i < 4; i++) content[i * 5] = diagonal;
    }

    Mat4::Mat4(const Mat4& in)
    {
        for (size_t i = 0; i < 16; i++) content[i] = in.content[i];
    }

    Mat4::Mat4(const f32* data)
    {
        for (size_t j = 0; j < 4; j++)
        {
            for (size_t i = 0; i < 4; i++)
            {
                content[j * 4 + i] = data[j + i * 4];
            }
        }
    }

    Mat4 Mat4::operator*(const Mat4& in) const
    {
        Mat4 out;
        for (size_t j = 0; j < 4; j++)
        {
            for (size_t i = 0; i < 4; i++)
            {
                f32 res = 0;
                for (size_t k = 0; k < 4; k++)
                    res += content[j + k * 4] * in.content[k + i * 4];

                out.content[j + i * 4] = res;
            }
        }
        return out;
    }

    Vec4 Mat4::operator*(const Vec4& in) const
    {
        Vec4 out;
        for (size_t i = 0; i < 4; i++)
        {
            f32 res = 0;
            for (size_t k = 0; k < 4; k++) res += content[i + k * 4] * in[k];
            out[i] = res;
        }
        return out;
    }

    Mat4 Mat4::CreateXRotationMatrix(f32 angle)
    {
        Mat4 out = Mat4(1);
        f32 radA = angle;
        f32 cosA = cosf(radA);
        f32 sinA = sinf(radA);
        out.at(1, 1) = cosA;
        out.at(2, 1) = -sinA;
        out.at(1, 2) = sinA;
        out.at(2, 2) = cosA;
        return out;
    }

    Mat4 Mat4::CreateYRotationMatrix(f32 angle)
    {
        Mat4 out = Mat4(1);
        f32 radA = angle;
        f32 cosA = cosf(radA);
        f32 sinA = sinf(radA);
        out.at(0, 0) = cosA;
        out.at(2, 0) = sinA;
        out.at(0, 2) = -sinA;
        out.at(2, 2) = cosA;
        return out;
    }

    Mat4 Mat4::CreateZRotationMatrix(f32 angle)
    {
        Mat4 out = Mat4(1);
        f32 radA = angle;
        f32 cosA = cosf(radA);
        f32 sinA = sinf(radA);
        out.at(0, 0) = cosA;
        out.at(1, 0) = -sinA;
        out.at(0, 1) = sinA;
        out.at(1, 1) = cosA;
        return out;
    }

    Mat4 Mat4::CreateScaleMatrix(const Vec3& scale)
    {
        Mat4 out;
        for (s32 i = 0; i < 3; i++) out.at(i, i) = scale[i];
        out.content[15] = 1;
        return out;
    }

    Mat4 Mat4::CreateTranslationMatrix(const Vec3& translation)
    {
        Mat4 out = Mat4(1);
        for (s32 i = 0; i < 3; i++) out.at(3, i) = translation[i];
        return out;
    }

    Mat4 Mat4::CreateTransformMatrix(const Vec3& position, const Quat& rotation, const Vec3& scale)
    {
        return CreateTranslationMatrix(position) * rotation.GetRotationMatrix4() * CreateScaleMatrix(scale);
    }

    Mat4 Mat4::CreateTransformMatrix(const Vec3& position, const Quat& rotation)
    {
        return CreateTranslationMatrix(position) * rotation.GetRotationMatrix4();
    }

    Mat4 Mat4::CreateTransformMatrix(const Vec3& position, const Vec3& rotation, const Vec3& scale)
    {
        return CreateTranslationMatrix(position) * CreateRotationMatrix(rotation) * CreateScaleMatrix(scale);
    }

    Mat4 Mat4::CreateTransformMatrix(const Vec3& position, const Vec3& rotation)
    {
        return CreateTranslationMatrix(position) * CreateRotationMatrix(rotation);
    }

    Mat4 Maths::Mat4::CreateRotationMatrix(const Quat& rot)
    {
        f32 xy = rot.v.x * rot.v.y;
        f32 xz = rot.v.x * rot.v.z;
        f32 yz = rot.v.y * rot.v.z;

        f32 xw = rot.v.x * rot.a;
        f32 yw = rot.v.y * rot.a;
        f32 zw = rot.v.z * rot.a;

        Mat4 out = Mat4(1);

        out.at(0, 0) = 1 - 2 * (rot.v.y * rot.v.y) - 2 * (rot.v.z * rot.v.z);
        out.at(1, 0) = 2 * xy - 2 * zw;
        out.at(2, 0) = 2 * xz + 2 * yw;

        out.at(0, 1) = 2 * xy + 2 * zw;
        out.at(1, 1) = 1 - 2 * (rot.v.x * rot.v.x) - 2 * (rot.v.z * rot.v.z);
        out.at(2, 1) = 2 * yz - 2 * xw;

        out.at(0, 2) = 2 * xz - 2 * yw;
        out.at(1, 2) = 2 * yz - 2 * xw;
        out.at(2, 2) = 1 - 2 * (rot.v.x * rot.v.x) - 2 * (rot.v.y * rot.v.y);

        return out;
    }

    Mat4 Maths::Mat4::CreateRotationMatrix(Vec3 angles)
    {
        return CreateYRotationMatrix(angles.y) * CreateXRotationMatrix(angles.x) * CreateZRotationMatrix(angles.z);
    }

    Mat4 Mat4::CreatePerspectiveProjectionMatrix(f32 near, f32 far, f32 fov, f32 ratio)
    {
        f32 s = 1.0f / tanf(Util::ToRadians(fov / 2.0f));
        f32 param1 = -(far + near) / (far - near);
        f32 param2 = -(2 * near * far) / (far - near);
        Mat4 out;
        out.at(0, 0) = s / ratio;
#ifdef INVERTED_PROJECTION
        out.at(1, 1) = -s; // Update for Vulkan (because Y axis is inverted from OpenGL)
#else
        out.at(1, 1) = s;
#endif
        out.at(2, 2) = param1;
        out.at(2, 3) = -1;
        out.at(3, 2) = param2;
        return out;
    }

    Mat4 Mat4::CreateOrthoProjectionMatrix(f32 near, f32 far, f32 fov, f32 aspect)
    {
        float s = 1.0f / fov;
        float param1 = -2 / (far - near);
        float param2 = -(far + near) / (far - near);
        Mat4 out;
        out.at(0, 0) = s / aspect;
#ifdef INVERTED_PROJECTION
        out.at(1, 1) = -s; // Update for Vulkan (because Y axis is inverted from OpenGL)
#else
        out.at(1, 1) = s;
#endif
        out.at(2, 2) = param1;
        out.at(3, 3) = 1;
        out.at(3, 2) = param2;
        return out;
    }

    Mat4 Mat4::CreateViewMatrix(const Vec3& position, const Vec3& focus, const Vec3& up)
    {
        Mat4 temp;
        Vec3 z = (position - focus).Normalize();
        Vec3 x = up.Cross(z).Normalize();
        Vec3 y = z.Cross(x);
        Vec3 delta = Vec3(-x.Dot(position), -y.Dot(position), -z.Dot(position));
        for (int i = 0; i < 3; i++)
        {
            temp.at(i, 0) = x[i];
            temp.at(i, 1) = y[i];
            temp.at(i, 2) = z[i];
            temp.at(3, i) = delta[i];
        }
        temp.at(3, 3) = 1;
        return temp;
    }

    Mat4 Mat4::Identity()
    {
        return Mat4(1);
    }

    Vec3 Maths::Mat4::GetScaleFromTranslation() const
    {
        Vec3 x = Vec3(content[0], content[1], content[2]);
        Vec3 y = Vec3(content[4], content[5], content[6]);
        Vec3 z = Vec3(content[8], content[9], content[10]);
        return Vec3(x.Length(), y.Length(), z.Length());
    }
}
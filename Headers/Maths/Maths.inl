#include <stdio.h>

#include "Maths.hpp"

#include <assert.h>
#ifdef _WIN32
#include <corecrt_math_defines.h>
#endif

namespace Maths
{

#pragma region Vec2

    inline Vec2 Vec2::operator+(const Vec2 a) const
    {
        return Vec2(a.x + x, a.y + y);
    }

    inline Vec2 Vec2::operator+(const f32 a) const
    {
        return Vec2(a + x, a + y);
    }

    inline Vec2& Vec2::operator+=(const Vec2 a)
    {
        x += a.x;
        y += a.y;
        return *this;
    }

    inline Vec2& Vec2::operator+=(const f32 a)
    {
        x += a;
        y += a;
        return *this;
    }

    inline Vec2 Vec2::operator-(const Vec2 a) const
    {
        return Vec2(x - a.x, y - a.y);
    }

    inline Vec2 Vec2::operator-(const f32 a) const
    {
        return Vec2(x - a, y - a);
    }

    inline Vec2& Vec2::operator-=(const Vec2 a)
    {
        x -= a.x;
        y -= a.y;
        return *this;
    }

    inline Vec2& Vec2::operator-=(const f32 a)
    {
        x -= a;
        y -= a;
        return *this;
    }

    inline Vec2 Vec2::operator-() const
    {
        return Vec2(-x, -y);
    }

    inline Vec2 Vec2::operator*(const Vec2 a) const
    {
        return Vec2(x * a.x, y * a.y);
    }

    inline Vec2 Vec2::operator*(const f32 a) const
    {
        return Vec2(x * a, y * a);
    }

    inline Vec2& Vec2::operator*=(const Vec2 a)
    {
        x *= a.x;
        y *= a.y;
        return *this;
    }

    inline Vec2& Vec2::operator*=(const f32 a)
    {
        x *= a;
        y *= a;
        return *this;
    }

    inline f32& Vec2::operator[](const size_t a)
    {
        return *((&x) + a);
    }

    inline const f32& Vec2::operator[](const size_t a) const
    {
        return *((&x) + a);
    }

#pragma endregion

#pragma region Vec3

    inline f32 Vec3::Dot() const
    {
        return (x * x + y * y + z * z);
    }

    inline f32 Vec3::Length() const
    {
        return sqrtf(Dot());
    }

    inline Vec3 Vec3::operator+(const Vec3& a) const
    {
        return Vec3(a.x + x, a.y + y, a.z + z);
    }

    inline Vec3 Vec3::operator+(const f32 a) const
    {
        return Vec3(a + x, a + y, a + z);
    }

    inline Vec3& Vec3::operator+=(const Vec3& a)
    {
        x += a.x;
        y += a.y;
        z += a.z;
        return *this;
    }

    inline Vec3& Vec3::operator+=(const f32 a)
    {
        x += a;
        y += a;
        z += a;
        return *this;
    }

    inline Vec3 Vec3::operator-(const Vec3& a) const
    {
        return Vec3(x - a.x, y - a.y, z - a.z);
    }

    inline Vec3 Vec3::operator-(const f32 a) const
    {
        return Vec3(x - a, y - a, z - a);
    }

    inline Vec3& Vec3::operator-=(const Vec3& a)
    {
        x -= a.x;
        y -= a.y;
        z -= a.z;
        return *this;
    }

    inline Vec3& Vec3::operator-=(const f32 a)
    {
        x -= a;
        y -= a;
        z -= a;
        return *this;
    }

    inline Vec3 Vec3::operator-() const
    {
        return Vec3(-x,-y,-z);
    }

    inline Vec3 Vec3::operator*(const Vec3& a) const
    {
        Vec3 res = Vec3(x * a.x, y * a.y, z * a.z);
        return res;
    }

    inline Vec3 Vec3::operator*(const f32 a) const
    {
        return Vec3(x * a, y * a, z * a);
    }

    inline Vec3& Vec3::operator*=(const Vec3& a)
    {
        x *= a.x;
        y *= a.y;
        z *= a.z;
        return *this;
    }

    inline Vec3& Vec3::operator*=(const f32 a)
    {
        x *= a;
        y *= a;
        z *= a;
        return *this;
    }

    inline Vec3 Vec3::operator/(const f32 a) const
    {
        return operator*(1 / a);
    }

    inline Vec3 Vec3::operator/(const Vec3& a) const
    {
        return Vec3(x / a.x, y / a.y, z / a.z);
    }

    inline Vec3& Vec3::operator/=(const Vec3& a)
    {
        x /= a.x;
        y /= a.y;
        z /= a.z;
        return *this;
    }

    inline Vec3& Vec3::operator/=(const f32 a)
    {
        x /= a;
        y /= a;
        z /= a;
        return *this;
    }

    inline bool Vec3::operator==(const Vec3& b) const
    {
        return (x == b.x && y == b.y && z == b.z);
    }

    inline bool Vec3::operator!=(const Vec3& b) const
    {
        return !operator==(b);
    }

    inline f32& Vec3::operator[](const size_t a)
    {
        return *((&x) + a);
    }

    inline const f32& Vec3::operator[](const size_t a) const
    {
        return *((&x) + a);
    }

    inline f32 Vec3::Dot(Vec3 a) const
    {
        return (a.x * x + a.y * y + a.z * z);
    }

    inline Vec3 Vec3::Reflect(const Vec3& normal)
    {
        return operator-(normal * (2 * Dot(normal)));
    }

    inline Vec3 Vec3::Refract(const Vec3& normal, f32 ior)
    {
        f32 cosi = Dot(normal);
        f32 cost2 = 1 - ior * ior * (1 - cosi * cosi);
        if (cost2 <= 0) return Vec3();
        return (operator*(ior) - normal * (ior * cosi + sqrtf(cost2))).Normalize();
    }

    inline Vec3 Vec3::Cross(Vec3 a) const
    {
        return Vec3((y * a.z) - (z * a.y), (z * a.x) - (x * a.z), (x * a.y) - (y * a.x));
    }

    inline Vec3 Vec3::Normalize() const
    {
        return operator/(Length());
    }

#pragma endregion

#pragma region Vec4

    inline Vec3 Vec4::GetVector() const
    {
        return Vec3(x, y, z);
    }

    inline Vec4 Vec4::Homogenize() const
    {
        return Vec4(GetVector() / w);
    }

    inline f32& Vec4::operator[](const size_t a)
    {
        return *((&x) + a);
    }

    inline const f32& Vec4::operator[](const size_t a) const
    {
        return *((&x) + a);
    }

#pragma endregion

#pragma region Mat4

    inline f32& Mat4::operator[](const size_t in)
    {
        assert(in < 16);
        return content[in];
    }

    inline const f32& Mat4::operator[](const size_t in) const
    {
        assert(in < 16);
        return content[in];
    }

    inline f32& Mat4::at(const u8 x, const u8 y)
    {
        assert(x < 4 && y < 4);
        return content[x*4+y];
    }

    inline const f32& Mat4::at(const u8 x, const u8 y) const
    {
        assert(x < 4 && y < 4);
        return content[x * 4 + y];
    }

#pragma endregion

#pragma region Quat

    inline Quat::Quat(const Mat4& in)
    {
        Vec3 scale = in.GetScaleFromTranslation();
        float tr = in.at(0, 0) / scale.x + in.at(1, 1) / scale.y + in.at(2, 2) / scale.z;
        if (tr > 0)
        {
            float S = sqrtf(tr + 1.0f) * 2; // S=4*qw 
            a = 0.25f * S;
            v = Vec3(in.at(1, 2) / scale.y - in.at(2, 1) / scale.z, in.at(2, 0) / scale.z - in.at(0, 2) / scale.x, in.at(0, 1) / scale.x - in.at(1, 0) / scale.y) / S;
        }
        else
        {
            if ((in.at(0, 0) / scale.x > in.at(1, 1) / scale.y) && (in.at(0, 0) / scale.x > in.at(2, 2) / scale.z))
            {
                float S = sqrtf(1.0f + in.at(0, 0) / scale.x - in.at(1, 1) / scale.y - in.at(2, 2) / scale.z) * 2; // S=4*qx 
                a = (in.at(1, 2) / scale.y - in.at(2, 1) / scale.z) / S;
                v = Vec3(0.25f * S, (in.at(1, 0) / scale.y + in.at(0, 1) / scale.x) / S, (in.at(2, 0) / scale.z + in.at(0, 2) / scale.x) / S);
            }
            else if (in.at(1, 1) / scale.y > in.at(2, 2) / scale.z)
            {
                float S = sqrtf(1.0f + in.at(1, 1) / scale.y - in.at(0, 0) / scale.x - in.at(2, 2) / scale.z) * 2; // S=4*qy
                a = (in.at(2, 0) / scale.z - in.at(0, 2) / scale.x) / S;
                v = Vec3((in.at(1, 0) / scale.y + in.at(0, 1) / scale.x) / S, 0.25f * S, (in.at(2, 1) / scale.z + in.at(1, 2) / scale.y) / S);
            }
            else
            {
                float S = sqrtf(1.0f + in.at(2, 2) / scale.z - in.at(0, 0) / scale.x - in.at(1, 1) / scale.y) * 2; // S=4*qz
                a = (in.at(0, 1) / scale.x - in.at(1, 0) / scale.y) / S;
                v = Vec3((in.at(2, 0) / scale.z + in.at(0, 2) / scale.x) / S, (in.at(2, 1) / scale.z + in.at(1, 2) / scale.y) / S, 0.25f * S);
            }
        }
    }

    inline f32 Quat::Dot() const
    {
        return a*a + v.Dot();
    }

    inline f32 Quat::Length() const
    {
        return sqrtf(Dot());
    }

    inline Quat Quat::Conjugate() const
    {
        return Quat(-v, a);
    }

    inline Quat Quat::Normalize() const
    {
        return operator/(Length());
    }

    inline Quat Quat::NormalizeAxis() const
    {
        f32 vsq = v.Dot();
        f32 asq = a * a;
        if (asq > 1.0f || vsq < 0.0001f) return Normalize();
        f32 b = sqrtf((1 - asq) / (vsq));
        return Quat(v * b, a).Normalize();
    }

    inline Quat Quat::Inverse() const
    {
        if (Dot() < 1e-5f) return *this;
        return Conjugate()/Length();
    }

    inline Quat Quat::AxisAngle(Vec3 axis, f32 angle)
    {
        f32 hAngle = angle / 2;
        return Quat(axis * sinf(hAngle), cosf(hAngle));
    }

    inline Quat Quat::FromEuler(Vec3 euler)
    {
        Quat qx = Quat::AxisAngle(Vec3(1, 0, 0), euler.x);
        Quat qy = Quat::AxisAngle(Vec3(0, 1, 0), euler.y);
        Quat qz = Quat::AxisAngle(Vec3(0, 0, 1), euler.z);
        return qy * qx * qz;
    }

    inline f32 Quat::GetAngle()
    {
        return 2 * acosf(a);
    }

    inline Vec3 Quat::GetAxis()
    {
        f32 factor = sqrtf(1-a*a);
        return v / factor;
    }

    inline Mat4 Quat::GetRotationMatrix4() const
    {
        Mat4 result;
        f32 b = v.x;
        f32 c = v.y;
        f32 d = v.z;
        result.at(0, 0) = 2 * (a * a + b * b) - 1;
        result.at(1, 0) = 2 * (b * c - d * a);
        result.at(2, 0) = 2 * (b * d + c * a);
        result.at(0, 1) = 2 * (b * c + d * a);
        result.at(1, 1) = 2 * (a * a + c * c) - 1;
        result.at(2, 1) = 2 * (c * d - b * a);
        result.at(0, 2) = 2 * (b * d - c * a);
        result.at(1, 2) = 2 * (c * d + b * a);
        result.at(2, 2) = 2 * (a * a + d * d) - 1;
        result.at(3, 3) = 1;
        return result;
    }

    inline Quat Quat::operator+(const Quat& other) const
    {
        return Quat(v + other.v, a + other.a);
    }

    inline Quat Quat::operator-(const Quat& other) const
    {
        return Quat(v - other.v, a - other.a);
    }

    inline Quat Quat::operator-() const
    {
        return Quat(-v, -a);
    }

    inline Quat Quat::operator*(const Quat& other) const
    {
        return Quat(other.v * a + v * other.a + v.Cross(other.v), a*other.a - v.Dot(other.v));
    }

    inline Vec3 Quat::operator*(const Vec3& other) const
    {
        Quat tmp = operator*(Quat(other, 0.0f)) * Inverse();
        return Vec3(tmp.v);
    }

    inline Quat Quat::operator*(const f32 scalar) const
    {
        return Quat(v * scalar, a * scalar);
    }

    inline Quat Quat::operator/(const Quat& other) const
    {
        return Quat(v / other.v, a / other.a);
    }

    inline Quat Quat::operator/(const f32 scalar) const
    {
        return Quat(v / scalar, a / scalar);
    }

    inline Vec3 Quat::GetRight() const
    {
        return operator*(Vec3(1, 0, 0));
    }

    inline Vec3 Quat::GetUp() const
    {
        return operator*(Vec3(0, 1, 0));
    }

    inline Vec3 Quat::GetFront() const
    {
        return operator*(Vec3(0, 0, 1));
    }

    Quat Quat::Slerp(const Quat& a, Quat b, f32 alpha)
    {
        Quat result = Quat();
        f32 cosHalfTheta = a.a * b.a + a.v.x * b.v.x + a.v.y * b.v.y + a.v.z * b.v.z;
        if (cosHalfTheta < 0) {
            b = -b;
            cosHalfTheta = -cosHalfTheta;
        }
        if (fabsf(cosHalfTheta) >= 1.0f) {
            result = a;
            return result;
        }
        f32 halfTheta = acosf(cosHalfTheta);
        f32 sinHalfTheta = sqrtf(1.0f - cosHalfTheta * cosHalfTheta);
        if (fabsf(sinHalfTheta) < 0.001f)
        {
            result = a * 0.5f + b * 0.5f;
            return result;
        }
        f32 ratioA = sinf((1 - alpha) * halfTheta) / sinHalfTheta;
        f32 ratioB = sinf(alpha * halfTheta) / sinHalfTheta;
        result = a * ratioA + b * ratioB;
        return result;
    }

#pragma endregion

#pragma region Utils

    inline f32 Util::ToRadians(f32 in)
    {
        return in / 180.0f * (f32)M_PI;
    }

    inline f32 Util::ToDegrees(f32 in)
    {
        return in * 180.0f / (f32)M_PI;
    }

    inline f32 Util::MinF(f32 a, f32 b)
    {
        if (a > b)
            return b;
        return a;
    }

    inline f32 Util::MaxF(f32 a, f32 b)
    {
        if (a > b)
            return a;
        return b;
    }

#pragma endregion

}
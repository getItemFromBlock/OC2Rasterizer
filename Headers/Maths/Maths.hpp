#pragma once

#include <math.h>

#include "Types.hpp"

namespace Maths
{

    static const f32 VEC_COLLINEAR_PRECISION = 0.001f;
    static const f32 VEC_HIGH_VALUE = 1e38f;

    class Vec2;
    class Quat;

    class IVec2
    {
    public:
        s32 x, y;
        inline IVec2() : x(0), y(0) {}
        inline IVec2(const IVec2& in) : x(in.x), y(in.y) {}
        inline IVec2(const Vec2 in);
        inline IVec2(const s32 a, const s32 b) : x(a), y(b) {}
    };
    
    class Vec2
    {
    public:
        f32 x;
        f32 y;

        // Return a new empty Vec2
        inline Vec2() : x(0), y(0) {}

        // Return a new Vec2 initialised with 'a' and 'b'
        inline Vec2(f32 a, f32 b) : x(a), y(b) {}

        inline Vec2(f32 value) : Vec2(value, value) {}

        // Return a new Vec2 initialised with 'in'
        inline Vec2(const Vec2& in) : x(in.x), y(in.y) {}

        inline Vec2 operator+(const Vec2 a) const;
        inline Vec2& operator+=(const Vec2 a);
        inline Vec2 operator+(const f32 a) const;
        inline Vec2& operator+=(const f32 a);

        inline Vec2 operator-(const Vec2 a) const;
        inline Vec2& operator-=(const Vec2 a);
        inline Vec2 operator-(const f32 a) const;
        inline Vec2& operator-=(const f32 a);

        inline Vec2 operator-() const;

        inline Vec2 operator*(const Vec2 a) const;
        inline Vec2& operator*=(const Vec2 a);
        inline Vec2 operator*(const f32 a) const;
        inline Vec2& operator*=(const f32 a);

        inline const f32& operator[](const size_t a) const;

        inline f32& operator[](const size_t a);
    };

    class Vec3
    {
    public:
        f32 x;
        f32 y;
        f32 z;

        inline Vec3() : x(0), y(0), z(0) {}

        inline Vec3(f32 content) : x(content), y(content), z(content) {}

        inline Vec3(f32 a, f32 b, f32 c) : x(a), y(b), z(c) {}

        // Return a new Vec3 initialised with 'in'
        inline Vec3(const Vec3& in) : x(in.x), y(in.y), z(in.z) {}

        inline f32 Dot() const;

        inline f32 Length() const;

        inline Vec3 operator+(const Vec3& a) const;
        inline Vec3 operator+(const f32 a) const;
        inline Vec3& operator+=(const Vec3& a);
        inline Vec3& operator+=(const f32 a);

        inline Vec3 operator-(const Vec3& a) const;
        inline Vec3 operator-(const f32 a) const;
        inline Vec3& operator-=(const Vec3& a);
        inline Vec3& operator-=(const f32 a);

        inline Vec3 operator-() const;

        inline Vec3 operator*(const Vec3& a) const;
        inline Vec3 operator*(const f32 a) const;
        inline Vec3& operator*=(const Vec3& a);
        inline Vec3& operator*=(const f32 a);

        inline Vec3 operator/(const Vec3& b) const;
        inline Vec3 operator/(const f32 a) const;
        inline Vec3& operator/=(const Vec3& a);
        inline Vec3& operator/=(const f32 a);

        inline bool operator==(const Vec3& b) const;
        inline bool operator!=(const Vec3& b) const;

        inline const f32& operator[](const size_t a) const;

        inline f32& operator[](const size_t a);

        inline Vec3 Reflect(const Vec3& normal);

        inline Vec3 Refract(const Vec3& normal, f32 ior);

        // Return the dot product of 'a' and 'b'
        inline f32 Dot(Vec3 a) const;

        // Return the z component of the cross product of 'a' and 'b'
        inline Vec3 Cross(Vec3 a) const;

        // Return a vector with the same direction that 'in', but with length 1
        inline Vec3 Normalize() const;

    };

    class Vec4
    {
    public:
        f32 x;
        f32 y;
        f32 z;
        f32 w;

        // Return a new empty Vec4
        inline Vec4() : x(0), y(0), z(0), w(0) {}

        // Return a new Vec4 initialised with 'a', 'b', 'c' and 'd'
        inline Vec4(f32 a, f32 b, f32 c, f32 d = 1) : x(a), y(b), z(c), w(d) {}

        // Return a new Vec4 initialised with 'in'
        inline Vec4(const Vec3& in, f32 wIn = 1.0f) : x(in.x), y(in.y), z(in.z), w(wIn) {}

        // Return a new Vec4 initialised with 'in'
        inline Vec4(const Vec4& in) : x(in.x), y(in.y), z(in.z), w(in.w) {}

        // Return the Vec3 of Vec4
        inline Vec3 GetVector() const;

        // Divide each components by w, or set to VEC_HIGH_VALUE if w equals 0
        inline Vec4 Homogenize() const;

        inline f32& operator[](const size_t a);
        inline const f32& operator[](const size_t a) const;
    };

    class Mat4
    {
    public:
        /* data of the matrix : content[y][x]
         * Matrix is indexed with:
         *
         * 00 | 04 | 08 | 12
         * 01 | 05 | 09 | 13
         * 02 | 06 | 10 | 14
         * 03 | 07 | 11 | 15
         *
        */
        f32 content[16] = { 0 };

        Mat4() {}

        Mat4(f32 diagonal);

        Mat4(const Mat4& in);

        Mat4(const f32* data);

        Mat4 operator*(const Mat4& a) const;

        Vec4 operator*(const Vec4& a) const;

        static Mat4 Identity();

        static Mat4 CreateTransformMatrix(const Vec3& position, const Vec3& rotation, const Vec3& scale);

        static Mat4 CreateTransformMatrix(const Vec3& position, const Vec3& rotation);

        static Mat4 CreateTransformMatrix(const Vec3& position, const Quat& rotation, const Vec3& scale);

        static Mat4 CreateTransformMatrix(const Vec3& position, const Quat& rotation);

        static Mat4 CreateTranslationMatrix(const Vec3& translation);

        static Mat4 CreateScaleMatrix(const Vec3& scale);

        static Mat4 CreateRotationMatrix(const Quat& rot);

        static Mat4 CreateRotationMatrix(Vec3 angles);

        static Mat4 CreateXRotationMatrix(f32 angle);

        static Mat4 CreateYRotationMatrix(f32 angle);

        static Mat4 CreateZRotationMatrix(f32 angle);

        // aspect ratio is width / height
        static Mat4 CreatePerspectiveProjectionMatrix(f32 near, f32 far, f32 fov, f32 aspect);

        static Mat4 CreateOrthoProjectionMatrix(f32 near, f32 far, f32 fov, f32 aspect);

        static Mat4 CreateViewMatrix(const Vec3& position, const Vec3& focus, const Vec3& up);

        inline f32& operator[](const size_t a);

        inline const f32& operator[](const size_t a) const;

        inline f32& at(const u8 x, const u8 y);
        inline const f32& at(const u8 x, const u8 y) const;

        Vec3 GetScaleFromTranslation() const;
    };

    class Quat
    {
    public:
        Vec3 v;
        f32 a;

        inline Quat() : v(), a(1) {}

        inline Quat(Vec3 vector, f32 real) : v(vector), a(real) {}

        inline Quat(const Mat4& in);

        // Return the length squared
        inline f32 Dot() const;

        // Return the length
        inline f32 Length() const;

        inline Quat Conjugate() const;

        inline Quat Inverse() const;

        inline Quat Normalize() const;

        inline Quat NormalizeAxis() const;

        // Makes a quaternion representing a rotation in 3d space. Angle is in radians.
        static Quat AxisAngle(Vec3 axis, f32 angle);

        // Makes a quaternion from Euler angles (angle order is YXZ)
        static Quat FromEuler(Vec3 euler);

        inline f32 GetAngle();

        inline Vec3 GetAxis();

        inline Mat4 GetRotationMatrix4() const;

        inline Quat operator+(const Quat& other) const;

        inline Quat operator-(const Quat& other) const;

        inline Quat operator-() const;

        inline Quat operator*(const Quat& other) const;

        inline Vec3 operator*(const Vec3& other) const;

        inline Quat operator*(const f32 scalar) const;

        inline Quat operator/(const Quat& other) const;

        inline Quat operator/(const f32 scalar) const;

        inline Vec3 GetRight() const;

        inline Vec3 GetUp() const;

        inline Vec3 GetFront() const;

        inline static Quat Slerp(const Quat& a, Quat b, f32 alpha);
    };

    namespace Util
    {
        // Return the given angular value in degrees converted to radians
        inline f32 ToRadians(f32 in);

        // Return the given angular value in radians converted to degrees
        inline f32 ToDegrees(f32 in);

        inline f32 MinF(f32 a, f32 b);

        inline f32 MaxF(f32 a, f32 b);
    };
}

#include "Maths.inl"
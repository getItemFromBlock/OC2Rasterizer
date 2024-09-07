#pragma once

#include "Maths.hpp"
#include <string>

#define PI    FP32(3.14159265f)
#define H_PI  FP32(1.570796327f)
#define Q_PI  FP32(0.785398163f)
#define H3_PI FP32(4.7123889804f)
#define T_PI  FP32(6.283185307f)
#define F1    FP32(-0.129006138f)
#define F2    FP32(0.954929659f)

class FP32
{
public:
	s32 value;

    FP32() : value(0) {}

    FP32(const FP32& a) : value(a.value) {}
    FP32(const FP32* a) : value(a->value) {}

    FP32(f32 value);
    FP32(s32 value);

    f32 ToFloat() const;
    s32 ToInt() const;

    FP32 operator+(const FP32 a) const;
    FP32& operator+=(const FP32 a);

    FP32 operator-(const FP32 a) const;
    FP32& operator-=(const FP32 a);

    FP32 operator-() const;

    FP32 operator*(const FP32 a) const;
    FP32& operator*=(const FP32 a);

    FP32 operator/(const FP32 a) const;
    FP32& operator/=(const FP32 a);

    auto operator<=>(const FP32&) const = default;

    static FP32 Sqrt(FP32 v);
    static FP32 InvSqrt(FP32 v);
    static FP32 Sin(FP32 v);
    static FP32 Cos(FP32 v);
    static FP32 Tan(FP32 v);
    static u32 ToFloat(FP32 v);
    static FP32 FromFloat(u32 v);

private:

};

class FVec2
{
public:
    FP32 x;
    FP32 y;

    FVec2() : x((s32)0), y((s32)0) {}
    FVec2(FP32 a, FP32 b) : x(a), y(b) {}

    FP32& operator[](const size_t a) { return *(&x + a); }
    FVec2 operator+(const FVec2& a) const { return FVec2(x + a.x, y + a.y); }
    FVec2 operator-(const FVec2& a) const { return FVec2(x - a.x, y - a.y); }
    FVec2 operator*(const FP32 a) const { return FVec2(x * a, y * a); }
    FVec2 operator/(const FP32 a) const { return FVec2(x / a, y / a); }
};

class FVec3
{
public:
    FP32 x;
    FP32 y;
    FP32 z;

    FVec3() : x((s32)0), y((s32)0), z((s32)0) {};
    FVec3(FP32 a, FP32 b, FP32 c) : x(a), y(b), z(c) {}
    FVec3(Maths::Vec3 a) : x(a.x), y(a.y), z(a.z) {}

    FP32 Length() const { return FP32::Sqrt(Dot()); }
    FP32 InvLength() const { return FP32::InvSqrt(Dot()); }
    FP32 Dot(const FVec3& a) const { return x * a.x + y * a.y + z * a.z; }
    FP32 Dot() const { return x * x + y * y + z * z; }
    FVec3 Normalize() const { return operator*(InvLength()); }
    FP32& operator[](const size_t a) { return *(&x + a); }
    const FP32& operator[](const size_t a) const { return *(&x + a); }
    FVec3 operator+(const FVec3& a) const { return FVec3(x + a.x, y + a.y, z + a.z); }
    FVec3 operator-(const FVec3& a) const { return FVec3(x - a.x, y - a.y, z - a.z); }
    FVec3 operator*(const FP32 a) const { return FVec3(x * a, y * a, z * a); }
    FVec3 operator*(const FVec3 a) const { return FVec3(x * a.x, y * a.y, z * a.z); }
    FVec3 operator/(const FP32 a) const { return FVec3(x / a, y / a, z / a); }
};

class FVec4
{
public:
    FP32 x;
    FP32 y;
    FP32 z;
    FP32 w;

    FVec4() : x((s32)0), y((s32)0), z((s32)0), w((s32)0) {};
    FVec4(FP32 a, FP32 b, FP32 c, FP32 d) : x(a), y(b), z(c), w(d) {}
    FVec4(Maths::Vec4 a) : x(a.x), y(a.y), z(a.z), w(a.w) {}
    FVec4(FVec3 a) : x(a.x), y(a.y), z(a.z), w((s32)1) {}
    FVec4(FVec3 a, FP32 e) : x(a.x), y(a.y), z(a.z), w(e) {}

    FVec4 Homogenize() const;
    FVec3 GetVector() const;
    FP32& operator[](const size_t a) { return *(&x + a); }
    const FP32& operator[](const size_t a) const { return *(&x + a); }
};

class FMat
{
public:
    FP32 content[16] = {};

    FMat() {}
    FMat(FP32 diagonal);

    FP32& at(const u8 x, const u8 y);
    const FP32& at(const u8 x, const u8 y) const;

    FMat operator*(const FMat& a) const;

    FVec4 operator*(const FVec4& a) const;

    static FMat CreateTranslationMatrix(const FVec3& translation);
    // Angles must be clamped AND in radians
    static FMat CreateRotationMatrix(const FVec3& rotation);
    static FMat CreateScaleMatrix(const FVec3& scale);
    static FMat CreateYRotationMatrix(FP32 a);
    static FMat CreateXRotationMatrix(FP32 a);
    static FMat CreateZRotationMatrix(FP32 a);
    static FMat CreateTransformMatrix(const FVec3& position, const FVec3& rotation);
    static FMat CreateTransformMatrix(const FVec3& position, const FVec3& rotation, const FVec3& scale);
    static FMat CreatePerspectiveProjectionMatrix(FP32 n, FP32 f, FP32 fov, FP32 ratio);
};
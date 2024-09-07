#include "Maths/FP32.hpp"

#include <cmath>
#include <bit>

FP32::FP32(f32 a)
{
	value = (s32)round(131072.0f * a);
}

FP32::FP32(s32 a)
{
	value = a << 17;
}

f32 FP32::ToFloat() const
{
	return value / 131072.0f;
}

s32 FP32::ToInt() const
{
	u32 sign = (u32)value & 0x80000000;
	s32 r = value;
	if (sign)
	{
		r = -value;
	}
	r = r >> 17;
	if (sign)
	{
		r = -r;
	}
	return r;
}

FP32 FP32::operator-() const
{
	FP32 result = FP32(this);
	result.value = -result.value;
	return result;
}

FP32 FP32::operator+(const FP32 a) const
{
	FP32 result = FP32(this);
	result.value = value + a.value;
	return result;
}

FP32& FP32::operator+=(const FP32 a)
{
	value = value + a.value;
	return *this;
}

FP32 FP32::operator-(const FP32 a) const
{
	FP32 result = FP32(this);
	result.value = value - a.value;
	return result;
}

FP32& FP32::operator-=(const FP32 a)
{
	value = value - a.value;
	return *this;
}

FP32 FP32::operator*(const FP32 a) const
{
	FP32 result = FP32(this);
	result *= a;
	return result;
}

FP32& FP32::operator*=(const FP32 a)
{
	u32 sv = value & 0x80000000;
	u32 v = value;
	if (sv)
	{
		v = -value;
	}
	u32 sa = a.value & 0x80000000;
	u32 av = a.value;
	if (sa)
	{
		av = -a.value;
	}
	u64 t = (u64)v * av;
	// Rounding does not change a lot
	//t += 0x10000;
	t = t >> 17;
	if (t >= 0x80000000)
	{
		t = 0x7fffffff;
	}
	if (sv ^ sa)
	{
		t = (u64)(-(s64)t);
	}
	value = (s32)t;
	return *this;
}

FP32 FP32::operator/(const FP32 a) const
{
	FP32 result = FP32(this);
	result /= a;
	return result;
}

FP32& FP32::operator/=(const FP32 a)
{
	if (a.value == 0)
	{
		value = value >= 0 ? 0x7fffffff : 0x80000000;
		return *this;
	}
	u32 sv = value & 0x80000000;
	u32 v = value;
	if (sv)
	{
		v = -value;
	}
	u32 sa = a.value & 0x80000000;
	u32 av = a.value;
	if (sa)
	{
		av = -a.value;
	}
	std::lldiv_t r = std::lldiv((u64)v << 17, av);
	u32 r2;
	if (r.quot >= 0x80000000)
	{
		r2 = 0x7fffffff;
	}
	else
	{
		r2 = (u32)r.quot;
		// Rounding
		//if (r.rem * 2 >= av)
		//{
		//	r2 += 1;
		//}
	}
	if (sv ^ sa)
	{
		r2 = (u32)(-(s32)r2);
	}
	value = r2;
	return *this;
}

static u32 usqrt4(u32 val)
{
	u32 a, b;

	if (val < 2) return val; /* avoid div/0 */

	a = 1255;       /* starting point is relatively unimportant */

	b = val / a; a = (a + b) >> 1;
	b = val / a; a = (a + b) >> 1;
	b = val / a; a = (a + b) >> 1;
	b = val / a; a = (a + b) >> 1;
	b = val / a; a = (a + b) >> 1;

	return a;
}

FP32 FP32::Sqrt(FP32 v)
{
	if (v <= 0) return v;
	
	v.value = (u32)v.value << 1;
	v.value = usqrt4(v.value);
	v.value = v.value << 8;

	return v;
}

// FP32 implementation of the famous Fast Inverse Square Root algorithm
FP32 FP32::InvSqrt(FP32 v)
{
	if (v <= 0) return v;
	FP32 x2;
	x2.value = v.value >> 1;
	u32 i = FP32::ToFloat(v);
	i = 0x5f3759df - (i >> 1);
	FP32 y = FP32::FromFloat(i);
	y = y * (FP32(1.5f) - (y * y * x2));
	//y = y * (FP32(1.5f) - (x2 * y * y));
	return y;
}

FP32 FP32::Sin(FP32 v)
{
	assert(v < T_PI && v >= 0);
	bool neg = false;
	if (v > PI)
	{
		neg = true;
		v -= PI;
	}
	if (v > H_PI) v = PI - v;
	v = v*v*v * F1 + v * F2;
	if (neg) v = -v;
	return v;
}

FP32 FP32::Cos(FP32 v)
{
	if (v > H3_PI) v -= T_PI;
	return Sin(v + H_PI);
}

FP32 FP32::Tan(FP32 v)
{
	return Sin(v) / Cos(v);
}

u32 FP32::ToFloat(FP32 v)
{
	if (v == 0) return 0;
	u32 sign = v.value & 0x80000000;
	u32 value = (u32)v.value;
	if (sign)
	{
		value = (u32)(-v.value);
	}
	s32 e = 14 - std::countl_zero(value);
	s32 e2 = 6 - e;
	if (e2 >= 0)
	{
		value = value << e2;
	}
	else
	{
		value = value >> (-e2);
	}
	e += 127;
	value = value & 0x007fffff;
	value |= sign;
	value |= e << 23;
	return value;
}

FP32 FP32::FromFloat(u32 v)
{
	u32 sign = v & 0x80000000;
	s32 value = v & 0x7fffffff;
	if (value == 0) return 0;
	s32 e = value & 0x7f800000;
	value = value & 0x7fffff;
	value = value | 0x800000;
	e = e >> 23;
	e -= 133;
	if (e >= 0)
	{
		value = value << e;
	}
	else
	{
		e = -e;
		value = value >> e;
	}
	if (sign != 0)
	{
		value = -value;
	}
	FP32 result;
	result.value = value;
	return result;
}

FVec4 FVec4::Homogenize() const
{
	return FVec4(GetVector() / w);
}

FVec3 FVec4::GetVector() const
{
	return FVec3(x, y, z);
}

FMat::FMat(FP32 diagonal)
{
	for (int i = 0; i < 4; i++) content[i * 5] = diagonal;
}

FP32& FMat::at(const u8 x, const u8 y)
{
	assert(x < 4 && y < 4);
	return content[x * 4 + y];
}

const FP32& FMat::at(const u8 x, const u8 y) const
{
	assert(x < 4 && y < 4);
	return content[x * 4 + y];
}

FMat FMat::operator*(const FMat& a) const
{
	FMat out;
	for (int j = 0; j < 4; j++)
	{
		for (int i = 0; i < 4; i++)
		{
			FP32 res = 0;
			for (int k = 0; k < 4; k++)
				res += content[j + k * 4] * a.content[k + i * 4];

			out.content[j + i * 4] = res;
		}
	}
	return out;
}

FVec4 FMat::operator*(const FVec4& a) const
{
	FVec4 out;
	for (int i = 0; i < 4; i++)
	{
		FP32 res = 0;
		for (int k = 0; k < 4; k++) res += content[i + k * 4] * a[k];
		out[i] = res;
	}
	return out;
}

FMat FMat::CreateXRotationMatrix(FP32 angle)
{
	FMat out = FMat(1);
	FP32 cosA = FP32::Cos(angle);
	FP32 sinA = FP32::Sin(angle);
	out.at(1, 1) = cosA;
	out.at(2, 1) = -sinA;
	out.at(1, 2) = sinA;
	out.at(2, 2) = cosA;
	return out;
}

FMat FMat::CreateYRotationMatrix(FP32 angle)
{
	FMat out = FMat(1);
	FP32 cosA = FP32::Cos(angle);
	FP32 sinA = FP32::Sin(angle);
	out.at(0, 0) = cosA;
	out.at(2, 0) = sinA;
	out.at(0, 2) = -sinA;
	out.at(2, 2) = cosA;
	return out;
}

FMat FMat::CreateZRotationMatrix(FP32 angle)
{
	FMat out = FMat(1);
	FP32 cosA = FP32::Cos(angle);
	FP32 sinA = FP32::Sin(angle);
	out.at(0, 0) = cosA;
	out.at(1, 0) = -sinA;
	out.at(0, 1) = sinA;
	out.at(1, 1) = cosA;
	return out;
}

FMat FMat::CreateScaleMatrix(const FVec3& scale)
{
	FMat out;
	for (s32 i = 0; i < 3; i++) out.at(i, i) = scale[i];
	out.content[15] = 1;
	return out;
}

FMat FMat::CreateTranslationMatrix(const FVec3& translation)
{
	FMat out = FMat(1);
	for (s32 i = 0; i < 3; i++) out.at(3, i) = translation[i];
	return out;
}

FMat FMat::CreateTransformMatrix(const FVec3& position, const FVec3& rotation, const FVec3& scale)
{
	return CreateTranslationMatrix(position) * CreateRotationMatrix(rotation) * CreateScaleMatrix(scale);
}

FMat FMat::CreateTransformMatrix(const FVec3& position, const FVec3& rotation)
{
	return CreateTranslationMatrix(position) * CreateRotationMatrix(rotation);
}

FMat FMat::CreateRotationMatrix(const FVec3& angles)
{
	return CreateYRotationMatrix(angles.y) * CreateXRotationMatrix(angles.x) * CreateZRotationMatrix(angles.z);
}

FMat FMat::CreatePerspectiveProjectionMatrix(FP32 near, FP32 far, FP32 fov, FP32 ratio)
{
	FP32 s = FP32((s32)1) / FP32::Tan(fov / FP32((s32)2));
	FP32 param1 = -(far + near) / (far - near);
	FP32 param2 = -(FP32((s32)2) * near * far) / (far - near);
	FMat out;
	out.at(0, 0) = s / ratio;
	out.at(1, 1) = s;
	out.at(2, 2) = param1;
	out.at(2, 3) = -1;
	out.at(3, 2) = param2;
	return out;
}
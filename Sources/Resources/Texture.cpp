#include "Resources/Texture.hpp"

#include "Resources/ModelLoader.hpp"
#include "Defines.hpp"

using namespace Resources;
using namespace Maths;

Texture::Texture() : buffer(NULL)
{
}

Texture::Texture(u32* buffer, IVec2 res) :
    buffer(buffer),
    resolution(res),
    lResolution((1 << Util::ULog2(res.x)) - 1, (1 << Util::ULog2(res.y)) - 1)
{
}

void Texture::Destroy()
{
	if (buffer) ModelLoader::FreeImageData(buffer);
}

Vec3 Texture::SampleCube(Vec3 dir) const
{
    IVec2 r = ProcessDirection(dir);
    u32 face = r.x;
    u32 index = r.y;
    u32 col = buffer[index + face * resolution.x * resolution.x];
    Vec3 result;
    result.x = (f32)(col & 0xff);
    result.y = (f32)((col >> 8) & 0xff);
    result.z = (f32)((col >> 16) & 0xff);
    return result;
}

Vec4 Texture::Sample(Vec2 uv) const
{
    u32 index = GetIndexFromUV(uv);
    u32 col = buffer[index];
    Vec4 result;
    result.x = (f32)(col & 0xff);
    result.y = (f32)((col >> 8) & 0xff);
    result.z = (f32)((col >> 16) & 0xff);
    result.w = (f32)((col >> 24) & 0xff);
    return result;
}

u32 Texture::SampleCubeRaw(Vec3 dir) const
{
    IVec2 r = ProcessDirection(dir);
    u32 face = r.x;
    u32 index = r.y;
    return buffer[index + face * resolution.x * resolution.x];
}

u32 Texture::SampleRaw(Vec2 uv) const
{
    u32 index = GetIndexFromUV(uv);
    return buffer[index];
}

u32 Resources::Texture::GetIndexFromUV(Vec2 uv) const
{
    s32 x = (s32)(floorf(uv.x * resolution.x));
    s32 y = (s32)(floorf(uv.y * resolution.y));
#ifndef TEX_REPEAT
    if (x < 0) x = 0;
    if (x >= resolution.x) x = resolution.x - 1;
    if (y < 0) y = 0;
    if (y >= resolution.y) y = resolution.y - 1;
#else
    x &= lResolution.x;
    y &= lResolution.y;
#endif
    assert(x >= 0 && x < resolution.x && y >= 0 && y < resolution.y);
    return x + y * resolution.x;
}

u32 Resources::Texture::GetIndexFromUVCubed(Vec2 uv) const
{
    s32 x = (s32)(floorf(uv.x * resolution.x));
    s32 y = (s32)(floorf(uv.y * resolution.x));

    x &= lResolution.x;
    y &= lResolution.x;

    assert(x >= 0 && x < resolution.x && y >= 0 && y < resolution.x);
    return x + y * resolution.x;
}

IVec2 Texture::ProcessDirection(Maths::Vec3 dir) const
{
    dir = dir.Normalize();
    u32 i = dir.GetLargestScalar();
    f32 val = dir[i];
    dir = dir / val; // intersect with cube surface
    Vec2 uv = dir.DeleteScalarAt(i);
    u32 face = i * 2 + (val < 0 ? 1 : 0);
    u32 index = GetIndexFromUVCubed((uv + 1) * 0.5f);
    return IVec2(face, index);
}

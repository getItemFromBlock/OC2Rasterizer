#include "Rasterizer.hpp"

#include "Maths/Maths.hpp"
#include "Resources/ModelLoader.hpp"
#include "Defines.hpp"

using namespace Maths;
using namespace Resources;

const Vec3 lightDir = Vec3(-3, 2, 2).Normalize();

#define HSIZEX SIZEX/2
#define HSIZEY SIZEY/2

#define TEX_ALPHA
#define TEX_REPEAT
#define SPECULAR

float EdgeFunction(const Vec2 p, const Vec2 a, const Vec2 b)
{
    return (p.x - a.x) * (b.y - a.y) - (p.y - a.y) * (b.x - a.x);
}

Triangle* tris;
u32* tex;
u32 triCount = 0;
IVec2 tRes;

Vec3 sample(Vec2 uv)
{
    s32 x = (s32)(floorf(uv.x * tRes.x));
    s32 y = (s32)(floorf(uv.y * tRes.y));
#ifndef TEX_REPEAT
    if (x < 0) x = 0;
    if (x >= tRes.x) x = tRes.x - 1;
    if (y < 0) y = 0;
    if (y >= tRes.y) y = tRes.y - 1;
#else
    x = x % tRes.x;
    if (x < 0) x += tRes.x;
    y = y % tRes.y;
    if (y < 0) y += tRes.y;
#endif
    assert(x >= 0 && x < tRes.x && y >= 0 && y < tRes.y);
    u32 col = tex[x + (y * tRes.x)];
    Vec3 result;
    result.x = (f32)(col & 0xff);
    result.y = (f32)((col >> 8) & 0xff);
    result.z = (f32)((col >> 16) & 0xff);
    return result;
}

Vec4 sample2(Vec2 uv)
{
    s32 x = (s32)(floorf(uv.x * tRes.x));
    s32 y = (s32)(floorf(uv.y * tRes.y));
#ifndef TEX_REPEAT
    if (x < 0) x = 0;
    if (x >= tRes.x) x = tRes.x - 1;
    if (y < 0) y = 0;
    if (y >= tRes.y) y = tRes.y - 1;
#else
    x = x % tRes.x;
    if (x < 0) x += tRes.x;
    y = y % tRes.y;
    if (y < 0) y += tRes.y;
#endif
    assert(x >= 0 && x < tRes.x && y >= 0 && y < tRes.y);
    u32 col = tex[x + (y * tRes.x)];
    Vec4 result;
    result.x = (f32)(col & 0xff);
    result.y = (f32)((col >> 8) & 0xff);
    result.z = (f32)((col >> 16) & 0xff);
    result.w = (f32)((col >> 24) & 0xff);
    return result;
}

void Rasterizer::Init(const char* path)
{
    ModelData data = ModelLoader::ParseModelFile(path, &triCount, tRes);
    tris = data.faces;
    tex = data.tex;
}

void Rasterizer::DeInit()
{
    if (tris != NULL)
    {
        free(tris);
        tris = NULL;
        ModelLoader::FreeImageData(tex);
        tex = NULL;
    }
}

void Rasterizer::DrawScreen(RenderThread& th, f32 dt)
{
    f32 tm = dt * 0.2f;
    f32 tm2 = tm * 2;
    tm2 = sinf(tm2) * 0.4f;
    Mat4 m = Mat4::CreateTransformMatrix(Vec3(0, 0, -5), Vec3(0, tm, tm2));
    //std::copy(content, content + 16, &m.content->value);
    // screen render
    for (u32 t = 0; t < triCount; ++t)
    {
        Vec3 points[3];
        Vec3 normals[3];
        Vec2 uvs[3];
#ifdef SPECULAR
        Vec3 spos[3];
#endif
        for (int k = 0; k < 3; k++)
        {
            Vertex& d = tris[t].data[k];
            points[k] = (m * Vec4(d.pos, 1)).GetVector();
            points[k].z = 1 / points[k].z;
            points[k].x = 2 * points[k].x * -points[k].z * HSIZEY + HSIZEX;
            points[k].y = 2 * points[k].y * points[k].z * HSIZEY + HSIZEY;
            normals[k] = (m * Vec4(d.norm, 0)).GetVector() * points[k].z;
            uvs[k] = d.uv * points[k].z;
#ifdef SPECULAR
            spos[k] = (m * Vec4(d.pos, 1)).GetVector() * points[k].z;
#endif
        }

        f32 area = EdgeFunction(Vec2(points[0].x, points[0].y), Vec2(points[1].x, points[1].y), Vec2(points[2].x, points[2].y));
        if (area < 0) continue;
        area = 1 / area;

        s32 minY = (s32)(Util::MinF(Util::MinF(points[0].y, points[1].y), points[2].y));
        s32 maxY = (s32)(Util::MaxF(Util::MaxF(points[0].y, points[1].y), points[2].y));
        s32 minX = (s32)(Util::MinF(Util::MinF(points[0].x, points[1].x), points[2].x));
        s32 maxX = (s32)(Util::MaxF(Util::MaxF(points[0].x, points[1].x), points[2].x));

        Vec3 A;
        Vec3 B;
        Vec3 row;
        Vec2 pos = Vec2(minX + 0.5f, minY + 0.5f);
        for (int i = 0; i < 3; i++)
        {
            A[i] = (points[(i + 1) % 3].y - points[(i + 2) % 3].y);
            B[i] = (points[(i + 2) % 3].x - points[(i + 1) % 3].x);
            row[i] = EdgeFunction(pos, Vec2(points[(i + 1) % 3].x, points[(i + 1) % 3].y), Vec2(points[(i + 2) % 3].x, points[(i + 2) % 3].y));
        }
        for (s32 y = minY; y <= maxY; y++)
        {
            //assert(y < res.y && y >= 0);
            if (y < 0 || y >= SIZEY) continue;
            bool inside = false;
            for (s32 x = minX; x <= maxX; x++)
            {
                //assert(x < res.x && x >= 0);
                if (x < 0 || x >= SIZEX) continue;
                //assert(pIndex >= 0 && pIndex < SIZEX * SIZEY);
                //if (pIndex < 0 || pIndex >= pixelCount) continue;
                Vec3 w = row - B * (f32)(y - minY) - A * (f32)(x - minX);
                //assert(pIndex >= 0 && pIndex < pixelCount);
                if (w[0] < 0 || w[1] < 0 || w[2] < 0)
                {
                    if (inside) break;
                    continue;
                }
                inside = true;
                w = w * area;
                f32 depth = 0;
#ifdef SPECULAR
                Vec3 worldPos;
#endif
                Vec3 normal;
                Vec2 uv;
                for (int k = 0; k < 3; k++)
                {
                    depth += w[k] * points[k].z;
#ifdef SPECULAR
                    worldPos = worldPos + spos[k] * w[k];
#endif
                    normal = normal + normals[k] * w[k];
                    uv = uv + uvs[k] * w[k];
                }
                //if (depth == 0) depth = FP32(0.0001f);
                depth = 1 / depth;
                s32 pIndex = y * SIZEX + x;
#ifndef TEX_ALPHA
                if (depth < th.GetDepth(pIndex)) continue;

                th.SetDepth(pIndex, depth);
                uv = uv * depth;
                Vec3 color = sample(uv); // +FVec3(deltaB, deltaB, deltaB);
#else
                uv = uv * depth;
                Vec4 colortmp = sample2(uv);
                if (depth < th.GetDepth(pIndex) || colortmp.w < 0.5f) continue;
                th.SetDepth(pIndex, depth);
                Vec3 color = colortmp.GetVector();
#endif
                normal = (normal * depth).Normalize();
                f32 deltaA = (lightDir.Dot(normal));
                deltaA *= 0.75f;
                deltaA += 0.25f;
                if (deltaA < 0.1f) deltaA = 0.1f;
                color = color * deltaA;
#ifdef SPECULAR
                worldPos = worldPos * depth;
                Vec3 halfV = (lightDir - worldPos.Normalize()).Normalize();
                //FP32 deltaB = FP32(powf(FMax(normal.Dot(halfV), 0).ToFloat(), 64.0f) * 255.0f);
                f32 deltaB = Util::MaxF(normal.Dot(halfV), 0);
                for (int i = 0; i < 6; i++)
                {
                    deltaB *= deltaB;
                }
                deltaB *= 255;
                color = color + Vec3(deltaB, deltaB, deltaB);
#endif
                //depth = depth * FP32((s32)-32);
                //color = FVec3(depth, depth, depth);
                
                for (int i = 0; i < 3; i++)
                {
                    if (color[i] < 0) color[i] = 0;
                    if (color[i] > 255) color[i] = 255;
                }
                u32 c = ((u32)color.x << 16) + ((u32)(color.y) << 8) + (u32)(color.z);
                th.SetColor(x, y, c);
            }
        }
    }
}
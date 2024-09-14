#include "Rasterizer.hpp"

#include "Maths/Maths.hpp"
#include "Defines.hpp"
#include "RenderThread.hpp"

using namespace Maths;
using namespace Resources;

const Vec3 lightDir = Vec3(-3, 5, 2).Normalize();

float EdgeFunction(const Vec2 p, const Vec2 a, const Vec2 b)
{
    return (p.x - a.x) * (b.y - a.y) - (p.y - a.y) * (b.x - a.x);
}

void Rasterizer::Init(const char* path, const char* skyboxPath)
{
    ModelData data = ModelLoader::ParseModelFile(path, skyboxPath, &triCount);
    tris = data.faces;
    texture = Texture(data.tex, data.tRes);
    skybox = Texture(data.sky, data.sRes);
}

Rasterizer::~Rasterizer()
{
    if (tris != NULL)
    {
        free(tris);
        tris = NULL;
        texture.Destroy();
        skybox.Destroy();
    }
}

void Rasterizer::DrawSkybox(RenderThread* th, const Mat4& v)
{
    const IVec2 res = th->getResolution();
    const Vec2 half = Vec2(res.x / 2, res.y / 2);
    const Vec2 hRes = Vec2(1.0f / half.x, 1.0f / half.y);
    const Vec3 dx = (v * Vec4(hRes.x, 0, 0, 0)).GetVector();
    const Vec3 dy = (v * Vec4(0, -hRes.y, 0, 0)).GetVector();
    const Vec3 dz = (v * Vec4(0, 0, -1, 0)).GetVector();
    for (u32 y = 0; y < (u32)(res.y); y++)
    {
        for (u32 x = 0; x < (u32)(res.x); x++)
        {
            Vec2 uv = Vec2((f32)(x), (f32)(y)) - half;
            Vec3 dir = (dx * uv.x + dy * uv.y + dz);
            u32 color = skybox.SampleCubeRaw(Vec3(dir.x, dir.y, dir.z));
            u32 c = ((color & 0xff) << 16) | (color & 0xff00) | ((color & 0xff0000) >> 16);
            th->SetColor(x, y, c);
        }
    }
}

void Rasterizer::DrawScreen(RenderThread* th, f32 dt)
{
    f32 tm = dt * 0.2f;
    //f32 tm2 = tm * 2;
    //tm2 = sinf(tm2) * 0.4f;
    Mat4 m = Mat4::CreateTransformMatrix(Vec3(0, 0, 0), Vec3(0, 0, 0));
    tm *= 2.0f;
    const Vec3 cameraPos = Vec3(sin(tm) * 7, sin(tm * 0.846876f) * 2.0f, cos(tm) * 7);
    Mat4 v = Mat4::CreateViewMatrix(cameraPos, Vec3(0, 0, 0), Vec3(0, 1, 0));
    if (skybox.IsValid())
    {
        DrawSkybox(th, v.FastInverse());
    }
    Mat4 mv = v * m;
    const IVec2 res = th->getResolution();
    const IVec2 hRes = IVec2(res.x/2, res.y/2);

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
            points[k] = (mv * Vec4(d.pos, 1)).GetVector();
            points[k].z = 1 / points[k].z;
            points[k].x = 2 * points[k].x * -points[k].z * hRes.y + hRes.x;
            points[k].y = 2 * points[k].y * points[k].z * hRes.y + hRes.y;
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
            if (y < 0 || y >= res.y) continue;
            bool inside = false;
            for (s32 x = minX; x <= maxX; x++)
            {
                //assert(x < res.x && x >= 0);
                if (x < 0 || x >= res.x) continue;
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
                if (depth < -1.0f || depth >= 0.0f) continue;
                depth = 1 / depth;
                s32 pIndex = y * res.x + x;
#ifndef TEX_ALPHA
                if (depth < th->GetDepth(pIndex)) continue;

                th->SetDepth(pIndex, depth);
                uv = uv * depth;
                Vec3 color = texture.Sample(uv).GetVector(); // +FVec3(deltaB, deltaB, deltaB);
#else
                uv = uv * depth;
                Vec4 colortmp = texture.Sample(uv);
                if (depth < th->GetDepth(pIndex) || colortmp.w < 0.5f) continue;
                th->SetDepth(pIndex, depth);
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
                const Vec3 view = (cameraPos - worldPos).Normalize();
                Vec3 halfV = (lightDir + view).Normalize();
                //FP32 deltaB = FP32(powf(FMax(normal.Dot(halfV), 0).ToFloat(), 64.0f) * 255.0f);
                f32 deltaB = powf(Util::MaxF(normal.Dot(halfV), 0), 64.0f);
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
                u32 c = ((u32)color.x << 16) | ((u32)(color.y) << 8) | (u32)(color.z);
                th->SetColor(x, y, c);
            }
        }
    }
}
#pragma once

#include <stdio.h>

#include "Maths/Maths.hpp"

namespace Resources
{
	struct VertexData
	{
		Maths::Vec3 pos;
		Maths::Vec3 norm;
		Maths::Vec2 uv;
	};

	struct TriangleData
	{
		VertexData data[3];
	};

	struct Vertex
	{
		Maths::Vec3 pos;
		Maths::Vec3 norm;
		Maths::Vec2 uv;
	};

	struct Triangle
	{
		Vertex data[3];
	};

	struct ModelData
	{
		Triangle* faces = NULL;
		u32* tex = NULL;
		u32* sky = NULL;
		Maths::IVec2 tRes;
		Maths::IVec2 sRes;
	};

	namespace ModelLoader
	{
		u32 GetFileSize(FILE* in);
		char* LoadFile(const char* path, u32* sizeOut);
		bool SaveFile(const char* path, const u32* data, u32 size);
		ModelData ParseModelFile(const char* source, const char* skybox, u32* triCount);
		void FreeImageData(u32* data);
#ifdef _WIN32
		void CreateModelFile(const char* source, const char* tex, const char* dest);
#endif
	}
}

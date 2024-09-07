#pragma once

#include "RenderThread.hpp"
#include "Types.hpp"

namespace Rasterizer
{
	void DrawScreen(RenderThread& th, f32 dt);
	void Init(const char* path);
	void DeInit();
};

#pragma once

#include "Types.hpp"
#include "Signal.hpp"
#include "Maths/Maths.hpp"
#include "Defines.hpp"
#include "Rasterizer.hpp"

#ifdef _WIN32
#include <vector>
#include <Windows.h>
#endif


class RenderThread
{
public:
	RenderThread(const char* file, const char* background, u32 scale);
	RenderThread(u32 scale = 2);
	~RenderThread();

#ifdef _WIN32
	void RenderFrame(HDC hdc, Maths::IVec2 resolution);
#else
	void RenderFrame(FILE* out);
#endif
	void SetColor(u32 x, u32 y, u32 color);
	f32 GetDepth(u32 i);
	void SetDepth(u32 i, f32 d);
	f32 GetTotalTime();
	const Maths::IVec2 getResolution() const { return Maths::IVec2(resX, resY); };
private:
	//std::chrono::steady_clock::time_point start;
	u64 start;
	const u32 scaleFactor;
	const u32 resX, resY;
	
#ifdef _WIN32
	std::vector<u32> outputBuffer;
	u32 colorBuffer[SIZEX * SIZEY];
#else
	u16 colorBuffer[SIZEX * SIZEY];
	u16 stagingBuffer[SIZEX * SIZEY];
#endif
	f32 depthBuffer[SIZEX * SIZEY];

	Rasterizer rasterizer;

	//Maths::Vec3 position = Maths::Vec3(-5.30251f, 6.38824f, -7.8891f);
	//Maths::Vec2 rotation = Maths::Vec2(static_cast<f32>(M_PI_2) - 1.059891f, 0.584459f);
	//f32 fov = 3.55f;

	void ClearScreen();
	void ClearDBOnly();
#ifdef _WIN32
	void CopyToScreen(HDC hdc, Maths::IVec2 res);
#else
	void CopyToScreen(FILE* out);
#endif
};
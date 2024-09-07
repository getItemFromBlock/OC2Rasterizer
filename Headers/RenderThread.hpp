#pragma once

#include "Types.hpp"
#include "Signal.hpp"
#include "Maths/Maths.hpp"
#include "Defines.hpp"

#ifdef _WIN32
#include <vector>
#include <Windows.h>
#endif


class RenderThread
{
public:
	RenderThread() {};
	~RenderThread() {};

	void Init(const char* file);
	void DeInit();
#ifdef _WIN32
	void Init();
	void RenderFrame(HDC hdc, Maths::IVec2 resolution);
#else
	void RenderFrame(FILE* out);
#endif // _WIN32
	void SetColor(u32 x, u32 y, u32 color);
	f32 GetDepth(u32 i);
	void SetDepth(u32 i, f32 d);
	f32 GetTotalTime();
private:
	//std::chrono::steady_clock::time_point start;
	u64 start;
	
#ifdef _WIN32
	std::vector<u32> outputBuffer;
	u32 colorBuffer[SIZEX * SIZEY];
#else
	u16 colorBuffer[SIZEX * SIZEY];
	u16 stagingBuffer[RESX * RESY];
#endif
	f32 depthBuffer[SIZEX * SIZEY];

	//Maths::Vec3 position = Maths::Vec3(-5.30251f, 6.38824f, -7.8891f);
	//Maths::Vec2 rotation = Maths::Vec2(static_cast<f32>(M_PI_2) - 1.059891f, 0.584459f);
	//f32 fov = 3.55f;

	void ClearScreen();
#ifdef _WIN32
	void CopyToScreen(HDC hdc, Maths::IVec2 res);
#else
	void CopyToScreen(FILE* out);
#endif
};
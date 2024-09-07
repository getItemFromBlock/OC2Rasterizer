#include "RenderThread.hpp"

#include <assert.h>
#include <time.h>

#include "Rasterizer.hpp"
#include "Resources/ModelLoader.hpp"

using namespace Maths;

#ifdef _WIN32
LARGE_INTEGER getFILETIMEoffset()
{
	SYSTEMTIME s;
	FILETIME f;
	LARGE_INTEGER t;

	s.wYear = 1970;
	s.wMonth = 1;
	s.wDay = 1;
	s.wHour = 0;
	s.wMinute = 0;
	s.wSecond = 0;
	s.wMilliseconds = 0;
	SystemTimeToFileTime(&s, &f);
	t.QuadPart = f.dwHighDateTime;
	t.QuadPart <<= 32;
	t.QuadPart |= f.dwLowDateTime;
	return (t);
}

int clock_gettime(int X, struct timeval* tv)
{
	LARGE_INTEGER           t;
	FILETIME            f;
	double                  microseconds;
	static LARGE_INTEGER    offset;
	static double           frequencyToMicroseconds;
	static int              initialized = 0;
	static BOOL             usePerformanceCounter = 0;

	if (!initialized) {
		LARGE_INTEGER performanceFrequency;
		initialized = 1;
		usePerformanceCounter = QueryPerformanceFrequency(&performanceFrequency);
		if (usePerformanceCounter) {
			QueryPerformanceCounter(&offset);
			frequencyToMicroseconds = (double)performanceFrequency.QuadPart / 1000000.;
		}
		else {
			offset = getFILETIMEoffset();
			frequencyToMicroseconds = 10.;
		}
	}
	if (usePerformanceCounter) QueryPerformanceCounter(&t);
	else {
		GetSystemTimeAsFileTime(&f);
		t.QuadPart = f.dwHighDateTime;
		t.QuadPart <<= 32;
		t.QuadPart |= f.dwLowDateTime;
	}

	t.QuadPart -= offset.QuadPart;
	microseconds = (double)t.QuadPart / frequencyToMicroseconds;
	t.QuadPart = (s64)microseconds;
	tv->tv_sec = (u32)(t.QuadPart / 1000000);
	tv->tv_usec = (u32)(t.QuadPart % 1000000);
	return (0);
}
#define CLOCK_MONOTONIC_RAW 0
#endif

// Get time in microseconds
u64 GetNow()
{
#ifdef _WIN32
	struct timeval now;
	clock_gettime(CLOCK_MONOTONIC_RAW, &now);
	u64 result = (now.tv_sec) * 1000000 + (now.tv_usec);
#else
	struct timespec now;
	clock_gettime(CLOCK_MONOTONIC_RAW, &now);
	u64 result = (now.tv_sec) * 1000000 + (now.tv_nsec) / 1000;
#endif // _WIN32
	return result;
}

u16 GetColorFrom24(const u32 value)
{
	return ((value & 0xF80000) >> 8) | ((value & 0xFC00) >> 5) | ((value & 0xF8) >> 3);
}

void RenderThread::SetColor(u32 x, u32 y, u32 color)
{
#ifdef _WIN32
	color = color & 0x00f8fcf8;
	colorBuffer[x + y * SIZEX] = color;
#else

	colorBuffer[x + y * SIZEX] = GetColorFrom24(color);
#endif
}

float RenderThread::GetDepth(u32 i)
{
	return depthBuffer[i];
}

void RenderThread::SetDepth(u32 i, f32 depth)
{
	depthBuffer[i] = depth;
}

#ifdef _WIN32
void RenderThread::CopyToScreen(HDC hdc, IVec2 res)
{
	int counter = 0;
	for (int j = 0; j < res.y; j++)
	{
		for (int i = 0; i < res.x; i++)
		{
			int x = i * SIZEX / res.x;
			int y = j * SIZEY / res.y;
			outputBuffer[counter] = colorBuffer[x + y * SIZEX];
			counter++;
		}
	}

	//HDC hdc = GetDC(hwnd);
	if (!hdc) return;
	BITMAPINFO info = { 0 };
	info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	info.bmiHeader.biWidth = res.x;
	info.bmiHeader.biHeight = -res.y; // top-down image 
	info.bmiHeader.biPlanes = 1;
	info.bmiHeader.biBitCount = 32;
	info.bmiHeader.biCompression = BI_RGB;
	info.bmiHeader.biSizeImage = sizeof(u32) * res.x * res.y;
	int t = SetDIBitsToDevice(hdc, 0, 0, res.x, res.y, 0, 0, 0, res.y, outputBuffer.data(), &info, DIB_RGB_COLORS);
	//int r = ReleaseDC(hwnd, hdc);
}

void RenderThread::RenderFrame(HDC hdc, Maths::IVec2 res)
{
	if (static_cast<u64>(res.x) * res.y > outputBuffer.size()) outputBuffer.resize(static_cast<u64>(res.x) * res.y);
	ClearScreen();
	f32 iTime = GetTotalTime();
	//Rasterizer::DrawScreen(*this, FP32(frame*0.025f));
	Rasterizer::DrawScreen(*this, iTime);
	CopyToScreen(hdc, res);
}
#else
void RenderThread::CopyToScreen(FILE* out)
{
	for (u32 j = 0; j < SIZEY; j++)
	{
		for (u32 i = 0; i < SIZEX; i++)
		{
			u16 pixel = colorBuffer[j * SIZEX + i];
			for (u32 k = 0; k < 2; k++)
			{
				for (u32 l = 0; l < 2; l++)
				{
					stagingBuffer[(j * 2 + k) * RESX + 2 * i + l] = pixel;
				}
			}
		}
	}
	fseek(out, 0, SEEK_SET);
	fwrite(stagingBuffer, sizeof(unsigned short), RESX * RESY, out);
}

void RenderThread::RenderFrame(FILE* out)
{
	ClearScreen();
	f32 iTime = GetTotalTime();
	Rasterizer::DrawScreen(*this, iTime);
	CopyToScreen(out);
}
#endif

f32 RenderThread::GetTotalTime()
{
	u64 now = GetNow();
	u64 micros = now - start;
	return micros / 1000000.0f;
}

void RenderThread::ClearScreen()
{
	f32 min = -INFINITY;
	for (u32 i = 0; i < SIZEX * SIZEY; i++)
	{
		colorBuffer[i] = 0;
		//colorBuffer[i] = 0x202020;
		depthBuffer[i] = min;
	}
}

#ifdef _WIN32
void RenderThread::Init()
{
	Resources::ModelLoader::CreateModelFile("Assets/Models/spaceship_v2.obj",	"Assets/Textures/ship.png",				"Assets/Output/spaceship.bin");
	Resources::ModelLoader::CreateModelFile("Assets/Models/ball.obj",			"Assets/Textures/ball.png",				"Assets/Output/ball.bin");
	Resources::ModelLoader::CreateModelFile("Assets/Models/controller_all.obj",	"Assets/Textures/controller.png",		"Assets/Output/controller.bin");
	Resources::ModelLoader::CreateModelFile("Assets/Models/infuser.obj",		"Assets/Textures/infuser.png",			"Assets/Output/infuser.bin");
	Resources::ModelLoader::CreateModelFile("Assets/Models/cube.obj",			"Assets/Textures/patterned.png",		"Assets/Output/cube.bin");
	Resources::ModelLoader::CreateModelFile("Assets/Models/cube2.obj",			"Assets/Textures/patterned2.png",		"Assets/Output/cube2.bin");
	Resources::ModelLoader::CreateModelFile("Assets/Models/bean.obj",			"Assets/Textures/patterned.png",		"Assets/Output/bean.bin");
	Resources::ModelLoader::CreateModelFile("Assets/Models/torus.obj",			"Assets/Textures/earth.png",			"Assets/Output/torus.bin");
	Resources::ModelLoader::CreateModelFile("Assets/Models/blocks.obj",			"Assets/Textures/blocks.png",			"Assets/Output/blocks.bin");
	Resources::ModelLoader::CreateModelFile("Assets/Models/earth.obj",			"Assets/Textures/earth.png",			"Assets/Output/earth.bin");
	Resources::ModelLoader::CreateModelFile("Assets/Models/amogus.obj",			"Assets/Textures/amogus_palette.png",	"Assets/Output/amogus.bin");
	Resources::ModelLoader::CreateModelFile("Assets/Models/golem.obj",			"Assets/Textures/golem.png",			"Assets/Output/golem.bin");
	Resources::ModelLoader::CreateModelFile("Assets/Models/tnt.obj",			"Assets/Textures/tnt.png",				"Assets/Output/tnt.bin");

	Rasterizer::Init("Assets/Output/tnt.bin");
	start = GetNow();
}
#endif

void RenderThread::Init(const char * file)
{
	Rasterizer::Init(file);
	start = GetNow();
}

void RenderThread::DeInit()
{
	Rasterizer::DeInit();
}

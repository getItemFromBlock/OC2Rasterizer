#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#include <Types.hpp>
#include <RenderThread.hpp>

const char* helpText =
"Usage: rasterizer [OPTIONS]... file\n"
"Draw the given 3D model stored in binary format\n"
"Options:\n"
"-s			Set resolution scaling factor\n"
"-t			Set max render time\n"
"-b			Set next argument as the background image\n"
"--help		Display this information\n"
"\n"
"See https://github.com/getItemFromBlock/OC2Rasterizer/\n";

struct Parameters
{
	const char* model = NULL;
	const char* skybox = NULL;
	f32 renderTime = 15;
	s32 scale = 2;
};

bool ReadInteger(s32& i, char const* s)
{
	char* ptr = NULL;
	u32 tmp = strtol(s, &ptr, 0);
	if (!ptr)
	{
		return false;
	}
	i = tmp;
	return true;
}

bool ReadFloat(f32& i, char const* s)
{
	char* ptr = NULL;
	f32 tmp = strtof(s, &ptr);
	if (!ptr)
	{
		return false;
	}
	i = tmp;
	return true;
}

bool ParseArgs(int argc, char* argv[], Parameters& params)
{
	for (s32 i = 1; i < argc; ++i)
	{
		if (!argv[i] || !argv[i][0])
		{
			printf("Error - null argument at index %d!", i);
			return true;
		}
		if (!strcmp(argv[i], "--help"))
		{
			printf(helpText);
		}
		if (argv[i][0] != '-')
		{
			params.model = argv[i];
			continue;
		}
		switch (argv[i][1])
		{
		case 's':
			if (i + 1 == argc || !ReadInteger(params.scale, argv[i + 1]) || params.scale <= 0)
			{
				printf("Error - scale must be a positive integer\n");
				return true;
			}
			++i;
			break;
		case 't':
			if (i + 1 == argc || !ReadFloat(params.renderTime, argv[i + 1]))
			{
				printf("Error - render time must be a number greater than 0\n");
				return true;
			}
			++i;
			break;
		case 'b':
			if (i + 1 == argc || !argv[i + 1] || !argv[i + 1][0])
			{
				printf("Error - background must be a valid path\n");
				return true;
			}
			params.skybox = argv[i + 1];
			++i;
			break;
		default:
			printf("Warning - unknown option %s\n", argv[i]);
			break;
		}
	}
	return false;
}

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        printf(helpText);
        return 0;
    }

	Parameters params;
	if (ParseArgs(argc, argv, params))
	{
		return 0;
	}

    printf("Loading file %s\n", params.model);
    RenderThread render = RenderThread(params.model, params.skybox, params.scale);
    printf("File loaded\n");

    FILE* out = fopen("/dev/fb0", "wb");
    if (out == NULL)
    {
        printf("Error - cannot open output buffer\n");
        printf("Error code: %d\n", errno);
        return 1;
    }

    printf("Rendering frames for %.2f seconds\n", params.renderTime);
    u32 frameCount = 1;
    while (render.GetTotalTime() < params.renderTime)
    {
        f32 start = render.GetTotalTime();
        printf("Rendering frame %d\n", frameCount);
        render.RenderFrame(out);
        f32 end = render.GetTotalTime();
        printf("Rendered in: %.2f, total time: %.2f\n", end-start, end);
        frameCount++;
    }

    fclose(out);

    return 0;
}

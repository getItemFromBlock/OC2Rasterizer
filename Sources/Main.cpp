#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#include <Types.hpp>
#include <RenderThread.hpp>

int main(int argc, char* argv[])
{
    if (argc < 2 || argc > 3)
    {
        printf("Usage: rasterizer <model> [time]\n");
        return 0;
    }

    printf("Loading file %s\n", argv[1]);
    RenderThread render;
    render.Init(argv[1]);
    printf("File loaded\n");

    FILE* out = fopen("/dev/fb0", "wb");
    if (out == NULL)
    {
        printf("Error - cannot open output buffer\n");
        printf("Error code: %d\n", errno);
        render.DeInit();
        return 1;
    }

    f32 totalTime = 15;
    if (argc == 3)
    {
        totalTime = atof(argv[2]);
        if (totalTime <= 0)
        {
            printf("Error - render time must be a number greater than 0\n", errno);
            render.DeInit();
            return 1;
        }
    }
    printf("Rendering frames for %.2f seconds\n", totalTime);
    u32 frameCount = 1;
    while (render.GetTotalTime() < totalTime)
    {
        f32 start = render.GetTotalTime();
        printf("Rendering frame %d\n", frameCount);
        render.RenderFrame(out);
        f32 end = render.GetTotalTime();
        printf("Rendered in: %f\n", end-start);
        frameCount++;
    }

    render.DeInit();
    fclose(out);

    return 0;
}

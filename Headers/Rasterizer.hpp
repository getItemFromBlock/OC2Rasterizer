#pragma once

#include "Resources/ModelLoader.hpp"
#include "Resources/Texture.hpp"

class RenderThread;

class Rasterizer
{
public:
	Rasterizer() {};
	void Init(const char* path, const char* skyboxPath = NULL);
	~Rasterizer();

	void DrawScreen(RenderThread* th, f32 dt);
	void DrawSkybox(RenderThread* th, const Maths::Mat4& v);

	bool HasSkyboxLoaded() const { return skybox.IsValid(); }

private:
	Resources::Triangle* tris = NULL;
	Resources::Texture texture;
	Resources::Texture skybox;
	u32 triCount = 0;
};

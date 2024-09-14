#pragma once

#include "Maths/Maths.hpp"

namespace Resources
{
	class Texture
	{
	public:
		Texture();
		Texture(u32* buffer, Maths::IVec2 res);
		~Texture() {};

		void Destroy();
		bool IsValid() const { return buffer; }

		Maths::Vec3 SampleCube(Maths::Vec3 dir) const;
		Maths::Vec4 Sample(Maths::Vec2 uv) const;
		u32 SampleCubeRaw(Maths::Vec3 dir) const;
		u32 SampleRaw(Maths::Vec2 uv) const;

	private:
		u32* buffer;
		Maths::IVec2 resolution;
		Maths::IVec2 lResolution;

		u32 GetIndexFromUV(Maths::Vec2 uv) const;
		u32 GetIndexFromUVCubed(Maths::Vec2 uv) const;
		Maths::IVec2 ProcessDirection(Maths::Vec3 dir) const;
	};
}
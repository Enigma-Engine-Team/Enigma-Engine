#pragma once

#include "GLDevice.h"
#include "GLInstance.h"
#include "ISurface.h"

namespace EnigmaRHI
{
	class GLSurface : public ISurface
	{
	public:
		void Create(float _width, float _height) override { width = _width; height = _height; };
		void Destroy() override {};

		GLSurface& API_GL() override { return (*this); }
	};
}
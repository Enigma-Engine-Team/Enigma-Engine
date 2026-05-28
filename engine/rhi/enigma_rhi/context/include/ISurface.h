#pragma once

#include "IInstance.h"
#include <iostream>

namespace EnigmaRHI
{
	class GLSurface;

	class ISurface
	{
	public:

		virtual ~ISurface() = default;
		virtual void Create(float _width, float _height) = 0;
		virtual void Destroy() = 0;

		virtual GLSurface& API_GL() { throw std::runtime_error("Bad API Call: object is not a GLSurface"); }

		unsigned int GetWidth() const { return width; }
		unsigned int GetHeight() const { return height; }

	protected:

		unsigned int width;
		unsigned int height;
	};
}

#pragma once

#include <iostream>

#include "IFormat.h"
#include "ISwapChain.h"


namespace EnigmaRHI
{
	class GLRenderPass;

	class IRenderPass
	{
	public:

		virtual ~IRenderPass() = default;
		virtual void Create() = 0;
		virtual void Destroy() = 0;

		virtual void SetViewport(int x, int y, int width, int height) = 0;
		virtual void ClearColor(float r, float g, float b, float a) = 0;
		virtual void ClearBuffer(unsigned int clearFlags) = 0;

		virtual GLRenderPass& API_GL() { throw std::runtime_error("Bad API Call: object is not a GLRenderPass"); }
	};
}

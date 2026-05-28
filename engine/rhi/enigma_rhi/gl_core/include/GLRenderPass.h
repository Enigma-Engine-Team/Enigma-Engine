#pragma once

#include "GLDevice.h"
#include "IRenderPass.h"
#include "GLSwapChain.h"
#include <array>

namespace EnigmaRHI
{
	class GLRenderPass : public IRenderPass
	{
	public:

		GLRenderPass() = default;

		void Create() override;
		void Destroy() override;
		void SetViewport(int x, int y, int width, int height) override;
		void ClearColor(float r, float g, float b, float a) override;
		void ClearBuffer(unsigned int clearFlags) override;

		GLRenderPass& API_GL() override { return (*this); }
	};
}
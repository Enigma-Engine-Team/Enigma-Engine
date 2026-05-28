#pragma once

#include "ISwapChain.h"
#include "GLDevice.h"
#include "GLImage.h"
#include <cstdint>

namespace EnigmaRHI
{
	class GLSwapChain : public ISwapChain
	{
	public:

		GLSwapChain() = default;

		void Create(ISurface* surface) override;
		void Destroy() override;

		GLSwapChain& API_GL() override { return (*this); }

	private:

		IImage* sceneColorTarget;
		IImage* sceneDepthTarget;
	};
}
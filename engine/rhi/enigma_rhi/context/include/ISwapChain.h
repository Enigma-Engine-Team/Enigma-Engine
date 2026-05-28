#pragma once

#include "ICommandPool.h"
#include "IFramebuffer.h"


namespace EnigmaRHI
{
	class GLSwapChain;

	class ISwapChain
	{
	public:

		virtual ~ISwapChain() = default;
		virtual void Create(ISurface* surface) = 0;
		virtual void Destroy() = 0;

		virtual GLSwapChain& API_GL() { throw std::runtime_error("Bad API Call: object is not a GLSwapChain"); }

		IFramebuffer* renderFramebuffer = nullptr;
	};
}
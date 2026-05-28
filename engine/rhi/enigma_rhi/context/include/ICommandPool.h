#pragma once
#include "IDevice.h"
#include "ICommandBuffer.h"
#include <iostream>
#include <vector>

namespace EnigmaRHI
{
	class GLCommandPool;

	class ICommandPool
	{
	public:

		virtual ~ICommandPool() = default;
		virtual void Create() = 0;
		virtual void Destroy() = 0;

		virtual ICommandBuffer& GetCommandBuffer() = 0;

		virtual GLCommandPool& API_GL() { throw std::runtime_error("Bad API Call: object is not a GLCommandPool"); }
	};
}

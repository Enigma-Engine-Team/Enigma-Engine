#pragma once

#include "ICommandPool.h"
#include "GLCommandBuffer.h"

namespace EnigmaRHI
{
	class GLCommandPool : public ICommandPool
	{

	public:

		GLCommandPool() = default;

		void Create() override;

		void Destroy() override;

		ICommandBuffer& GetCommandBuffer() override { return commandBuffer; }

		GLCommandPool& API_GL() override { return (*this); }

	private:

		GLCommandBuffer commandBuffer;
	};
}
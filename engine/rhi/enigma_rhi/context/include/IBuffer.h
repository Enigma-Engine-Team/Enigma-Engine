#pragma once

#include <iostream>
#include <memory>

#include "IDevice.h"
#include "ICommandPool.h"

namespace EnigmaRHI
{
	class GLBuffer;

	struct DescriptorBufferInfo
	{
		IBuffer* buffer;
		size_t offset;
		size_t range;
	};

	class IBuffer
	{
	public:

		virtual ~IBuffer() = default;

		virtual void Create(size_t size, EBufferTarget target, const void* data, EBufferUsage usage) = 0;
		virtual void CreateDescriptorBufferInfo() = 0;
		virtual void CopyBuffer(IBuffer* dstBuffer, size_t size) = 0;

		virtual void MapMemory(size_t offset, size_t size, uint32_t flags) = 0;
		virtual void CopyData(const void* src, size_t size, int offset = 0) = 0;
		virtual void UnMapMemory() = 0;

		virtual void Bind(EBufferTarget target) = 0;
		virtual void Unbind(EBufferTarget target) = 0;

		virtual void Destroy() = 0;

		DescriptorBufferInfo bufferInfo{};

		virtual GLBuffer& API_GL() { throw std::runtime_error("Bad API Call: object is not a GLBuffer"); }
	};
}
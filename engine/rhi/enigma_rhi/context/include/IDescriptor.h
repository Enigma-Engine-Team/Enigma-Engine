#pragma once

#include <iostream>

#include "IDevice.h"
#include "IImage.h"
#include "IBuffer.h"

#include <vector>

namespace EnigmaRHI
{
	class GLDescriptor;


	struct Binding
	{
		uint32_t binding;

		unsigned int imageId;
		IBuffer* buffer = nullptr;
	};

	
	class IDescriptor
	{
	public:

		virtual ~IDescriptor() = default;

		virtual void Create() = 0;
		virtual void Destroy() = 0;

		virtual void AddBufferBinding(uint32_t binding) = 0;
		virtual void AddImageBinding(uint32_t binding) = 0;

		virtual void BindBuffer(uint32_t binding, EBufferTarget target, IBuffer* buffer) = 0;

		virtual void BindImage(uint32_t binding, EImageType type, unsigned int imageId) = 0;

		virtual GLDescriptor& API_GL() { throw std::runtime_error("Bad API Call: object is not a GLDescriptor"); }

	protected:
		std::vector<Binding> bindings;
	};
}

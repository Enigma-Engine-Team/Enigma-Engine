#pragma once

#include <iostream>
#include "IDevice.h"
#include "ISwapChain.h"


namespace EnigmaRHI
{
	class GLShaderModule;

	class IShaderModule
	{
	public:

		virtual ~IShaderModule() = default;
		virtual void Create(const std::string& filename, EShaderType shaderType) = 0;

		virtual GLShaderModule& API_GL() { throw std::runtime_error("Bad API Call: object is not a GLShaderModule"); }
	};
}

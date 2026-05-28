#pragma once

#include <iostream>
#include "ISwapChain.h"
#include "IDevice.h"
#include "IRenderPass.h"
#include "IDescriptor.h"
#include "IShaderModule.h"
#include "IGraphicsPipeline.h"

namespace EnigmaRHI
{
	class GLPipeline;

	class IPipeline
	{
	public:

		virtual ~IPipeline() = default;
		virtual void Create(const GraphicsPipeline& gp, IShaderModule* vertShader, IShaderModule* fragShader, IShaderModule* geomShader = nullptr) = 0;
		virtual void Destroy() = 0;
		virtual void ApplyGraphicsPipeline(IDevice* device) = 0;
		virtual void SendToGPU(const char* name, int value) const = 0;
		virtual void SendToGPU(const char* name, unsigned int lenght, int* value) const = 0;
		virtual void SendToGPU(const char* name, float value) const = 0;
		virtual void SendToGPU(const char* name, float x, float y) const = 0;
		virtual void SendToGPU(const char* name, float x, float y, float z) const = 0;
		virtual void SendToGPU(const char* name, float x, float y, float z, float w) const = 0;
		virtual void SendToGPU(const char* name, const float matrix[16]) const = 0;

		virtual EnigmaRHI::GLPipeline& API_GL() { throw std::runtime_error("Bad API Call: object is not a GLPipeline"); }
	};
}

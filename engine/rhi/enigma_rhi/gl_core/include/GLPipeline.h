#pragma once
#include "GLShaderModule.h"
#include "GLSwapChain.h"
#include "GLDescriptor.h"
#include "GLRenderPass.h"
#include "IPipeline.h"

namespace EnigmaRHI
{
	class GLPipeline : public IPipeline
	{
	public:

		GLPipeline() = default;

		void Create(const GraphicsPipeline& gp, IShaderModule* vertShader, IShaderModule* fragShader, IShaderModule* geomShader = nullptr) override;
		void Destroy() override;
		void ApplyGraphicsPipeline(IDevice* device) override;
		void SendToGPU(const char* name, int value) const override;
		void SendToGPU(const char* name, unsigned int lenght, int* value) const override;
		void SendToGPU(const char* name, float value) const override;
		void SendToGPU(const char* name, float x, float y) const override;
		void SendToGPU(const char* name, float x, float y, float z) const override;
		void SendToGPU(const char* name, float x, float y, float z, float w) const override;
		void SendToGPU(const char* name, const float matrix[16]) const override;

		GLPipeline& API_GL() override { return (*this); }
		unsigned int programID = 0;

	public:

		GraphicsPipeline GetGraphicsPipeline() const { return graphicsPipeline; };
		unsigned int GetProgram() const { return programID; };
		void ApplyTotalGraphicsPipeline();

	private: 

		GraphicsPipeline graphicsPipeline;
		bool CreateProgram(IShaderModule* vertShader, IShaderModule* fragShader, IShaderModule* geomShader);
		bool IsIDNull() const;
	};
}
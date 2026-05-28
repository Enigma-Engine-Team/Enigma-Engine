#pragma once

#include "IRenderInterface.h"

#include "GLCommandPool.h"
#include "GLPipeline.h"
#include "GLSurface.h"
#include "GLVertexInput.h"
#include "GLFramebuffer.h"

namespace EnigmaRHI
{
	class GLRenderInterface : public IRenderInterface
	{
	public:

		GLRenderInterface() = default;
		~GLRenderInterface() = default;

	private:

		IBuffer* InstantiateBuffer() override { return new GLBuffer(); }
		ICommandBuffer* InstantiateCommandBuffer() override { return new GLCommandBuffer(); }
		ICommandPool* InstantiateCommandPool() override { return new GLCommandPool(); }
		IDescriptor* InstantiateDescriptor() override { return new GLDescriptor(); }
		IFramebuffer* InstantiateFramebuffer() override { return new GLFramebuffer(); }
		IDevice* InstantiateDevice() override { return new GLDevice(); }
		IImage* InstantiateImage() override { return new GLImage(); }
		IInstance* InstantiateInstance() override { return new GLInstance(); }
		IPipeline* InstantiatePipeline() override { return new GLPipeline(); }
		IShaderModule* InstantiateShaderModule() override { return new GLShaderModule(); }
		IRenderPass* InstantiateRenderPass() override { return new GLRenderPass(); }
		ISurface* InstantiateSurface() override { return new GLSurface(); }
		ISwapChain* InstantiateSwapChain() override { return new GLSwapChain(); }
		IVertexInput* InstantiateVertexInput() override { return new GLVertexInput(); }
	};
}
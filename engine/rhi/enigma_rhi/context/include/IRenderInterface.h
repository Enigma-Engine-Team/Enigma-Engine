#pragma once
#include "IBuffer.h"
#include "ICommandBuffer.h"
#include "ICommandPool.h"
#include "IDescriptor.h"
#include "IDevice.h"
#include "IFramebuffer.h"
#include "IImage.h"
#include "IInstance.h"
#include "IPipeline.h"
#include "IRenderPass.h"
#include "IShaderModule.h"
#include "ISurface.h"
#include "ISwapChain.h"
#include "IVertexInput.h"

namespace EnigmaRHI
{
	class GLRenderInterface;

	enum ERenderAPI
	{
		OGL,
		VULKAN,
		DX,
		METAL,
	};

	class IRenderInterface
	{
	public:

		virtual ~IRenderInterface() = default;

		static IRenderInterface* CreateRenderInterface(ERenderAPI api);

		virtual IBuffer* InstantiateBuffer() = 0;
		virtual void DeleteBuffer(IBuffer* buffer) { delete buffer; }

		virtual ICommandBuffer* InstantiateCommandBuffer() = 0;
		virtual void DeleteCommandBuffer(ICommandBuffer* commandBuffer) { delete commandBuffer; }

		virtual ICommandPool* InstantiateCommandPool() = 0;
		virtual void DeleteCommandPool(ICommandPool* commandPool) { delete commandPool; }

		virtual IDescriptor* InstantiateDescriptor() = 0;
		virtual void DeleteDescriptor(IDescriptor* descriptor) { delete descriptor; }

		virtual IDevice* InstantiateDevice() = 0;
		virtual void DeleteDevice(IDevice* device) { delete device; }

		virtual IFramebuffer* InstantiateFramebuffer() = 0;
		virtual void DeleteFramebuffer(IFramebuffer* fb) { delete fb; }

		virtual IImage* InstantiateImage() = 0;
		virtual void DeleteImage(IImage* image) { delete image; }

		virtual IInstance* InstantiateInstance() = 0;
		virtual void DeleteInstance(IInstance* instance) { delete instance; }

		virtual IPipeline* InstantiatePipeline() = 0;
		virtual void DeletePipeline(IPipeline* pipeline) { delete pipeline; }

		virtual IRenderPass* InstantiateRenderPass() = 0;
		virtual void DeleteRenderPass(IRenderPass* renderPass) { delete renderPass; }

		virtual IShaderModule* InstantiateShaderModule() = 0;
		virtual void DeleteShaderModule(IShaderModule* shaderModule) { delete shaderModule; }

		virtual ISurface* InstantiateSurface() = 0;
		virtual void DeleteSurface(ISurface* surface) { delete surface; }

		virtual ISwapChain* InstantiateSwapChain() = 0;
		virtual void DeleteSwapChain(ISwapChain* swapChain) { delete swapChain; }

		virtual IVertexInput* InstantiateVertexInput() = 0;
		virtual void DeleteVertexInput(IVertexInput* vertexInput) { delete vertexInput; }

		static ERenderAPI GetGraphicsAPI() { return currentGraphicsAPI; };

	private:

		static ERenderAPI currentGraphicsAPI;
	};
}


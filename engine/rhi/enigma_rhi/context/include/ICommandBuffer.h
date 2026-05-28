#pragma once

#include "IFormat.h"

namespace EnigmaRHI
{
    class GLCommandBuffer;

    class IDevice;
	class ICommandPool;
	class IPipeline;
    class ISync;
	class IDescriptor;
	class IRenderPass;
	class ISwapChain;
	class IBuffer;
    class IVertexInput;
    class IFramebuffer;

    class ICommandBuffer
    {
    public:
        virtual ~ICommandBuffer() = default;

        virtual void Create() = 0;

        virtual void Begin() = 0;
        virtual void BeginRenderPass(IRenderPass* renderPass, ISwapChain* framebuffer, int width, int height) = 0;
        virtual void BindPipeline(IDevice* device, IPipeline* pipeline) = 0;
        virtual void BindDescriptorSet(IDescriptor* descriptor) = 0;
        virtual void EndRenderPass(ISwapChain* swapChain) = 0;
        virtual void End() = 0;

        virtual void BindVertexBuffer(IBuffer *buffer, uint32_t stride) = 0;
        virtual void BindIndexBuffer(IBuffer* buffer) = 0;

        virtual void BindVertexInput(IVertexInput* vertexInput) = 0;

        virtual void DrawIndexed(EDrawMode drawMode, uint32_t indexCount, uint32_t instanceCount = 1) = 0;
        virtual void Draw(EDrawMode drawMode, uint32_t vertexCount, uint32_t instanceCount = 1) = 0;

		virtual GLCommandBuffer& API_GL() { throw std::runtime_error("Bad API Call: object is not a GLCommandBuffer"); }
    };
}
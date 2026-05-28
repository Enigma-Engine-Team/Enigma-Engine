#pragma once

#include "ICommandBuffer.h"
#include "GLDevice.h"

#include <array>

namespace EnigmaRHI
{
    class GLCommandBuffer : public ICommandBuffer
    {
    public:

        GLCommandBuffer() = default;

        void Create() override;

        void Begin() override;
        void BeginRenderPass(IRenderPass* renderPass, ISwapChain* swapChain, int width, int height) override;
        void BindPipeline(IDevice* device, IPipeline* pipeline) override;
        void BindDescriptorSet(IDescriptor* descriptor) override;
        void EndRenderPass(ISwapChain* swapChain) override;
        void End() override;

        void BindVertexBuffer(IBuffer* buffer, uint32_t stride) override;
        void BindIndexBuffer(IBuffer* buffer) override;

        void BindVertexInput(IVertexInput* vertexInput) override;
        
        void DrawIndexed(EDrawMode drawMode, uint32_t indexCount, uint32_t instanceCount = 1) override;
        void Draw(EDrawMode drawMode, uint32_t vertexCount, uint32_t instanceCount = 1) override;

        GLCommandBuffer& API_GL() override { return *this; };
    };
}
#include "GLCommandBuffer.h"

#include "GLPipeline.h"
#include "GLDescriptor.h"
#include "GLBuffer.h"
#include "GLSwapChain.h"
#include "GLRenderPass.h"
#include "GLVertexInput.h"
#include "IFramebuffer.h"
#include "GLUtilities.h"

void EnigmaRHI::GLCommandBuffer::Create()
{
}

void EnigmaRHI::GLCommandBuffer::Begin()
{
}

void EnigmaRHI::GLCommandBuffer::BeginRenderPass(IRenderPass* renderPass, ISwapChain* swapChain, int width, int height)
{
    swapChain->renderFramebuffer->Bind();
    swapChain->renderFramebuffer->Resize(width, height);
    renderPass->SetViewport(0, 0, width, height);
    renderPass->ClearColor(0.f, 0.f, 0.f, 1.f);
    renderPass->ClearBuffer(EMask::COLOR | EMask::DEPTH);
}

void EnigmaRHI::GLCommandBuffer::BindPipeline(IDevice* device, IPipeline* pipeline)
{
    glUseProgram(pipeline->API_GL().GetProgram());
    pipeline->ApplyGraphicsPipeline(device);
}

void EnigmaRHI::GLCommandBuffer::BindDescriptorSet(IDescriptor* descriptor)
{
}

void EnigmaRHI::GLCommandBuffer::EndRenderPass(ISwapChain* swapChain)
{
    swapChain->renderFramebuffer->Unbind();
}

void EnigmaRHI::GLCommandBuffer::End()
{
}

void EnigmaRHI::GLCommandBuffer::BindVertexBuffer(IBuffer* buffer, uint32_t stride)
{
    glBindVertexBuffer(0, buffer->API_GL().GetBuffer(), 0, stride);
}

void EnigmaRHI::GLCommandBuffer::BindIndexBuffer(IBuffer* buffer)
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer->API_GL().GetBuffer());
}

void EnigmaRHI::GLCommandBuffer::BindVertexInput(IVertexInput* vertexInput)
{
    glBindVertexArray(vertexInput->API_GL().GetVAO());
}

void EnigmaRHI::GLCommandBuffer::DrawIndexed(EDrawMode drawMode, uint32_t indexCount, uint32_t instanceCount)
{
    if (instanceCount > 1)
    {
        glDrawElementsInstanced(
            GLUtilities::DrawModeToGL(drawMode),
            indexCount,
            GL_UNSIGNED_INT,
            0,
            instanceCount
        );
    }
    else
    {
        glDrawElements(
            GLUtilities::DrawModeToGL(drawMode),
            indexCount,
            GL_UNSIGNED_INT,
            0
        );
    }
}

void EnigmaRHI::GLCommandBuffer::Draw(EDrawMode drawMode, uint32_t vertexCount, uint32_t instanceCount)
{
    if (instanceCount > 1)
    {
        glDrawArraysInstanced(GLUtilities::DrawModeToGL(drawMode), 0, vertexCount, instanceCount);
    }
    else
    {
        glDrawArrays(GLUtilities::DrawModeToGL(drawMode), 0, vertexCount);
    }
}

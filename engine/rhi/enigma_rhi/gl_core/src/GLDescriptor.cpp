#include "GLDescriptor.h"
#include "GLUtilities.h"
#include <iostream>
#include <string>

void EnigmaRHI::GLDescriptor::AddImageBinding(uint32_t binding)
{
    Binding b{};
    b.binding = binding;

    bindings.push_back(b);
}

void EnigmaRHI::GLDescriptor::AddBufferBinding(uint32_t binding)
{
    Binding b{};
    b.binding = binding;

    bindings.push_back(b);
}

void EnigmaRHI::GLDescriptor::Create()
{
}

void EnigmaRHI::GLDescriptor::Destroy()
{
    bindings.clear();
}

void EnigmaRHI::GLDescriptor::BindBuffer(uint32_t binding, EBufferTarget target, IBuffer* buffer)
{
    for (auto& b : bindings) 
    {
        if (b.binding == binding) 
        {
            b.buffer = buffer;
            glBindBufferBase(GLUtilities::BufferTargetToGL(target), b.binding, b.buffer->API_GL().GetBuffer());
            return;
        }
    }
}

void EnigmaRHI::GLDescriptor::BindImage(uint32_t binding, EImageType type, unsigned int imageId)
{
    for (auto& b : bindings) 
    {
        if (b.binding == binding) 
        {
            b.imageId = imageId;
            glActiveTexture(GL_TEXTURE0 + b.binding);
            glBindTexture(GLUtilities::ImageTypeToGL(type), b.imageId);
            return;
        }
    }
}
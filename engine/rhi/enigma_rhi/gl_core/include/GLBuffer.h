#pragma once

#include "IBuffer.h"
#include <glad/glad.h>

namespace EnigmaRHI
{

    class GLBuffer : public IBuffer
    {
    public:

        GLBuffer() = default;

        void Create(size_t size, EBufferTarget target, const void* data, EBufferUsage usage) override;
        void Destroy() override;

        void CreateDescriptorBufferInfo() override;
        void CopyBuffer(IBuffer* dstBuffer, size_t size) override;

        void MapMemory(size_t offset, size_t size, uint32_t flags) override;
        void CopyData(const void* src, size_t size, int offset = 0) override;
        void UnMapMemory() override;

        void Bind(EBufferTarget target) override;
        void Unbind(EBufferTarget target) override;

        GLBuffer& API_GL() override { return *this; }

    public:

        unsigned int GetBuffer() const { return buffer; }

    private:

        unsigned int buffer = 0;
        GLenum target = GL_ARRAY_BUFFER;

        void* mappedMemory = nullptr;
    };
}
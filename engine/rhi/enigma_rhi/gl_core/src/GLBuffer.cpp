#include "GLBuffer.h"
#include "GLUtilities.h"
#include <cstring>

void EnigmaRHI::GLBuffer::Create(size_t size, EBufferTarget ETarget, const void* data, EBufferUsage usage)
{
    glGenBuffers(1, &buffer);

    target = GLUtilities::BufferTargetToGL(ETarget);

    glBindBuffer(target, buffer);
    glBufferData(target, size, data, GLUtilities::BufferUsageToGL(usage));
    glBindBuffer(target, 0);
}

void EnigmaRHI::GLBuffer::Destroy()
{
    if (buffer)
    {
        glDeleteBuffers(1, &buffer);
        buffer = 0;
    }
}

void EnigmaRHI::GLBuffer::CreateDescriptorBufferInfo()
{
    bufferInfo.buffer = this;
    bufferInfo.offset = 0;
    bufferInfo.range = 0;
}

void EnigmaRHI::GLBuffer::CopyBuffer(IBuffer* dstBuffer, size_t size)
{
    GLBuffer& dst = dstBuffer->API_GL();

    glBindBuffer(GL_COPY_READ_BUFFER, buffer);
    glBindBuffer(GL_COPY_WRITE_BUFFER, dst.GetBuffer());

    glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, size);

    glBindBuffer(GL_COPY_READ_BUFFER, 0);
    glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
}

void EnigmaRHI::GLBuffer::MapMemory(size_t offset, size_t size, uint32_t flags)
{
    glBindBuffer(target, buffer);
    mappedMemory = glMapBufferRange(
        target, offset, size,
        GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT
    );
}

void EnigmaRHI::GLBuffer::CopyData(const void* src, size_t size, int offset)
{
    glBindBuffer(target, buffer);
    glBufferSubData(target, offset, size, src);
}

void EnigmaRHI::GLBuffer::UnMapMemory()
{
    glBindBuffer(target, buffer);
    glUnmapBuffer(target);
    glBindBuffer(target, 0);
    mappedMemory = nullptr;
}

void EnigmaRHI::GLBuffer::Bind(EBufferTarget target)
{
	glBindBuffer(GLUtilities::BufferTargetToGL(target), buffer);
}

void EnigmaRHI::GLBuffer::Unbind(EBufferTarget target)
{
	glBindBuffer(GLUtilities::BufferTargetToGL(target), 0);
}

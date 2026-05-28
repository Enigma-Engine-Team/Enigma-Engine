#include "../include/GLVertexInput.h"
#include "../include/GLBuffer.h"
#include "GLUtilities.h"

void EnigmaRHI::GLVertexInput::Create()
{
    glGenVertexArrays(1, &vaoID);
}

void EnigmaRHI::GLVertexInput::AddVertexAttribute(unsigned int index, int size, EDataType type, bool normalized, unsigned int offset)
{
    glEnableVertexAttribArray(index);
    glVertexAttribFormat(index, size, GLUtilities::DataTypeToGL(type), normalized, offset);
    glVertexAttribBinding(index, 0);
}

void EnigmaRHI::GLVertexInput::Bind() const
{
    glBindVertexArray(vaoID);
}

void EnigmaRHI::GLVertexInput::Unbind() const
{
    glBindVertexArray(0);
}


void EnigmaRHI::GLVertexInput::Destroy()
{
}

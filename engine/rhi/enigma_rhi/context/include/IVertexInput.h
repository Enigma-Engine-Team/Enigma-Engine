#pragma once

#include "IBuffer.h"

namespace EnigmaRHI 
{
    class GLVertexInput;

    class IVertexInput 
    {
    public:
        virtual ~IVertexInput() = default;
        virtual void Create() = 0;
        virtual void Destroy() = 0;
        virtual void AddVertexAttribute(unsigned int index, int size, EDataType type, bool normalized, unsigned int offset) = 0;
		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

        virtual GLVertexInput& API_GL() { throw std::runtime_error("Bad API Call: object is not a GLVertexInput"); };
    };
}
#pragma once

#include "IDescriptor.h"
#include "GLImage.h"
#include "GLBuffer.h"

#include <vector>

namespace EnigmaRHI
{
    class GLDescriptor : public IDescriptor
    {
    public:

        GLDescriptor() = default;

        void AddImageBinding(uint32_t binding) override;
        void AddBufferBinding(uint32_t binding) override;

        void Create() override;
        void Destroy() override;

        void BindBuffer(uint32_t binding, EBufferTarget target, IBuffer* buffer) override;

        void BindImage(uint32_t binding, EImageType type, unsigned int imageId) override;

        GLDescriptor& API_GL() override { return *this; }
    };
}
#pragma once

#include "IDevice.h"
#include "IImage.h"

namespace EnigmaRHI
{
	class GLFramebuffer;

    struct FramebufferInfo 
    {
        unsigned int width;
        unsigned int height;
        std::vector<IImage*> colorAttachments;
        IImage* depthAttachment = nullptr;
        EAttachment depthAttachmentMode = EAttachment::DEPTH_STENCIL_ATTACHMENT;
    };

	class IFramebuffer
	{
	public:

        virtual ~IFramebuffer() = default;

        virtual void Create(const FramebufferInfo& info) = 0;
        virtual void Destroy() = 0;
        virtual void Resize(uint32_t width, uint32_t height) = 0;

        virtual void Bind(EFramebufferTarget target = EFramebufferTarget::READ_DRAW) = 0;
        virtual void Unbind() = 0;

		virtual void Blit(IFramebuffer* target, uint32_t w, uint32_t h, EMask mask, EFilteringMode filter) = 0;

        virtual void BindSubImage(EImageType imageType, IImage* image, int index, unsigned int mip = 0) = 0;

        virtual uint32_t GetColorAttachmentID(uint32_t index) const = 0;

		virtual GLFramebuffer& API_GL() { throw std::runtime_error("Bad API Call: object is not a GLFramebuffer"); }

    protected:
		FramebufferInfo info;
	};
}
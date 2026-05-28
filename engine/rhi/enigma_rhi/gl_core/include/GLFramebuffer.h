#pragma once

#include "IFramebuffer.h"
#include "GLImage.h"

namespace EnigmaRHI
{
	class GLFramebuffer : public IFramebuffer
	{
	public:
		void Create(const FramebufferInfo& info) override;
		void Destroy() override;
		void Resize(uint32_t width, uint32_t height) override;
		void BindSubImage(EImageType imageType, IImage* image, int index, unsigned int mip = 0) override;

		void Bind(EFramebufferTarget target = EFramebufferTarget::READ_DRAW) override;
		void Unbind() override;

		void Blit(IFramebuffer* target, uint32_t w, uint32_t h, EMask mask, EFilteringMode filter) override;

		uint32_t GetColorAttachmentID(uint32_t index = 0) const override;

		GLFramebuffer& API_GL() { return (*this); }

	private:

		unsigned int fboId = 0;
		std::vector<IImage*> attachments;
	};
}
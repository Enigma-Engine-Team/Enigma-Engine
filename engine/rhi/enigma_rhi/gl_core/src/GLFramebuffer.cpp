#include "GLFramebuffer.h"
#include "IRenderPass.h"
#include "GLUtilities.h"

void EnigmaRHI::GLFramebuffer::Create(const FramebufferInfo& _info)
{
    info = _info;
    attachments = info.colorAttachments;

    glGenFramebuffers(1, &fboId);
    glBindFramebuffer(GL_FRAMEBUFFER, fboId);

    std::vector<GLenum> drawBuffers;
    for (size_t i = 0; i < info.colorAttachments.size(); i++)
    {
        GLuint texID = info.colorAttachments[i]->API_GL().GetID();
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + (GLenum)i,
            GL_TEXTURE_2D, texID, 0);

        drawBuffers.push_back(GL_COLOR_ATTACHMENT0 + (GLenum)i);
    }

    if (!drawBuffers.empty()) {
        glDrawBuffers((GLsizei)drawBuffers.size(), drawBuffers.data());
    }
    else
    {
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
    }

    if (info.depthAttachment)
    {
        glFramebufferTexture(GL_FRAMEBUFFER, GLUtilities::AttachmentToGL(info.depthAttachmentMode),
            info.depthAttachment->API_GL().GetID(), 0);
    }

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE) {
        switch (status) {
        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
            printf("Erreur: Attachement incomplet (vérifie les formats de texture).\n"); break;
        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
            printf("Erreur: Les dimensions des textures ne correspondent pas.\n"); break;
        case GL_FRAMEBUFFER_UNSUPPORTED:
            printf("Erreur: Format non supporté par la carte graphique.\n"); break;
        default:
            printf("Erreur: Code d'erreur inconnu 0x%x\n", status); break;
        }
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void EnigmaRHI::GLFramebuffer::Resize(uint32_t width, uint32_t height)
{
    // 8K resolution max to provide resize error
    uint32_t maxWidth = 7680;  
    uint32_t maxHeight = 4320;

    if (width <= 0 || width > maxWidth || height <= 0 || height > maxHeight)
        return;

    if (info.depthAttachment)
    {
        info.depthAttachment->Resize(width, height);
    }

    for (auto& colorBuffer : attachments)
    {
        colorBuffer->Resize(width, height);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, fboId);

    if (info.depthAttachment) {
        glFramebufferTexture(GL_FRAMEBUFFER, GLUtilities::AttachmentToGL(info.depthAttachmentMode),
            info.depthAttachment->API_GL().GetID(), 0);
    }

    if (attachments.size() > 0)
    {
        for (size_t i = 0; i < attachments.size(); i++) {
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i,
                GL_TEXTURE_2D, attachments[i]->GetID(), 0);
        }
    }
    else
    {
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
    }

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        printf("Erreur: Framebuffer incomplet après Resize!\n");
    }
}

void EnigmaRHI::GLFramebuffer::BindSubImage(EnigmaRHI::EImageType imageType, EnigmaRHI::IImage* image, int index, unsigned int mip)
{
    glBindFramebuffer(GL_FRAMEBUFFER, fboId);

    if(imageType == EImageType::TYPE_CUBE_MAP)
    {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + index, image->GetID(), mip);
	}
    else
    {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, image->GetID(), 0);
	}

    GLenum drawBuf = GL_COLOR_ATTACHMENT0;
    glDrawBuffers(1, &drawBuf);

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE) {
        printf("Erreur: Framebuffer incomplet dans BindSubImage! Status: 0x%x\n", status);
    }
}

void EnigmaRHI::GLFramebuffer::Bind(EFramebufferTarget target)
{
    glBindFramebuffer(GLUtilities::FramebufferTargetToGL(target), fboId);
}

void EnigmaRHI::GLFramebuffer::Destroy()
{
    if (fboId != 0)
    {
        glDeleteFramebuffers(1, &fboId);
        fboId = 0;
    }
}

void EnigmaRHI::GLFramebuffer::Unbind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

uint32_t EnigmaRHI::GLFramebuffer::GetColorAttachmentID(uint32_t index) const
{
    if (index < attachments.size())
    {
        return attachments[index]->API_GL().GetID();
    }
    return 0;
}

void EnigmaRHI::GLFramebuffer::Blit(IFramebuffer* target, uint32_t w, uint32_t h, EMask mask, EFilteringMode filter)
{
    Bind(EFramebufferTarget::READ);
    target->Bind(EFramebufferTarget::DRAW);
    glBlitFramebuffer(
        0, 0, w, h,
        0, 0, w, h,
        GLUtilities::MaskToGL(mask),
        GLUtilities::FilteringModeToGL(filter)
    );
}
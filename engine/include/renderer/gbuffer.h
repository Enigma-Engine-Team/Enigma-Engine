#pragma once
#include "render_context.h"

class GBuffer
{
public:
    GBuffer() = default;

    void Create(RenderContext* context, uint32_t width, uint32_t height);
    void Bind();
    void Unbind();
    void Resize(uint32_t width, uint32_t height);
    void BlitDepthTo(EnigmaRHI::IFramebuffer* target, uint32_t width, uint32_t height);
    void Destroy();

    EnigmaRHI::IImage* GetPosition() const { return gPosition; }
    EnigmaRHI::IImage* GetNormal() const { return gNormal; }
    EnigmaRHI::IImage* GetAlbedoAO() const { return gAlbedoAO; }
    EnigmaRHI::IImage* GetMetalRough() const { return gMetalRough; }
    EnigmaRHI::IImage* GetDepth() const { return gDepth; }
    EnigmaRHI::IImage* GetClearCoatFactors() { return gClearCoatFactors; }
    EnigmaRHI::IImage* GetClearCoatNormal() { return gClearCoatNormal; }
    EnigmaRHI::IImage* GetEmissiveMap() { return gEmissive; };

    EnigmaRHI::IFramebuffer* framebuffer = nullptr;
private:
    void AllocateTextures(uint32_t width, uint32_t height);

    RenderContext* ctx = nullptr;

    EnigmaRHI::IImage* gPosition = nullptr;
    EnigmaRHI::IImage* gNormal = nullptr;
    EnigmaRHI::IImage* gAlbedoAO = nullptr;
    EnigmaRHI::IImage* gMetalRough = nullptr;
    EnigmaRHI::IImage* gClearCoatFactors = nullptr;
    EnigmaRHI::IImage* gClearCoatNormal = nullptr;
    EnigmaRHI::IImage* gEmissive = nullptr;
    EnigmaRHI::IImage* gDepth = nullptr;

    uint32_t width = 0;
    uint32_t height = 0;
};
#include "renderer/gbuffer.h"

void GBuffer::Create(RenderContext* context, uint32_t _width, uint32_t _height)
{
    ctx = context;
    width = _width;
    height = _height;

    AllocateTextures(width, height);

    EnigmaRHI::FramebufferInfo fboInfo
    {
        .width = width,
        .height = height,
        .colorAttachments = { gPosition, gNormal, gAlbedoAO, gMetalRough, gClearCoatFactors, gClearCoatNormal, gEmissive },
        .depthAttachment = gDepth
    };

    framebuffer = ctx->GetRHI()->InstantiateFramebuffer();
    framebuffer->Create(fboInfo);
}

void GBuffer::AllocateTextures(uint32_t w, uint32_t h)
{
    EnigmaRHI::IRenderInterface* rhi = ctx->GetRHI();

    auto makeRT = [&](EnigmaRHI::EImageFormat internalFmt,
        EnigmaRHI::EImageFormat transferFmt,
        EnigmaRHI::EDataType    pixelType) -> EnigmaRHI::IImage*
        {
            EnigmaRHI::ImageSampler sampler
            {
                .imageType = EnigmaRHI::EImageType::TYPE_2D,
                .minFilter = EnigmaRHI::EFilteringMode::NEAREST,
                .magFilter = EnigmaRHI::EFilteringMode::NEAREST,
                .wrapU = EnigmaRHI::EWrappingMode::CLAMP_TO_EDGE,
                .wrapV = EnigmaRHI::EWrappingMode::CLAMP_TO_EDGE,
                .wrapW = EnigmaRHI::EWrappingMode::CLAMP_TO_EDGE,
                .pixelType = pixelType,
                .generateMipMaps = false
            };

            EnigmaRHI::ImageData td
            {
                .width = w,
                .height = h,
                .internalFormat = internalFmt,
                .sampler = sampler,
                .format = transferFmt,
                .data = nullptr
            };

            EnigmaRHI::IImage* img = rhi->InstantiateImage();
            img->Create(td);
            return img;
        };

    // World-space position
    gPosition = makeRT(EnigmaRHI::EImageFormat::RGBA32F,
        EnigmaRHI::EImageFormat::RGB32F,
        EnigmaRHI::EDataType::FLOAT);

    // World-space normal
    gNormal = makeRT(EnigmaRHI::EImageFormat::RGBA32F,
        EnigmaRHI::EImageFormat::RGB32F,
        EnigmaRHI::EDataType::FLOAT);

    // Albedo (RGB) + AO (A)
    gAlbedoAO = makeRT(EnigmaRHI::EImageFormat::RGBA,
        EnigmaRHI::EImageFormat::RGBA8,
        EnigmaRHI::EDataType::UNSIGNED_BYTE);

    // Metallic (R) + Roughness (G)
    gMetalRough = makeRT(EnigmaRHI::EImageFormat::RG8,
        EnigmaRHI::EImageFormat::RG8,
        EnigmaRHI::EDataType::UNSIGNED_BYTE);

    gClearCoatFactors = makeRT(EnigmaRHI::EImageFormat::RG16F,
        EnigmaRHI::EImageFormat::RG16F,
        EnigmaRHI::EDataType::FLOAT);

    gClearCoatNormal = makeRT(EnigmaRHI::EImageFormat::RGB16F,
        EnigmaRHI::EImageFormat::RGB16F,
        EnigmaRHI::EDataType::FLOAT);

    gEmissive = makeRT(EnigmaRHI::EImageFormat::RGB16F,
        EnigmaRHI::EImageFormat::RGB16F,
        EnigmaRHI::EDataType::FLOAT);

    // Depth + Stencil
    EnigmaRHI::ImageSampler depthSampler
    {
        .imageType = EnigmaRHI::EImageType::TYPE_2D,
        .minFilter = EnigmaRHI::EFilteringMode::NEAREST,
        .magFilter = EnigmaRHI::EFilteringMode::NEAREST,
        .wrapU = EnigmaRHI::EWrappingMode::CLAMP_TO_EDGE,
        .wrapV = EnigmaRHI::EWrappingMode::CLAMP_TO_EDGE,
        .wrapW = EnigmaRHI::EWrappingMode::CLAMP_TO_EDGE,
        .pixelType = EnigmaRHI::EDataType::UNSIGNED_INT_24_8,
        .generateMipMaps = false
    };

    EnigmaRHI::ImageData depthTd
    {
        .width = w,
        .height = h,
        .internalFormat = EnigmaRHI::EImageFormat::D32_SFLOAT_S8_UINT,
        .sampler = depthSampler,
        .format = EnigmaRHI::EImageFormat::D32_SFLOAT_S8_UINT,
        .data = nullptr
    };

    gDepth = rhi->InstantiateImage();
    gDepth->Create(depthTd);
}

void GBuffer::Bind()
{
    framebuffer->Bind();
}

void GBuffer::Unbind()
{
    framebuffer->Unbind();
}

void GBuffer::Resize(uint32_t newWidth, uint32_t newHeight)
{
    if (newWidth == width && newHeight == height)
        return;

    width = newWidth;
    height = newHeight;

    framebuffer->Resize(width, height);
}

void GBuffer::BlitDepthTo(EnigmaRHI::IFramebuffer* target, uint32_t w, uint32_t h)
{
    target->Blit(target, w, h, EnigmaRHI::EMask::DEPTH, EnigmaRHI::EFilteringMode::NEAREST);
    framebuffer->Unbind();
}

void GBuffer::Destroy()
{
    if (gPosition) 
    { 
        gPosition->Destroy();   
        delete gPosition;   
    }
    if (gNormal) 
    { 
        gNormal->Destroy();     
        delete gNormal;     
    }
    if (gAlbedoAO) 
    { 
        gAlbedoAO->Destroy();  
        delete gAlbedoAO;
    }
    if (gMetalRough) 
    { 
        gMetalRough->Destroy(); 
        delete gMetalRough; 
    }
    if (gClearCoatFactors) 
    { 
        gClearCoatFactors->Destroy(); 
        delete gClearCoatFactors; 
    }
    if (gClearCoatNormal) 
    { 
        gClearCoatNormal->Destroy();  
        delete gClearCoatNormal; 
    }
    if (gEmissive) 
    { 
        gEmissive->Destroy();
        delete gEmissive;
    }
    if (gDepth) 
    { 
        gDepth->Destroy();      
        delete gDepth;      
    }

    if (framebuffer)
    {
        framebuffer->Destroy();
        delete framebuffer;
    }
}
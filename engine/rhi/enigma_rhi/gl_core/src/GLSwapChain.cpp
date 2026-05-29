#include "../include/GLSwapChain.h"
#include "../include/GLFramebuffer.h"

void EnigmaRHI::GLSwapChain::Create(ISurface* surface)
{
	int height = surface->GetHeight();
	int width = surface->GetWidth();

	sceneColorTarget = new GLImage();
	sceneDepthTarget = new GLImage();
    renderFramebuffer = new GLFramebuffer();

	EnigmaRHI::ImageSampler samplerColor{
		.imageType = EnigmaRHI::EImageType::TYPE_2D,
		.minFilter = EnigmaRHI::EFilteringMode::LINEAR,
		.magFilter = EnigmaRHI::EFilteringMode::LINEAR,
		.wrapU = EnigmaRHI::EWrappingMode::CLAMP_TO_EDGE,
		.wrapV = EnigmaRHI::EWrappingMode::CLAMP_TO_EDGE,
		.wrapW = EnigmaRHI::EWrappingMode::CLAMP_TO_EDGE,
		.pixelType = EnigmaRHI::EDataType::FLOAT,
		.generateMipMaps = false
	};

	EnigmaRHI::ImageData colorImageData
	{
		.width = static_cast<uint32_t>(surface->GetWidth()),
		.height = static_cast<uint32_t>(surface->GetHeight()),
		.internalFormat = EnigmaRHI::EImageFormat::RGB16F,
		.sampler = samplerColor,
		.format = EnigmaRHI::EImageFormat::RGBA8,
		.data = nullptr
	};

	sceneColorTarget->Create(colorImageData);

	EnigmaRHI::ImageSampler samplerDepth{
		.imageType = EnigmaRHI::EImageType::TYPE_2D,
		.minFilter = EnigmaRHI::EFilteringMode::NEAREST,
		.magFilter = EnigmaRHI::EFilteringMode::NEAREST,
		.wrapU = EnigmaRHI::EWrappingMode::CLAMP_TO_EDGE,
		.wrapV = EnigmaRHI::EWrappingMode::CLAMP_TO_EDGE,
		.wrapW = EnigmaRHI::EWrappingMode::CLAMP_TO_EDGE,
		.pixelType = EnigmaRHI::EDataType::UNSIGNED_INT_24_8,
		.generateMipMaps = false
	};

	EnigmaRHI::ImageData depthImageData
	{
		.width = static_cast<uint32_t>(surface->GetWidth()),
		.height = static_cast<uint32_t>(surface->GetHeight()),
		.internalFormat = EnigmaRHI::EImageFormat::D24_UNORM_S8_UINT,
		.sampler = samplerDepth,
		.format = EnigmaRHI::EImageFormat::D24_UNORM_S8_UINT,
		.data = nullptr
	};

	sceneDepthTarget->Create(depthImageData);

	EnigmaRHI::FramebufferInfo fboInfo{
		.width = surface->GetWidth(),
		.height = surface->GetHeight(),
		.colorAttachments = { sceneColorTarget },
		.depthAttachment = sceneDepthTarget
	};

	renderFramebuffer->Create(fboInfo);
}

void EnigmaRHI::GLSwapChain::Destroy()
{
	sceneColorTarget->Destroy();
	sceneDepthTarget->Destroy();
	renderFramebuffer->Destroy();

	delete sceneColorTarget;
	delete sceneDepthTarget;
	delete renderFramebuffer;
}
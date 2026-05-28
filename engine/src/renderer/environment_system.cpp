#include "renderer/environment_system.h"
#include "IFramebuffer.h"
#include "resources/resource_manager.h"
#include "renderer/render_context.h"
#include "resources/texture.h"

EnvironmentSystem::EnvironmentSystem()
{
	envCtx = new EnvironmentContext();
}

void EnvironmentSystem::Create(EnigmaRHI::IRenderInterface* rhi)
{
	CreateCaptureFramebuffer(rhi, 2048);

	CreateEnvironmentMap(rhi, 2048);
	CreateIrradianceMap(rhi, 256);
	CreatePrefilteredMap(rhi, 128);
}

EnigmaRHI::IFramebuffer* EnvironmentSystem::CreateCaptureFramebuffer(EnigmaRHI::IRenderInterface* rhi, unsigned int framebufferTexturesSize)
{
	EnigmaRHI::IFramebuffer* fb = rhi->InstantiateFramebuffer();

	EnigmaRHI::IImage* captureDepthFramebufferImage = rhi->InstantiateImage();

	EnigmaRHI::ImageSampler samplerDepth
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

	captureDepthFramebufferImage->Create({ framebufferTexturesSize, framebufferTexturesSize, 1, EnigmaRHI::EImageFormat::D24_UNORM_S8_UINT, samplerDepth, EnigmaRHI::EImageFormat::D24_UNORM_S8_UINT, nullptr });

	EnigmaRHI::FramebufferInfo captureFBOInfo
	{
		.width = framebufferTexturesSize,
		.height = framebufferTexturesSize,
		.colorAttachments = {},
		.depthAttachment = captureDepthFramebufferImage
	};

	fb->Create(captureFBOInfo);

	return fb;
}

EnigmaRHI::IImage* EnvironmentSystem::CreateEnvironmentMap(EnigmaRHI::IRenderInterface* rhi, unsigned int textureSize)
{
	EnigmaRHI::IImage* environment = rhi->InstantiateImage();

	EnigmaRHI::ImageSampler sampler
	{
		.imageType = EnigmaRHI::EImageType::TYPE_CUBE_MAP,
		.minFilter = EnigmaRHI::EFilteringMode::LINEAR_MIPMAP_LINEAR,
		.magFilter = EnigmaRHI::EFilteringMode::LINEAR,
		.wrapU = EnigmaRHI::EWrappingMode::CLAMP_TO_EDGE,
		.wrapV = EnigmaRHI::EWrappingMode::CLAMP_TO_EDGE,
		.wrapW = EnigmaRHI::EWrappingMode::CLAMP_TO_EDGE,
		.pixelType = EnigmaRHI::EDataType::FLOAT,
		.generateMipMaps = false
	};

	environment->Create({ textureSize, textureSize, 1, EnigmaRHI::EImageFormat::RGB16F, sampler, EnigmaRHI::EImageFormat::RGB8, nullptr });

	return environment;
}

Enviro* EnvironmentSystem::FindEnvironment(std::string path)
{
	auto it = loadedEnvironment.find(path);
	if (it != loadedEnvironment.end())
	{
		return it->second;
	}
	return nullptr;
}

EnigmaRHI::IImage* EnvironmentSystem::CreateIrradianceMap(EnigmaRHI::IRenderInterface* rhi, unsigned int textureSize)
{
	EnigmaRHI::IImage* irradiance = rhi->InstantiateImage();
	EnigmaRHI::ImageSampler sampler
	{
		.imageType = EnigmaRHI::EImageType::TYPE_CUBE_MAP,
		.minFilter = EnigmaRHI::EFilteringMode::LINEAR,
		.magFilter = EnigmaRHI::EFilteringMode::LINEAR,
		.wrapU = EnigmaRHI::EWrappingMode::CLAMP_TO_EDGE,
		.wrapV = EnigmaRHI::EWrappingMode::CLAMP_TO_EDGE,
		.wrapW = EnigmaRHI::EWrappingMode::CLAMP_TO_EDGE,
		.pixelType = EnigmaRHI::EDataType::FLOAT,
		.generateMipMaps = false
	};
	irradiance->Create({ textureSize, textureSize, 1, EnigmaRHI::EImageFormat::RGB16F, sampler, EnigmaRHI::EImageFormat::RGB8, nullptr });

	return irradiance;
}


EnigmaRHI::IImage* EnvironmentSystem::CreatePrefilteredMap(EnigmaRHI::IRenderInterface* rhi, unsigned int textureSize)
{
	EnigmaRHI::IImage* prefiltered = rhi->InstantiateImage();

	EnigmaRHI::ImageSampler sampler
	{
		.imageType = EnigmaRHI::EImageType::TYPE_CUBE_MAP,
		.minFilter = EnigmaRHI::EFilteringMode::LINEAR_MIPMAP_LINEAR,
		.magFilter = EnigmaRHI::EFilteringMode::LINEAR,
		.wrapU = EnigmaRHI::EWrappingMode::CLAMP_TO_EDGE,
		.wrapV = EnigmaRHI::EWrappingMode::CLAMP_TO_EDGE,
		.wrapW = EnigmaRHI::EWrappingMode::CLAMP_TO_EDGE,
		.pixelType = EnigmaRHI::EDataType::FLOAT,
		.generateMipMaps = true
	};

	prefiltered->Create({ textureSize, textureSize, 1, EnigmaRHI::EImageFormat::RGB16F, sampler, EnigmaRHI::EImageFormat::RGB8, nullptr });

	return prefiltered;
}

void EnvironmentSystem::CaptureSkyboxFaces(EnigmaRHI::IPipeline* pipeline, EnigmaRHI::IImage* imageToFill, EnigmaRHI::IRenderPass* rp, EnigmaRHI::ICommandBuffer& cmd, unsigned int mip)
{
	for (int i = 0; i < 6; i++)
	{
		pipeline->SendToGPU("view", captureViews[i].m);
		currentEnvironment->captureFramebuffer->BindSubImage(EnigmaRHI::EImageType::TYPE_CUBE_MAP, imageToFill, i, mip);
		rp->ClearBuffer(EnigmaRHI::EMask::COLOR | EnigmaRHI::EMask::DEPTH);

		envCtx->currentSkybox->Render(cmd);
	}
}

void EnvironmentSystem::ComputeEnvironment(RenderContext* ctx, EnigmaRHI::ICommandBuffer& cmd, EnigmaRHI::IDescriptor* gemoetryDescriptor, EnigmaRHI::IDescriptor* lightningDescriptor, EnigmaRHI::IRenderPass* renderPass)
{
	cmd.BindVertexInput(envCtx->skyboxVertexMode);

	// Environment cubemap
	cmd.BindPipeline(ctx->GetDevice(), envCtx->equirectangleToCubemapPipeline);
	gemoetryDescriptor->BindImage(3, EnigmaRHI::EImageType::TYPE_2D, envCtx->currentSkybox->GetTexture()->GetID());
	envCtx->equirectangleToCubemapPipeline->SendToGPU("projection", captureProjection.m);
	renderPass->SetViewport(0, 0, 2048, 2048);
	currentEnvironment->captureFramebuffer->Bind();
	CaptureSkyboxFaces(envCtx->equirectangleToCubemapPipeline, currentEnvironment->environmentCubemap, renderPass, cmd);
	currentEnvironment->captureFramebuffer->Unbind();
	currentEnvironment->environmentCubemap->GenerateMips(EnigmaRHI::EImageType::TYPE_CUBE_MAP);

	// Irradiance map
	currentEnvironment->captureFramebuffer->Resize(256, 256);
	cmd.BindPipeline(ctx->GetDevice(), envCtx->irradiancePipeline);
	envCtx->irradiancePipeline->SendToGPU("projection", captureProjection.m);
	lightningDescriptor->BindImage(9, EnigmaRHI::EImageType::TYPE_CUBE_MAP, currentEnvironment->environmentCubemap->GetID());
	renderPass->SetViewport(0, 0, 256, 256);
	currentEnvironment->captureFramebuffer->Bind();
	CaptureSkyboxFaces(envCtx->irradiancePipeline, currentEnvironment->irradianceMap, renderPass, cmd);
	currentEnvironment->captureFramebuffer->Unbind();


	// Prefiltered map
	cmd.BindPipeline(ctx->GetDevice(), envCtx->prefilteringPipeline);
	envCtx->prefilteringPipeline->SendToGPU("projection", captureProjection.m);
	lightningDescriptor->BindImage(9, EnigmaRHI::EImageType::TYPE_CUBE_MAP, currentEnvironment->environmentCubemap->GetID());
	currentEnvironment->captureFramebuffer->Bind();

	unsigned int maxMipLevels = 5;
	for (unsigned int mip = 0; mip < maxMipLevels; ++mip)
	{
		unsigned int mipWidth = static_cast<unsigned int>(128 * std::pow(0.5, mip));
		unsigned int mipHeight = static_cast<unsigned int>(128 * std::pow(0.5, mip));
		currentEnvironment->captureFramebuffer->Resize(mipWidth, mipHeight);
		currentEnvironment->captureFramebuffer->Bind();
		renderPass->SetViewport(0, 0, mipWidth, mipHeight);

		float roughnessF = (float)mip / (float)(maxMipLevels - 1);
		envCtx->prefilteringPipeline->SendToGPU("roughness", roughnessF);

		CaptureSkyboxFaces(envCtx->prefilteringPipeline, currentEnvironment->prefilteredMap, renderPass, cmd, mip);
	}
	currentEnvironment->captureFramebuffer->Unbind();

	isEnvironmentComputed = true;
}

void EnvironmentSystem::SetCurrentSkybox(std::string skyboxPath, EnigmaRHI::IRenderInterface* rhi)
{
	if (skyboxPath == cacheSkyboxPath)
		return;

	cacheSkyboxPath = skyboxPath;
	currentEnvironment = FindEnvironment(skyboxPath);

	if (currentEnvironment)
	{
		envCtx->SetCurrentSkybox(currentEnvironment->sky);
		return;
	}

	Enviro* newEnviro = new Enviro();
	Skybox* sky = new Skybox();
	sky->Create(rhi, skyboxPath);

	envCtx->SetCurrentSkybox(sky);

	newEnviro->captureFramebuffer = CreateCaptureFramebuffer(rhi, 2048);
	newEnviro->environmentCubemap = CreateEnvironmentMap(rhi, 2048);
	newEnviro->irradianceMap = CreateIrradianceMap(rhi, 256);
	newEnviro->prefilteredMap = CreatePrefilteredMap(rhi, 128);
	newEnviro->sky = sky;

	loadedEnvironment.insert({ skyboxPath, newEnviro });

	currentEnvironment = newEnviro;

	isEnvironmentComputed = false;
}

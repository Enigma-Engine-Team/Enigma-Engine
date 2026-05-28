#include "renderer/environment_context.h"

void EnvironmentContext::CreateContext(EnigmaRHI::IRenderInterface* rhi)
{
	LoadSkyboxMesh(rhi);

	skyboxVertexMode = rhi->InstantiateVertexInput();
	skyboxVertexMode->Create();

	skyboxVertexMode->Bind();
	skyboxVertexMode->AddVertexAttribute(0, 3, EnigmaRHI::EDataType::FLOAT, false, 0);

	CreateEnvironmentPipelines(rhi);

	CreateLutMap(rhi, 512);
}

void EnvironmentContext::CreateEnvironmentShaders(EnigmaRHI::IRenderInterface* rhi)
{
	skyboxVertShader = rhi->InstantiateShaderModule();
	skyboxFragShader = rhi->InstantiateShaderModule();

	cubemapVertShader = rhi->InstantiateShaderModule();
	equirectangleToCubemapFragShader = rhi->InstantiateShaderModule();
	irradianceFragShader = rhi->InstantiateShaderModule();
	prefilteringFragShader = rhi->InstantiateShaderModule();

	skyboxVertShader->Create("shaders/background.vert", EnigmaRHI::EShaderType::VERTEX);
	skyboxFragShader->Create("shaders/background.frag", EnigmaRHI::EShaderType::FRAGMENT);

	cubemapVertShader->Create("shaders/cubemap.vert", EnigmaRHI::EShaderType::VERTEX);
	equirectangleToCubemapFragShader->Create("shaders/equirectangle_to_cubemap.frag", EnigmaRHI::EShaderType::FRAGMENT);
	irradianceFragShader->Create("shaders/irradiance_convulation.frag", EnigmaRHI::EShaderType::FRAGMENT);
	prefilteringFragShader->Create("shaders/prefilter.frag", EnigmaRHI::EShaderType::FRAGMENT);
}

void EnvironmentContext::LoadSkyboxMesh(EnigmaRHI::IRenderInterface* rhi)
{
	skyboxMesh = ResourceManager::GetInstance().Get<Model>("cube.gltf");
	if (skyboxMesh == nullptr)
	{
		skyboxMesh = ResourceManager::GetInstance().Create<Model>("cube.gltf", rhi);
	}
	skyboxMesh->CreateGPUResources(rhi);
}

void EnvironmentContext::CreateEnvironmentPipelines(EnigmaRHI::IRenderInterface* rhi)
{
	equirectangleToCubemapPipeline = rhi->InstantiatePipeline();
	irradiancePipeline = rhi->InstantiatePipeline();
	prefilteringPipeline = rhi->InstantiatePipeline();
	skyboxPipeline = rhi->InstantiatePipeline();

	EnigmaRHI::GraphicsPipeline sharedGP
	{
		.cullMode = EnigmaRHI::ECullMode::FRONT,
		.frontFaceMode = EnigmaRHI::EFrontFaceMode::COUNTER_CLOCK_WISE,
		.depthTestEnable = true,
		.compareMode = EnigmaRHI::EDepthCompareOp::LEQUAL,
		.polygonMode = EnigmaRHI::EPolygonMode::FILL,
		.depthWriteEnable = true,
	};

	CreateEnvironmentShaders(rhi);

	equirectangleToCubemapPipeline->Create(sharedGP, cubemapVertShader, equirectangleToCubemapFragShader);
	irradiancePipeline->Create(sharedGP, cubemapVertShader, irradianceFragShader);
	prefilteringPipeline->Create(sharedGP, cubemapVertShader, prefilteringFragShader);
	skyboxPipeline->Create(sharedGP, skyboxVertShader, skyboxFragShader);
}


void EnvironmentContext::CreateLutMap(EnigmaRHI::IRenderInterface* rhi, unsigned int textureSize)
{
	brdfLutMap = rhi->InstantiateImage();

	Texture texture;

	texture.Load("shaders/brdf.hdr", rhi);

	EnigmaRHI::ImageSampler sampler
	{
		.imageType = EnigmaRHI::EImageType::TYPE_2D,
		.minFilter = EnigmaRHI::EFilteringMode::LINEAR,
		.magFilter = EnigmaRHI::EFilteringMode::LINEAR,
		.wrapU = EnigmaRHI::EWrappingMode::CLAMP_TO_EDGE,
		.wrapV = EnigmaRHI::EWrappingMode::CLAMP_TO_EDGE,
		.wrapW = EnigmaRHI::EWrappingMode::CLAMP_TO_EDGE,
		.pixelType = EnigmaRHI::EDataType::FLOAT,
		.generateMipMaps = false
	};

	brdfLutMap->Create({ textureSize, textureSize, 1, EnigmaRHI::EImageFormat::RGB16F, sampler, EnigmaRHI::EImageFormat::RGB16F, texture.GetData() });

	texture.FreeData();
}

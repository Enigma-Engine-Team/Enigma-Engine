#include "renderer/ssao_pass.h"
#include <iostream>
#include <random>
#include "renderer/gbuffer.h"
#include "window/input_manager.h"

void SSAORenderer::Create(float width, float height, EnigmaRHI::IRenderInterface* rhi)
{
	CreateDescriptor(rhi);
	CreateHeaderBuffer(rhi);
	CreatePipelines(rhi);
	CreateFBOs(width, height, rhi);
	CreateNoiseTexture(ComputeNoise(rhi), rhi);
}

void SSAORenderer::CreateDescriptor(EnigmaRHI::IRenderInterface* rhi)
{
	ssaoDescriptor = rhi->InstantiateDescriptor();

	ssaoDescriptor->AddBufferBinding(0); // SSAOHeader
	ssaoDescriptor->AddImageBinding(1); // color
	ssaoDescriptor->AddImageBinding(2); // gNormal
	ssaoDescriptor->AddImageBinding(3); // noiseTexture

	ssaoDescriptor->Create();
}

void SSAORenderer::CreateHeaderBuffer(EnigmaRHI::IRenderInterface* rhi)
{
	ssaoHeaderBuffer = rhi->InstantiateBuffer();
	ssaoHeaderBuffer->Create(sizeof(SSAOHeader), EnigmaRHI::EBufferTarget::UNIFORM_BUFFER, nullptr, EnigmaRHI::EBufferUsage::STATIC_DRAW);
	ssaoHeaderBuffer->CreateDescriptorBufferInfo();
	ssaoHeaderBuffer->bufferInfo.range = sizeof(SSAOHeader);
}

void SSAORenderer::CreatePipelines(EnigmaRHI::IRenderInterface* rhi)
{
	ssaoPipeline = rhi->InstantiatePipeline();
	ssaoFrag = rhi->InstantiateShaderModule();
	ssaoVert = rhi->InstantiateShaderModule();

	ssaoBlurPipeline = rhi->InstantiatePipeline();
	ssaoBlurFrag = rhi->InstantiateShaderModule();
	ssaoBlurVert = rhi->InstantiateShaderModule();

	ssaoVert->Create("shaders/full_screen_quad.vert", EnigmaRHI::EShaderType::VERTEX);
	ssaoFrag->Create("shaders/ssao.frag", EnigmaRHI::EShaderType::FRAGMENT);

	ssaoBlurVert->Create("shaders/full_screen_quad.vert", EnigmaRHI::EShaderType::VERTEX);
	ssaoBlurFrag->Create("shaders/ssao_blur.frag", EnigmaRHI::EShaderType::FRAGMENT);
	
	EnigmaRHI::GraphicsPipeline ssaoGP
	{
		.cullMode = EnigmaRHI::ECullMode::BACK,
		.frontFaceMode = EnigmaRHI::EFrontFaceMode::COUNTER_CLOCK_WISE,
		.depthTestEnable = false,
	};

	ssaoPipeline->Create(ssaoGP, ssaoVert, ssaoFrag);
	ssaoBlurPipeline->Create(ssaoGP, ssaoBlurVert, ssaoBlurFrag);
}

void SSAORenderer::CreateFBOs(float FboWidth, float FboHeight, EnigmaRHI::IRenderInterface* rhi)
{
	ssaoFBO = rhi->InstantiateFramebuffer();
	ssaoColorBuffer = rhi->InstantiateImage();

	ssaoBlurFBO = rhi->InstantiateFramebuffer();
	ssaoBlurColorBuffer = rhi->InstantiateImage();

	unsigned int finalWidth = static_cast<unsigned int>(FboWidth);
	unsigned int finalHeight = static_cast<unsigned int>(FboHeight);

	EnigmaRHI::ImageSampler ssaoSampler
	{
		.imageType = EnigmaRHI::EImageType::TYPE_2D,
		.minFilter = EnigmaRHI::EFilteringMode::NEAREST,
		.magFilter = EnigmaRHI::EFilteringMode::NEAREST,
		.pixelType = EnigmaRHI::EDataType::FLOAT,
		.generateMipMaps = false,
	};

	EnigmaRHI::ImageData ssaoData
	{
		.width = finalWidth,
		.height = finalHeight,
		.internalFormat = EnigmaRHI::EImageFormat::R8,
		.sampler = ssaoSampler,
		.format = EnigmaRHI::EImageFormat::R8,
	};

	ssaoColorBuffer->Create(ssaoData);
	ssaoBlurColorBuffer->Create(ssaoData);

	ssaoFBO->Create({ finalWidth, finalHeight, {ssaoColorBuffer} });
	ssaoBlurFBO->Create({ finalWidth, finalHeight, {ssaoBlurColorBuffer} });
}

std::array<Math::Vector3D, 16> SSAORenderer::ComputeNoise(EnigmaRHI::IRenderInterface* rhi)
{
	std::default_random_engine generator;
	std::uniform_real_distribution<float> randomFloats(0.0f, 1.f);

	for (unsigned int i = 0; i < kernelSize; ++i)
	{
		Math::Vector3D sample
		{
			randomFloats(generator) * 2.0f - 1.0f,
			randomFloats(generator) * 2.0f - 1.0f,
			randomFloats(generator)
		};

		sample.Normalize();
		sample *= randomFloats(generator);

		float scale = static_cast<float>(i) / static_cast<float>(kernelSize);
		scale = Math::Lerp(0.1f, 1.0f, scale * scale);
		sample *= scale;
		ssaoKernel.push_back(Math::Vector4D(sample, 0.0f));
	}

	std::array<Math::Vector3D, 16> globalNoise;

	for (int i = 0; i < 16; i++)
	{
		Math::Vector3D noise = Math::Vector3D(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, 0.0f);
		globalNoise[i] = noise;
	}

	return globalNoise;
}

void SSAORenderer::CreateNoiseTexture(std::array<Math::Vector3D, 16> noiseTextureData, EnigmaRHI::IRenderInterface* rhi)
{
	noiseTexture = rhi->InstantiateImage();

	EnigmaRHI::ImageSampler noiseSampler
	{
		.imageType = EnigmaRHI::EImageType::TYPE_2D,
		.minFilter = EnigmaRHI::EFilteringMode::NEAREST,
		.magFilter = EnigmaRHI::EFilteringMode::NEAREST,
		.wrapU = EnigmaRHI::EWrappingMode::REPEAT,
		.wrapV = EnigmaRHI::EWrappingMode::REPEAT,
		.pixelType = EnigmaRHI::EDataType::FLOAT,
		.generateMipMaps = false,
	};

	EnigmaRHI::ImageData noiseData
	{
		.width = 4u,
		.height = 4u,
		.internalFormat = EnigmaRHI::EImageFormat::RGB32F,
		.sampler = noiseSampler,
		.format = EnigmaRHI::EImageFormat::RGB32F,
		.data = &noiseTextureData[0],
	};
	
	noiseTexture->Create(noiseData);
}

void SSAORenderer::Destroy(EnigmaRHI::IRenderInterface* rhi)
{
	ssaoFBO->Destroy();
	ssaoBlurFBO->Destroy();
	ssaoDescriptor->Destroy();
	ssaoColorBuffer->Destroy();
	ssaoBlurColorBuffer->Destroy();

	rhi->DeleteFramebuffer(ssaoFBO);
	rhi->DeleteFramebuffer(ssaoBlurFBO);
	rhi->DeleteImage(ssaoColorBuffer);
	rhi->DeleteImage(ssaoBlurColorBuffer);
	rhi->DeleteDescriptor(ssaoDescriptor);
}

void SSAORenderer::BindForLighning(GBuffer* currentGBuffer, const Math::Vector2D& resolution, const Math::Matrix4x4& projectionMatrix, const Math::Matrix4x4& viewMatrix)
{
	if (InputManager::GetKeyDown(KEY_M))
		radius = 0.f;
	if (InputManager::GetKeyDown(KEY_N))
		radius = 0.5f;

	SSAOHeader header
	{
		.projection = projectionMatrix,
		.view = viewMatrix,
		.radius = radius,
		.bias = bias,
		.kernelSize = kernelSize,
		.width = resolution.x,
		.height = resolution.y,
	};

	for (int i = 0; i < kernelSize; i++)
		header.samples[i] = ssaoKernel[i];

	ssaoHeaderBuffer->CopyData(&header, sizeof(SSAOHeader));

	ssaoDescriptor->BindBuffer(0, EnigmaRHI::EBufferTarget::UNIFORM_BUFFER, ssaoHeaderBuffer);
	ssaoDescriptor->BindImage(1, EnigmaRHI::EImageType::TYPE_2D, currentGBuffer->GetPosition()->GetID());
	ssaoDescriptor->BindImage(2, EnigmaRHI::EImageType::TYPE_2D, currentGBuffer->GetNormal()->GetID());
	ssaoDescriptor->BindImage(3, EnigmaRHI::EImageType::TYPE_2D, noiseTexture->GetID());
}

void SSAORenderer::Render(EnigmaRHI::IDevice* device, EnigmaRHI::IRenderPass* renderPass, EnigmaRHI::ICommandBuffer& cmd)
{
	ssaoFBO->Bind();
	renderPass->ClearBuffer(EnigmaRHI::EMask::COLOR);
	cmd.BindPipeline(device, ssaoPipeline);
}
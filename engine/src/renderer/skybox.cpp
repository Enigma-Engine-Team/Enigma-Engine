#include "renderer/skybox.h"
#include "resources/texture.h"

void Skybox::Create(EnigmaRHI::IRenderInterface* rhi, std::string path)
{
	HDRTexture = rhi->InstantiateImage();
	Texture texture;
	texture.Load(path.c_str(), rhi);

	EnigmaRHI::ImageSampler HDRSampler
	{
		.imageType = EnigmaRHI::EImageType::TYPE_2D,
		.minFilter = EnigmaRHI::EFilteringMode::LINEAR,
		.magFilter = EnigmaRHI::EFilteringMode::LINEAR,
		.wrapU = EnigmaRHI::EWrappingMode::CLAMP_TO_EDGE,
		.wrapV = EnigmaRHI::EWrappingMode::CLAMP_TO_EDGE,
		.pixelType = EnigmaRHI::EDataType::FLOAT,
		.generateMipMaps = false,
	};

	EnigmaRHI::ImageData texData
	{
		.width = texture.GetWidth(),
		.height = texture.GetHeight(),
		.internationalFormat = EnigmaRHI::EImageFormat::RGB16F,
		.sampler = HDRSampler,
		.format = EnigmaRHI::EImageFormat::RGB8,
		.data = texture.GetData()
	};

	HDRTexture->Create(texData);

	texture.FreeData();
}

void Skybox::Render(EnigmaRHI::ICommandBuffer& cmd)
{
	if (!skyboxMesh)
		return;

	for(Mesh* m : skyboxMesh->GetSubMeshes())
		m->Draw(cmd);
}
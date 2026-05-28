#include "renderer/stb_image.h"

#include "resources/texture.h"
#include "debug/log.h"

unsigned int Texture::GetID()
{
	if (!isGen)
	{
		GenImage();
		isGen = true;
	}

	if(image != nullptr)
		return image->GetID();
	else
		return 0;
}

void Texture::LoadTexture(const char* filePath)
{
	stbi_set_flip_vertically_on_load(false);

	data = stbi_load(filePath, &width, &height, &channels, 4);

	channels = 4;
	imageFormat = EnigmaRHI::EImageFormat::RGBA8;

	if (!data)
		Debug::LogError("Failed to load texture image !");
}

void Texture::LoadHDR(const char* path)
{
    stbi_set_flip_vertically_on_load(true);

    data = reinterpret_cast<unsigned char*>(stbi_loadf(path, &width, &height, &channels, 0));

	if(channels == 3)
		imageFormat = EnigmaRHI::EImageFormat::RGB16F;

	if (!data)
		Debug::LogError("Failed to load texture image !");
}

void Texture::GenImage()
{
	if (image == nullptr)
		return;

	EnigmaRHI::EDataType pType = EnigmaRHI::EDataType::UNSIGNED_BYTE;
	if (imageFormat == EnigmaRHI::EImageFormat::RGB16F || imageFormat == EnigmaRHI::EImageFormat::RGB32F)
	{
		pType = EnigmaRHI::EDataType::FLOAT;
	}

	EnigmaRHI::ImageSampler s
	{
		.imageType = EnigmaRHI::EImageType::TYPE_2D,
		.minFilter = EnigmaRHI::EFilteringMode::LINEAR_MIPMAP_LINEAR,
		.magFilter = EnigmaRHI::EFilteringMode::LINEAR,
		.wrapU = EnigmaRHI::EWrappingMode::REPEAT,
		.wrapV = EnigmaRHI::EWrappingMode::REPEAT,
		.pixelType = pType,
		.generateMipMaps = true,
	};

	if (imageInternalFormat == EnigmaRHI::EImageFormat::UNDEFINED)
		imageInternalFormat = imageFormat;

	EnigmaRHI::ImageData imageData
	{
		.width = static_cast<uint32_t>(width),
		.height = static_cast<uint32_t>(height),
		.internationalFormat = imageInternalFormat,
		.sampler = s,
		.format = imageFormat,
		.data = data,
	};

	image->Create(imageData);
}

void Texture::Load(const std::filesystem::path& filepath, EnigmaRHI::IRenderInterface* rhi)
{
	filename = filepath.filename().string();
	std::string ext = filepath.extension().string();
	image = rhi->InstantiateImage();

	if (ext == ".hdr")
		LoadHDR(filepath.string().c_str());
	else
		LoadTexture(filepath.string().c_str());
}

void Texture::FreeData()
{
	stbi_image_free(data);
}
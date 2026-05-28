#pragma once

#include "IFormat.h"
#include <iostream>
#include "iresource.h"
#include "utilities/macro.h"

class ENIGMA_API Texture : public IResource
{
public:

	Texture() = default;
	virtual ~Texture() override {};

	void Load(const std::filesystem::path& filepath, EnigmaRHI::IRenderInterface* rhi) override;

	void FreeData();

	void OverrideInternalFormat(EnigmaRHI::EImageFormat internalFormat) { imageInternalFormat = internalFormat; };

	uint32_t GetWidth() const { return static_cast<uint32_t>(width); }
	uint32_t GetHeight() const { return static_cast<uint32_t>(height); }
	uint32_t GetChannels() const { return static_cast<uint32_t>(channels); }
	EnigmaRHI::EImageFormat GetImageFormat() const { return imageFormat; }
	EnigmaRHI::EImageFormat GetInternalImageFormat() const { return imageInternalFormat; };
	unsigned char* GetData() const { return data; }
	unsigned int GetID();

private:

	void LoadTexture(const char* filePath);
	void LoadHDR(const char* path);
	void GenImage();

	EnigmaRHI::IImage* image = nullptr;

	int width = 0;
	int height = 0;
	int channels = 0;
	EnigmaRHI::EImageFormat imageFormat = EnigmaRHI::EImageFormat::UNDEFINED;
	EnigmaRHI::EImageFormat imageInternalFormat = EnigmaRHI::EImageFormat::UNDEFINED;
	unsigned char* data = nullptr;

	bool isGen = false;
};
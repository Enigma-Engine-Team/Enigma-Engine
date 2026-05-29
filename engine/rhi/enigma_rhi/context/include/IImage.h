#pragma once

#include <iostream>

#include "IDevice.h"
#include "ICommandPool.h"
#include "ImageSampler.h"

namespace EnigmaRHI
{
	class GLImage;

	struct ImageData
	{
		uint32_t width;
		uint32_t height;
		uint32_t depth = 1;
		EImageFormat internalFormat;
		ImageSampler sampler;
		EImageFormat format;
		void* data;
	};

	class IImage
	{
	public:

		virtual ~IImage() = default;

		virtual void Create(ImageData imageData, bool createAsAHandle = false) = 0;
		virtual void Destroy() = 0;

		virtual void Bind() = 0;
		virtual void Unbind() = 0;
		virtual void Resize(uint32_t newWidth, uint32_t newHeight) = 0;
		virtual void GenerateMips(EImageType imageType) = 0;
		virtual void SetPixelStorageMode(EPackingMode packingMode, unsigned int alignment) = 0;
		virtual void SetTextureWrapping(EImageType imageType, EWrappingMode wrapS, EWrappingMode wrapT, EWrappingMode wrapR = EWrappingMode::REPEAT) = 0;
		virtual void SetTextureFiltering(EImageType imageType, EFilteringMode minFilter, EFilteringMode magFilter) = 0;
		virtual void TextureSubImage(uint32_t xOffset, uint32_t yOffset, uint32_t zOffset, uint32_t width, uint32_t height, EImageFormat format, EDataType pixelType, void* data, EDimensionMode dimensionMode = EDimensionMode::TWO_DIMENSION) = 0;

		unsigned int GetID() const { return imageId; };
		uint64_t GetHandle() const { return textureHandle; };

		virtual GLImage& API_GL() { throw std::runtime_error("Bad API Call: object is not a GLImage"); }

	protected:

		uint64_t textureHandle = 0;
		unsigned int imageId = 0;
		ImageData imageData;
	};
}
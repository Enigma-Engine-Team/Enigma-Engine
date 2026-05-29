#include "../include/GLImage.h"
#include "../include/GLUtilities.h"

#include <string>

void EnigmaRHI::GLImage::Create(ImageData _imageData, bool createAsAHandle)
{
	imageData = _imageData;
	int GLImageType = GLUtilities::ImageTypeToGL(imageData.sampler.imageType);

	glGenTextures(1, &imageId);
	glBindTexture(GLImageType, imageId);

	switch (imageData.sampler.imageType)
	{
	case EImageType::TYPE_2D:
		GenerateImage(imageData);

		if (imageData.sampler.returnOnCreate)
			return;

		break;

	case EImageType::TYPE_CUBE_MAP:
		GenerateCubemap(imageData);

		if (imageData.sampler.returnOnCreate)
			return;

		break;

	case EImageType::TYPE_2D_ARRAY:
		GenerateArrayImage(imageData);

		if (imageData.sampler.returnOnCreate)
			return;

		break;
	}

	glTexParameteri(GLImageType, GL_TEXTURE_WRAP_S, GLUtilities::WrappingModeToGL(imageData.sampler.wrapU));
	glTexParameteri(GLImageType, GL_TEXTURE_WRAP_T, GLUtilities::WrappingModeToGL(imageData.sampler.wrapV));
	glTexParameteri(GLImageType, GL_TEXTURE_WRAP_R, GLUtilities::WrappingModeToGL(imageData.sampler.wrapW));
	glTexParameteri(GLImageType, GL_TEXTURE_MIN_FILTER, GLUtilities::FilteringModeToGL(imageData.sampler.minFilter));
	glTexParameteri(GLImageType, GL_TEXTURE_MAG_FILTER, GLUtilities::FilteringModeToGL(imageData.sampler.magFilter));

	if (imageData.sampler.borderColor)
		glTexParameterfv(GLImageType, GL_TEXTURE_BORDER_COLOR, imageData.sampler.borderColor);

	if (imageData.sampler.generateMipMaps)
		glGenerateMipmap(GLUtilities::ImageTypeToGL(imageData.sampler.imageType));

	if (createAsAHandle)
	{
		textureHandle = glGetTextureHandleARB(imageId);
		glMakeTextureHandleResidentARB(textureHandle);
	}

	glBindTexture(GLImageType, 0);
}

void EnigmaRHI::GLImage::Bind()
{
	glBindTexture(GLUtilities::ImageTypeToGL(imageData.sampler.imageType), imageId);
}

void EnigmaRHI::GLImage::Unbind()
{
	glBindTexture(GLUtilities::ImageTypeToGL(imageData.sampler.imageType), 0);
}

void EnigmaRHI::GLImage::Resize(uint32_t newWidth, uint32_t newHeight)
{
	if(newWidth == imageData.width && newHeight == imageData.height)
		return;

	if (newWidth == 0 || newHeight == 0)
		return;

	if (imageId != 0) 
		glDeleteTextures(1, &imageId);

	ImageData newImageData = this->imageData;
	newImageData.width = newWidth;
	newImageData.height = newHeight;
	newImageData.data = nullptr;

	Create(newImageData);
}

void EnigmaRHI::GLImage::GenerateMips(EImageType imageType)
{
	glBindTexture(GLUtilities::ImageTypeToGL(imageType), imageId);
	glGenerateMipmap(GLUtilities::ImageTypeToGL(imageType));
}

void EnigmaRHI::GLImage::SetPixelStorageMode(EPackingMode packingMode, unsigned int alignment)
{
	glPixelStorei(GLUtilities::PackingModeToGL(packingMode), alignment);
}

void EnigmaRHI::GLImage::TextureSubImage(uint32_t xOffset, uint32_t yOffset, uint32_t zOffset, uint32_t width, uint32_t height, EImageFormat format, EDataType pixelType, void* data, EDimensionMode dimensionMode)
{
	switch (dimensionMode)
	{
		case EDimensionMode::TWO_DIMENSION:
			glTexSubImage2D(GLUtilities::ImageTypeToGL(imageData.sampler.imageType), 0, xOffset, yOffset, width, height, GLUtilities::FormatToGL(format), GLUtilities::DataTypeToGL(pixelType), data);
			break;
		case EDimensionMode::THREE_DIMENSION:
			glTexSubImage3D(GLUtilities::ImageTypeToGL(imageData.sampler.imageType), 0, xOffset, yOffset, zOffset, width, height, 1, GLUtilities::FormatToGL(format), GLUtilities::DataTypeToGL(pixelType), data);
			break;
		case EDimensionMode::ONE_DIMENSION:
			glTexSubImage1D(GLUtilities::ImageTypeToGL(imageData.sampler.imageType), 0, xOffset, width, GLUtilities::FormatToGL(format), GLUtilities::DataTypeToGL(pixelType), data);
			break;
	}
}

void EnigmaRHI::GLImage::SetTextureWrapping(EImageType imageType, EWrappingMode wrapS, EWrappingMode wrapT, EWrappingMode wrapR)
{
	glTexParameteri(GLUtilities::ImageTypeToGL(imageType), GL_TEXTURE_WRAP_S, GLUtilities::WrappingModeToGL(wrapS));
	glTexParameteri(GLUtilities::ImageTypeToGL(imageType), GL_TEXTURE_WRAP_T, GLUtilities::WrappingModeToGL(wrapT));
	glTexParameteri(GLUtilities::ImageTypeToGL(imageType), GL_TEXTURE_WRAP_R, GLUtilities::WrappingModeToGL(wrapR));
}

void EnigmaRHI::GLImage::SetTextureFiltering(EImageType imageType, EFilteringMode minFilter, EFilteringMode magFilter)
{
	glTexParameteri(GLUtilities::ImageTypeToGL(imageType), GL_TEXTURE_MIN_FILTER, GLUtilities::FilteringModeToGL(minFilter));
	glTexParameteri(GLUtilities::ImageTypeToGL(imageType), GL_TEXTURE_MAG_FILTER, GLUtilities::FilteringModeToGL(magFilter));
}

void EnigmaRHI::GLImage::GenerateCubemap(ImageData cubemapData)
{
	for (unsigned int i = 0; i < 6; ++i)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GLUtilities::FormatToGLInternal(cubemapData.internalFormat), cubemapData.width,
			cubemapData.height, 0,GLUtilities::FormatToGL(cubemapData.format), GLUtilities::DataTypeToGL(cubemapData.sampler.pixelType), cubemapData.data);
	}
}

void EnigmaRHI::GLImage::GenerateImage(ImageData imageData)
{
	switch (imageData.sampler.dimensionMode)
	{
		case EDimensionMode::TWO_DIMENSION:
			glTexImage2D(GLUtilities::ImageTypeToGL(imageData.sampler.imageType), 0, GLUtilities::FormatToGLInternal(imageData.internalFormat), imageData.width, imageData.height, 0, GLUtilities::FormatToGL(imageData.format), GLUtilities::DataTypeToGL(imageData.sampler.pixelType), imageData.data);
			break;
		case EDimensionMode::THREE_DIMENSION:
			glTexImage3D(GLUtilities::ImageTypeToGL(imageData.sampler.imageType), 0, GLUtilities::FormatToGLInternal(imageData.internalFormat), imageData.width, imageData.height, imageData.depth, 0, GLUtilities::FormatToGL(imageData.format), GLUtilities::DataTypeToGL(imageData.sampler.pixelType), imageData.data);
			break;
		case EDimensionMode::ONE_DIMENSION:
			glTexImage1D(GLUtilities::ImageTypeToGL(imageData.sampler.imageType), 0, GLUtilities::FormatToGLInternal(imageData.internalFormat), imageData.width, 0, GLUtilities::FormatToGL(imageData.format), GLUtilities::DataTypeToGL(imageData.sampler.pixelType), imageData.data);
			break;
	}
}

void EnigmaRHI::GLImage::GenerateArrayImage(ImageData imageData)
{
	GenerateImage(imageData);
}
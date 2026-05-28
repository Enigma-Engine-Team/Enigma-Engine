#pragma once

#include "GLBuffer.h"
#include "IImage.h"

namespace EnigmaRHI
{
    class GLImage : public IImage
    {
    public: // Only inherited function

        GLImage() = default;

        void Create(ImageData texData, bool createAsAHandle = false) override;
        void Destroy() override {};

        void Bind() override;
        void Unbind() override;
		void Resize(uint32_t newWidth, uint32_t newHeight) override;
        void GenerateMips(EImageType imageType) override;
		void SetPixelStorageMode(EPackingMode packingMode, unsigned int alignment) override;
		void TextureSubImage(uint32_t xOffset, uint32_t yOffset, uint32_t zOffset, uint32_t width, uint32_t height, EImageFormat format, EDataType pixelType, void* data, EDimensionMode dimensionMode = EDimensionMode::TWO_DIMENSION) override;
        void SetTextureWrapping(EImageType imageType, EWrappingMode wrapS, EWrappingMode wrapT, EWrappingMode wrapR = EWrappingMode::REPEAT) override;
        void SetTextureFiltering(EImageType imageType, EFilteringMode minFilter, EFilteringMode magFilter) override;

        GLImage& API_GL() override { return (*this); }

    private:

		void GenerateCubemap(ImageData cubemapData);
		void GenerateImage(ImageData imageData);
		void GenerateArrayImage(ImageData imageData);
    };
}

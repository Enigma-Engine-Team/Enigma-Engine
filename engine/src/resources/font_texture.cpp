#include "resources/font_texture.h"
#include "renderer/text_character.h"
#include "renderer/text_renderer.h"
#include "debug/log.h"
#include <rttr/registration.h>

RTTR_REGISTRATION
{
	rttr::registration::class_<FontTexture>("FontTexture")
		 .property("filename", &IResource::filename);
}

void FontTexture::Load(const std::filesystem::path& filepath, EnigmaRHI::IRenderInterface* rhi)
{
	FT_Library ft = TextRenderer::GetFontLibrary();
	if(!ft)
	{
		Debug::LogError("FreeType Library not initialized");
		return;
	}

	FT_Face face;
	if (FT_New_Face(ft, filepath.string().c_str(), 0, &face))
	{
		Debug::LogError("Failed to load font");
		return;
	}
	else
	{
		FT_Set_Pixel_Sizes(face, 256, 256);

		fontImage = rhi->InstantiateImage();
		fontImage->SetPixelStorageMode(EnigmaRHI::EPackingMode::UNPACK_ALIGNMENT, 1);

		EnigmaRHI::ImageData imageData
		{
			.width = 256,
			.height = 256,
			.depth = 128,
			.internationalFormat = EnigmaRHI::EImageFormat::R8,
			.sampler =
			{
				.imageType = EnigmaRHI::EImageType::TYPE_2D_ARRAY,
				.dimensionMode = EnigmaRHI::EDimensionMode::THREE_DIMENSION,
				.pixelType = EnigmaRHI::EDataType::UNSIGNED_BYTE,
				.returnOnCreate = true,
			},
			.format = EnigmaRHI::EImageFormat::R8,
			.data = 0,
		};

		fontImage->Create(imageData);

		for (unsigned char c = 0; c < 128; c++)
		{
			if (FT_Load_Char(face, c, FT_LOAD_RENDER))
			{
				Debug::LogError("Failed to load Glyph");
				continue;
			}

			fontImage->TextureSubImage(0, 0, int(c), face->glyph->bitmap.width, face->glyph->bitmap.rows, 
				EnigmaRHI::EImageFormat::R8, EnigmaRHI::EDataType::UNSIGNED_BYTE, face->glyph->bitmap.buffer, 
				EnigmaRHI::EDimensionMode::THREE_DIMENSION);

			fontImage->SetTextureWrapping(EnigmaRHI::EImageType::TYPE_2D_ARRAY, EnigmaRHI::EWrappingMode::CLAMP_TO_EDGE, EnigmaRHI::EWrappingMode::CLAMP_TO_EDGE);
			fontImage->SetTextureFiltering(EnigmaRHI::EImageType::TYPE_2D_ARRAY, EnigmaRHI::EFilteringMode::LINEAR, EnigmaRHI::EFilteringMode::LINEAR);

			TextCharacter character
			{
				.textureID = int(c),
				.size = { static_cast<float>(face->glyph->bitmap.width), static_cast<float>(face->glyph->bitmap.rows) },
				.bearing = { static_cast<float>(face->glyph->bitmap_left), static_cast<float>(face->glyph->bitmap_top) },
				.advance = static_cast<unsigned int>(face->glyph->advance.x)
			};

			characters.insert(std::pair<char, TextCharacter>(c, character));
		}
		fontImage->Unbind();
	}

	filename = filepath.filename().string();

	FT_Done_Face(face);
}

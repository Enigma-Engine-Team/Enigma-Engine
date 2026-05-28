#pragma once

#include "utilities/macro.h"
#include "iresource.h"
#include "IImage.h"
#include "renderer/text_character.h"
#include <map>

class ENIGMA_API FontTexture : public IResource
{
public:
	FontTexture() = default;
	~FontTexture() = default;

	void Load(const std::filesystem::path& filepath, EnigmaRHI::IRenderInterface* rhi) override;
	EnigmaRHI::IImage* GetFontImage() const { return fontImage; }
	std::map<unsigned char, TextCharacter> GetCharacters() const { return characters; }

private: 

	EnigmaRHI::IImage* fontImage = nullptr;
	std::map<unsigned char, TextCharacter> characters;
};
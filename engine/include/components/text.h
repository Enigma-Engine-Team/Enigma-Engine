#pragma once

#include "widget.h"
#include "emath/emath.h"
#include "resources/font_texture.h"


class ENIGMA_API Text : public Widget
{
public:
	Text() = default;
	virtual ~Text() = default;
	void Create() override;
	void Render(Math::Matrix4x4 camVP, Math::Matrix4x4 camViewMatrix, EnigmaRHI::ICommandBuffer& cmd, EnigmaRHI::IDevice* device) override;
	void Destroy() override;

	void SetText(std::string _text);
	
	void SetFontTexture(FontTexture* newFont) { font = newFont; };

private:
	RTTR_REGISTRATION_FRIEND
	std::string text = "TEXT";
	float fontSize = 12.f;
	Math::Vector2D screenPos = {960.f, 540.f};
	Math::Vector4D color = { 1.f, 1.f, 1.f, 1.f };
	FontTexture* font = nullptr;
	RTTR_ENABLE(IComponent)
};
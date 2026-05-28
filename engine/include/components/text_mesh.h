#pragma once

#include "utilities/macro.h"
#include "resources/font_texture.h"
#include "widget.h"

class ENIGMA_API TextMesh : public Widget
{
public:
	TextMesh() = default;
	~TextMesh() = default;

	void Create() override;
	void Render(Math::Matrix4x4 camVP, Math::Matrix4x4 camViewMatrix, EnigmaRHI::ICommandBuffer& cmd, EnigmaRHI::IDevice* device) override;
	void Destroy() override;

	void SetFontTexture(FontTexture* newFont) { font = newFont; };

private:

	RTTR_REGISTRATION_FRIEND;
	Math::Vector3D localPos = Math::Vector3D::Zero;
	std::string text = "Text";
	FontTexture* font = nullptr;
	float fontSize = 1;
	Math::Vector4D color = Math::Vector4D::One;
	bool faceCamera = false;
	RTTR_ENABLE(IComponent)
};
#pragma once

#include "utilities/macro.h"
#include "components/widget.h"
#include "emath/emath.h"
#include "resources/font_texture.h"
#include "IImage.h"
#include <string>
#include <functional>

class ENIGMA_API Button : public Widget 
{
    RTTR_ENABLE(Widget)
public:
    void Create() override;
    void Render(Math::Matrix4x4 camVP, Math::Matrix4x4 camViewMatrix, EnigmaRHI::ICommandBuffer& cmd, EnigmaRHI::IDevice* device) override;
    void Destroy() override;
    void OnUpdate();

    void SetFontTexture(FontTexture* newFont) { fT = newFont; };
    void SetOnClick(std::function<void()> cb) { onClick = cb; }

private:

    RTTR_REGISTRATION_FRIEND
    std::string text = "Button";

    Math::Vector4D baseColor = { 0.2f, 0.2f, 0.2f, 1.0f };
    Math::Vector4D hoverColor = { 0.35f, 0.35f, 0.35f, 1.0f };
    Math::Vector4D pressedColor = { 0.1f, 0.1f, 0.1f, 1.0f };
    Math::Vector4D textColor = { 1.0f, 1.0f, 1.0f, 1.0f };

    float fontSize = 10.f;
    float width = 200.f;
    float height = 70.f;
    float radius = 8.f;

    bool isHovered = false;
    bool isPressed = false;

    FontTexture* fT = nullptr;
    EnigmaRHI::IImage* bgTexture = nullptr;

    Math::Vector2D screenPos = { 960.f, 540.f };

    std::function<void()> onClick;
};
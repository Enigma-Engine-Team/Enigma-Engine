#include "components/button.h"
#include "renderer/widget_renderer.h"
#include "gameobject/gameobject.h"
#include "window/input_manager.h"
#include "engine/engine.h"

RTTR_REGISTRATION
{
    rttr::registration::class_<Button>("Button")
        .constructor<>()(rttr::policy::ctor::as_raw_ptr)
        .property("Text", &Button::text)
        (
            rttr::metadata(UI_DATA_METADATA_KEY, UIData{EUIType::MULTILINE_TEXT, 0.f, 0.f, "%.0f", "General"})
        )
        .property("Width", &Button::width)
        (
            rttr::metadata(UI_DATA_METADATA_KEY, UIData{EUIType::DRAG_FLOAT, 1.f, 2000.f, "%.1f", "Layout"})
        )
        .property("Height", &Button::height)
        (
            rttr::metadata(UI_DATA_METADATA_KEY, UIData{EUIType::DRAG_FLOAT, 1.f, 2000.f, "%.1f", "Layout"})
        )
        .property("Radius", &Button::radius)
        (
            rttr::metadata(UI_DATA_METADATA_KEY, UIData{EUIType::DRAG_FLOAT, 0.f, 100.f, "%.1f", "Style"})
        )
        .property("Base Color", &Button::baseColor)
        (
            rttr::metadata(UI_DATA_METADATA_KEY, UIData{EUIType::COLOR4, 0.f, 0.f, "%.0f", "Style"})
        )
        .property("Hover Color", &Button::hoverColor)
        (
            rttr::metadata(UI_DATA_METADATA_KEY, UIData{EUIType::COLOR4, 0.f, 0.f, "%.0f", "Style"})
        )
        .property("Pressed Color", &Button::pressedColor)
        (
            rttr::metadata(UI_DATA_METADATA_KEY, UIData{EUIType::COLOR4, 0.f, 0.f, "%.0f", "Style"})
        )
        .property("Text Color", &Button::textColor)
        (
            rttr::metadata(UI_DATA_METADATA_KEY, UIData{EUIType::COLOR4, 0.f, 0.f, "%.0f", "Style"})
        )
        .property("Font", &Button::fT)
        (
            rttr::metadata(UI_DATA_METADATA_KEY, UIData{EUIType::FONT_INPUT, 0.f, 0.f, "%.0f", "Style"})
        )
        .property("Font Size", &Button::fontSize)
        (
            rttr::metadata(UI_DATA_METADATA_KEY, UIData{EUIType::DRAG_FLOAT, 1.f, 100.f, "%.2f", "Style"})
        );
}

void Button::Create()
{
    fT = WidgetRenderer::GetInstance().GetDefaultFontTexture();
    WidgetRenderer::GetInstance().RegisterWidget(this);
    gameObject->SetLayer(ELayer::UI);
}

void Button::OnUpdate()
{
    if (Engine::IsInGameBuild())
    {
        Math::Vector2D mousePos2 = InputManager::GetMousePos();
        InputManager::UpdateGameRect({ mousePos2.x, mousePos2.y }, 1920.f, 1080.f);
    }

    Math::Vector3D pos = gameObject->transform.position;
    Math::Vector2D finalPos = Math::Vector2D(pos.x, pos.y) + screenPos;

    Math::Vector2D buttonBorder = { finalPos.x - width * 0.5f, finalPos.y - height * 0.5f };
    buttonBorder = InputManager::ToGamePos(buttonBorder);

    Math::Vector2D mousePos = InputManager::GetInversedGameMousePos();

    Math::Vector2D size = { width, height };
    size = InputManager::ToGamePos(size);
    
    isHovered = mousePos.x >= buttonBorder.x && mousePos.x <= buttonBorder.x + size.x &&
        mousePos.y >= buttonBorder.y && mousePos.y <= buttonBorder.y + size.y;

    if (isHovered && InputManager::GetMouseButtonClicked(MOUSE_BUTTON_LEFT) && !isPressed)
    {
        isPressed = true;
        if (onClick) 
            onClick();
    }
    if (!InputManager::GetMouseButtonDown(MOUSE_BUTTON_LEFT))
        isPressed = false;
}

void Button::Render(Math::Matrix4x4 camVP, Math::Matrix4x4 camViewMatrix, EnigmaRHI::ICommandBuffer& cmd, EnigmaRHI::IDevice* device)
{
    if(WidgetRenderer::GetInstance().IsInputActive())
        OnUpdate();

    Math::Vector3D pos = gameObject->transform.position;
    Math::Vector2D finalPos = Math::Vector2D(pos.x, pos.y) + screenPos;

    float bx = finalPos.x - width * 0.5f;
    float by = finalPos.y - height * 0.5f;

    Math::Vector4D bgColor = baseColor;
    if (isPressed)      
        bgColor = pressedColor;
    else if (isHovered) 
        bgColor = hoverColor;

    if (bgTexture)
        WidgetRenderer::GetInstance().DrawRoundedQuad(bx, by, width, height, radius, bgColor, cmd, device);
    else
        WidgetRenderer::GetInstance().DrawRoundedQuad(bx, by, width, height, radius, bgColor, cmd, device);

    
    WidgetRenderer::GetInstance().GetTextRenderer()->Render(text, finalPos.x, finalPos.y - (fontSize / 2.f), fontSize, textColor, fT, cmd, device);
}

void Button::Destroy()
{
    WidgetRenderer::GetInstance().RemoveWidget(this);
    gameObject->RebuildLayer();
}
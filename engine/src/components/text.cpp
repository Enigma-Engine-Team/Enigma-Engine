#include "components/text.h"
#include "renderer/widget_renderer.h"
#include "debug/log.h"
#include "gameobject/gameobject.h"

RTTR_REGISTRATION
{
	rttr::registration::class_<Text>("Text")
		.constructor<>()(rttr::policy::ctor::as_raw_ptr)
		.property("text", &Text::text)
		(
			rttr::metadata(UI_DATA_METADATA_KEY, UIData{EUIType::MULTILINE_TEXT, 0.f, 0.f, "%.0f", "General"})
		)
		.property("Screen Position", &Text::screenPos)
		(
			rttr::metadata(UI_DATA_METADATA_KEY, UIData{EUIType::VEC2, 0.f, 1920.f, "%.0f", "Style"})
		)
		.property("font", &Text::font)
		(
			rttr::metadata(UI_DATA_METADATA_KEY, UIData{EUIType::FONT_INPUT, 0.f, 0.f, "%.0f", "Style"})
		)
		.property("Font color", &Text::color)
		(
			rttr::metadata(UI_DATA_METADATA_KEY, UIData{EUIType::COLOR4, 0.f, 0.f, "%.0f", "Style"})
		)
		.property("Font Size", &Text::fontSize)
		(
			rttr::metadata(UI_DATA_METADATA_KEY, UIData{ EUIType::DRAG_FLOAT, 1.f, 100.f, "%.2f", "Style" })
		);
}

void Text::Create()
{
	font = WidgetRenderer::GetInstance().GetDefaultFontTexture();
	WidgetRenderer::GetInstance().RegisterWidget(this);
	gameObject->SetLayer(ELayer::UI);
}

void Text::Render(Math::Matrix4x4 camVP, Math::Matrix4x4 camViewMatrix, EnigmaRHI::ICommandBuffer& cmd, EnigmaRHI::IDevice* device)
{
	Math::Vector3D pos = gameObject->transform.position;

	Math::Vector2D finalScreenPos = Math::Vector2D(pos.x, pos.y) + screenPos;
	WidgetRenderer::GetInstance().GetTextRenderer()->Render(text, finalScreenPos.x, finalScreenPos.y, fontSize, color, font, cmd, device);
}

void Text::Destroy()
{
	WidgetRenderer::GetInstance().RemoveWidget(this);
	gameObject->RebuildLayer();
}

void Text::SetText(std::string _text)
{
	text = _text;
}
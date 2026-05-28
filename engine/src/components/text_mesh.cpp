#include "components/text_mesh.h"
#include "renderer/widget_renderer.h"
#include "gameobject/gameobject.h"

RTTR_REGISTRATION
{
	rttr::registration::class_<TextMesh>("TextMesh")
		.constructor<>()(rttr::policy::ctor::as_raw_ptr)
		.property("Text", &TextMesh::text)
		(
			rttr::metadata(UI_DATA_METADATA_KEY, UIData{EUIType::MULTILINE_TEXT, 0.f, 0.f, "%.0f", "General"})
		)
		.property("Font", &TextMesh::font)
		(
			rttr::metadata(UI_DATA_METADATA_KEY, UIData{EUIType::FONT_INPUT, 0.f, 0.f, "%.0f", "General"})
		)
		.property("Local Position", &TextMesh::localPos)
		(
			rttr::metadata(UI_DATA_METADATA_KEY, UIData{EUIType::VEC3, 0.f, 0.f, "%.0f", "Style"})
		)
		.property("Font color", &TextMesh::color)
		(
			rttr::metadata(UI_DATA_METADATA_KEY, UIData{EUIType::COLOR4, 0.f, 0.f, "%.0f", "Style"})
		)
		.property("Font Size", &TextMesh::fontSize)
		(
			rttr::metadata(UI_DATA_METADATA_KEY, UIData{ EUIType::DRAG_FLOAT, 1.f, 100.f, "%.2f", "Style" })
		)
		.property("Face Camera", &TextMesh::faceCamera)
		(
			rttr::metadata(UI_DATA_METADATA_KEY, UIData{ EUIType::BOOL, 0.f, 0.f, "%.0f", "Style" })
		);
}

void TextMesh::Create()
{
	font = WidgetRenderer::GetInstance().GetDefaultFontTexture();
	WidgetRenderer::GetInstance().RegisterWidget(this);
}

void TextMesh::Render(Math::Matrix4x4 camVP, Math::Matrix4x4 camViewMatrix, EnigmaRHI::ICommandBuffer& cmd, EnigmaRHI::IDevice* device)
{
	Math::Vector3D worldPos = gameObject->transform.position + localPos;
	Math::Quaternion rot = gameObject->transform.worldRotation.Conjugate();
	WidgetRenderer::GetInstance().GetTextRenderer()->Render3D(text, worldPos, fontSize, color, camVP, camViewMatrix, font, cmd, device, faceCamera, rot);
}

void TextMesh::Destroy()
{
	WidgetRenderer::GetInstance().RemoveWidget(this);
}

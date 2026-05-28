#pragma once

#include "debug/log.h"
#include "camera.h"
#include "resources/font_texture.h"
#include "renderer/text_renderer.h"
#include "components/widget.h"


class ENIGMA_API WidgetRenderer
{
public:
	WidgetRenderer() = default;

	static WidgetRenderer& GetInstance();

	void Create(EnigmaRHI::IRenderInterface* rhi);
	void Destroy();

	void RenderWidgets(Math::Matrix4x4 camVP, Math::Matrix4x4 camViewMatrix, EnigmaRHI::ICommandBuffer& cmd, EnigmaRHI::IDevice* device);
	void DrawRoundedQuad(float x, float y, float w, float h, float radius,
		Math::Vector4D color, EnigmaRHI::ICommandBuffer& cmd,
		EnigmaRHI::IDevice* device);

	void DrawRoundedQuadTextured(float x, float y, float w, float h, float radius,
		EnigmaRHI::IImage* texture, EnigmaRHI::ICommandBuffer& cmd,
		EnigmaRHI::IDevice* device);

	void EnableInput() { inputActive = true; };
	void DisableInput() { inputActive = false; };

	bool IsInputActive() const { return inputActive; };

	void RegisterWidget(Widget* widget) { widgetsToRender.push_back(widget); }
	void RemoveWidget(Widget* widget);
	void ClearWidgets() { widgetsToRender.clear(); };

	FontTexture* GetFontTexture(const std::string& fontTexture);
	FontTexture* GetDefaultFontTexture();
	TextRenderer* GetTextRenderer() const { return textRenderer; };
	void LoadFont(const std::string& fontPath);

private:

	void InitQuadPipeline();
	std::vector<Widget*> widgetsToRender;

	EnigmaRHI::IRenderInterface* rhi = nullptr;
	TextRenderer* textRenderer = nullptr;

	EnigmaRHI::IPipeline* quadPipeline = nullptr;
	EnigmaRHI::IBuffer* quadVBO = nullptr;
	EnigmaRHI::IVertexInput* quadVAO = nullptr;
	EnigmaRHI::IShaderModule* quadVertShader = nullptr;
	EnigmaRHI::IShaderModule* quadFragShader = nullptr;
	EnigmaRHI::IDescriptor* quadDescriptor = nullptr;

	bool inputActive = false;
};
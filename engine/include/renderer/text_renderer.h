#pragma once

#include "utilities/macro.h"
#include "IRenderInterface.h"
#include <vector>
#include "emath/emath.h"
#include "resources/font_texture.h"

#include <ft2build.h>
#include FT_FREETYPE_H

class WidgetRenderer;

class ENIGMA_API TextRenderer
{
public:

	TextRenderer() = default;
	~TextRenderer() = default;

	void Create(EnigmaRHI::IRenderInterface* rhi);
	void Destroy();

	void Render(const std::string& text, float x, float y, float scale, Math::Vector4D color, FontTexture* fontTexture, EnigmaRHI::ICommandBuffer& cmd, EnigmaRHI::IDevice* device);
	void Render3D(const std::string& text, Math::Vector3D worldPos, float scale, Math::Vector4D color, Math::Matrix4x4 vp, Math::Matrix4x4 view, FontTexture* fontTexture, EnigmaRHI::ICommandBuffer& cmd, EnigmaRHI::IDevice* device, bool faceCamera, Math::Quaternion rotation);

	static Math::Vector2D GetTextSize(const std::string& text, float scale);
	static FT_Library GetFontLibrary() { return ft; }
	static constexpr unsigned int textLimit = 256;

private:

	void InitTextLibrary();
	void InitTextRenderer(EnigmaRHI::IRenderInterface* rhi);
	void InitTextPipeline(EnigmaRHI::IRenderInterface* rhi);

	static void TextRenderCall(int lenght, EnigmaRHI::ICommandBuffer& commandBuffer);
	static void TextRenderCall3D(int length, Math::Matrix4x4 vp, EnigmaRHI::ICommandBuffer& cmd);

	static Math::Matrix4x4 ComputeBillboardMatrix(Math::Vector3D pos, Math::Vector3D up, Math::Vector3D right, float scale);


	static FT_Library ft;

	static std::vector<int> letterMap;
	static std::vector<Math::Matrix4x4> transforms;

	static EnigmaRHI::IBuffer* textBuffer;
	static EnigmaRHI::IBuffer* textInstanceTransformSSBO;
	static EnigmaRHI::IVertexInput* textVertexInput;
	static EnigmaRHI::IShaderModule* textVertShader;
	static EnigmaRHI::IShaderModule* textFragShader;
	static EnigmaRHI::IPipeline* text2DPipeline;
	static EnigmaRHI::IPipeline* text3DPipeline;
	static EnigmaRHI::IDescriptor* textDescriptor;
};
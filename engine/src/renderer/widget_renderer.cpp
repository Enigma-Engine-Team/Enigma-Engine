#include "renderer/widget_renderer.h"
#include "resources/resource_manager.h"
#include "resources/font_texture.h"

WidgetRenderer& WidgetRenderer::GetInstance()
{
	static WidgetRenderer widgetRenderer;
	return widgetRenderer;
}

void WidgetRenderer::Create(EnigmaRHI::IRenderInterface* _rhi)
{
	this->rhi = _rhi;
	textRenderer = new TextRenderer();
	textRenderer->Create(rhi);
	LoadFont("Inter-Regular.ttf");
	InitQuadPipeline();
}

void WidgetRenderer::Destroy()
{
	delete textRenderer;
	textRenderer = nullptr;
}

void WidgetRenderer::RenderWidgets(Math::Matrix4x4 camVP, Math::Matrix4x4 camViewMatrix, EnigmaRHI::ICommandBuffer& cmd, EnigmaRHI::IDevice* device)
{
	for (auto w : widgetsToRender)
	{
		w->Render(camVP, camViewMatrix, cmd, device);
	}
}

void WidgetRenderer::DrawRoundedQuad(float x, float y, float w, float h, float radius, Math::Vector4D color, EnigmaRHI::ICommandBuffer& cmd, EnigmaRHI::IDevice* device)
{
    Math::Matrix4x4 ortho = Math::Matrix4x4::Orthographic(0.f, 1920.f, 0.f, 1080.f, -1.f, 1.f);

    cmd.BindPipeline(device, quadPipeline);
    cmd.BindVertexInput(quadVAO);
    cmd.BindVertexBuffer(quadVBO, 2 * sizeof(float));

    quadPipeline->SendToGPU("projection", ortho.m);
    quadPipeline->SendToGPU("position", x, y);
    quadPipeline->SendToGPU("size", w, h);
    quadPipeline->SendToGPU("radius", radius);
    quadPipeline->SendToGPU("bgColor", color.x, color.y, color.z, color.w);
    quadPipeline->SendToGPU("useTexture", false);

    cmd.Draw(EnigmaRHI::EDrawMode::TRIANGLE_STRIP, 4);
}

void WidgetRenderer::DrawRoundedQuadTextured(float x, float y, float w, float h, float radius, EnigmaRHI::IImage* texture, EnigmaRHI::ICommandBuffer& cmd, EnigmaRHI::IDevice* device)
{
    Math::Matrix4x4 ortho = Math::Matrix4x4::Orthographic(0.f, 1920.f, 0.f, 1080.f, -1.f, 1.f);

    cmd.BindPipeline(device, quadPipeline);
    cmd.BindVertexInput(quadVAO);
    cmd.BindVertexBuffer(quadVBO, 2 * sizeof(float));

    quadDescriptor->BindImage(0, EnigmaRHI::EImageType::TYPE_2D, texture->GetID());

    quadPipeline->SendToGPU("projection", ortho.m);
    quadPipeline->SendToGPU("position", x, y);
    quadPipeline->SendToGPU("size", w, h);
    quadPipeline->SendToGPU("radius", radius);
    quadPipeline->SendToGPU("useTexture", true);

    cmd.Draw(EnigmaRHI::EDrawMode::TRIANGLE_STRIP, 4);
}

void WidgetRenderer::RemoveWidget(Widget* widget)
{
	auto it = std::find(widgetsToRender.begin(), widgetsToRender.end(), widget);
	if (it != widgetsToRender.end())
	{
		widgetsToRender.erase(it);
	}
}

void WidgetRenderer::InitQuadPipeline()
{
    quadVertShader = rhi->InstantiateShaderModule();
    quadFragShader = rhi->InstantiateShaderModule();
    quadPipeline = rhi->InstantiatePipeline();
    quadDescriptor = rhi->InstantiateDescriptor();
    quadVBO = rhi->InstantiateBuffer();
    quadVAO = rhi->InstantiateVertexInput();

    quadVertShader->Create("shaders/quad.vert", EnigmaRHI::EShaderType::VERTEX);
    quadFragShader->Create("shaders/quad.frag", EnigmaRHI::EShaderType::FRAGMENT);

    EnigmaRHI::GraphicsPipeline quadGP
    {
        .cullMode = EnigmaRHI::ECullMode::DISABLED,
        .frontFaceMode = EnigmaRHI::EFrontFaceMode::COUNTER_CLOCK_WISE,
        .blendEnable = true,
        .sourceFactor = EnigmaRHI::EBlendFactor::SRC_ALPHA,
        .destFactor = EnigmaRHI::EBlendFactor::ONE_MINUS_SRC_ALPHA,
        .depthTestEnable = false,
        .polygonMode = EnigmaRHI::EPolygonMode::FILL,
        .depthWriteEnable = false,
    };

    quadPipeline->Create(quadGP, quadVertShader, quadFragShader);

    quadDescriptor->AddImageBinding(0); // bgTexture
    quadDescriptor->Create();

    float vertexData[] = {
        0.0f, 1.0f,
        0.0f, 0.0f,
        1.0f, 1.0f,
        1.0f, 0.0f,
    };

    quadVBO->Create(sizeof(vertexData), EnigmaRHI::EBufferTarget::ARRAY_BUFFER,
        vertexData, EnigmaRHI::EBufferUsage::STATIC_DRAW);

    quadVAO->Create();
    quadVAO->Bind();
    quadVAO->AddVertexAttribute(0, 2, EnigmaRHI::EDataType::FLOAT, false, 0);
    quadVAO->Unbind();
}

void WidgetRenderer::LoadFont(const std::string& fontPath)
{
	ResourceManager::GetInstance().Create<FontTexture>(fontPath, rhi, "shaders");
}

FontTexture* WidgetRenderer::GetFontTexture(const std::string& fontTexture)
{
	if (FontTexture* fontT = ResourceManager::GetInstance().Get<FontTexture>(fontTexture))
	{
		return fontT;
	}
	
	return ResourceManager::GetInstance().Create<FontTexture>(fontTexture, rhi);
}

FontTexture* WidgetRenderer::GetDefaultFontTexture()
{
	return ResourceManager::GetInstance().Get<FontTexture>("Inter-Regular.ttf");
}

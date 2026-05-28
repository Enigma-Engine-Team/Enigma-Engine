#include "renderer/text_renderer.h"
#include "debug/log.h"
#include "resources/resource_manager.h"
#include "resources/font_texture.h"
#include "GLBuffer.h"
#include "GLPipeline.h"
#include "emath/emath.h"

FT_Library TextRenderer::ft;
std::vector<int> TextRenderer::letterMap;
std::vector<Math::Matrix4x4> TextRenderer::transforms;
EnigmaRHI::IBuffer* TextRenderer::textBuffer = nullptr;
EnigmaRHI::IBuffer* TextRenderer::textInstanceTransformSSBO = nullptr;
EnigmaRHI::IVertexInput* TextRenderer::textVertexInput = nullptr;
EnigmaRHI::IShaderModule* TextRenderer::textVertShader = nullptr;
EnigmaRHI::IShaderModule* TextRenderer::textFragShader = nullptr;
EnigmaRHI::IPipeline* TextRenderer::text2DPipeline = nullptr;
EnigmaRHI::IPipeline* TextRenderer::text3DPipeline = nullptr;
EnigmaRHI::IDescriptor* TextRenderer::textDescriptor = nullptr;

void TextRenderer::Create(EnigmaRHI::IRenderInterface* rhi)
{
	InitTextLibrary();
	InitTextRenderer(rhi);
	InitTextPipeline(rhi);
}

void TextRenderer::Destroy()
{
	if (ft)
	{
		FT_Done_FreeType(ft);
		ft = nullptr;
	}
}

void TextRenderer::Render(const std::string& text, float x, float y, float scale, Math::Vector4D color, FontTexture* fontTexture, EnigmaRHI::ICommandBuffer& cmd, EnigmaRHI::IDevice* device)
{
	cmd.BindPipeline(device, text2DPipeline);
	cmd.BindVertexInput(textVertexInput);
	cmd.BindVertexBuffer(textBuffer, 2 * sizeof(float));
	textBuffer->Bind(EnigmaRHI::EBufferTarget::ARRAY_BUFFER);

	text2DPipeline->SendToGPU("textColor", color.x, color.y, color.z, color.w);
	textDescriptor->BindBuffer(0, EnigmaRHI::EBufferTarget::SHADER_STORAGE_BUFFER, textInstanceTransformSSBO);
	textDescriptor->BindImage(1, EnigmaRHI::EImageType::TYPE_2D_ARRAY, fontTexture->GetFontImage()->GetID());

	scale /= 10.0f;

	Math::Vector2D size = GetTextSize(text, scale);

	float finalScale = scale * 48.0f / 256.0f;

	float currentX = x - (size.x / 2.0f);
	float currentY = y + (size.y / 4.0f);

	float copyX = currentX;

	std::map<unsigned char, TextCharacter> characters = fontTexture->GetCharacters();
	int workingIndex = 0;

    for (auto c = text.begin(); c != text.end(); c++)
    {
        TextCharacter ch = characters[*c];

        if (*c == '\n') 
		{
            currentY -= ((ch.size.y)) * 1.3 * finalScale;
            currentX = copyX;
        }
        else if (*c == ' ') 
		{
            currentX += (ch.advance >> 6) * finalScale;
        }
        else 
		{
			float xpos = currentX + ch.bearing.x * finalScale;
			float ypos = currentY - (256 - ch.bearing.y) * finalScale;

			transforms[workingIndex] = Math::Matrix4x4::TRS(Math::Vector3D(xpos, ypos - (size.y / 2.f), 0), Math::Vector3D(0, 0, 0), Math::Vector3D(256 * finalScale, 256 * finalScale, 1));
            letterMap[workingIndex] = ch.textureID;

            // render quad
            // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
            currentX += (ch.advance >> 6) * finalScale; // bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
            workingIndex++;

            if (workingIndex == textLimit)
			{
                TextRenderCall(workingIndex, cmd);
                workingIndex = 0;
            }
        }
    }

	TextRenderCall(workingIndex, cmd);

	textBuffer->Unbind(EnigmaRHI::EBufferTarget::ARRAY_BUFFER);
	textVertexInput->Unbind();
}

void TextRenderer::Render3D(const std::string& text, Math::Vector3D worldPos, float scale, Math::Vector4D color, Math::Matrix4x4 vp, Math::Matrix4x4 view, FontTexture* fontTexture, EnigmaRHI::ICommandBuffer& cmd, EnigmaRHI::IDevice* device, bool faceCamera, Math::Quaternion rot)
{
	cmd.BindPipeline(device, text3DPipeline);
	cmd.BindVertexInput(textVertexInput);
	cmd.BindVertexBuffer(textBuffer, 2 * sizeof(float));
	textBuffer->Bind(EnigmaRHI::EBufferTarget::ARRAY_BUFFER);

	text3DPipeline->SendToGPU("textColor", color.x, color.y, color.z, color.w);
	textDescriptor->BindBuffer(0, EnigmaRHI::EBufferTarget::SHADER_STORAGE_BUFFER, textInstanceTransformSSBO);
	textDescriptor->BindImage(1, EnigmaRHI::EImageType::TYPE_2D_ARRAY, fontTexture->GetFontImage()->GetID());

	scale /= 1000.f;

	Math::Vector2D textSize = GetTextSize(text, scale);
	float finalScale = scale * 48.0f / 256.0f;

	float cursorX = -(textSize.x / 2.0f);
	float cursorY = (textSize.y / 2.0f);

	float copyX = cursorX;

    Math::Vector3D right = Math::Vector3D(view.m[0], view.m[4], view.m[8]);
    Math::Vector3D up = Math::Vector3D(view.m[1], view.m[5], view.m[9]);

    scale = scale * 48.0f / 256.0f;

    std::map<unsigned char, TextCharacter> characters = fontTexture->GetCharacters();
    int workingIndex = 0;

    for (auto c = text.begin(); c != text.end(); c++)
    {
        TextCharacter ch = characters[*c];

        if (*c == '\n') 
		{
            cursorY -= ch.size.y * 1.3f * finalScale;
            cursorX = copyX;
        }
        else if (*c == ' ') 
		{
            cursorX += (ch.advance >> 6) * finalScale;
        }
        else 
		{
            float xpos = cursorX + ch.bearing.x * finalScale;
            float ypos = cursorY - (256 - ch.bearing.y) * finalScale;

			float s = 256.f * scale;

			if (faceCamera)
			{
				Math::Vector3D letterWorldPos = worldPos + (right * xpos) + (up * ypos);
				transforms[workingIndex] = ComputeBillboardMatrix(letterWorldPos, up, right, s);
			}
			else
			{
				Math::Vector3D letterWorldPos = worldPos + (rot.RotateVector(Math::Vector3D::Right) * xpos) + (rot.RotateVector(Math::Vector3D::Up) * ypos);
				letterWorldPos.y -= (textSize.y * 0.75f);
				transforms[workingIndex] = Math::Quaternion::TRS(letterWorldPos, rot, Math::Vector3D(s, s, 1));
			}

            letterMap[workingIndex] = ch.textureID;

            cursorX += (ch.advance >> 6) * finalScale;
            workingIndex++;

            if (workingIndex == textLimit)
			{
                TextRenderCall3D(workingIndex, vp, cmd);
                workingIndex = 0;
            }
        }
    }

    TextRenderCall3D(workingIndex, vp, cmd);

    textBuffer->Unbind(EnigmaRHI::EBufferTarget::ARRAY_BUFFER);
    textVertexInput->Unbind();
}

void TextRenderer::TextRenderCall(int length, EnigmaRHI::ICommandBuffer& cmd)
{
	if (length == 0)
		return;

	textInstanceTransformSSBO->Bind(EnigmaRHI::EBufferTarget::SHADER_STORAGE_BUFFER);
	textInstanceTransformSSBO->CopyData(&transforms[0], sizeof(Math::Matrix4x4) * length);
	text2DPipeline->SendToGPU("letterMap", length, &letterMap[0]);
	Math::Matrix4x4 ortho = Math::Matrix4x4::Orthographic(0.f, 1920.f, 0.f, 1080.f, -1.f, 1.f);
	text2DPipeline->SendToGPU("projection", ortho.m);

	cmd.Draw(EnigmaRHI::EDrawMode::TRIANGLE_STRIP, 4, length);
}

void TextRenderer::TextRenderCall3D(int length, Math::Matrix4x4 vp, EnigmaRHI::ICommandBuffer& cmd)
{
	if (length == 0) return;

	textInstanceTransformSSBO->Bind(EnigmaRHI::EBufferTarget::SHADER_STORAGE_BUFFER);
	textInstanceTransformSSBO->CopyData(&transforms[0], sizeof(Math::Matrix4x4) * length);
	text3DPipeline->SendToGPU("letterMap", length, &letterMap[0]);
	text3DPipeline->SendToGPU("projection", vp.m); // VP au lieu de l'ortho
	cmd.Draw(EnigmaRHI::EDrawMode::TRIANGLE_STRIP, 4, length);
}

Math::Matrix4x4 TextRenderer::ComputeBillboardMatrix(Math::Vector3D pos, Math::Vector3D up, Math::Vector3D right, float s)
{
	Math::Matrix4x4 t = Math::Matrix4x4::Identity;

	t.m[0] = right.x * s;
	t.m[1] = right.y * s;
	t.m[2] = right.z * s;
	t.m[3] = 0.0f;

	t.m[4] = up.x * s;
	t.m[5] = up.y * s;
	t.m[6] = up.z * s;
	t.m[7] = 0.0f;

	Math::Vector3D forward = right.CrossProduct(up);
	t.m[8] = forward.x;
	t.m[9] = forward.y;
	t.m[10] = forward.z;
	t.m[11] = 0.0f;

	t.m[12] = pos.x;
	t.m[13] = pos.y;
	t.m[14] = pos.z;
	t.m[15] = 1.0f;

	return t;
}

Math::Vector2D TextRenderer::GetTextSize(const std::string& text, float scale)
{
	float width = 0.0f;
	float currentLineWidth = 0.0f;
	int lines = 1;

	float finalScale = scale * 48.0f / 256.0f;
	FontTexture* fontTexture = ResourceManager::GetInstance().Get<FontTexture>("Inter-Regular.ttf");
	auto characters = fontTexture->GetCharacters();

	for (char c : text)
	{
		if (c == '\n')
		{
			width = std::max(width, currentLineWidth);
			currentLineWidth = 0.0f;
			lines++;
			continue;
		}
		TextCharacter ch = characters[c];
		currentLineWidth += (ch.advance >> 6) * finalScale;
	}
	width = std::max(width, currentLineWidth);

	float height = lines * (256.0f * finalScale);

	return Math::Vector2D(width, height);
}

void TextRenderer::InitTextLibrary()
{
	if (FT_Init_FreeType(&ft))
	{
		Debug::LogError("Could not init FreeType Library");
		return;
	}
}

void TextRenderer::InitTextRenderer(EnigmaRHI::IRenderInterface* rhi)
{
	textBuffer = rhi->InstantiateBuffer();
	textInstanceTransformSSBO = rhi->InstantiateBuffer();
	textVertexInput = rhi->InstantiateVertexInput();

	for (int i = 0; i < textLimit; i++)
	{
		letterMap.push_back(0);
		transforms.push_back(Math::Matrix4x4::Identity);
	}
	
	float vertexData[] = 
	{
		0.0f,1.0f,
		0.0f,0.0f,
		1.0f,1.0f,
		1.0f,0.0f,
	};

	textBuffer->Create(sizeof(vertexData), EnigmaRHI::EBufferTarget::ARRAY_BUFFER, vertexData, EnigmaRHI::EBufferUsage::DYNAMIC_DRAW);
	textInstanceTransformSSBO->Create(sizeof(Math::Matrix4x4) * textLimit, EnigmaRHI::EBufferTarget::SHADER_STORAGE_BUFFER,
		nullptr, EnigmaRHI::EBufferUsage::DYNAMIC_DRAW);
	
	textVertexInput->Create();
	textVertexInput->Bind();
	textVertexInput->AddVertexAttribute(0, 2, EnigmaRHI::EDataType::FLOAT, false, 0);
	textVertexInput->Unbind();
}

void TextRenderer::InitTextPipeline(EnigmaRHI::IRenderInterface* rhi)
{
	textVertShader = rhi->InstantiateShaderModule();
	textFragShader = rhi->InstantiateShaderModule();
	text2DPipeline = rhi->InstantiatePipeline();
	text3DPipeline = rhi->InstantiatePipeline();
	textDescriptor = rhi->InstantiateDescriptor();

	textVertShader->Create("shaders/text.vert", EnigmaRHI::EShaderType::VERTEX);
	textFragShader->Create("shaders/text.frag", EnigmaRHI::EShaderType::FRAGMENT);

	EnigmaRHI::GraphicsPipeline text2DGP
	{
		.cullMode = EnigmaRHI::ECullMode::DISABLED,
		.frontFaceMode = EnigmaRHI::EFrontFaceMode::COUNTER_CLOCK_WISE,
		.blendEnable = true,
		.sourceFactor = EnigmaRHI::EBlendFactor::SRC_ALPHA,
		.destFactor = EnigmaRHI::EBlendFactor::ONE_MINUS_SRC_ALPHA,
		.depthTestEnable = false,
		.compareMode = EnigmaRHI::EDepthCompareOp::LESS,
		.polygonMode = EnigmaRHI::EPolygonMode::FILL,
		.depthWriteEnable = false,
	};

	EnigmaRHI::GraphicsPipeline text3DGP = text2DGP;
	text3DGP.depthTestEnable = true;
	text3DGP.depthWriteEnable = false;

	text2DPipeline->Create(text2DGP, textVertShader, textFragShader);
	text3DPipeline->Create(text3DGP, textVertShader, textFragShader);

	textDescriptor->AddBufferBinding(0); // instance transforms
	textDescriptor->AddImageBinding(1); // font atlas
	textDescriptor->Create();
}


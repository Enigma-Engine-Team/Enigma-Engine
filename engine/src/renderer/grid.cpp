#include "renderer/grid.h"
#include "glad/glad.h"

void Grid::Create(EnigmaRHI::IRenderInterface* rhi)
{
	gridPipeline = rhi->InstantiatePipeline();
	gridVert = rhi->InstantiateShaderModule();
	gridFrag = rhi->InstantiateShaderModule();
	gridMode = rhi->InstantiateVertexInput();

	gridVert->Create("shaders/grid.vert", EnigmaRHI::EShaderType::VERTEX);
	gridFrag->Create("shaders/grid.frag", EnigmaRHI::EShaderType::FRAGMENT);

	EnigmaRHI::GraphicsPipeline gridGraphicsPipeline
	{
		.cullMode = EnigmaRHI::ECullMode::DISABLED,
		.frontFaceMode = EnigmaRHI::EFrontFaceMode::COUNTER_CLOCK_WISE,
		.blendEnable = true,
		.sourceFactor = EnigmaRHI::EBlendFactor::SRC_ALPHA,
		.destFactor = EnigmaRHI::EBlendFactor::ONE_MINUS_SRC_ALPHA,
		.combinationMode = EnigmaRHI::EBlendOp::ADD,
		.depthTestEnable = true,
		.polygonMode = EnigmaRHI::EPolygonMode::FILL,
		.depthWriteEnable = false
	};

	gridPipeline->Create(gridGraphicsPipeline, gridVert, gridFrag);
	gridMode->Create();

	gridMode->Bind();
	gridMode->AddVertexAttribute(0, 3, EnigmaRHI::EDataType::FLOAT, false, 0);
	gridMode->Unbind();
}

void Grid::Render(EnigmaRHI::ICommandBuffer& cmd) const
{
	gridMode->Bind();
	//glDepthMask(GL_FALSE);
	cmd.Draw(EnigmaRHI::EDrawMode::TRIANGLES, 6);
	//glDepthMask(GL_TRUE);
	gridMode->Unbind();
}

void Grid::Destroy(EnigmaRHI::IRenderInterface* rhi)
{
	gridMode->Destroy();
	rhi->DeleteVertexInput(gridMode);

	gridPipeline->Destroy();
	rhi->DeletePipeline(gridPipeline);

	delete gridVert;
	delete gridFrag;
}
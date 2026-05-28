#include "renderer/gizmo_renderer.h"
#include "ICommandBuffer.h"
#include <vector>
#include <iostream>
#include <array>
#include "resources/texture.h"

void GizmoRenderer::Create(EnigmaRHI::IRenderInterface* rhi)
{
	CreateGizmoPipeline(rhi);
	CreateGizmoVertexInput(rhi);
	CreateGizmoDescriptor(rhi);
	CreateGizmoBuffer(rhi);
	CreateGizmoMeshes(rhi);
}

void GizmoRenderer::Destroy()
{
	gizmoPipeline->Destroy();
	gizmoVertexMode->Destroy();
	gizmoDescriptor->Destroy();
	gizmoBuffer->Destroy();
}

void GizmoRenderer::Render(const std::vector<GizmoCommand>& commands, const Math::Matrix4x4 currentVP, EnigmaRHI::IDevice* device, EnigmaRHI::ICommandBuffer& command, bool useCommandColor)
{
	if (!renderGizmos)
		return;

	gizmoVertexMode->Bind();

	command.BindDescriptorSet(gizmoDescriptor);
	gizmoDescriptor->BindBuffer(0, EnigmaRHI::EBufferTarget::UNIFORM_BUFFER, gizmoBuffer);

	for (auto& cmd : commands)
	{
		GizmoGPUData data
		{
			.transform = cmd.transform,
			.vp = currentVP,
		};

		if(useCommandColor)
			data.color = cmd.gizmoColor;
		else
			data.color = cmd.gizmoColor / 2.f;

		gizmoBuffer->CopyData(&data, sizeof(GizmoGPUData));

		switch (cmd.type) 
		{
		case GizmoCommand::EGizmoType::ICON:
			command.BindPipeline(device, billboardPipeline);
			gizmoDescriptor->BindBuffer(0, EnigmaRHI::EBufferTarget::UNIFORM_BUFFER, gizmoBuffer);
			gizmoDescriptor->BindImage(1, EnigmaRHI::EImageType::TYPE_2D, cmd.billboardTexture);
			billboardPipeline->SendToGPU("opacity", cmd.billboardOpacity);

			command.BindVertexBuffer(quadVBO, 5 * sizeof(float)); // pos + tex_coords
			command.BindIndexBuffer(quadEBO);
			command.DrawIndexed(EnigmaRHI::EDrawMode::TRIANGLES, 6);
			break;
		case GizmoCommand::EGizmoType::CUBE:
			command.BindPipeline(device, gizmoPipeline);
			command.BindVertexBuffer(cubeVBO, 3 * sizeof(float)); // pos
			command.BindIndexBuffer(cubeEBO);
			command.DrawIndexed(EnigmaRHI::EDrawMode::LINES, cubeIndexCount);
			break;
		case GizmoCommand::EGizmoType::CIRCLE:
			command.BindPipeline(device, gizmoPipeline);
			command.BindVertexBuffer(circleVBO, 3 * sizeof(float)); // pos
			command.BindIndexBuffer(circleEBO);
			command.DrawIndexed(EnigmaRHI::EDrawMode::LINES, circleIndexCount);
			break;
		case GizmoCommand::EGizmoType::MESH:
			command.BindPipeline(device, gizmoPipeline);
			for (auto* mesh : cmd.meshes)
			{
				mesh->Draw(command);
			}
			break;
		case GizmoCommand::EGizmoType::LINE:
			command.BindPipeline(device, gizmoPipeline);
			float lineVerts[6] = {
			cmd.from.x, cmd.from.y, cmd.from.z,
			cmd.to.x,   cmd.to.y,   cmd.to.z,
			};
			lineVBO->CopyData(lineVerts, sizeof(lineVerts));
			command.BindVertexBuffer(lineVBO, 3 * sizeof(float));
			command.Draw(EnigmaRHI::EDrawMode::LINES, 2);
			break;
		}
	}

	gizmoVertexMode->Unbind();
}

void GizmoRenderer::CreateGizmoPipeline(EnigmaRHI::IRenderInterface* rhi)
{
	gizmoPipeline = rhi->InstantiatePipeline();
	billboardPipeline = rhi->InstantiatePipeline();
	gizmoPipelineSecond = rhi->InstantiatePipeline();
	gizmoVertShader = rhi->InstantiateShaderModule();
	gizmoFragShader = rhi->InstantiateShaderModule();
	billboardVertShader = rhi->InstantiateShaderModule();
	billboardFragShader = rhi->InstantiateShaderModule();

	gizmoFragShader->Create("shaders/gizmo.frag", EnigmaRHI::EShaderType::FRAGMENT);
	gizmoVertShader->Create("shaders/gizmo.vert", EnigmaRHI::EShaderType::VERTEX);

	billboardFragShader->Create("shaders/billboard.frag", EnigmaRHI::EShaderType::FRAGMENT);
	billboardVertShader->Create("shaders/billboard.vert", EnigmaRHI::EShaderType::VERTEX);

	EnigmaRHI::GraphicsPipeline gizmoGP
	{
		.cullMode = EnigmaRHI::ECullMode::DISABLED,
		.frontFaceMode = EnigmaRHI::EFrontFaceMode::COUNTER_CLOCK_WISE,
		.blendEnable = true,
		.sourceFactor = EnigmaRHI::EBlendFactor::SRC_ALPHA,
		.destFactor = EnigmaRHI::EBlendFactor::ONE_MINUS_SRC_ALPHA,
		.depthTestEnable = true,
		.compareMode = EnigmaRHI::EDepthCompareOp::LESS,
		.polygonMode = EnigmaRHI::EPolygonMode::LINE,
		.lineWidth = 2.f,
		.depthWriteEnable = true,
	};

	EnigmaRHI::GraphicsPipeline gizmoGPSecond
	{
		.cullMode = EnigmaRHI::ECullMode::DISABLED,
		.frontFaceMode = EnigmaRHI::EFrontFaceMode::CLOCK_WISE,
		.blendEnable = true,
		.sourceFactor = EnigmaRHI::EBlendFactor::SRC_ALPHA,
		.destFactor = EnigmaRHI::EBlendFactor::ONE_MINUS_SRC_ALPHA,
		.depthTestEnable = true,
		.compareMode = EnigmaRHI::EDepthCompareOp::GREATER,
		.polygonMode = EnigmaRHI::EPolygonMode::LINE,
		.lineWidth = 1.f,
		.depthWriteEnable = true,
	};

	EnigmaRHI::GraphicsPipeline billboardGP = gizmoGP;
	billboardGP.polygonMode = EnigmaRHI::EPolygonMode::FILL;

	gizmoPipelineSecond->Create(gizmoGPSecond, gizmoVertShader, gizmoFragShader);
	gizmoPipeline->Create(gizmoGP, gizmoVertShader, gizmoFragShader);
	billboardPipeline->Create(billboardGP, billboardVertShader, billboardFragShader);
}

void GizmoRenderer::CreateGizmoVertexInput(EnigmaRHI::IRenderInterface* rhi)
{
	gizmoVertexMode = rhi->InstantiateVertexInput();
	gizmoVertexMode->Create();

	gizmoVertexMode->Bind();
	gizmoVertexMode->AddVertexAttribute(0, 3, EnigmaRHI::EDataType::FLOAT, false, 0);
	gizmoVertexMode->AddVertexAttribute(1, 2, EnigmaRHI::EDataType::FLOAT, false, 3 * sizeof(float));
	gizmoVertexMode->Unbind();
}

void GizmoRenderer::CreateGizmoDescriptor(EnigmaRHI::IRenderInterface* rhi)
{
	gizmoDescriptor = rhi->InstantiateDescriptor();
	gizmoDescriptor->AddBufferBinding(0); // GizmoGPUData
	gizmoDescriptor->AddImageBinding(1); // Billboard texture
	gizmoDescriptor->Create();
}

void GizmoRenderer::CreateGizmoBuffer(EnigmaRHI::IRenderInterface* rhi)
{
	uint64_t bufferSize = sizeof(GizmoGPUData);

	gizmoBuffer = rhi->InstantiateBuffer();
	gizmoBuffer->Create(bufferSize, EnigmaRHI::EBufferTarget::UNIFORM_BUFFER, nullptr, EnigmaRHI::EBufferUsage::STATIC_DRAW);
	gizmoBuffer->CreateDescriptorBufferInfo();
	gizmoBuffer->bufferInfo.range = sizeof(GizmoGPUData);
}

void GizmoRenderer::CreateGizmoMeshes(EnigmaRHI::IRenderInterface* rhi)
{
	CreateCubeMesh(rhi);
	CreateCircleMesh(rhi);
	CreateQuadMesh(rhi);
}

void GizmoRenderer::CreateCubeMesh(EnigmaRHI::IRenderInterface* rhi)
{
	static const float cubeVerts[] = {
		-0.5f,-0.5f,-0.5f,  // back  bottom left
		 0.5f,-0.5f,-0.5f,  // back  bottom right
		 0.5f, 0.5f,-0.5f,  // back  top    right
		-0.5f, 0.5f,-0.5f,  // back  top    left
		-0.5f,-0.5f, 0.5f,  // front bottom left
		 0.5f,-0.5f, 0.5f,  // font bottom right
		 0.5f, 0.5f, 0.5f,  // font top    right
		-0.5f, 0.5f, 0.5f,  // font top    left
	};
	static const uint32_t cubeIdx[] = {
		// Face back
		0,1,  1,2,  2,3,  3,0,
		// Face front
		4,5,  5,6,  6,7,  7,4,
		// Cot�s
		0,4,  1,5,  2,6,  3,7
	};
	cubeIndexCount = 24;

	cubeVBO = rhi->InstantiateBuffer();
	cubeVBO->Create(sizeof(cubeVerts), EnigmaRHI::EBufferTarget::ARRAY_BUFFER, cubeVerts, EnigmaRHI::EBufferUsage::STATIC_DRAW);

	cubeEBO = rhi->InstantiateBuffer();
	cubeEBO->Create(sizeof(cubeIdx), EnigmaRHI::EBufferTarget::ELEMENT_BUFFER, cubeIdx, EnigmaRHI::EBufferUsage::STATIC_DRAW);
}

void GizmoRenderer::CreateCircleMesh(EnigmaRHI::IRenderInterface* rhi)
{
	const int segments = 32;

	float circleVerts[segments * 3]{};

	for(int i = 0; i < segments; i ++)
	{
		float angle = 2.0f * Math::PI * (float)i / (float)segments;

		circleVerts[i * 3 + 0] = cosf(angle); // x
		circleVerts[i * 3 + 1] = sinf(angle); // y
		circleVerts[i * 3 + 2] = 0.0f;        // z
	}

	uint32_t circleIndx[segments * 2];
	for (int i = 0; i < segments; i++)
	{
		circleIndx[i * 2 + 0] = i;                  // current point
		circleIndx[i * 2 + 1] = (i + 1) % segments; // next point
	}

	circleIndexCount = segments * 2;

	circleVBO = rhi->InstantiateBuffer();
	circleVBO->Create(sizeof(circleVerts), EnigmaRHI::EBufferTarget::ARRAY_BUFFER, circleVerts, EnigmaRHI::EBufferUsage::STATIC_DRAW);

	circleEBO = rhi->InstantiateBuffer();
	circleEBO->Create(sizeof(circleIndx), EnigmaRHI::EBufferTarget::ELEMENT_BUFFER, circleIndx, EnigmaRHI::EBufferUsage::STATIC_DRAW);

	float lineVerts[6] = {
		0.f, 0.f, 0.f,
		0.f, 0.f, 0.f
	};

	lineVBO = rhi->InstantiateBuffer();
	lineVBO->Create(sizeof(lineVerts), EnigmaRHI::EBufferTarget::ARRAY_BUFFER, lineVerts, EnigmaRHI::EBufferUsage::STATIC_DRAW);
}

void GizmoRenderer::CreateQuadMesh(EnigmaRHI::IRenderInterface* rhi)
{
	float quadVertices[] = {
		// positions        // texture Coords
		-1.0f,  1.0f, 0.0f, 0.0f, 0.0f,
		-1.0f, -1.0f, 0.0f, 0.0f, 1.0f,
		 1.0f,  1.0f, 0.0f, 1.0f, 0.0f,
		1.0f, -1.0f, 0.0f, 1.0f, 1.0f,
	};
	static const uint32_t quadIdx[] = {
		0, 1, 2,  // top-left, bottom-left, top-right
		1, 3, 2   // bottom-left, bottom-right, top-right
	};

	quadVBO = rhi->InstantiateBuffer();
	quadVBO->Create(sizeof(quadVertices), EnigmaRHI::EBufferTarget::ARRAY_BUFFER, quadVertices, EnigmaRHI::EBufferUsage::STATIC_DRAW);

	quadEBO = rhi->InstantiateBuffer();
	quadEBO->Create(sizeof(quadIdx), EnigmaRHI::EBufferTarget::ELEMENT_BUFFER, quadIdx, EnigmaRHI::EBufferUsage::STATIC_DRAW);
}

#pragma once

#include "gizmo.h"
#include "IRenderInterface.h"

struct GizmoGPUData
{
	Math::Matrix4x4 transform;

	Math::Matrix4x4 vp;

	Math::Vector4D color;
};

class GizmoRenderer
{
public:

	void Create(EnigmaRHI::IRenderInterface* rhi);
	void Destroy();

	void SetGizmosActive(bool activeGizmos) { renderGizmos = activeGizmos; };
	bool IsGizmosActive() const { return renderGizmos; };

	void SetDisplayAABBActive(bool activeAABBDisplay) { renderAABB = activeAABBDisplay; };
	bool DisplayAABB() const { return renderAABB ;}

	void Render(const std::vector<GizmoCommand>& commands, const Math::Matrix4x4 currentVP, EnigmaRHI::IDevice* device, EnigmaRHI::ICommandBuffer& command, bool useCommandColor = true);
	EnigmaRHI::IPipeline* GetFrontPipeline() { return gizmoPipeline; };
	EnigmaRHI::IPipeline* GetBackPipeline() { return gizmoPipelineSecond; };

private:

	void CreateGizmoPipeline(EnigmaRHI::IRenderInterface* rhi);
	void CreateGizmoVertexInput(EnigmaRHI::IRenderInterface* rhi);
	void CreateGizmoDescriptor(EnigmaRHI::IRenderInterface* rhi);
	void CreateGizmoBuffer(EnigmaRHI::IRenderInterface* rhi);
	void CreateGizmoMeshes(EnigmaRHI::IRenderInterface* rhi);

	void CreateCubeMesh(EnigmaRHI::IRenderInterface* rhi);
	void CreateCircleMesh(EnigmaRHI::IRenderInterface* rhi);
	void CreateQuadMesh(EnigmaRHI::IRenderInterface* rhi);

	EnigmaRHI::IPipeline* gizmoPipeline;
	EnigmaRHI::IPipeline* billboardPipeline;
	EnigmaRHI::IPipeline* gizmoPipelineSecond;
	EnigmaRHI::IVertexInput* gizmoVertexMode;
	EnigmaRHI::IShaderModule* gizmoVertShader;
	EnigmaRHI::IShaderModule* gizmoFragShader;
	EnigmaRHI::IShaderModule* billboardVertShader;
	EnigmaRHI::IShaderModule* billboardFragShader;
	EnigmaRHI::IDescriptor* gizmoDescriptor;
	EnigmaRHI::IBuffer* gizmoBuffer;

	EnigmaRHI::IBuffer* quadVBO;
	EnigmaRHI::IBuffer* quadEBO;
	EnigmaRHI::IBuffer* cubeVBO;
	EnigmaRHI::IBuffer* cubeEBO;
	EnigmaRHI::IBuffer* circleVBO;
	EnigmaRHI::IBuffer* circleEBO;
	EnigmaRHI::IBuffer* lineVBO;
	EnigmaRHI::IBuffer* frustrumVBO;
	EnigmaRHI::IBuffer* frustrumEBO;

	int cubeIndexCount;
	int circleIndexCount;
	int frustrumIndexCount;

	bool renderGizmos = true;
	bool renderAABB = false;
};
#pragma once

#include "IPipeline.h"
#include "IRenderInterface.h"

class RenderPipelines
{
public:

	void Create(EnigmaRHI::IRenderInterface* rhi);
	void Destroy(EnigmaRHI::IRenderInterface* rhi);

	EnigmaRHI::IPipeline* GetGeometryPipeline() const { return geometryPipeline; };
	EnigmaRHI::IPipeline* GetLightningPipeline() const { return lightingPipeline; };
	EnigmaRHI::IPipeline* GetFXAAPipeline() const { return fxaaPipeline; };
	EnigmaRHI::IPipeline* GetFrontTransparentPipeline() const { return transparentFrontPipeline; };
	EnigmaRHI::IPipeline* GetBackTransparentPipeline() const { return transparentBackPipeline; };
	EnigmaRHI::IPipeline* GetBloomPipeline() const { return finalBloomPipeline; };

	EnigmaRHI::IDescriptor* GetGeometryDescriptor() const { return geometryDescriptor; };
	EnigmaRHI::IDescriptor* GetLightningDescriptor() const { return lightingDescriptor; };

	EnigmaRHI::IVertexInput* GetMeshVertexMode() const { return meshVertexMode; };

private:

	void CreatePipelines(EnigmaRHI::IRenderInterface* rhi);
	void CreateGeometryDescriptor(EnigmaRHI::IRenderInterface* rhi);
	void CreateLightingDescriptor(EnigmaRHI::IRenderInterface* rhi);

	//Gemoetry Pass
	EnigmaRHI::IPipeline* geometryPipeline = nullptr;
	EnigmaRHI::IShaderModule* geometryVertShader = nullptr;
	EnigmaRHI::IShaderModule* geometryFragShader = nullptr;
	EnigmaRHI::IDescriptor* geometryDescriptor = nullptr;

	//Light pass
	EnigmaRHI::IPipeline* lightingPipeline = nullptr;
	EnigmaRHI::IShaderModule* lightingVertShader = nullptr;
	EnigmaRHI::IShaderModule* lightingFragShader = nullptr;
	EnigmaRHI::IDescriptor* lightingDescriptor = nullptr;

	//Transparent pass
	EnigmaRHI::IPipeline* transparentBackPipeline = nullptr;
	EnigmaRHI::IPipeline* transparentFrontPipeline = nullptr;
	EnigmaRHI::IShaderModule* forwardVertShader = nullptr;
	EnigmaRHI::IShaderModule* forwardFragShader = nullptr;

	//FXAA Pass
	EnigmaRHI::IPipeline* fxaaPipeline = nullptr;
	EnigmaRHI::IShaderModule* fxaaVertShader = nullptr;
	EnigmaRHI::IShaderModule* fxaaFragShader = nullptr;

	EnigmaRHI::IPipeline* finalBloomPipeline = nullptr;
	EnigmaRHI::IShaderModule* finalBloomVertShader = nullptr;
	EnigmaRHI::IShaderModule* finalBloomFragShader = nullptr;

	EnigmaRHI::IVertexInput* meshVertexMode = nullptr;
};
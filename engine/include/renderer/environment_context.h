#pragma once

#include "IRenderInterface.h"
#include "skybox.h"
#include "resources/model.h"
#include "resources/resource_manager.h"

class EnvironmentSystem;

class EnvironmentContext
{
public:

	void CreateContext(EnigmaRHI::IRenderInterface* rhi);
	EnigmaRHI::IPipeline* GetSkyboxPipeline() const { return skyboxPipeline; };
	void SetCurrentSkybox(Skybox* sky) { currentSkybox = sky; currentSkybox->SetModel(skyboxMesh); };
	void RenderSkybox(EnigmaRHI::ICommandBuffer& cmd) { currentSkybox->Render(cmd); };

private:

	void CreateEnvironmentShaders(EnigmaRHI::IRenderInterface* rhi);
	void LoadSkyboxMesh(EnigmaRHI::IRenderInterface* rhi);
	void CreateEnvironmentPipelines(EnigmaRHI::IRenderInterface* rhi);
	void CreateLutMap(EnigmaRHI::IRenderInterface* rhi, unsigned int textureSize);

	EnigmaRHI::IPipeline* equirectangleToCubemapPipeline = nullptr;
	EnigmaRHI::IPipeline* irradiancePipeline = nullptr;
	EnigmaRHI::IPipeline* prefilteringPipeline = nullptr;
	EnigmaRHI::IPipeline* skyboxPipeline = nullptr;

	EnigmaRHI::IShaderModule* skyboxVertShader;
	EnigmaRHI::IShaderModule* skyboxFragShader;

	EnigmaRHI::IShaderModule* cubemapVertShader;
	EnigmaRHI::IShaderModule* equirectangleToCubemapFragShader;
	EnigmaRHI::IShaderModule* irradianceFragShader;
	EnigmaRHI::IShaderModule* prefilteringFragShader;

	EnigmaRHI::IImage* brdfLutMap = nullptr;

	Skybox* currentSkybox = nullptr;
	Model* skyboxMesh = nullptr;

	EnigmaRHI::IVertexInput* skyboxVertexMode = nullptr;

	friend class EnvironmentSystem;
};
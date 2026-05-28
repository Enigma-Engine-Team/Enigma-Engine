#pragma once

#include "emath/emath.h"
#include "environment_context.h"
#include <unordered_map>

class RenderContext;

struct Enviro
{
	EnigmaRHI::IFramebuffer* captureFramebuffer = nullptr;

	EnigmaRHI::IImage* environmentCubemap = nullptr;
	EnigmaRHI::IImage* irradianceMap = nullptr;
	EnigmaRHI::IImage* prefilteredMap = nullptr;

	Skybox* sky;
};

class EnvironmentSystem
{
public:
    EnvironmentSystem();
	~EnvironmentSystem() = default;

	void Create(EnigmaRHI::IRenderInterface* rhi);
	void ComputeEnvironment(RenderContext* ctx, EnigmaRHI::ICommandBuffer& cmd, EnigmaRHI::IDescriptor* gemoetryDescriptor, EnigmaRHI::IDescriptor* lightningDescriptor, EnigmaRHI::IRenderPass* renderPass);
	void SetCurrentSkybox(std::string skyboxPath, EnigmaRHI::IRenderInterface* rhi);

	EnvironmentContext* envCtx = nullptr;

	EnigmaRHI::IImage* GetIrradianceMap() const { return currentEnvironment->irradianceMap; };
	EnigmaRHI::IImage* GetPrefilteredMapp() const { return currentEnvironment->prefilteredMap; };
	EnigmaRHI::IImage* GetBrdfMap() const { return envCtx->brdfLutMap; };
	EnigmaRHI::IImage* GetSkyboxTexture() const { return envCtx->currentSkybox->GetTexture(); };

	bool NeedToCompute() const { return !isEnvironmentComputed; };

private:

	void CaptureSkyboxFaces(EnigmaRHI::IPipeline* pipeline, EnigmaRHI::IImage* imageToFill, EnigmaRHI::IRenderPass* rp, EnigmaRHI::ICommandBuffer& cm, unsigned int mip = 0);
	EnigmaRHI::IImage* CreateIrradianceMap(EnigmaRHI::IRenderInterface* rhi, unsigned int textureSize);
	EnigmaRHI::IImage* CreatePrefilteredMap(EnigmaRHI::IRenderInterface* rhi, unsigned int textureSize);
	EnigmaRHI::IImage* CreateEnvironmentMap(EnigmaRHI::IRenderInterface* rhi, unsigned int textureSize);
	EnigmaRHI::IFramebuffer* CreateCaptureFramebuffer(EnigmaRHI::IRenderInterface* rhi, unsigned int framebufferTexturesSize);
    void LoadSkyboxMesh(EnigmaRHI::IRenderInterface* rhi);
	
	Enviro* FindEnvironment(std::string path);
	Enviro* currentEnvironment = nullptr;

	std::string cacheSkyboxPath = "";

	std::unordered_map<std::string, Enviro*> loadedEnvironment;

	bool isEnvironmentComputed = false;

	const Math::Matrix4x4 captureViews[6] =
	{
		Math::Matrix4x4::LookAt(Math::Vector3D(0.0f, 0.0f, 0.0f), Math::Vector3D(1.0f,  0.0f,  0.0f), Math::Vector3D(0.0f, -1.0f,  0.0f)),
		Math::Matrix4x4::LookAt(Math::Vector3D(0.0f, 0.0f, 0.0f), Math::Vector3D(-1.0f,  0.0f,  0.0f), Math::Vector3D(0.0f, -1.0f,  0.0f)),
		Math::Matrix4x4::LookAt(Math::Vector3D(0.0f, 0.0f, 0.0f), Math::Vector3D(0.0f,  1.0f,  0.0f), Math::Vector3D(0.0f,  0.0f,  1.0f)),
		Math::Matrix4x4::LookAt(Math::Vector3D(0.0f, 0.0f, 0.0f), Math::Vector3D(0.0f, -1.0f,  0.0f), Math::Vector3D(0.0f,  0.0f, -1.0f)),
		Math::Matrix4x4::LookAt(Math::Vector3D(0.0f, 0.0f, 0.0f), Math::Vector3D(0.0f,  0.0f,  1.0f), Math::Vector3D(0.0f, -1.0f,  0.0f)),
		Math::Matrix4x4::LookAt(Math::Vector3D(0.0f, 0.0f, 0.0f), Math::Vector3D(0.0f,  0.0f, -1.0f), Math::Vector3D(0.0f, -1.0f,  0.0f))
	};

	Math::Matrix4x4 captureProjection = Math::Matrix4x4::Perspective(90.0f, 1.0f, 0.1f, 10.0f);
};
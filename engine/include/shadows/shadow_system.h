#pragma once

#include "IRenderInterface.h"
#include "scenes/scene.h"
#include "ishadow_map.h"
#include "renderer/light_manager.h"

struct CameraProxy;

class ShadowSystem
{
public:

	void Create(EnigmaRHI::IRenderInterface* rhi);
	void Destroy(EnigmaRHI::IRenderInterface* rhi);
	void BindForLightning(EnigmaRHI::IPipeline* lightningPipeline, EnigmaRHI::IDescriptor* lightningDescriptor, const std::vector<Light*>& lights);

	void RenderShadows(Scene* currentScene, LightManager* lightMng,
		EnigmaRHI::ICommandBuffer& cmd, EnigmaRHI::IRenderPass* renderPass,
		EnigmaRHI::IRenderInterface* rhi, EnigmaRHI::IDevice* device,
		EnigmaRHI::IVertexInput* meshVertexMode,
		EnigmaRHI::IDescriptor* lightningDescriptor,
		const CameraProxy& camera);

	IShadowMap* CreateShadowMap(ELightType lightType, EnigmaRHI::IRenderInterface* rhi);

private:
	
	void RenderScene(Scene* scene, EnigmaRHI::ICommandBuffer& cmd,
		EnigmaRHI::IRenderInterface* rhi, IShadowMap* sm, Light* light);

	EnigmaRHI::IShaderModule* directionalShadowFrag = nullptr;
	EnigmaRHI::IShaderModule* directionalShadowVert = nullptr;
	EnigmaRHI::IShaderModule* directionalShadowGeom = nullptr;
	EnigmaRHI::IPipeline* directionalShadowPipeline = nullptr;

	EnigmaRHI::IShaderModule* spotShadowVert = nullptr;
	EnigmaRHI::IPipeline* spotShadowPipeline = nullptr;

	EnigmaRHI::IShaderModule* pointShadowVert = nullptr;
	EnigmaRHI::IShaderModule* pointShadowFrag = nullptr;
	EnigmaRHI::IShaderModule* pointShadowGeom = nullptr;
	EnigmaRHI::IPipeline* pointShadowPipeline = nullptr;

	std::vector<IShadowMap*> shadowMaps;

	std::unordered_map<uint32_t, IShadowMap*> shadowMapCache;
};
#include "renderer/render_resources.h"
#include "renderer/renderer.h"
#include "engine/engine.h"

void RenderResources::Create(EnigmaRHI::IRenderInterface* rhi)
{
	LightManager::GetInstance().CreateStorageBuffer(rhi);

	environment = new EnvironmentSystem();
	environment->envCtx->CreateContext(rhi);

	grid = new Grid();
	grid->Create(rhi);

	if(!Engine::IsInGameBuild())
		gizmoRenderer.Create(rhi);

	shadowSystem = new ShadowSystem();
	shadowSystem->Create(rhi);
}

void RenderResources::Destroy()
{
}
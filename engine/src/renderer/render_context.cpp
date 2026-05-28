#include "renderer/render_context.h"

void RenderContext::Create()
{
	rhi = EnigmaRHI::IRenderInterface::CreateRenderInterface(EnigmaRHI::ERenderAPI::OGL);
	instance = rhi->InstantiateInstance();
	surface = rhi->InstantiateSurface();
	device = rhi->InstantiateDevice();
	commandPool = rhi->InstantiateCommandPool();
	swapChain = rhi->InstantiateSwapChain();

	instance->Create();
	surface->Create(1920, 1080);
	device->Create();
	commandPool->Create();
	swapChain->Create(surface);
}

void RenderContext::Destroy()
{
	swapChain->Destroy();
	commandPool->Destroy();
	device->Destroy();
	surface->Destroy();
	instance->Destroy();

	rhi->DeleteSwapChain(swapChain);
	rhi->DeleteCommandPool(commandPool);
	rhi->DeleteDevice(device);
	rhi->DeleteSurface(surface);
	rhi->DeleteInstance(instance);
}


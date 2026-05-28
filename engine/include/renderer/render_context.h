#pragma once

#include "IRenderInterface.h"

class Engine;

class RenderContext
{
public:

    void Create();
    void Destroy();

    EnigmaRHI::IDevice* GetDevice() { return device; }
    EnigmaRHI::IRenderInterface* GetRHI() { return rhi; }

private:

    EnigmaRHI::IRenderInterface* rhi;
    EnigmaRHI::IDevice* device;
    EnigmaRHI::IInstance* instance;
    EnigmaRHI::ISurface* surface;
    EnigmaRHI::ISwapChain* swapChain;
    EnigmaRHI::ICommandPool* commandPool;

    friend class Renderer;
    friend class GameRenderInstance;
    friend class EditorRenderInstance;
    friend class Engine;
};
#pragma once
#include "render_instance.h"

class GameRenderInstance : public RenderInstance
{
public:
    void Setup(RenderContext* ctx, RenderPipelines* pipelines,
        RenderResources* resources, EnigmaRHI::IRenderPass* renderPass) override;
    void Render(Scene* scene, EnigmaRHI::IDevice* device, EnigmaRHI::IRenderPass* renderPass, EnigmaRHI::ICommandBuffer& cmd) override;
    void Destroy() override;

    unsigned int GetOutputTextureID() const override;
    void SetResolution(float w, float h) override { resolution = { w, h }; }

private:
    
    void RenderWidgets(Scene* scene, EnigmaRHI::ICommandBuffer& cmd);
};
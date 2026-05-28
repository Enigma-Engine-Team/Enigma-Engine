#pragma once
#include "render_instance.h"

class EditorRenderInstance : public RenderInstance
{
public:
    void Setup(RenderContext* ctx, RenderPipelines* pipelines,
        RenderResources* resources, EnigmaRHI::IRenderPass* renderPass) override;
    void Render(Scene* scene, EnigmaRHI::IDevice* device, EnigmaRHI::IRenderPass* renderPass, EnigmaRHI::ICommandBuffer& cmd) override;
    void Destroy() override;
    unsigned int GetOutputTextureID() const override;
    void SetResolution(float w, float h) override { resolution = { w, h }; }
    void SetSelectedGameObject(GameObject* go) { selectedGameObject = go; }

private:

    GameObject* selectedGameObject = nullptr;

    void RegisterGizmoCommands(Scene* scene);
    void RenderGizmos(Scene* scene, EnigmaRHI::IDevice* device, EnigmaRHI::ICommandBuffer& cmd);
    void RenderGrid(EnigmaRHI::ICommandBuffer& cmd);
};
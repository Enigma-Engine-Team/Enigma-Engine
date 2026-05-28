#pragma once
#include "ishadow_map.h"

class ENIGMA_API SpotMap : public IShadowMap
{
public:
    void Create(EnigmaRHI::IRenderInterface* rhi) override;
    void Destroy(EnigmaRHI::IRenderInterface* rhi) override;
    void UpdateLightSpaceMatrix(const CameraProxy& camera, Light* light) override;
    EShadowMapType GetType() const override { return EShadowMapType::Spot; }

    float GetFarPlane() const { return farPlane; }

private:
    void CreateDepthMap(EnigmaRHI::IRenderInterface* rhi);
    void CreateMatricesUBO(EnigmaRHI::IRenderInterface* rhi);

    float nearPlane = 0.1f;
    float farPlane = 100.0f;
};
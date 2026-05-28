#pragma once
#include "ishadow_map.h"
#include <array>

class ENIGMA_API PointMap : public IShadowMap
{
public:
    void Create(EnigmaRHI::IRenderInterface* rhi) override;
    void Destroy(EnigmaRHI::IRenderInterface* rhi) override;
    void UpdateLightSpaceMatrix(const CameraProxy& camera, Light* light) override;
    EShadowMapType GetType() const override { return EShadowMapType::Point; }

    float GetFarPlane()  const { return farPlane; }
    float GetNearPlane() const { return nearPlane; }

private:
    void CreateDepthCubeMap(EnigmaRHI::IRenderInterface* rhi);
    void CreateMatricesUBO(EnigmaRHI::IRenderInterface* rhi);

    std::array<Math::Matrix4x4, 6> BuildFaceMatrices(const Math::Vector3D& lightPos) const;

    float nearPlane = 0.1f;
    float farPlane = 100.0f;
};
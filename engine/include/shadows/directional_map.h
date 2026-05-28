#pragma once

#include "ishadow_map.h"

class ENIGMA_API DirectionalMap : public IShadowMap
{
public:

	void Create(EnigmaRHI::IRenderInterface* rhi) override;
	void Destroy(EnigmaRHI::IRenderInterface* rhi) override;
	void UpdateLightSpaceMatrix(const CameraProxy& camera, Light* light) override;

	EnigmaRHI::IImage* GetDepthMapArray() const { return depthMap; }
	const std::vector<float>& GetCascadeLevels() const { return shadowCascadeLevels; }
	int GetCascadeCount() const { return shadowCascadeLevels.size() + 1; }
	EShadowMapType GetType() const override { return EShadowMapType::Directional; }
	float GetShadowDistance() const { return shadowDistance; };

private:

	void CreateDepthMaps(EnigmaRHI::IRenderInterface* rhi);
	void CreateMatricesUBO(EnigmaRHI::IRenderInterface* rhi);
	std::vector<Math::Vector4D> GetFrustumCornersWorldSpace(const Math::Matrix4x4& projview);
	std::vector<Math::Vector4D> GetFrustumCornersWorldSpace(Math::Matrix4x4 proj, Math::Matrix4x4 view);

	Math::Matrix4x4 GetLightSpaceMatrix(const Math::Vector3D& lightDir, const float nearPlane, const float farPlane, float FOV, Math::Matrix4x4 view, float aspectRatio, Math::Vector3D camPos);

	std::vector<Math::Matrix4x4> GetLightSpaceMatrices(const CameraProxy& camera, Light* light);

	std::vector<float> shadowCascadeLevels;
	const float shadowDistance = 1000.f;
};
#pragma once

#include "components/icomponent.h"
#include "emath/emath.h"
#include "IRenderInterface.h"
#include "IBuffer.h"

enum class ELightType
{
	DIRECTIONAL,
	SPOT,
	POINT
};

struct LightGPUData
{
	Math::Vector3D position{ 0.f, 0.f, 0.f };
	int lightType = 0;

	Math::Vector3D color = { 1.f, 1.f, 1.f };
	float intensity = 1.f;

	Math::Vector3D direction = { 0.f, -1.f, 0.f };
	float range = 5.f;

	float innerCutOff = 21.8f;
	float outerCutOff = 30.f;

	int padding0 = 0;
	int padding1 = 0;

	Math::Matrix4x4 lightSpaceMatrix = Math::Matrix4x4::Identity;
};

class ENIGMA_API Light : public IComponent
{
public:
	Light() = default;

	void Create() override;
	void Destroy() override;
	void Update();
	void OnDraw(Math::Vector3D camPos = Math::Vector3D::Zero) override;

	int GetLightType() const { return lightData.lightType; }
	void SetLightType(int type) { lightData.lightType = type; }
	void SetLightTypeEnum(ELightType type) { lightData.lightType = static_cast<int>(type); }

	Math::Vector3D GetColor() const { return lightData.color; }
	void SetColor(Math::Vector3D color) { lightData.color = color; }

	float GetIntensity() const { return lightData.intensity; }
	void SetIntensity(float intensity) { lightData.intensity = intensity; }

	float GetRange() const { return lightData.range; }
	void SetRange(float range) { lightData.range = range; }

	float GetInnerCutOff() const { return lightData.innerCutOff; }
	void SetInnerCutOff(float cutOff) { lightData.innerCutOff = cutOff; }

	float GetOuterCutOff() const { return lightData.outerCutOff; }
	void SetOuterCutOff(float cutOff) { lightData.outerCutOff = cutOff; }

	LightGPUData GetLightData() const { return lightData; }
	void SetLightData(const LightGPUData& data) { lightData = data; }

	Math::Vector3D GetPosition() const { return lightData.position; }

	Math::Vector3D GetDirection() const { return lightData.direction; };

	bool CastShadows() const { return castShadows; };
	void SetCastShadows(bool cast) { castShadows = cast; }

	void SetLightSpaceMatrix(Math::Matrix4x4 newMatrix) { lightData.lightSpaceMatrix = newMatrix; };

	void SetBillboardScale(float billboardScale) { fallOfScaleBillboard = billboardScale; }
	void SetBillboardAlpha(float billboardAlpha) { fallOfAlpha = billboardAlpha; }

	float GetBillboardScale() const { return fallOfScaleBillboard; }
	float GetBillboardAlpha() const { return fallOfAlpha; }

private:
	RTTR_REGISTRATION_FRIEND
	LightGPUData lightData;
	RTTR_ENABLE(IComponent)

	bool castShadows = true;

	float fallOfScaleBillboard;
	float fallOfAlpha;
};
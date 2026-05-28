#include "components/light.h"
#include "gameobject/gameobject.h"
#include "renderer/light_manager.h"
#include "renderer/gizmo.h"
#include "engine/engine.h"

RTTR_REGISTRATION
{
	rttr::registration::class_<Light>("Light")
		.constructor<>()(rttr::policy::ctor::as_raw_ptr)
		.property("Light Type", &Light::GetLightType, &Light::SetLightType)
		(
			rttr::metadata(UI_DATA_METADATA_KEY, UIData{EUIType::COMBO, 0.f, 0.f, "%.0f", "General", {"Directional", "Spot", "Point"}})
		)
		.property("Inner CutOff", &Light::GetInnerCutOff, &Light::SetInnerCutOff)
		(
			rttr::metadata(UI_DATA_METADATA_KEY, UIData{EUIType::SLIDER_FLOAT, 0.f, 179.f, "%.1f", "Shape"})
		)
		.property("Outer CutOff", &Light::GetOuterCutOff, &Light::SetOuterCutOff)
		(
			rttr::metadata(UI_DATA_METADATA_KEY, UIData{ EUIType::SLIDER_FLOAT, 0.f, 179.f, "%.1f", "Shape"})
		)

		.property("Color", &Light::GetColor, &Light::SetColor)
		(
			rttr::metadata(UI_DATA_METADATA_KEY, UIData{EUIType::COLOR3, 0.f, 1.f, "%.0f", "Emission"})
		)

		.property("Intensity", &Light::GetIntensity, &Light::SetIntensity)
		(
			rttr::metadata(UI_DATA_METADATA_KEY, UIData{EUIType::DRAG_FLOAT, 0.f, 1000.f, "%.2f", "Emission"})
		)
		.property("Range", &Light::GetRange, &Light::SetRange)
		(
			rttr::metadata(UI_DATA_METADATA_KEY, UIData{EUIType::DRAG_FLOAT, 0.f, 1000.f, "%.2f", "Emission"})
		);

	rttr::registration::class_<LightGPUData>("LightGPUData")
		.property("LightData", &Light::lightData);
}

void Light::Create()
{
	SetLightData(LightGPUData());
	LightManager::GetInstance().AddLight(this);
}

void Light::Destroy()
{
	LightManager::GetInstance().RemoveLight(this);
}

void Light::Update()
{
	lightData.position = gameObject->transform.worldPosition;
	lightData.direction = gameObject->transform.worldRotation.Conjugate().Forward();
}

void Light::OnDraw(Math::Vector3D camPos)
{
	Gizmos::color = Math::Vector4D(lightData.color, 1.f);

	switch (lightData.lightType)
	{
	case static_cast<int>(ELightType::POINT):
	{
		Gizmos::DrawSphere(lightData.position, lightData.range);
		return;
	}
	case static_cast<int>(ELightType::SPOT):
	{
		Math::Vector3D targetPos = lightData.position + lightData.direction.Normalized() * lightData.range;

		float outerCutOffRad = Math::MyToRadians(lightData.outerCutOff * 0.5f);
		float innerCutOffRad = Math::MyToRadians(lightData.innerCutOff * 0.5f);
		float outerRadius = tan(outerCutOffRad) * lightData.range;
		float innerRadius = tan(innerCutOffRad) * lightData.range;

		Gizmos::color = Math::Vector4D(lightData.color, 1.f);
		Gizmos::DrawCone(lightData.position, targetPos, outerRadius);

		Gizmos::color = Math::Vector4D(lightData.color, 0.3f);
		Gizmos::DrawCone(lightData.position, targetPos, innerRadius);

		return;
	}
	case static_cast<int>(ELightType::DIRECTIONAL):
	{
		float radius = std::clamp(lightData.position.Distance(camPos), 0.1f, 10.f) * 0.05f;

		Math::Vector3D targetPos = lightData.position + lightData.direction.Normalized();

		Math::Vector3D dir = (targetPos - lightData.position).Normalized();

		Math::Vector3D worldUp = std::fabs(dir.DotProduct(Math::Vector3D(0.f, 1.f, 0.f))) > 0.99f ? Math::Vector3D(0, 0, 1) : Math::Vector3D(0, 1, 0);

		Math::Vector3D right = (worldUp.CrossProduct(dir)).Normalized();
		Math::Vector3D up = (dir.CrossProduct(right)).Normalized();

		Math::Vector3D offsets[] = {
			right,
			-right,
			up,
			-up,
			(right + up).Normalized(),
			(right - up).Normalized(),
			(-right + up).Normalized(),
			(-right - up).Normalized()
		};

		Gizmos::DrawCircle(lightData.position, gameObject->transform.worldRotation.Conjugate(), radius);

		for (int i = 0; i < 8; ++i)
		{
			float lineLength = 3.f * radius;

			Math::Vector3D base = lightData.position + offsets[i] * radius;
			Math::Vector3D tip = base + (dir * lineLength);
			Gizmos::DrawLine(tip, base);
		}
	}
	}
}

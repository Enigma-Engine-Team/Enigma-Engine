#include "renderer/light_manager.h"
#include "debug/log.h"
#include "renderer/gizmo.h"
#include "engine/engine.h"
#include "scenes/scene_manager.h"

constexpr auto MAX_LIGHT = 100;

LightManager& LightManager::GetInstance()
{
	static LightManager lightManager;
	return lightManager;
}

void LightManager::RemoveLight(Light* light)
{
    auto it = std::find(lightsInScene.begin(), lightsInScene.end(), light);
    if (it != lightsInScene.end())
    {
        lightsInScene.erase(it);
    }
}

void LightManager::CreateStorageBuffer(EnigmaRHI::IRenderInterface* rhi)
{
	lightStorageBuffer = rhi->InstantiateBuffer();
	size_t bufferSize = sizeof(LightGPUData) * MAX_LIGHT;
	lightStorageBuffer->Create(bufferSize, EnigmaRHI::EBufferTarget::SHADER_STORAGE_BUFFER, nullptr, EnigmaRHI::EBufferUsage::STATIC_DRAW);
	lightStorageBuffer->CreateDescriptorBufferInfo();
	lightStorageBuffer->bufferInfo.range = bufferSize;
}

void LightManager::UpdateStorageBuffer()
{
    LightBufferHeader header = { (int)lightsInScene.size(), {0, 0, 0} };

    bufferDatas.resize(lightsInScene.size());
    for (int i = 0; i < lightsInScene.size(); ++i) 
    {
        lightsInScene[i]->Update();
        bufferDatas[i] = lightsInScene[i]->GetLightData();

		if(!Engine::IsInGameBuild())
			DrawLightBillboards(lightsInScene[i]);
    }

    lightStorageBuffer->CopyData(&header, sizeof(LightBufferHeader));

    if (!bufferDatas.empty())
    {
        lightStorageBuffer->CopyData(
            bufferDatas.data(),
            bufferDatas.size() * sizeof(LightGPUData),
            sizeof(LightBufferHeader)
        );
    }
}

void LightManager::DrawLightBillboards(Light* light)
{
	Math::Vector3D lightPos = light->GetPosition();

	EngineCamera* cam = SceneManager::GetInstance().GetCurrentScene()->GetEngineCam();
	const float minScaleFactor = 0.2f;
	const float maxScaleFactor = 5.f;
	float dist = (lightPos - cam->GetPosition()).Magnitude();
	float fallOfScaleFactor = std::clamp(dist / 10.f, minScaleFactor, maxScaleFactor);
	float fallOfAlpha = static_cast<float>(std::clamp(dist / 2.0 - 1.0, 0.0, 1.0));

	light->SetBillboardAlpha(fallOfAlpha);
	light->SetBillboardScale(fallOfScaleFactor);

	switch (light->GetLightType())
	{
	case static_cast<int>(ELightType::POINT):
	{
		Gizmos::DrawIcon(lightPos, fallOfAlpha, fallOfScaleFactor, Engine::GetEditorTexture("point_icon.png"));
		return;
	}
	case static_cast<int>(ELightType::SPOT):
	{
		Gizmos::DrawIcon(lightPos, fallOfAlpha, fallOfScaleFactor, Engine::GetEditorTexture("spot_icon.png"));
		return;
	}
	case static_cast<int>(ELightType::DIRECTIONAL):
	{
		Gizmos::DrawIcon(lightPos, fallOfAlpha, fallOfScaleFactor, Engine::GetEditorTexture("directional_icon.png"));
		return;
	}
	}
}

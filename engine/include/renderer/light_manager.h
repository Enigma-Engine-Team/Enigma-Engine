#pragma once

#include "components/light.h"


struct LightBufferHeader 
{
	int count;
	int padding[3];
};

class ENIGMA_API LightManager
{
public:
	LightManager() = default;

	static LightManager& GetInstance();
	void AddLight(Light* light) { lightsInScene.push_back(light); }
	void RemoveLight(Light* light);
	void ClearLights() { lightsInScene.clear(); bufferDatas.clear(); }

	std::vector<Light*> GetLightsInScene() { return lightsInScene; };
	Light* GetLight(int index) { return lightsInScene[index]; };

	void CreateStorageBuffer(EnigmaRHI::IRenderInterface* rhi);
	void UpdateStorageBuffer();

	EnigmaRHI::IBuffer* GetLightStorageBuffer() const { return lightStorageBuffer; }

private: 

	void DrawLightBillboards(Light* light);

	EnigmaRHI::IBuffer* lightStorageBuffer = nullptr;
	std::vector<Light*> lightsInScene;
	std::vector<LightGPUData> bufferDatas;
};
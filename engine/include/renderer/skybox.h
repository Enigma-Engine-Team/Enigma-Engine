#pragma once

#include "IImage.h"
#include "IRenderInterface.h"
#include "resources/model.h"

class Skybox
{
public:
	Skybox() = default;
	~Skybox() = default;
	void Create(EnigmaRHI::IRenderInterface* rhi, std::string path);
	void SetModel(Model* mesh) { skyboxMesh = mesh; }
	void Render(EnigmaRHI::ICommandBuffer& cmd);
	EnigmaRHI::IImage* GetTexture() const { return HDRTexture; }
	bool IsInitialized() const { return isInitialized; };

private:

	EnigmaRHI::IImage* HDRTexture;
	Model* skyboxMesh;
	bool isInitialized = false;
};
#pragma once

#include "IRenderInterface.h"

class Grid
{
public:
	
	Grid() = default;
	~Grid() = default;
	
	void Create(EnigmaRHI::IRenderInterface* rhi);
	void Render(EnigmaRHI::ICommandBuffer& cmd) const;
	void Destroy(EnigmaRHI::IRenderInterface* rhi);

	EnigmaRHI::IPipeline* GetPipeline() { return gridPipeline; };

private: 

	EnigmaRHI::IVertexInput* gridMode = nullptr;
	EnigmaRHI::IPipeline* gridPipeline = nullptr;
	EnigmaRHI::IShaderModule* gridVert = nullptr;
	EnigmaRHI::IShaderModule* gridFrag = nullptr;
};
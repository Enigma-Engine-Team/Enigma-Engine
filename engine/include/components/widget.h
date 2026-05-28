#pragma once

#include "icomponent.h"
#include "ICommandBuffer.h"

class ENIGMA_API Widget : public IComponent
{
public:
	Widget() = default;
	~Widget() = default;

	void Create() override;

	virtual void Render(Math::Matrix4x4 camVP, Math::Matrix4x4 camViewMatrix, EnigmaRHI::ICommandBuffer& cmd, EnigmaRHI::IDevice* device) = 0;

private:
	RTTR_REGISTRATION_FRIEND
	RTTR_ENABLE(IComponent)
};
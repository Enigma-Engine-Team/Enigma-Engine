#pragma once

#include <filesystem>
#include <string>
#include "IRenderInterface.h"
#include "utilities/macro.h"
#include <rttr/registration_friend.h>

class ENIGMA_API IResource
{
public:
	IResource() = default;
	virtual ~IResource() = default;

	virtual void Load(const std::filesystem::path& filepath, EnigmaRHI::IRenderInterface* rhi) = 0;
	std::string GetName();
protected:
	RTTR_REGISTRATION_FRIEND
	std::string filename = "empty";
};
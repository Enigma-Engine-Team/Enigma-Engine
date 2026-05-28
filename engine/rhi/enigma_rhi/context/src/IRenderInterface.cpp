#include "IRenderInterface.h"
#include "GLRenderInterface.h"

EnigmaRHI::ERenderAPI EnigmaRHI::IRenderInterface::currentGraphicsAPI = EnigmaRHI::ERenderAPI::OGL;

EnigmaRHI::IRenderInterface* EnigmaRHI::IRenderInterface::CreateRenderInterface(ERenderAPI api)
{
	switch (api)
	{
		case ERenderAPI::OGL:
			return new GLRenderInterface();
			break;

		default:
			throw std::runtime_error("Unsupported API");
			break;
	}

	currentGraphicsAPI = api;
}
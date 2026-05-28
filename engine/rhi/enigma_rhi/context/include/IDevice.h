#pragma once

#include "ISurface.h"
#include "IFormat.h"
#include "IGraphicsPipeline.h"
#include <iostream>

namespace EnigmaRHI
{
	class GLDevice;
	class IPipeline;

	class IDevice
	{
	public:

		virtual ~IDevice() = default;
		virtual void Create() = 0;
		virtual void Destroy() = 0;

		virtual void ApplyPipelineCache(IPipeline* pipeline) = 0;

		GraphicsPipeline pipelineCache;

		bool pipelineCacheInitialized = false;

		virtual GLDevice& API_GL() { throw std::runtime_error("Bad API Call: object is not a GLDevice"); }
	};
}

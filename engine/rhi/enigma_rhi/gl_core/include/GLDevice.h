#pragma once

#include "IDevice.h"

namespace EnigmaRHI
{
	class GLDevice : public IDevice
	{
	public:
		GLDevice() = default;

		void Create() override;
		void Destroy() override {};

		void ApplyPipelineCache(IPipeline* pipeline) override;

		GLDevice& API_GL() override { return (*this); }
	};
}
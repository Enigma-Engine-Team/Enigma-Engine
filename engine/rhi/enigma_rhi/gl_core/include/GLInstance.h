#pragma once

#include <stdlib.h>
#include <vector>
#include <iostream>

#include "IInstance.h"

namespace EnigmaRHI
{
	class GLInstance : public IInstance
	{
	public:
		GLInstance() = default;

		void Create() override {};
		void Destroy() override {};

		GLInstance& API_GL() override { return (*this); }
	};
}
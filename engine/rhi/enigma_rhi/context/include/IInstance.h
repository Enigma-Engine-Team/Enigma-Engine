#pragma once

#include <iostream>

namespace EnigmaRHI
{
	class GLInstance;

	class IInstance
	{
	public:
		virtual ~IInstance() = default;
		virtual void Create() = 0;
		virtual void Destroy() = 0;

		virtual GLInstance& API_GL() { throw std::runtime_error("Bad API Call: object is not a GLInstance"); }
	};
}
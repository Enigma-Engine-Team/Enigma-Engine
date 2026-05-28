#pragma once
#include <glad/glad.h>
#include "GLDevice.h"
#include "IShaderModule.h"
#include <iostream>
#include <stdlib.h>
#include <filesystem>
#include <fstream>
#include <sstream>

namespace EnigmaRHI
{
	class GLShaderModule : public IShaderModule
	{
	public:

		GLShaderModule() = default;
		void Create(const std::string& filename, EnigmaRHI::EShaderType shaderType) override;

		GLShaderModule& API_GL() override { return (*this); }


	public:

		unsigned int GetModule() const { return module; };

	private:

		unsigned int module = 0;

		std::string ReadShader(const std::string& filename);
		void CreateShaderModule(const char* shaderCode, EShaderType shaderType);
		void CheckCompileErrors(unsigned int module, std::string type);
	};
}
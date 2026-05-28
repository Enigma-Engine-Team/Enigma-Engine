#include "../include/GLPipeline.h"
#include "../include/GLUtilities.h"

void EnigmaRHI::GLPipeline::Create(const GraphicsPipeline& gp, IShaderModule* vertShader, IShaderModule* fragShader, IShaderModule* geomShader)
{
	graphicsPipeline = gp;

	if (CreateProgram(vertShader, fragShader, geomShader))
	{
		glDeleteShader(vertShader->API_GL().GetModule());
		glDeleteShader(fragShader->API_GL().GetModule());

		if (geomShader)
			glDeleteShader(geomShader->API_GL().GetModule());
	}
}

bool EnigmaRHI::GLPipeline::CreateProgram(IShaderModule* vertShader, IShaderModule* fragShader, IShaderModule* geomShader)
{
	programID = glCreateProgram();

	if (IsIDNull())
		return false;

	glAttachShader(programID, vertShader->API_GL().GetModule());
	glAttachShader(programID, fragShader->API_GL().GetModule());

	if (geomShader != nullptr)
		glAttachShader(programID, geomShader->API_GL().GetModule());

	glLinkProgram(programID);

	int success;
	char infoLog[1024];
	glGetProgramiv(programID, GL_LINK_STATUS, &success);
	if (!success) 
	{
		glGetProgramInfoLog(programID, 1024, NULL, infoLog);
		printf(
			"ERREUR::PROGRAM_LINKING::PROGRAM \n%s \n-------------------- \n",
			infoLog);
		return false;
	}
	return true;
}

bool EnigmaRHI::GLPipeline::IsIDNull() const
{
	if (programID == 0)
	{
		printf("ERREUR::SHADER::ID_NULL");
			return true;
	}
	return false;
}

void EnigmaRHI::GLPipeline::ApplyTotalGraphicsPipeline()
{
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	if (graphicsPipeline.depthTestEnable)
	{
		glEnable(GL_DEPTH_TEST);
		glDepthMask(graphicsPipeline.depthWriteEnable);
		glDepthFunc(GLUtilities::DepthCompareOpToGL(graphicsPipeline.compareMode));
	}
	else
	{
		glDisable(GL_DEPTH_TEST);
		glDepthMask(false);
	}
	
	if (graphicsPipeline.cullMode != ECullMode::DISABLED)
	{
		glEnable(GL_CULL_FACE);
		glCullFace(GLUtilities::CullModeToGL(graphicsPipeline.cullMode));
	}
	else
		glDisable(GL_CULL_FACE);

	if (graphicsPipeline.blendEnable)
	{
		glEnable(GL_BLEND);
		glBlendFunc(GLUtilities::BlendFactorToGL(graphicsPipeline.sourceFactor), GLUtilities::BlendFactorToGL(graphicsPipeline.destFactor));
		glBlendEquation(GLUtilities::BlendOpToGL(graphicsPipeline.combinationMode));
	}
	else
		glDisable(GL_BLEND);
	
	glPolygonMode(GL_FRONT_AND_BACK, GLUtilities::PolygonModeToGL(graphicsPipeline.polygonMode));
	glFrontFace(GLUtilities::FrontFaceModeToGL(graphicsPipeline.frontFaceMode));
}

void EnigmaRHI::GLPipeline::Destroy()
{
	if (programID != 0)
	{
		glDeleteProgram(programID);
		programID = 0;
	}
}

void EnigmaRHI::GLPipeline::ApplyGraphicsPipeline(IDevice* device)
{
	device->ApplyPipelineCache(this);
}

void EnigmaRHI::GLPipeline::SendToGPU(const char* name, int value) const
{
	int location = glGetUniformLocation(programID, name);
	glUniform1i(location, value);
}

void EnigmaRHI::GLPipeline::SendToGPU(const char* name, unsigned int lenght, int* value) const
{
	int location = glGetUniformLocation(programID, name);
	glUniform1iv(location, lenght, value);
}

void EnigmaRHI::GLPipeline::SendToGPU(const char* name, float value) const
{
	int location = glGetUniformLocation(programID, name);
	glUniform1f(location, value);
}

void EnigmaRHI::GLPipeline::SendToGPU(const char* name, float x, float y) const
{
	int location = glGetUniformLocation(programID, name);
	glUniform2f(location, x, y);
}

void EnigmaRHI::GLPipeline::SendToGPU(const char* name, float x, float y, float z) const
{
	int location = glGetUniformLocation(programID, name);
	glUniform3f(location, x, y, z);
}

void EnigmaRHI::GLPipeline::SendToGPU(const char* name, float x, float y, float z, float w) const
{
	int location = glGetUniformLocation(programID, name);
	glUniform4f(location, x, y, z, w);
}

void EnigmaRHI::GLPipeline::SendToGPU(const char* name, const float matrix[16]) const
{
	int location = glGetUniformLocation(programID, name);
	glUniformMatrix4fv(location, 1, GL_FALSE, &matrix[0]);
}


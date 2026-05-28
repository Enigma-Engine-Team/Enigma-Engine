#include "GLRenderPass.h"
#include "GLUtilities.h"

void EnigmaRHI::GLRenderPass::Create()
{
}

void EnigmaRHI::GLRenderPass::Destroy()
{}

void EnigmaRHI::GLRenderPass::SetViewport(int x, int y, int width, int height)
{
    glViewport(x, y, width, height);
}

void EnigmaRHI::GLRenderPass::ClearColor(float r, float g, float b, float a)
{
    glClearColor(r, g, b, a);
}

void EnigmaRHI::GLRenderPass::ClearBuffer(unsigned int clearFlags)
{
	GLbitfield mask = GLUtilities::MaskToGL(static_cast<EMask>(clearFlags));

    if (mask != 0) 
        glClear(mask);
}

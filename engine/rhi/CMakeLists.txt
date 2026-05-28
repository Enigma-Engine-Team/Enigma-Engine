cmake_minimum_required(VERSION 3.25)
project(EnigmaRHI)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

SET(EnigmaRHI_LIB_SOURCE
	enigma_rhi/gl_core/src/GLDevice.cpp
	enigma_rhi/gl_core/src/GLImage.cpp
	enigma_rhi/gl_core/src/GLShaderModule.cpp
	enigma_rhi/gl_core/src/GLUtilities.cpp
	enigma_rhi/gl_core/src/GLPipeline.cpp
	enigma_rhi/gl_core/src/GLCommandBuffer.cpp
	enigma_rhi/gl_core/src/GLSwapChain.cpp
	enigma_rhi/gl_core/src/GLRenderPass.cpp
	enigma_rhi/gl_core/src/GLBuffer.cpp
	enigma_rhi/gl_core/src/GLDescriptor.cpp
	enigma_rhi/gl_core/src/GLCommandPool.cpp
	enigma_rhi/gl_core/src/GLVertexInput.cpp
	enigma_rhi/gl_core/src/GLFramebuffer.cpp

	enigma_rhi/context/src/IRenderInterface.cpp

	glad/src/glad.c
)

add_library(EnigmaRHI STATIC ${EnigmaRHI_LIB_SOURCE})

find_package(OpenGL REQUIRED)
target_link_libraries(EnigmaRHI PUBLIC OpenGL::GL)

target_include_directories(EnigmaRHI PUBLIC
	glad/include
	enigma_rhi/gl_core/include
	enigma_rhi/context/include
)
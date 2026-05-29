#include "renderer/render_pipelines.h"
#include "utilities/shader_common.h"

void RenderPipelines::Create(EnigmaRHI::IRenderInterface* rhi)
{
	CreatePipelines(rhi);
	CreateGeometryDescriptor(rhi);
	CreateLightingDescriptor(rhi);

	meshVertexMode = rhi->InstantiateVertexInput();
	meshVertexMode->Create();

	meshVertexMode->Bind();
	meshVertexMode->AddVertexAttribute(0, 3, EnigmaRHI::EDataType::FLOAT, false, 0);
	meshVertexMode->AddVertexAttribute(1, 3, EnigmaRHI::EDataType::FLOAT, false, 3 * sizeof(float));
	meshVertexMode->AddVertexAttribute(2, 2, EnigmaRHI::EDataType::FLOAT, false, 6 * sizeof(float));
	meshVertexMode->Unbind();
}

void RenderPipelines::CreatePipelines(EnigmaRHI::IRenderInterface* rhi)
{
	geometryVertShader = rhi->InstantiateShaderModule();
	geometryFragShader = rhi->InstantiateShaderModule();
	geometryVertShader->Create("shaders/gbuffer.vert", EnigmaRHI::EShaderType::VERTEX);
	geometryFragShader->Create("shaders/gbuffer.frag", EnigmaRHI::EShaderType::FRAGMENT);

	lightingVertShader = rhi->InstantiateShaderModule();
	lightingFragShader = rhi->InstantiateShaderModule();
	lightingVertShader->Create("shaders/full_screen_quad.vert", EnigmaRHI::EShaderType::VERTEX);
	lightingFragShader->Create("shaders/deferred_lighting.frag", EnigmaRHI::EShaderType::FRAGMENT);

	fxaaVertShader = rhi->InstantiateShaderModule();
	fxaaFragShader = rhi->InstantiateShaderModule();
	fxaaVertShader->Create("shaders/full_screen_quad.vert", EnigmaRHI::EShaderType::VERTEX);
	fxaaFragShader->Create("shaders/fxaa.frag", EnigmaRHI::EShaderType::FRAGMENT);

	forwardVertShader = rhi->InstantiateShaderModule();
	forwardFragShader = rhi->InstantiateShaderModule();
	forwardVertShader->Create("shaders/forward_pbr.vert", EnigmaRHI::EShaderType::VERTEX);
	forwardFragShader->Create("shaders/forward_pbr.frag", EnigmaRHI::EShaderType::FRAGMENT);

	finalBloomFragShader = rhi->InstantiateShaderModule();
	finalBloomVertShader = rhi->InstantiateShaderModule();
	finalBloomVertShader->Create("shaders/full_screen_quad.vert", EnigmaRHI::EShaderType::VERTEX);
	finalBloomFragShader->Create("shaders/bloom_pass.frag", EnigmaRHI::EShaderType::FRAGMENT);

	EnigmaRHI::GraphicsPipeline geometryGP
	{
		.cullMode = EnigmaRHI::ECullMode::BACK,
		.frontFaceMode = EnigmaRHI::EFrontFaceMode::COUNTER_CLOCK_WISE,
		.depthTestEnable = true,
		.compareMode = EnigmaRHI::EDepthCompareOp::LESS,
		.polygonMode = EnigmaRHI::EPolygonMode::FILL,
		.depthWriteEnable = true,
	};

	geometryPipeline = rhi->InstantiatePipeline();
	geometryPipeline->Create(geometryGP, geometryVertShader, geometryFragShader);

	EnigmaRHI::GraphicsPipeline lightingGP
	{
		.cullMode = EnigmaRHI::ECullMode::DISABLED,
		.frontFaceMode = EnigmaRHI::EFrontFaceMode::COUNTER_CLOCK_WISE,
		.depthTestEnable = true,
		.compareMode = EnigmaRHI::EDepthCompareOp::LESS,
		.polygonMode = EnigmaRHI::EPolygonMode::FILL,
		.depthWriteEnable = true,
	};

	lightingPipeline = rhi->InstantiatePipeline();
	lightingPipeline->Create(lightingGP, lightingVertShader, lightingFragShader);

	EnigmaRHI::GraphicsPipeline fxaaGP
	{
		.cullMode = EnigmaRHI::ECullMode::DISABLED,
		.frontFaceMode = EnigmaRHI::EFrontFaceMode::COUNTER_CLOCK_WISE,
		.depthTestEnable = true,
		.compareMode = EnigmaRHI::EDepthCompareOp::LESS,
		.polygonMode = EnigmaRHI::EPolygonMode::FILL,
		.depthWriteEnable = true,
	};

	fxaaPipeline = rhi->InstantiatePipeline();
	fxaaPipeline->Create(fxaaGP, fxaaVertShader, fxaaFragShader);

	EnigmaRHI::GraphicsPipeline transparentGP
	{
		.cullMode = EnigmaRHI::ECullMode::DISABLED,
		.frontFaceMode = EnigmaRHI::EFrontFaceMode::COUNTER_CLOCK_WISE,
		.blendEnable = true,
		.sourceFactor = EnigmaRHI::EBlendFactor::SRC_ALPHA,
		.destFactor = EnigmaRHI::EBlendFactor::ONE_MINUS_SRC_ALPHA,
		.depthTestEnable = true,
		.compareMode = EnigmaRHI::EDepthCompareOp::LESS,
		.depthWriteEnable = false,
	};

	EnigmaRHI::GraphicsPipeline backFaceGP = transparentGP;
	backFaceGP.cullMode = EnigmaRHI::ECullMode::FRONT;
	backFaceGP.depthWriteEnable = false;

	transparentBackPipeline = rhi->InstantiatePipeline();
	transparentBackPipeline->Create(backFaceGP, forwardVertShader, forwardFragShader);

	EnigmaRHI::GraphicsPipeline frontFaceGP = transparentGP;
	frontFaceGP.cullMode = EnigmaRHI::ECullMode::BACK;
	frontFaceGP.depthWriteEnable = false;

	transparentFrontPipeline = rhi->InstantiatePipeline();
	transparentFrontPipeline->Create(frontFaceGP, forwardVertShader, forwardFragShader);

	EnigmaRHI::GraphicsPipeline bloomGP;
	bloomGP.depthTestEnable = true;
	bloomGP.depthWriteEnable = true;
	bloomGP.frontFaceMode = EnigmaRHI::EFrontFaceMode::COUNTER_CLOCK_WISE;

	finalBloomPipeline = rhi->InstantiatePipeline();
	finalBloomPipeline->Create(bloomGP, finalBloomVertShader, finalBloomFragShader);
}

void RenderPipelines::CreateGeometryDescriptor(EnigmaRHI::IRenderInterface* rhi)
{
	geometryDescriptor = rhi->InstantiateDescriptor();
	geometryDescriptor->AddBufferBinding(0); // Camera
	geometryDescriptor->AddBufferBinding(2);// MeshData
	geometryDescriptor->AddImageBinding(3); //albedo
	geometryDescriptor->AddImageBinding(4);// normal
	geometryDescriptor->AddImageBinding(5);// metallic (+ roughness if available)
	geometryDescriptor->AddImageBinding(6); // roughness
	geometryDescriptor->AddImageBinding(7); // ao
	geometryDescriptor->AddImageBinding(8); // clear coat
	geometryDescriptor->AddImageBinding(9); // clear coat roughness
	geometryDescriptor->AddImageBinding(10); // clear coat roughness
	geometryDescriptor->AddBufferBinding(11); // material data
	geometryDescriptor->AddBufferBinding(12); // lights
	geometryDescriptor->AddImageBinding(13); // irradiance
	geometryDescriptor->AddImageBinding(14); // prefilter
	geometryDescriptor->AddImageBinding(15); // brdf
	geometryDescriptor->AddImageBinding(16); // emissive
	geometryDescriptor->Create();
}

void RenderPipelines::CreateLightingDescriptor(EnigmaRHI::IRenderInterface* rhi)
{
	lightingDescriptor = rhi->InstantiateDescriptor();
	lightingDescriptor->AddImageBinding(0);  // position
	lightingDescriptor->AddImageBinding(1);  // normal
	lightingDescriptor->AddImageBinding(2);  // albedo + ao
	lightingDescriptor->AddImageBinding(3);  // metallic + roughness
	lightingDescriptor->AddBufferBinding(4); // camera
	lightingDescriptor->AddBufferBinding(5); // lights
	lightingDescriptor->AddImageBinding(6);  // irradiance
	lightingDescriptor->AddImageBinding(7);  // prefilter
	lightingDescriptor->AddImageBinding(8);  // brdf
	lightingDescriptor->AddImageBinding(9);  // HDR skybox
	lightingDescriptor->AddImageBinding(10); // clear coat factors
	lightingDescriptor->AddImageBinding(11); // clear coat normal
	lightingDescriptor->AddImageBinding(12); // CSM directional shadow map
	lightingDescriptor->AddBufferBinding(13); // lightSpaceMatrices (CSM)
	lightingDescriptor->AddImageBinding(14); // emissive
	lightingDescriptor->AddImageBinding(15); // ssao texture

	for (int i = 0; i < MAX_SPOT_LIGHTS; ++i)
		lightingDescriptor->AddImageBinding(SPOT_SHADOW_BINDING + i);

	for (int i = 0; i < MAX_POINT_LIGHTS; ++i)
		lightingDescriptor->AddImageBinding(POINT_SHADOW_BINDING + i);

	lightingDescriptor->Create();
}

void RenderPipelines::Destroy(EnigmaRHI::IRenderInterface* rhi)
{
	geometryPipeline->Destroy();
	lightingPipeline->Destroy();
	transparentBackPipeline->Destroy();
	transparentFrontPipeline->Destroy();
	fxaaPipeline->Destroy();

	geometryDescriptor->Destroy();
	lightingDescriptor->Destroy();

	rhi->DeleteShaderModule(geometryVertShader);
	rhi->DeleteShaderModule(geometryFragShader);
	rhi->DeletePipeline(geometryPipeline);
	rhi->DeleteShaderModule(lightingVertShader);
	rhi->DeleteShaderModule(lightingFragShader);
	rhi->DeletePipeline(lightingPipeline);
	rhi->DeleteShaderModule(forwardFragShader);
	rhi->DeleteShaderModule(forwardVertShader);
	rhi->DeletePipeline(transparentBackPipeline);
	rhi->DeletePipeline(transparentFrontPipeline);

	rhi->DeleteDescriptor(geometryDescriptor);
	rhi->DeleteDescriptor(lightingDescriptor);
}


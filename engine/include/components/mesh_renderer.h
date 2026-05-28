#pragma once
#include <string>
#include "components/icomponent.h"
#include "resources/Mesh.h"
#include "IRenderInterface.h"
#include "emath/emath.h"
#include "material.h"
#include <rttr/registration.h>
#include <rttr/registration_friend.h>

struct RenderMeshGPUData
{
	Math::Matrix4x4 TRS;
	Math::Matrix4x4 normalMatrix;
};

class ENIGMA_API MeshRenderer : public IComponent
{
public:
	MeshRenderer() = default;
	void Destroy() override;
	void Create() override;
	void OnDraw(Math::Vector3D camPos = Math::Vector3D::Zero) override;
	void Recreate(EnigmaRHI::IRenderInterface* rhi);

	void CreateMeshRenderData(EnigmaRHI::IRenderInterface* rhi);
	void UpdateMeshRenderData(Math::Matrix4x4 TRS, EnigmaRHI::IRenderInterface* rhi);
	void Render(EnigmaRHI::ICommandBuffer& cmd, EnigmaRHI::IDescriptor* objectDescriptor);
	void BindMaterial(EnigmaRHI::IDescriptor* objectDescriptor);

	EnigmaRHI::IBuffer* GetRenderMeshDatas() const { return renderMeshDataBuffer; }
	Mesh* GetMesh();
	void SetMesh(Mesh* mesh, int index, std::string modelName);
	Material* GetMaterial() const { return material; }
	void SetMaterial(Material* material);
	Math::AABB GetAABB() const{ return aabb; };
	void SetAABB(const Math::AABB& newAABB) { aabb = newAABB; };

	bool CastShadows() const { return castShadows; };

	std::string modelName = "";

private:
	RTTR_REGISTRATION_FRIEND

	Mesh* mesh = nullptr;
	Material* material = nullptr;
	Material baseMaterial;
	std::string materialName = "";

	int meshIndex = 0;
	bool castShadows = true;

	EnigmaRHI::IBuffer* renderMeshDataBuffer = nullptr;

	Math::AABB aabb;

	RTTR_ENABLE(IComponent)
};
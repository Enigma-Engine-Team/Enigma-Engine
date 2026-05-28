#include "components/mesh_renderer.h"

#include "components/convex_collider.h"
#include "resources/resource_manager.h"
#include "resources/model.h"
#include "components/material_manager.h"
#include "components/mesh_collider.h"
#include "components/vehicle_controller.h"
#include "engine/engine.h"
#include "serialization/serializer.h"
#include "renderer/gizmo_renderer.h"

RTTR_REGISTRATION
{
	rttr::registration::class_<MeshRenderer>("MeshRenderer")
	.property("meshIndex", &MeshRenderer::meshIndex)
	.property("modelName", &MeshRenderer::modelName)
	.property("aabb", &MeshRenderer::aabb)
	.property("materialName", &MeshRenderer::materialName)
	(
		rttr::metadata(UI_DATA_METADATA_KEY, UIData{EUIType::MATERIAL_INPUT})
	)
	.property("Cast Shadows", &MeshRenderer::castShadows)
	(
		rttr::metadata(UI_DATA_METADATA_KEY, UIData{EUIType::BOOL})
	)
	.constructor<>()(rttr::policy::ctor::as_raw_ptr);
}

Mesh* MeshRenderer::GetMesh()
{
	return mesh;
}

void MeshRenderer::Destroy()
{
}

void MeshRenderer::Create()
{
	material = &baseMaterial;
	materialName = material->name;
}

void MeshRenderer::OnDraw(Math::Vector3D camPos)
{
	Gizmos::color = { 1.f, 0.f, 0.f, 1.f };

	// Bottom
	Gizmos::DrawLine(aabb.p1, aabb.p2);
	Gizmos::DrawLine(aabb.p2, aabb.p3);
	Gizmos::DrawLine(aabb.p3, aabb.p4);
	Gizmos::DrawLine(aabb.p4, aabb.p1);

	// Top
	Gizmos::DrawLine(aabb.p5, aabb.p6);
	Gizmos::DrawLine(aabb.p6, aabb.p7);
	Gizmos::DrawLine(aabb.p7, aabb.p8);
	Gizmos::DrawLine(aabb.p8, aabb.p5);

	// columns
	Gizmos::DrawLine(aabb.p1, aabb.p5);
	Gizmos::DrawLine(aabb.p2, aabb.p6);
	Gizmos::DrawLine(aabb.p3, aabb.p7);
	Gizmos::DrawLine(aabb.p4, aabb.p8);
}

void MeshRenderer::Recreate(EnigmaRHI::IRenderInterface* rhi)
{
	Model* model = ResourceManager::GetInstance().Get<Model>(modelName);
	if (model != nullptr)
	{
		mesh = model->GetSubMeshes()[meshIndex];
		CreateMeshRenderData(rhi);
		if (MeshCollider* mCollider = gameObject->GetComponent<MeshCollider>())
			mCollider->Create();
		if (ConvexCollider* cCollider = gameObject->GetComponent<ConvexCollider>())
			cCollider->Create();
	}
	else
	{
		return;
	}

	if (materialName == baseMaterial.name)
	{
		SetMaterial(model->GetMaterialForMesh(meshIndex));
		material->CreateMaterialData(rhi);
	}
	else
	{
		SetMaterial(MaterialManager::GetInstance().GetMaterial(materialName));
		material->CreateMaterialData(rhi);
	}
}

void MeshRenderer::SetMesh(Mesh* _mesh, int _index, std::string _modelName)
{
	mesh = _mesh;
	modelName = _modelName;
	meshIndex = _index;
}

void MeshRenderer::CreateMeshRenderData(EnigmaRHI::IRenderInterface* rhi)
{
	size_t bufferSize = sizeof(RenderMeshGPUData);
	renderMeshDataBuffer = rhi->InstantiateBuffer();
	renderMeshDataBuffer->Create(bufferSize, EnigmaRHI::EBufferTarget::UNIFORM_BUFFER, nullptr, EnigmaRHI::EBufferUsage::STATIC_DRAW);
	renderMeshDataBuffer->CreateDescriptorBufferInfo();
	renderMeshDataBuffer->bufferInfo.range = bufferSize;
}

void MeshRenderer::Render(EnigmaRHI::ICommandBuffer& cmd, EnigmaRHI::IDescriptor* geometryDescriptor)
{
	if (!mesh)
		return;

	material->Bind(geometryDescriptor);
	mesh->Draw(cmd);
}

void MeshRenderer::BindMaterial(EnigmaRHI::IDescriptor* geometryDescriptor)
{
	material->Bind(geometryDescriptor);
}

void MeshRenderer::UpdateMeshRenderData(Math::Matrix4x4 TRS, EnigmaRHI::IRenderInterface* rhi)
{
	if (!baseMaterial.GetMaterialDataBuffer())
		baseMaterial.CreateMaterialData(rhi);

	if (!material->GetMaterialDataBuffer() || !renderMeshDataBuffer)
	{
		Recreate(rhi);
	}

	Math::Matrix4x4 normalMat = Math::Matrix4x4(Math::Matrix3x3(TRS).Inverse().Transposite());
	RenderMeshGPUData meshUbo
	{
		.TRS = TRS,
		.normalMatrix = normalMat
	};

	renderMeshDataBuffer->CopyData(&meshUbo, sizeof(meshUbo));

	aabb.Update(gameObject->transform.global);
}

void MeshRenderer::SetMaterial(Material* _material)
{
	material = _material;
	materialName = material->name;
}

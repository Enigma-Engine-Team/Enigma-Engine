#include "components/mesh_collider.h"

#include "components/mesh_renderer.h"
#include "engine/engine.h"
#include "gameobject/gameobject.h"
#include "Jolt/Physics/Collision/Shape/MeshShape.h"
#include "physics/physics.h"
#include "renderer/gizmo.h"
#include "resources/resource_manager.h"

RTTR_REGISTRATION
{
    rttr::registration::class_<MeshCollider>("MeshCollider")
        .constructor<>()(rttr::policy::ctor::as_raw_ptr)
        .property("Collider Position", &MeshCollider::GetPosition, &MeshCollider::SetPosition)
            (rttr::metadata(UI_DATA_METADATA_KEY, UIData(EUIType::VEC3, 0.f, 1000.f)))
        .property("Collider Rotation", &MeshCollider::GetEulerRotation, &MeshCollider::SetRotationFromEuler)
            (rttr::metadata(UI_DATA_METADATA_KEY, UIData(EUIType::VEC3)));
}

void MeshCollider::Create()
{
    if (CreateShape())
        Collider::Create();
}

void MeshCollider::OnDraw(Math::Vector3D camPos)
{
	Gizmos::color = { 0.f, 1.f, 0.f, 1.f };
    std::vector<Mesh*> meshes;

    if (MeshRenderer* renderer = gameObject->GetComponent<MeshRenderer>())
        meshes.push_back(renderer->GetMesh());

    Gizmos::DrawMesh(meshes, gameObject->transform.global);
}

void MeshCollider::SetScale()
{
    CreateShape();
}

void MeshCollider::Destroy()
{
    Collider::Destroy();
}

bool MeshCollider::CreateShape()
{
    MeshRenderer* mRenderer = gameObject->GetComponent<MeshRenderer>();
    if (!mRenderer)
    {
        Debug::LogError("Failed to create mesh shape, there's no Component MeshRenderer");
        return false;
    }

    Mesh* mesh = mRenderer->GetMesh();
    if (mRenderer->modelName == "" || !mesh)
    {
        return false;
    }
    JPH::VertexList vertexList;
    std::vector<Vertex> vertices = mesh->GetVertices();
    Math::Vector3D globalScale = GetGlobalScale(gameObject);
    for (Vertex vertex : vertices)
    {
        vertexList.push_back(
        {
            vertex.position.x * globalScale.x,
            vertex.position.y * globalScale.y,
            vertex.position.z * globalScale.z
        }
        );
    }

    JPH::IndexedTriangleList triangles;
    std::vector<uint32_t> indices = mesh->GetIndices();
    triangles.reserve(indices.size() / 3 * 2);
    for (size_t i = 0; i + 2 < indices.size(); i += 3)
    {
        triangles.push_back(JPH::IndexedTriangle(indices[i], indices[i + 1], indices[i + 2]));
        triangles.push_back(JPH::IndexedTriangle(indices[i + 2], indices[i + 1], indices[i]));
    }

    JPH::MeshShapeSettings settings(vertexList, triangles);
    JPH::ShapeSettings::ShapeResult result = settings.Create();
    if (result.IsValid())
    {
        shape = result.Get();
        return true;
    }
    Debug::LogError("Failed to create mesh shape: " + static_cast<std::string>(result.GetError()));
    return false;
}


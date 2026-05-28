#include "components/convex_collider.h"

#include "components/mesh_renderer.h"
#include "components/vehicle_controller.h"
#include "gameobject/gameobject.h"
#include "Jolt/Physics/Collision/Shape/ConvexHullShape.h"
#include "renderer/gizmo.h"

RTTR_REGISTRATION
{
    rttr::registration::class_<ConvexCollider>("ConvexCollider")
        .constructor<>()(rttr::policy::ctor::as_raw_ptr)
        .property("Collider Position", &ConvexCollider::GetPosition, &ConvexCollider::SetPosition)
            (rttr::metadata(UI_DATA_METADATA_KEY, UIData(EUIType::VEC3, 0.f, 1000.f)))
        .property("Collider Rotation", &ConvexCollider::GetEulerRotation, &ConvexCollider::SetRotationFromEuler)
            (rttr::metadata(UI_DATA_METADATA_KEY, UIData(EUIType::VEC3)));
}

void ConvexCollider::Create()
{
    shape = CreateShape(gameObject);

    if (shape != nullptr)
        Collider::Create();
}

void ConvexCollider::OnDraw(Math::Vector3D camPos)
{
    Gizmos::color = { 0.f, 1.f, 0.f, 1.f };
    std::vector<Mesh*> meshes;

    if (MeshRenderer* renderer = gameObject->GetComponent<MeshRenderer>())
        meshes.push_back(renderer->GetMesh());

    Gizmos::DrawMesh(meshes, gameObject->transform.global);
}

void ConvexCollider::SetScale()
{
    shape = CreateShape(gameObject);
}

JPH::ShapeRefC ConvexCollider::CreateShape(GameObject* go)
{
    MeshRenderer* mRenderer = go->GetComponent<MeshRenderer>();
    if (!mRenderer)
    {
        Debug::LogError("Failed to create convex shape, there's no Component MeshRenderer");
        return nullptr;
    }

    Mesh* mesh = mRenderer->GetMesh();
    if (mRenderer->modelName == "" || !mesh)
    {
        return nullptr;
    }
    JPH::Array<JPH::Vec3> vertexList;
    std::vector<Vertex> vertices = mesh->GetVertices();
    Math::Vector3D globalScale = GetGlobalScale(go);
    vertexList.reserve(vertexList.size() + vertices.size());
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

    JPH::ConvexHullShapeSettings settings(vertexList);
    JPH::ShapeSettings::ShapeResult result = settings.Create();

    if (result.IsValid())
        return result.Get();

    Debug::LogError("Failed to create convex shape: " + static_cast<std::string>(result.GetError()));
    return nullptr;
}

JPH::ShapeRefC ConvexCollider::CreateShape(GameObject* go, std::vector<Mesh*>& meshes)
{
    JPH::Array<JPH::Vec3> vertexList;
    Math::Vector3D globalScale = GetGlobalScale(go);

    if (MeshRenderer* renderer = go->GetComponent<MeshRenderer>())
    {
        Mesh* mesh = renderer->GetMesh();
        meshes.push_back(mesh);
        std::vector<Vertex> childVertices = mesh->GetVertices();
        vertexList.reserve(vertexList.size() + childVertices.size());
        for (Vertex vertex : childVertices)
        {
            vertexList.push_back(
            {
                vertex.position.x * globalScale.x,
                vertex.position.y * globalScale.y,
                vertex.position.z * globalScale.z
            }
            );
        }
    }

    JPH::ConvexHullShapeSettings settings(vertexList);
    JPH::ShapeSettings::ShapeResult result = settings.Create();

    if (result.IsValid())
        return result.Get();

    Debug::LogError("Failed to create mesh shape: " + static_cast<std::string>(result.GetError()));
    return nullptr;
}

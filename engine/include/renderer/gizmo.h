#pragma once

#include "camera.h"
#include "resources/mesh.h"

struct GizmoCommand
{
    enum class EGizmoType
    {
        ICON,
        CUBE,
        CIRCLE,
        SPHERE,
        LINE, 
        FRUSTRUM,
        MESH
    };

    EGizmoType type;
    Math::Vector4D gizmoColor;
    Math::Matrix4x4 transform;    // Cube/Sphere
    Math::Vector3D from, to;     // Line

    Math::Vector3D  position;
    Math::Quaternion rotation;
    float fov;
    float aspect;
    float nearPlane;
    float farPlane;
	std::vector<Mesh*> meshes;
    unsigned int billboardTexture;
    float billboardOpacity;
};

class Gizmos
{
public:

	static Math::Vector4D color;

	static void DrawCube(const Math::Vector3D& pos, Math::Quaternion rot, const Math::Vector3D& s);
	static void DrawCircle(Math::Vector3D cente, Math::Quaternion rot, float radius);
    static void DrawSphere(Math::Vector3D center, float radius);
	static void DrawCylinder(Math::Vector3D center, Math::Quaternion rot, float radius, float height);
	static void DrawWheel(const Math::Vector3D& center, const Math::Vector3D& right, const Math::Vector3D& up, const Math::Vector3D& forward, float radius, float width);
    static void DrawLine(Math::Vector3D from, Math::Vector3D to);
	static void DrawMesh(const std::vector<Mesh*>& meshes, const Math::Matrix4x4& transform);
    static void DrawFrustum(ECameraMode mode, Math::Vector3D position, Math::Quaternion rotation, float fov, float aspect, float nearPlane, float farPlane);
    static void DrawCone(Math::Vector3D position, Math::Vector3D targetPos, float radius);
    static void DrawIcon(Math::Vector3D position, float fallOfAlpha, float scale, unsigned int textureID);

    static std::vector<GizmoCommand>& GetCommands();
    static void Clear();

private:

    static std::vector<GizmoCommand> commands;
	static void DrawPerspectiveFrustum(Math::Vector3D position, Math::Quaternion rotation, float fov, float aspect, float nearPlane, float farPlane);
	static void DrawOrthographicFrustum(Math::Vector3D position, Math::Quaternion rotation, float left, float right, float bottom, float top, float nearPlane, float farPlane);
};
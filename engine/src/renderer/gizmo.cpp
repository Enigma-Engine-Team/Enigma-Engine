#include "renderer/gizmo.h"
#include "renderer/color.h"
#include "scenes/scene_manager.h"

Math::Vector4D Gizmos::color = Color::GREEN;
void Gizmos::DrawCube(const Math::Vector3D& pos, Math::Quaternion rot, const Math::Vector3D& s)
{
	GizmoCommand command;
	command.type = GizmoCommand::EGizmoType::CUBE;
	command.gizmoColor = color;
	command.transform = Math::Quaternion::TRS(pos, rot.Conjugate(), s);
	commands.push_back(command);
}

void Gizmos::DrawCircle(Math::Vector3D center, Math::Quaternion rot, float radius)
{
	GizmoCommand command;
	command.type = GizmoCommand::EGizmoType::CIRCLE;
	command.gizmoColor = color;
	command.transform = Math::Quaternion::TRS(center, rot, Math::Vector3D(radius));
	commands.push_back(command);
}

void Gizmos::DrawSphere(Math::Vector3D center, float radius)
{
	DrawCircle(center, Math::Quaternion::Identity, radius);
	DrawCircle(center, Math::Quaternion::FromEuler({ 90.f, 0.f, 0.f }), radius);
	DrawCircle(center, Math::Quaternion::FromEuler({ 0.f, 90.f, 0.f }), radius);
}

void Gizmos::DrawCylinder(Math::Vector3D center, Math::Quaternion rot, float radius, float height)
{
	Math::Vector3D topCircle = (center + Math::Vector3D(0, height / 2.f, 0)).RotateAround(rot, center);
	Math::Vector3D bottomCircle = (center - Math::Vector3D(0, height / 2.f, 0)).RotateAround(rot, center);

	DrawCircle(topCircle, rot, radius);
	DrawCircle(bottomCircle, rot, radius);
}

void Gizmos::DrawWheel(const Math::Vector3D& center, const Math::Vector3D& right, const Math::Vector3D& up, const Math::Vector3D& forward, float radius, float width)
{
	const Math::Vector3D rightAxis = right.Normalized();
	const Math::Vector3D upAxis = up.Normalized();
	const Math::Vector3D forwardAxis = forward.Normalized();
	const Math::Vector3D sideOffset = rightAxis * (width * 0.5f);

	auto drawRim = [&](const Math::Vector3D& rimCenter)
	{
		constexpr int segments = 24;
		Math::Vector3D previousPoint = rimCenter + upAxis * radius;
		for (int i = 1; i <= segments; ++i)
		{
			const float angle = 2.f * Math::PI * static_cast<float>(i) / static_cast<float>(segments);
			const Math::Vector3D point = rimCenter + (upAxis * cosf(angle) + forwardAxis * sinf(angle)) * radius;
			DrawLine(previousPoint, point);
			previousPoint = point;
		}
	};

	const Math::Vector3D leftRimCenter = center + sideOffset;
	const Math::Vector3D rightRimCenter = center - sideOffset;
	drawRim(leftRimCenter);
	drawRim(rightRimCenter);

	for (int i = 0; i < 8; ++i)
	{
		const float angle = 2.f * Math::PI * static_cast<float>(i) / 8.f;
		const Math::Vector3D rimOffset = (upAxis * cosf(angle) + forwardAxis * sinf(angle)) * radius;
		DrawLine(leftRimCenter + rimOffset, rightRimCenter + rimOffset);
	}
}

void Gizmos::DrawLine(Math::Vector3D from, Math::Vector3D to)
{
	if (from == to)
		return;

	GizmoCommand command;
	command.type = GizmoCommand::EGizmoType::LINE;
	command.gizmoColor = color;
	command.transform = Math::Matrix4x4::Identity;
	command.from = from;
	command.to = to;
	commands.push_back(command);
}

void Gizmos::DrawMesh(const std::vector<Mesh*>& meshes, const Math::Matrix4x4& transform)
{
	GizmoCommand command;
	command.type = GizmoCommand::EGizmoType::MESH;
	command.gizmoColor = color;
	command.transform = transform;
	command.meshes = meshes;
	commands.push_back(command);
}

void Gizmos::DrawFrustum(ECameraMode mode, Math::Vector3D position, Math::Quaternion rotation, float fov, float aspect, float nearPlane, float farPlane)
{
	switch (mode)
	{
		case ECameraMode::PERSPECTIVE:
			DrawPerspectiveFrustum(position, rotation, fov, aspect, nearPlane, farPlane);
			break;
		case ECameraMode::ORTHOGRAPHIC:
			DrawOrthographicFrustum(position, rotation, -aspect, aspect, -1.f, 1.f, nearPlane, farPlane);
			break;
	}
}

void Gizmos::DrawCone(Math::Vector3D position, Math::Vector3D targetPos, float radius)
{
	Math::Vector3D dir = (targetPos - position).Normalized();

	Math::Vector3D worldUp = std::fabs(dir.DotProduct(Math::Vector3D(0.f, 1.f, 0.f))) > 0.99f ? Math::Vector3D(0, 0, 1) : Math::Vector3D(0, 1, 0);

	Math::Vector3D right = (worldUp.CrossProduct(dir)).Normalized();
	Math::Vector3D up = (dir.CrossProduct(right)).Normalized();

	Math::Vector3D p1 = targetPos + right * radius;
	Math::Vector3D p2 = targetPos - right * radius;
	Math::Vector3D p3 = targetPos + up * radius;
	Math::Vector3D p4 = targetPos - up * radius;

	DrawCircle(targetPos, Math::Quaternion::FromRotationMatrix(Math::Matrix3x3(right, up, dir)).Conjugate(), radius);
	DrawLine(position, p1);
	DrawLine(position, p2);
	DrawLine(position, p3);
	DrawLine(position, p4);
}

std::vector<GizmoCommand>& Gizmos::GetCommands()
{
	return commands;
}

void Gizmos::Clear()
{
	commands.clear();
}

std::vector<GizmoCommand> Gizmos::commands;

void Gizmos::DrawPerspectiveFrustum(Math::Vector3D position, Math::Quaternion rotation, float fov, float aspect, float nearPlane, float farPlane)
{
	float nearH = tanf(Math::MyToRadians(fov * 0.5f)) * nearPlane;
	float nearW = nearH * aspect;
	float farH = tanf(Math::MyToRadians(fov * 0.5f)) * farPlane;
	float farW = farH * aspect;

	Math::Vector3D corners[8] =
	{
		{ -nearW * 0.5f, -nearH * 0.5f, -nearPlane },
		{  nearW * 0.5f, -nearH * 0.5f, -nearPlane },
		{  nearW * 0.5f,  nearH * 0.5f, -nearPlane },
		{ -nearW * 0.5f,  nearH * 0.5f, -nearPlane },

		{ -farW * 0.5f, -farH * 0.5f, -farPlane },
		{  farW * 0.5f, -farH * 0.5f, -farPlane },
		{  farW * 0.5f,  farH * 0.5f, -farPlane },
		{ -farW * 0.5f,  farH * 0.5f, -farPlane },
	};

	Math::Matrix4x4 camTRS = Math::Quaternion::TRS(position, rotation.Conjugate(), Math::Vector3D(1.f));
	for (auto& c : corners)
	{
		c = {
			camTRS[0] * c.x + camTRS[1] * c.y + camTRS[2] * c.z + camTRS[12],
			camTRS[4] * c.x + camTRS[5] * c.y + camTRS[6] * c.z + camTRS[13],
			camTRS[8] * c.x + camTRS[9] * c.y + camTRS[10] * c.z + camTRS[14]
		};
	}

	DrawLine(corners[0], corners[1]);
	DrawLine(corners[1], corners[2]);
	DrawLine(corners[2], corners[3]);
	DrawLine(corners[3], corners[0]);
	DrawLine(corners[4], corners[5]);
	DrawLine(corners[5], corners[6]);
	DrawLine(corners[6], corners[7]);
	DrawLine(corners[7], corners[4]);
	DrawLine(corners[0], corners[4]);
	DrawLine(corners[1], corners[5]);
	DrawLine(corners[2], corners[6]);
	DrawLine(corners[3], corners[7]);
}

void Gizmos::DrawOrthographicFrustum(Math::Vector3D position, Math::Quaternion rotation, float left, float right, float bottom, float top, float nearPlane, float farPlane)
{
	Math::Vector3D corners[8] =
	{
		{ left,  bottom, -nearPlane },
		{ right, bottom, -nearPlane },
		{ right, top,    -nearPlane },
		{ left,  top,    -nearPlane },
		{ left,  bottom, -farPlane },
		{ right, bottom, -farPlane },
		{ right, top,    -farPlane },
		{ left,  top,    -farPlane },
	};
	Math::Matrix4x4 camTRS = Math::Quaternion::TRS(position, rotation.Conjugate(), Math::Vector3D(1.f));
	for (auto& c : corners)
	{
		c = {
			camTRS[0] * c.x + camTRS[1] * c.y + camTRS[2] * c.z + camTRS[12],
			camTRS[4] * c.x + camTRS[5] * c.y + camTRS[6] * c.z + camTRS[13],
			camTRS[8] * c.x + camTRS[9] * c.y + camTRS[10] * c.z + camTRS[14]
		};
	}
	DrawLine(corners[0], corners[1]);
	DrawLine(corners[1], corners[2]);
	DrawLine(corners[2], corners[3]);
	DrawLine(corners[3], corners[0]);
	DrawLine(corners[4], corners[5]);
	DrawLine(corners[5], corners[6]);
	DrawLine(corners[6], corners[7]);
	DrawLine(corners[7], corners[4]);
	DrawLine(corners[0], corners[4]);
	DrawLine(corners[1], corners[5]);
	DrawLine(corners[2], corners[6]);
	DrawLine(corners[3], corners[7]);
}

void Gizmos::DrawIcon(Math::Vector3D position, float fallOfAlpha, float scale, unsigned int textureID)
{
	//EngineCamera* cam = SceneManager::GetInstance().GetCurrentScene()->GetEngineCam();
	//const float minScaleFactor = 0.2f;
	//const float maxScaleFactor = 5.f;
	//float dist = (position - cam->GetPosition()).Magnitude();
	//float fallOfScaleFactor = std::clamp(dist / 10.f, minScaleFactor, maxScaleFactor);
	//float fallOfAlpha = static_cast<float>(std::clamp(dist / 2.0 - 1.0, 0.0, 1.0));

	GizmoCommand command;
	command.type = GizmoCommand::EGizmoType::ICON;
	command.transform = Math::Matrix4x4::TRS(position, Math::Vector3D::Zero, Math::Vector3D(scale, scale, scale));
	command.billboardTexture = textureID;
	command.billboardOpacity = fallOfAlpha;
	commands.push_back(command);
}
#pragma once

#include "window/window.h"
#include "emath/emath.h"
#include "IRenderInterface.h"
#include "IBuffer.h"

#include <rttr/registration.h>
#include <rttr/registration_friend.h>

enum ECameraMode
{
	PERSPECTIVE,
	ORTHOGRAPHIC
};

struct CameraData
{
	Math::Matrix4x4 projection;
	Math::Matrix4x4 view;
	Math::Matrix4x4 vp;
	Math::Vector3D viewPos;
	float padding;
};

class EngineCamera;

class Camera
{
public:

	Camera() = default;

	void Create(float FOV, float aspectRatio, float zNear, float zFar, Math::Vector3D position);
	void Destroy(EnigmaRHI::IRenderInterface* rhi);

	Math::Matrix4x4 GetQuaternionViewMatrix();

	Math::Frustum CreateFrustum() const;

	void CreateCameraDataBuffer(EnigmaRHI::IRenderInterface* rhi);
	void Update(float width, float height);

	void SetCamera(float FOV, float aspectRatio, float zNear, float zFar, Math::Vector3D position);
	EnigmaRHI::IBuffer* GetCameraDataBuffer() { return cameraDataBuffer; }

	bool IsUsingCamera() const { return useCamera; };
	void SetCameraMode(ECameraMode mode) { cameraMode = mode; }
	Math::Matrix4x4 GetOrtho() const { return ortho; };

private :

	RTTR_REGISTRATION_FRIEND
	EnigmaRHI::IBuffer* cameraDataBuffer = nullptr;

	Math::Matrix4x4 view = Math::Matrix4x4::Identity;
	Math::Matrix4x4 projection = Math::Matrix4x4::Identity;
	Math::Matrix4x4 ortho = Math::Matrix4x4::Identity;
	Math::Matrix4x4 VP = Math::Matrix4x4::Identity;

	Math::Frustum frustum;

	float panningSensitivity = 1.f;
	float mouseSensitivity = 10.f;
	float acceleration = 12.0f;
	float deceleration = 10.0f;

	float lastFrame = 0.f;

	Math::Vector3D position = Math::Vector3D::Zero;
	Math::Vector3D forward = Math::Vector3D::Zero;
	Math::Vector3D right = Math::Vector3D::Zero;
	Math::Vector3D up = Math::Vector3D::Zero;
	Math::Quaternion orientationQuat = Math::Quaternion(0.f, 0.f, 0.f, 1.f);

	float aspectRatio = 1.f;
	float zNear = 0.1f;
	float zFar = 500.f;
	float fov = 90.f;
	ECameraMode cameraMode = ECameraMode::PERSPECTIVE;

	Math::Vector3D velocity = Math::Vector3D::Zero;
	float maxSpeed = 15.0f;
	float scrollStep = 0.5f;
	bool isRightMousePressed = false;
	bool firstMouse = true;
	float lastX = 0.f;
	float lastY = 0.f;
	float deltaTime = 0.f;
	float speed = 1.f;
	float defaultSpeed = 1.f;
	float fastSpeed = 5.f;

	float exposure = 1.f;

	float lastScrollY = 0.f;

	bool isMiddleMouseButtonPressed = false;
	bool useCamera = false;
	friend class EngineCamera;
	friend class GameCamera;
};
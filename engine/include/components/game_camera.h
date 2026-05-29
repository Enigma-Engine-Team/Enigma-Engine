#pragma once

#include "renderer/camera.h"
#include "components/icomponent.h"

class ENIGMA_API GameCamera : public IComponent
{
public:
	GameCamera() = default;
	~GameCamera() override = default;
	void Create() override;
	void OnDraw(Math::Vector3D camPos = Math::Vector3D::Zero) override;

	void Update(float width, float height);

	ECameraMode GetCameraMode() const { return camera.cameraMode; }
	Math::Matrix4x4 GetVP() const { return camera.VP; };
	Math::Frustum GetFrustum() const { return camera.frustum; };
	float GetFOV() const { return camera.fov; }
	float GetZNear() const { return camera.zNear; }
	float GetZFar() const { return camera.zFar; }
	Math::Matrix4x4 GetView() const { return camera.view; };
	float GetAspectRatio() const { return camera.aspectRatio; };
	float GetExposure() const { return camera.exposure; };
	Math::Matrix4x4 GetProjection() const { return camera.projection; }

	void SetCameraMode(ECameraMode mode) { camera.cameraMode = mode; }
	void SetFOV(float fov) { camera.fov = fov; }
	void SetZNear(float zNear) { camera.zNear = zNear; }
	void SetZFar(float zFar) { camera.zFar = zFar; }
	void SetExposure(float newEV) { camera.exposure = newEV; };

	Math::Vector3D GetForward() const { return camera.forward; }

	EnigmaRHI::IBuffer* GetCameraDataBuffer() { return camera.GetCameraDataBuffer(); }
	void CreateCameraDataBuffer(EnigmaRHI::IRenderInterface* rhi) { camera.CreateCameraDataBuffer(rhi); }
	Math::Vector3D GetPosition() const { return camera.position; };

private:
	RTTR_REGISTRATION_FRIEND
	Camera camera;
	RTTR_ENABLE(IComponent)
};
#pragma once

#include "camera.h"

class EngineCamera
{
public:

	void Create();
	void Update(float width, float height);

	EnigmaRHI::IBuffer* GetCameraDataBuffer() { return camera.GetCameraDataBuffer(); }
	void CreateCameraDataBuffer(EnigmaRHI::IRenderInterface* rhi) { camera.CreateCameraDataBuffer(rhi); }
	Math::Matrix4x4 GetView() const { return camera.view; }
	Math::Matrix4x4 GetProjection() const { return camera.projection; }
	Math::Matrix4x4 GetVP() const { return camera.VP; }
	Math::Vector3D GetPosition() const { return camera.position; };
	Math::Vector3D GetForward() const { return camera.forward; }
	float GetFOV() const { return camera.fov; };
	float GetAspectRatio() const { return camera.aspectRatio; };
	float GetZNear() const { return camera.zNear; };
	float GetZFar() const { return camera.zFar; };
	float GetExposure() const { return camera.exposure; };
	Math::Frustum GetFrustum() const { return camera.frustum; };
	Math::Quaternion GetDirection() const { return camera.orientationQuat; };
	bool IsUsing() const { return camera.useCamera; };

private:
   
	RTTR_REGISTRATION_FRIEND
	void GetInputs();
	void MousePositionCallback();
	Camera camera;
};


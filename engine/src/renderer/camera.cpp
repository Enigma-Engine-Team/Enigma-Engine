#include "renderer/camera.h"
#include "emath/utility.h"

RTTR_REGISTRATION
{
    rttr::registration::class_<Camera>("Camera")
        .property("Position", &Camera::position)
        .property("Orientation", &Camera::orientationQuat)
        .property("Velocity", &Camera::velocity)
        .property("Exposure", &Camera::exposure);
}

void Camera::Create(float FOV, float aspectRatio, float zNear, float zFar, Math::Vector3D _position)
{
    SetCamera(FOV, aspectRatio, zNear, zFar, _position);
}

void Camera::CreateCameraDataBuffer(EnigmaRHI::IRenderInterface* rhi)
{
    uint64_t bufferSize = sizeof(CameraData);

    cameraDataBuffer = rhi->InstantiateBuffer();
    cameraDataBuffer->Create(bufferSize, EnigmaRHI::EBufferTarget::UNIFORM_BUFFER, nullptr, EnigmaRHI::EBufferUsage::STATIC_DRAW);
    cameraDataBuffer->CreateDescriptorBufferInfo();
    cameraDataBuffer->bufferInfo.range = sizeof(CameraData);
}

void Camera::Update(float width, float height)
{
	aspectRatio = width / height;

	forward = orientationQuat.Forward();
    right = orientationQuat.Right();
    up = orientationQuat.Up();
    view = GetQuaternionViewMatrix();
    projection = Math::Matrix4x4::Perspective(fov, aspectRatio, zNear, zFar);

    VP = cameraMode == ECameraMode::PERSPECTIVE ? (projection * view) : (ortho * view);

    CameraData ubo
    {
		.projection = projection,
		.view = view,
        .vp = VP,
        .viewPos = position
	};

    cameraDataBuffer->CopyData(&ubo, sizeof(ubo));

    frustum = CreateFrustum();
}

void Camera::SetCamera(float _fov, float _aspectRatio, float _zNear, float _zFar, Math::Vector3D _position)
{
	aspectRatio = _aspectRatio;
	zNear = _zNear;
	zFar = _zFar;
    fov = _fov;
    projection = Math::Matrix4x4::Perspective(fov, aspectRatio, zNear, zFar);
    ortho = Math::Matrix4x4::Orthographic(0.f, 1920.f, 0.f, 1080.f, -1.f, 1.f);

    position = _position;

    orientationQuat = Math::Quaternion::Identity;

    velocity = Math::Vector3D(0.0f);
    speed = 6.0f;
    maxSpeed = 100.0f;
    scrollStep = 0.5f;
    acceleration = 12.0f;
    deceleration = 10.0f;

    mouseSensitivity = 0.15f;
    panningSensitivity = 2.0f;

    useCamera = false;
    isMiddleMouseButtonPressed = false;

    deltaTime = 0.0f;
    lastFrame = 0.0f;
}

void Camera::Destroy(EnigmaRHI::IRenderInterface* rhi)
{
    cameraDataBuffer->UnMapMemory();
    cameraDataBuffer->Destroy();
    rhi->DeleteBuffer(cameraDataBuffer);
}

Math::Matrix4x4 Camera::GetQuaternionViewMatrix()
{
    Math::Matrix4x4 rotation = orientationQuat.ToRotMat4();
	Math::Matrix4x4 translation = Math::Matrix4x4::TranslateMatrix(-position);

	return rotation * translation;
}

Math::Frustum Camera::CreateFrustum() const
{
    Math::Frustum frustum;

    const float halfVSide = zFar * tanf(Math::MyToRadians(fov) * 0.5f);
    const float halfHSide = halfVSide * aspectRatio;

    const Math::Vector3D frontMultFar = forward * zFar;

    frustum.nearFace = Math::Plane(position + forward * zNear, forward);
    frustum.farFace = Math::Plane(position + frontMultFar, -forward);

    frustum.rightFace = Math::Plane(position, (frontMultFar - right * halfHSide).CrossProduct(up));
    frustum.leftFace = Math::Plane(position, up.CrossProduct(frontMultFar + right * halfHSide));

    frustum.topFace = Math::Plane(position, right.CrossProduct(frontMultFar - up * halfVSide));
    frustum.bottomFace = Math::Plane(position, (frontMultFar + up * halfVSide).CrossProduct(right));

    return frustum;
}
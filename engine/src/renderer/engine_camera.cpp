#include "renderer/engine_camera.h"
#include "window/input_manager.h"
#include "debug/log.h"

RTTR_REGISTRATION
{
    rttr::registration::class_<EngineCamera>("EngineCamera")
        .property("camera", &EngineCamera::camera);
}

void EngineCamera::Create()
{
    camera.Create(90.f, 1920.f / 1080.f, 0.1f, 800.f, Math::Vector3D(0.f, 0.f, 0.f));
}

void EngineCamera::Update(float width, float height)
{
    camera.right = camera.orientationQuat.RotateVector(Math::Vector3D(1.f, 0.f, 0.f)).Normalized();
    camera.up = camera.orientationQuat.RotateVector(Math::Vector3D(0.f, 1.f, 0.f)).Normalized();

	camera.Update(width, height);

    GetInputs();
}

void EngineCamera::GetInputs()
{
    float currentTime = static_cast<float>(glfwGetTime());
    camera.deltaTime = currentTime - camera.lastFrame;
    camera.lastFrame = currentTime;

    MousePositionCallback();

    Math::Vector3D moveDirection(0.0f);

    if (camera.useCamera && !camera.isMiddleMouseButtonPressed)
    {
        if (InputManager::GetKeyDown(KEY_Z)) moveDirection += camera.forward;
        if (InputManager::GetKeyDown(KEY_S)) moveDirection -= camera.forward;
        if (InputManager::GetKeyDown(KEY_D)) moveDirection += camera.right;
        if (InputManager::GetKeyDown(KEY_Q)) moveDirection -= camera.right;
        if (InputManager::GetKeyDown(KEY_E)) moveDirection += camera.up;
        if (InputManager::GetKeyDown(KEY_A)) moveDirection -= camera.up;
        if (InputManager::GetKeyDown(KEY_LEFT_SHIFT)) camera.speed = camera.fastSpeed;
            else camera.speed = camera.defaultSpeed;
    }

    Math::Vector3D targetVelocity(0.0f);

    if (moveDirection.Magnitude() > 0.0f)
        targetVelocity = (moveDirection).Normalized() * camera.speed;

    float interpSpeed = (targetVelocity.Magnitude() > 0.0f) ? camera.acceleration : camera.deceleration;

    camera.velocity = Math::Vector3D::Lerp(
        camera.velocity,
        targetVelocity,
        Math::Clamp(interpSpeed * camera.deltaTime, 0.0f, 1.0f)
    );

    camera.position += camera.velocity * camera.deltaTime;

    if (camera.useCamera && InputManager::IsScrolling())
    {
        camera.defaultSpeed += InputManager::GetMouseScroll().y * camera.scrollStep;
        camera.defaultSpeed = Math::Clamp(camera.defaultSpeed, 0.1f, camera.maxSpeed);
		camera.fastSpeed = camera.defaultSpeed * 3.f;
        InputManager::SetIsScrolling(false);
    }
}

void EngineCamera::MousePositionCallback()
{
    if (InputManager::GetMouseButtonDown(MOUSE_BUTTON_RIGHT) && !InputManager::IsInputBlocked())
    {
        if (!camera.isRightMousePressed)
        {
            InputManager::HideCursor();
            camera.isRightMousePressed = true;
        }
        camera.useCamera = true;

        Math::Vector2D delta = InputManager::GetMouseDelta();

		Math::Quaternion qYaw = Math::Quaternion::FromEuler(Math::Vector3D(0.f, -delta.x * camera.mouseSensitivity, 0.f));
		Math::Quaternion qPitch = Math::Quaternion::FromEuler(Math::Vector3D(-delta.y * camera.mouseSensitivity, 0.f, 0.f));

		camera.orientationQuat = qYaw * camera.orientationQuat * qPitch;
		camera.orientationQuat.Normalize();

        return;
    }

    if (camera.isRightMousePressed)
    {
        InputManager::ShowCursor();
        camera.isRightMousePressed = false;
        camera.useCamera = false;
        return;
    }

    if (InputManager::GetMouseButtonDown(MOUSE_BUTTON_MIDDLE) && !InputManager::IsInputBlocked())
    {
        if (!camera.isMiddleMouseButtonPressed)
        {
            InputManager::HideCursor();
            camera.isMiddleMouseButtonPressed = true;
        }

        Math::Vector2D delta = InputManager::GetMouseDelta();

        camera.position -= camera.right * delta.x * camera.panningSensitivity * camera.deltaTime;
        camera.position += camera.up * delta.y * camera.panningSensitivity * camera.deltaTime;
        return;
    }

    if (camera.isMiddleMouseButtonPressed)
    {
        InputManager::ShowCursor();
        camera.isMiddleMouseButtonPressed = false;
        return;
    }

    camera.useCamera = false;
}
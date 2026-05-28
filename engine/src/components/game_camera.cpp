#include "components/game_camera.h"
#include "gameobject/gameobject.h"
#include "gameobject/transform.h"
#include "renderer/gizmo.h"

RTTR_REGISTRATION
{
    rttr::registration::class_<GameCamera>("GameCamera")
        .constructor<>()(rttr::policy::ctor::as_raw_ptr)
        .property("Camera Mode", &GameCamera::GetCameraMode, &GameCamera::SetCameraMode)
            (rttr::metadata(UI_DATA_METADATA_KEY, UIData{ EUIType::COMBO, 0.f, 0.f, "", "Projection", {"Perspective", "Orthographic"}}))
        .property("FOV", &GameCamera::GetFOV, &GameCamera::SetFOV)
            (rttr::metadata(UI_DATA_METADATA_KEY, UIData{ EUIType::SLIDER_FLOAT, 1.f, 179.f, "%.1f", "Projection"}))
        .property("Near", &GameCamera::GetZNear, &GameCamera::SetZNear)
            (rttr::metadata(UI_DATA_METADATA_KEY, UIData{ EUIType::SLIDER_FLOAT, 0.001f, 10.f, "%.2f", "Clipping"}))
        .property("Far", &GameCamera::GetZFar, &GameCamera::SetZFar)
            (rttr::metadata(UI_DATA_METADATA_KEY, UIData{ EUIType::SLIDER_FLOAT, 100.f, 1000.f, "%.2f", "Clipping" }))
        .property("Exposure", &GameCamera::GetExposure, &GameCamera::SetExposure)
            (rttr::metadata(UI_DATA_METADATA_KEY, UIData{ EUIType::SLIDER_FLOAT, 0.01f, 10.f, "%.3f", "Physically Cam" }));
}

void GameCamera::Create()
{
	camera.Create(90.f, 1920.f / 1080.f, 0.1f, 1000.f, Math::Vector3D(0.f, 0.f, 0.f));
}

void GameCamera::OnDraw(Math::Vector3D camPos)
{
	Gizmos::color = { 1.f, 1.f, 1.f, 1.f };
	Gizmos::DrawFrustum(camera.cameraMode, gameObject->transform.worldPosition, gameObject->transform.worldRotation.Conjugate(), camera.fov, camera.aspectRatio, camera.zNear, camera.zFar);
}

void GameCamera::Update(float width, float height)
{
	camera.position = gameObject->transform.worldPosition;
	camera.orientationQuat = gameObject->transform.worldRotation.Conjugate();
	camera.Update(width, height);
}

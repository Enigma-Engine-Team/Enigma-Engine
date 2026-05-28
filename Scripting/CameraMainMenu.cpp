#include "CameraMainMenu.h"
#include <algorithm>

void CameraMainMenu::UpdateCamera(float deltaTime)
{
    actualLerpTime += deltaTime;
    t = actualLerpTime / lerpTime;
    t = std::min(t, 1.f);
    gameObject->transform.position = Vector3D::Lerp(firstPosition, lastPosition, t);
    gameObject->transform.rotation = Quaternion::Slerp(Quaternion::FromEuler(firstRotation), Quaternion::FromEuler(lastRotation), t);
}

bool CameraMainMenu::Reset()
{
    if (t >= 1.f)
    {
        ResetAll();
        return true;
    }
    return false;
}

void CameraMainMenu::ResetAll()
{
    gameObject->transform.position = firstPosition;
    gameObject->transform.rotation = Quaternion::FromEuler(firstRotation);
    actualLerpTime = 0.f;
    t = 0.f;
}

#include "PlayerHandler.h"
#include <engine/engine.h>
#include "components/mesh_renderer.h"

void PlayerHandler::CollideCheckPoint()
{
    actualCheckPoint++;
    if (actualCheckPoint >= numCheckPoint)
        finished = true;
}

void PlayerHandler::Brake(bool brake)
{
    Material* mat = brakeLight->GetComponent<MeshRenderer>()->GetMaterial();
    if (brake)
    {
        mat->GetMaterialGPUData().emissiveColorIntensity = Vector4D(1.f, 0.f, 0.f, 20.f);
    }
    else
        mat->GetMaterialGPUData().emissiveColorIntensity = Vector4D(0.f, 0.f, 0.f, 0.f);
        
}

void PlayerHandler::Restart()
{
    finished = false;
    actualCheckPoint = 0;
    Vector3D speed(0.f, 0.f, 0.f);
    gameObject->transform.SetPosition(idlePos);
    gameObject->transform.SetRotation(Quaternion::FromEuler(idleRot));
    gameObject->physicalBody->SetLinearVelocity(speed);
}

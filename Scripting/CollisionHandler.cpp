#include "CollisionHandler.h"

void CollisionHandler::OnCollisionEnter(Physicalbody& body1, Physicalbody& body2)
{
    Vector3D upVector = gameObject->transform.rotation.Up();
    body2.AddForce(upVector * impulseForce);
}
//
// Created by sasha on 30/09/2026.
//

#include "Test.h"

void Test::OnCollisionEnter(Physicalbody& body1, Physicalbody& body2)
{
    body1.AddForce(collisionPower);
}

#include "components/icomponent.h"


RTTR_REGISTRATION
{
    rttr::registration::class_<IComponent>("IComponent");
}

void IComponent::SetParent(GameObject* go)
{
    this->gameObject = go;
}

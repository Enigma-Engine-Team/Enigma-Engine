#pragma once
#include "components/icomponent.h"
#include "utilities/macro.h"

class ENIGMA_API SoundListener : public IComponent
{
public:
    SoundListener() = default;
    void Create() override;
    void Destroy() override;
    void Update() const;
private:
    RTTR_ENABLE(IComponent);
};
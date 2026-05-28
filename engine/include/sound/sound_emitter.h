#pragma once
#include "components/icomponent.h"
#include "sound.h"
#include "utilities/macro.h"

class ENIGMA_API SoundEmitter : public IComponent
{
public:
    SoundEmitter() = default;
    void Create() override;
    void Destroy() override;
    void Update();

    void AddSound(Sound* sound);
    void RemoveSound(Sound* sound);
    void ClearSounds();

    float GetVolume() const { return volume; }
    void SetVolume(float _volume);

    //Will be usefull to set current sound based on vehicle RPM.
    float ComputeLayerWeight(float rpm, float min, float center, float max);

private:
    std::vector<Sound*> sounds;
    float volume;

    RTTR_ENABLE(IComponent);
};
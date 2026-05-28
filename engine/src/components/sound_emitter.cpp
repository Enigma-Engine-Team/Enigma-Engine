#include "sound/sound_emitter.h"
#include "emath/vector.h"
#include "gameobject/gameobject.h"
#include "sound/sound_manager.h"
#include <algorithm>

RTTR_REGISTRATION
{
    rttr::registration::class_<SoundEmitter>("SoundEmitter")
		.constructor<>()(rttr::policy::ctor::as_raw_ptr)
        .property("Volume", &SoundEmitter::GetVolume, &SoundEmitter::SetVolume)
            (rttr::metadata(UI_DATA_METADATA_KEY, UIData(EUIType::DRAG_FLOAT, 0.f, 20.f)));
}

void SoundEmitter::Create()
{
    SoundManager::GetInstance().AddEmitter(*this);
}

void SoundEmitter::Destroy()
{
    SoundManager::GetInstance().RemoveEmitter(*this);
}

void SoundEmitter::Update()
{
    Math::Vector3D pos = gameObject->transform.worldPosition;
    for (auto* sound : sounds)
        sound->SetPosition(pos);
}

void SoundEmitter::AddSound(Sound* sound)
{
    if (!sound || std::find(sounds.begin(), sounds.end(), sound) != sounds.end())
        return;

    sounds.push_back(sound);
    const Math::Vector3D pos = gameObject ? gameObject->transform.worldPosition : Math::Vector3D::Zero;
    sound->SetPosition(pos);
}

void SoundEmitter::RemoveSound(Sound *sound)
{
    std::erase(sounds, sound);
}

void SoundEmitter::ClearSounds()
{
    sounds.clear();
}

void SoundEmitter::SetVolume(float _volume)
{
    for (auto* sound : sounds)
        sound->SetVolume(_volume);

    volume = _volume;
}

float SoundEmitter::ComputeLayerWeight(float rpm, float min, float center, float max) {
    if (rpm < min || rpm > max)
        return 0.f;
    if (rpm == center)
        return 1.f;
    if (rpm < center)
        return (rpm - min) / (center - min);
    return 1.f - ((rpm - center) / (max - center));
}

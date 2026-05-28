#pragma once
#include <string>
#include "utilities/macro.h"
#include "emath/emath.h"
#include "miniaudio.h"

class ENIGMA_API Sound
{
public:
    Sound() = default;
    Sound(const std::string& path);
    ~Sound();

    void Load(const std::string& path);
    void Play(bool playing) const;
    void Spatialize(bool spatialize) const;
    void Loop(bool loop) const;
    void SetVolume(float volume) const;
    float GetVolume() const;
    void SetPitch(float pitch) const;
    float GetPitch() const;
    void SetVelocity(const Math::Vector3D& velocity) const;
    void SetPosition(const Math::Vector3D& position) const;

    bool IsPlaying();

private:
    ma_sound* sound = nullptr;
};

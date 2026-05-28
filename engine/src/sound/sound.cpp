#include "sound/sound.h"

#include "debug/log.h"
#include "sound/sound_manager.h"

Sound::Sound(const std::string &path)
{
    Load(path);
}

Sound::~Sound()
{
    ma_sound_uninit(sound);
    delete sound;
}

void Sound::Load(const std::string &path)
{
    sound = new ma_sound;
    ma_result result = ma_sound_init_from_file(&SoundManager::GetInstance().engine, path.c_str(), 0, nullptr, nullptr, sound);

    if (result != MA_SUCCESS)
        Debug::Log("Failed to load sound: " + path);
}

void Sound::Play(bool playing) const
{
    if (!playing)
        ma_sound_stop(sound);
    else
        ma_sound_start(sound);
}

void Sound::Spatialize(bool spatialize) const
{
    ma_sound_set_spatialization_enabled(sound, spatialize);
}

void Sound::Loop(bool loop) const
{
    ma_sound_set_looping(sound, loop);
}

void Sound::SetVolume(float volume) const
{
    ma_sound_set_volume(sound, volume);
}

float Sound::GetVolume() const
{
    return ma_sound_get_volume(sound);
}

void Sound::SetPitch(float pitch) const
{
    ma_sound_set_pitch(sound, pitch);
}

float Sound::GetPitch() const
{
    return ma_sound_get_pitch(sound);
}

void Sound::SetVelocity(const Math::Vector3D& velocity) const
{
    ma_sound_set_velocity(sound, velocity.x, velocity.y, velocity.z);
}

void Sound::SetPosition(const Math::Vector3D& position) const
{
    ma_sound_set_position(sound, position.x, position.y, position.z);
}

bool Sound::IsPlaying()
{
    return ma_sound_is_playing(sound) == MA_TRUE;
}

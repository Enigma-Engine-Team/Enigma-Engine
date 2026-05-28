#include "sound/sound_listener.h"
#include "gameobject/gameobject.h"
#include "sound/sound_manager.h"

RTTR_REGISTRATION
{
    rttr::registration::class_<SoundListener>("SoundListener")
		.constructor<>()(rttr::policy::ctor::as_raw_ptr);
}

void SoundListener::Create()
{
    SoundManager::GetInstance().AddListener(*this);
}

void SoundListener::Destroy()
{
     SoundManager::GetInstance().RemoveListener(*this);
}

void SoundListener::Update() const
{
    Math::Vector3D pos = gameObject->transform.worldPosition;
    Math::Quaternion rot = gameObject->transform.worldRotation;

    ma_engine_listener_set_position(&SoundManager::GetInstance().engine, 0, pos.x, pos.y, pos.z);
    ma_engine_listener_set_direction(&SoundManager::GetInstance().engine, 0, -rot.Forward().x, rot.Forward().y, rot.Forward().z);
}

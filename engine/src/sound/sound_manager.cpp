#include "sound/sound_manager.h"
#include "debug/log.h"
#include "scenes/scene.h"
#include "scenes/scene_manager.h"
#include <algorithm>

SoundManager& SoundManager::GetInstance()
{
    static SoundManager instance;
    return instance;
}

void SoundManager::Update()
{
    SoundListener* activeListener = GetActiveListener();
    if (activeListener)
        activeListener->Update();

    for (SoundEmitter* emitter : soundEmitters)
    {
        emitter->Update();
    }
}

void SoundManager::AddListener(SoundListener& listener)
{
    if (std::find(soundListeners.begin(), soundListeners.end(), &listener) != soundListeners.end())
        return;

    soundListeners.push_back(&listener);
}

void SoundManager::RemoveListener(SoundListener& listener)
{
    std::erase(soundListeners, &listener);
}

void SoundManager::RemoveAllListeners()
{
    soundListeners.clear();
}

void SoundManager::AddEmitter(SoundEmitter& emitter)
{
    if (std::find(soundEmitters.begin(), soundEmitters.end(), &emitter) != soundEmitters.end())
        return;

    soundEmitters.push_back(&emitter);
}

void SoundManager::RemoveEmitter(SoundEmitter& emitter)
{
    std::erase(soundEmitters, &emitter);
}

void SoundManager::RemoveAllEmitters()
{
    soundEmitters.clear();
}

SoundListener* SoundManager::GetActiveListener() const
{
    if (soundListeners.empty())
        return nullptr;

    Scene* currentScene = SceneManager::GetInstance().GetCurrentScene();
    GameObject* gameCam = currentScene ? currentScene->GetGameCam() : nullptr;
    if (!gameCam)
        return soundListeners.front();

    for (SoundListener* listener : soundListeners)
    {
        if (listener && listener->gameObject == gameCam)
            return listener;
    }

    return soundListeners.front();
}

SoundManager::SoundManager()
{
    engineConfig = ma_engine_config_init();
    ma_result result = ma_engine_init(&engineConfig, &engine);
    if (result != MA_SUCCESS)
    {
        Debug::Log("Failed to initialize audio engine");
        return;
    }

    ma_engine_listener_set_world_up(&engine, 0, 0, 1, 0);
}

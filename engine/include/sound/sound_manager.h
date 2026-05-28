#pragma once

#include "miniaudio.h"
#include "sound_emitter.h"
#include "sound_listener.h"
#include "utilities/macro.h"

class ENIGMA_API SoundManager
{
public:
    static SoundManager& GetInstance();
    void Update();

    void AddListener(SoundListener& listener);
    void RemoveListener(SoundListener& listener);
    void RemoveAllListeners();

    void AddEmitter(SoundEmitter& emitter);
    void RemoveEmitter(SoundEmitter& emitter);
    void RemoveAllEmitters();

    ma_engine engine;
private:
    SoundManager();
    SoundListener* GetActiveListener() const;

    ma_engine_config engineConfig;

    std::vector<SoundEmitter*> soundEmitters;
    std::vector<SoundListener*> soundListeners;
};

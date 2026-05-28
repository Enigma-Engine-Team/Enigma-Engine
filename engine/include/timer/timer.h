#pragma once
#include <__msvc_chrono.hpp>
#include "../utilities/macro.h"

class ENIGMA_API Timer {
public:
    Timer();
    void Reset();
    void Update();
    bool Accumulate();
    float GetDeltaTime() const { return deltaTime; }
    float GetFixedDeltaTime() const { return fixedDeltaTime; }
    float GetAccumulator() const { return accumulator; }
    float GetAlpha() const { return accumulator / fixedDeltaTime; }

private:
    std::chrono::steady_clock::time_point currentTime;
    std::chrono::steady_clock::time_point lastTime;
    float deltaTime;
    float fixedDeltaTime;
    float accumulator = 0.0f;
};

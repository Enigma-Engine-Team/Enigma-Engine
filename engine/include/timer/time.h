#pragma once
#include <__msvc_chrono.hpp>
#include "../utilities/macro.h"

class ENIGMA_API Time {
public:
    Time();
    void Reset();
    void Update();
    bool Accumulate();
    static float GetTime() { return std::chrono::duration<float>(std::chrono::high_resolution_clock::now() - startTime).count(); }
    //Get the time accumulated with the Accumulate function
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
    inline static const auto startTime = std::chrono::high_resolution_clock::now();
};

#include "timer/timer.h"
#include <chrono>

#include "engine/engine.h"

Timer::Timer()
{
    currentTime = std::chrono::high_resolution_clock::now();
    lastTime = std::chrono::high_resolution_clock::now();
    deltaTime = 0.0f;
    fixedDeltaTime = 1.f / 60.f;
}

void Timer::Reset()
{
    currentTime = std::chrono::high_resolution_clock::now();
    lastTime = std::chrono::high_resolution_clock::now();
    deltaTime = 0.0f;
    accumulator = 0.0f;
}

void Timer::Update()
{
    currentTime = std::chrono::high_resolution_clock::now();
    auto duration = currentTime - lastTime;
    deltaTime = std::chrono::duration<float>(duration).count();
    lastTime = currentTime;
    if (Engine::IsRunning())
        accumulator += deltaTime;
}

bool Timer::Accumulate()
{
    bool accumulated = accumulator >= fixedDeltaTime;
    if (accumulated)
	    accumulator -= fixedDeltaTime;
    return accumulated;
}

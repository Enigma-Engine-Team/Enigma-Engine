#include <chrono>
#include "timer/time.h"
#include "engine/engine.h"

Time::Time()
{
    currentTime = std::chrono::high_resolution_clock::now();
    lastTime = std::chrono::high_resolution_clock::now();
    deltaTime = 0.0f;
    fixedDeltaTime = 1.f / 60.f;
}

void Time::Reset()
{
    currentTime = std::chrono::high_resolution_clock::now();
    lastTime = std::chrono::high_resolution_clock::now();
    deltaTime = 0.0f;
    accumulator = 0.0f;
}

void Time::Update()
{
    currentTime = std::chrono::high_resolution_clock::now();
    auto duration = currentTime - lastTime;
    deltaTime = std::chrono::duration<float>(duration).count();
    lastTime = currentTime;
    if (Engine::IsRunning())
        accumulator += deltaTime;
}

bool Time::Accumulate()
{
    bool accumulated = accumulator >= fixedDeltaTime;
    if (accumulated)
	    accumulator -= fixedDeltaTime;
    return accumulated;
}

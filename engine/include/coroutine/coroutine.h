#pragma once

#include <coroutine>
#include <chrono>
#include <vector>

using Clock = std::chrono::steady_clock;

class Scheduler
{
public:
    static Scheduler& GetInstance();

    void AddTimerTask(Clock::time_point time, std::coroutine_handle<> task);
    void AddBoolConditionTask(bool* varToCheck, bool expectedValue, std::coroutine_handle<> task);
    void AddNextFrameTask(std::coroutine_handle<> task);
    void Update();

private:
    struct BooleanCondition
    {
        bool* varToCheck = nullptr;
        bool expectedValue = false;
        std::coroutine_handle<> task;
    };

    std::vector<std::pair<Clock::time_point, std::coroutine_handle<>>> timerTasks;
    std::vector<BooleanCondition> boolConditionTasks;
    std::vector<std::coroutine_handle<>> nextFrameTasks;

    std::vector<std::coroutine_handle<>> tasksToResume;


    Scheduler() = default;
    ~Scheduler();

    void ResumeTasks();
    void UpdateTimerTasks();
    void UpdateConditionTasks();
    void UpdateNextFrameTasks();
};

struct Task
{
    struct promise_type
    {
        Task get_return_object() { return Task{ std::coroutine_handle<promise_type>::from_promise(*this) }; }
        std::suspend_never initial_suspend() noexcept { return {}; }
        std::suspend_never final_suspend() noexcept { return {}; }
        void return_void() {}
        void unhandled_exception() { std::terminate(); }
    };

    std::coroutine_handle<promise_type> handle{};
    explicit Task(std::coroutine_handle<promise_type> _handle) : handle(_handle) {}
    Task(Task&& pastTask) noexcept : handle(pastTask.handle)
    {
        pastTask.handle = {};
    }
    Task(const Task&) = delete;
    Task& operator=(const Task&) = delete;
};

struct Timer
{
    std::chrono::milliseconds duration;

    Timer(int milliseconds)
    {
        duration = std::chrono::milliseconds(milliseconds);
    }

    bool await_ready() const { return false; }
    void await_suspend(std::coroutine_handle<> h) const
    {
        Clock::time_point d = Clock::now() + duration;
        Scheduler::GetInstance().AddTimerTask(d, h);
    }
    void await_resume() const {}
};

struct BoolCondition
{
    bool* varToCheck = nullptr;
    bool expectedValue = false;

    BoolCondition(bool& _varToCheck, bool _expectedValue)
    {
        varToCheck = &_varToCheck;
        expectedValue = _expectedValue;
    }

    bool await_ready() const { return false; }
    void await_suspend(std::coroutine_handle<> handle) const
    {
        Scheduler::GetInstance().AddBoolConditionTask(varToCheck, expectedValue, handle);
    }
    void await_resume() const {}
};

struct NextFrame
{
    NextFrame() = default;

    bool await_ready() const { return false; }
    void await_suspend(std::coroutine_handle<> h) const
    {
        Scheduler::GetInstance().AddNextFrameTask(h);
    }
    void await_resume() const {}
};
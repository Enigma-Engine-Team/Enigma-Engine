#include "multithreading/async.h"

Async::Async()
{
	int threadMaxCount = std::thread::hardware_concurrency() - 1;
    for (int i = 0; i < threadMaxCount; ++i)
    {
        threads.emplace_back(std::thread(&Async::ThreadLoop, this));
    }
}

Async& Async::GetInstance()
{
    static Async instance;
	return instance;
}

Async::~Async()
{
    shouldStop = true;
    event.notify_all();

    for (std::thread& t : threads)
    {
        if (t.joinable())
        {
            t.join();
        }
    }
}

int Async::CreateTask(std::function<void()> callback)
{
    std::unique_lock<std::mutex> lock(localMutex);
	tasks.push({ index, callback });
	int prevIndex = index;
	++index;
    event.notify_one();

	return prevIndex;
}

void Async::WaitTask(int _index)
{
    std::unique_lock<std::mutex> lock(localMutex);
    event.wait(lock, [this, _index]
    {
        return indices.find(_index) != indices.end();
    });

    indices.erase(_index);
}

void Async::ThreadLoop()
{
    while (1)
    {
        std::pair<int, std::function<void()>> pair;
        {
            std::unique_lock<std::mutex> lock(localMutex);
            event.wait(lock, [this] {
                return !tasks.empty() || shouldStop;
                });

            if (shouldStop && tasks.empty())
            {
                return;
            }

            pair = tasks.front();
            tasks.pop();
        }
        std::function<void()> Task = pair.second;
        Task();

        std::unique_lock<std::mutex> lock(localMutex);
        indices.insert(pair.first);
        event.notify_all();
    }
}
#pragma once

#include <thread>
#include <mutex>
#include <vector>
#include <functional>
#include <atomic>
#include <queue>
#include <unordered_set>
#include <condition_variable>
#include "utilities/macro.h"

class ENIGMA_API Async
{
public:
	static Async& GetInstance();

	int CreateTask(std::function<void()> task);
	void WaitTask(int _index);

private:
	std::mutex localMutex;
	std::vector<std::thread> threads;
	std::queue<std::pair<int, std::function<void()>>> tasks;
	std::condition_variable event;
	std::atomic<int> index = 0;
	std::unordered_set<int> indices;
	std::atomic<bool> shouldStop = false;

	Async();
	~Async();

	void ThreadLoop();
};
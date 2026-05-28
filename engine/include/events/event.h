#pragma once

#include <unordered_map>
#include <functional>
#include "utilities/macro.h"

class ENIGMA_API Event
{
public:
	Event() = default;
	~Event() = default;

	int Subscribe(std::function<void()> function);
	void Unsubscribe(int index);
	void Call();

private:
	std::unordered_map<int, std::function<void()>> functions;
	int index = 0;
};
#include "events/event.h"

int Event::Subscribe(std::function<void()> function)
{
	functions.insert({ index, function });
	return index++;
}

void Event::Unsubscribe(int index)
{
	if (functions.count(index) != 0)
	{	
		functions.erase(index);
	}
}

void Event::Call()
{
	for (auto& [key, function] : functions)
	{
		function();
	}
}
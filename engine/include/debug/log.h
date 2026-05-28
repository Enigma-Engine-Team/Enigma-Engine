#pragma once
#include <filesystem>
#include <fstream>
#include <source_location>
#include <vector>
#include <mutex>
#include "to_string.h"
#include "../utilities/macro.h"

class ENIGMA_API Debug
{
public:

	struct Message
	{
		std::string content;
		int color;
	};

	static void Log(std::string content, const std::source_location& loc = std::source_location::current());
	static void LogSuccess(std::string content, const std::source_location& loc = std::source_location::current());
	static void LogWarning(std::string content, const std::source_location& loc = std::source_location::current());
	static void LogError(std::string content, const std::source_location& loc = std::source_location::current());
	static std::vector<Message>& GetMessages();

	Debug() = default;
	~Debug() = default;

private:
	static std::ofstream file;
	static std::mutex mut;

	static void Logging(std::string& content, int color, std::string tag, const std::source_location& loc = std::source_location::current());
	static std::vector<Message> messages;
};
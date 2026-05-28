#include "debug/log.h"
#include <iostream>

#define RED 31
#define GREEN 32
#define ORANGE 33
#define WHITE 37

std::ofstream Debug::file;
std::vector<Debug::Message> Debug::messages;
std::mutex Debug::mut;

void Debug::Logging(std::string& content, int color, std::string tag, const std::source_location& loc)
{
	if (!file.is_open())
	{
		std::filesystem::create_directory("logs");
		file.open("logs/log.txt");
	}

	std::string fileInfos = std::filesystem::path(loc.file_name()).filename().string();
	fileInfos = fileInfos + "(" + std::to_string(loc.line()) + ")";
	std::string message = fileInfos + ": " + "[" + tag + "] " + content + "\n";

	mut.lock();
	messages.push_back({ .content = message, .color = color });
	mut.unlock();

	std::cout << "\033[" << color << "m" << message << "\033[0m";

	file << message;
}

void Debug::Log(std::string content, const std::source_location& loc)
{
	Logging(content, WHITE, "INFO", loc);
}

void Debug::LogSuccess(std::string content, const std::source_location& loc)
{
	Logging(content, GREEN, "SUCCESS", loc);
}

void Debug::LogWarning(std::string content, const std::source_location& loc)
{
	Logging(content, ORANGE, "WARNING", loc);
}

void Debug::LogError(std::string content, const std::source_location& loc)
{
	Logging(content, RED, "ERROR", loc);
}

std::vector<Debug::Message>& Debug::GetMessages()
{
	return messages;
}
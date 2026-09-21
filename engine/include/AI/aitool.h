#pragma once
#include <string>
#include <httplib.h>
#include "../utilities/macro.h"

class ENIGMA_API AITool
{
public:
    struct Prompt
    {
        std::string sender;
        std::string content;
    };

    AITool();
    ~AITool() = default;
    void SendPrompt(const std::string& message);
    std::vector<Prompt>& GetPrompts();
    void ClearPrompts();

    void ApplyAction(const std::string& action);
    static AITool& GetInstance();

private:
    httplib::Client cli;
    std::string content;
    std::vector<Prompt> prompts;
};

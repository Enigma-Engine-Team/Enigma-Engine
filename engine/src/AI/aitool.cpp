#include "AI/aitool.h"

#include "nlohmann/json.hpp"
#include "nlohmann/json_fwd.hpp"
#include "rttr/method.h"

AITool::AITool() : cli("127.0.0.1", 11434)
{
}

void AITool::SendPrompt(const std::string& message)
{
    prompts.push_back({ "user", message });
    nlohmann::json body = {
        {"model", "llama3"},
        {"messages", {
                {
                    {"role", "user"},
                    {"content", message}
                }
        }},
        {"stream", false}
    };

    auto response = cli.Post(
        "/api/chat",
        body.dump(),
        "application/json"
    );

    if (!response)
        throw std::runtime_error(
            "Ollama connection error: " +
            httplib::to_string(response.error())
        );

    if (response->status != 200)
        throw std::runtime_error(
            "Ollama HTTP error: " +
            std::to_string(response->status)
        );

    auto json = nlohmann::json::parse(response->body);

    prompts.push_back({ "assistant", json["message"]["content"].get<std::string>() });
}

std::vector<AITool::Prompt>& AITool::GetPrompts()
{
    return prompts;
}

void AITool::ClearPrompts()
{
    prompts.clear();
}

void AITool::ApplyAction(const std::string& action)
{
    // rttr::method method;
    // method.invoke(action);
}

AITool& AITool::GetInstance()
{
    static AITool tool;
    return tool;
}

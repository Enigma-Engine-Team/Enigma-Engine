#include "panel/ai_console.h"

#include "AI/aitool.h"

UI::AiConsolePanel::AiConsolePanel()
{
}

void UI::AiConsolePanel::Draw()
{
    AITool& aiTool = AITool::GetInstance();

    ImGui::Begin("AIConsole");
    UI::Style::PushStyle(ImGuiCol_Button, Color::EnigmaGrey2);

    if (ImGui::Button("Clear"))
        aiTool.ClearPrompts();

    UI::Style::PopStyle();
    ImGui::SameLine();
    ImGui::Checkbox("Auto-Scroll", &autoScroll);
    ImGui::Separator();

    const float inputHeight = 60.0f;

    ImGui::BeginChild("ChatHistory", ImVec2(0,-inputHeight - ImGui::GetStyle().ItemSpacing.y), ImGuiChildFlags_None, ImGuiWindowFlags_None);

    const float availableWidth = ImGui::GetContentRegionAvail().x;
    const float maxMessageWidth = availableWidth * 0.80f;

    for (const AITool::Prompt& message : aiTool.GetPrompts())
    {
        const bool isUser = message.sender == "user";

        if (isUser)
        {
            ImGui::SetWindowFontScale(1.20f);

            const ImVec2 senderSize = ImGui::CalcTextSize(message.sender.c_str());
            ImGui::SetCursorPosX(availableWidth - senderSize.x);
            ImGui::TextUnformatted(message.sender.c_str());

            ImGui::SetWindowFontScale(1.0f);
            ImVec2 textSize = ImGui::CalcTextSize(message.content.c_str(), nullptr, false, maxMessageWidth);
            ImGui::SetCursorPosX(availableWidth - textSize.x);
            ImGui::PushTextWrapPos(availableWidth);
            ImGui::TextUnformatted(message.content.c_str());

            ImGui::PopTextWrapPos();
        }

        else
        {
            ImGui::SetWindowFontScale(1.20f);

            ImGui::TextUnformatted(message.sender.c_str());
            ImGui::SetWindowFontScale(1.0f);
            ImGui::PushTextWrapPos(ImGui::GetCursorPosX() + maxMessageWidth);
            ImGui::TextWrapped("%s",message.content.c_str());
            ImGui::PopTextWrapPos();
        }

        ImGui::Spacing();
        ImGui::Spacing();
    }

    if (autoScroll)
        ImGui::SetScrollHereY(1.0f);

    ImGui::EndChild();
    ImGui::Separator();

    static char prompt[4096] = "";

    ImGui::InputTextMultiline("##Prompt", prompt, sizeof(prompt), ImVec2(-100.0f,inputHeight));
    ImGui::SameLine();

    bool send = false;

    if (ImGui::Button("Send",ImVec2(90.0f, inputHeight)))
        send = true;

    if (ImGui::IsItemFocused())
    {
        if (ImGui::GetIO().KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_Enter))
            send = true;
    }

    if (send && strlen(prompt) > 0)
    {
        aiTool.SendPrompt(prompt);
        memset(prompt, 0, sizeof(prompt));
        ImGui::SetKeyboardFocusHere(-1);
    }

    ImGui::End();
}
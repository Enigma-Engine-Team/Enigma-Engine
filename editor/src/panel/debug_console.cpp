#include "panel/debug_console.h"
#include "debug/log.h"

#define RED 31
#define GREEN 32
#define ORANGE 33
#define WHITE 37

UI::DebugConsolePanel::DebugConsolePanel()
{
}

void UI::DebugConsolePanel::Draw()
{
    ImGui::Begin("Console");

    UI::Style::PushStyle(ImGuiCol_Button, Color::EnigmaGrey2);
    if (ImGui::Button("Clear"))
    {
        Debug::GetMessages().clear();
    }
    UI::Style::PopStyle();

    ImGui::SameLine();

    ImGui::Checkbox("Auto-Scroll", &autoScroll);

    ImGui::Separator();

    ImGui::BeginChild("ScrollingRegion", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

    for (Debug::Message& message : Debug::GetMessages())
    {
        ImGuiStyle& style = ImGui::GetStyle();
        switch (message.color)
        {
            case GREEN:
                style.Colors[ImGuiCol_Text] = ImVec4(0.f, 1.f, 0.f, 1.f);
                break;
            case ORANGE:
                style.Colors[ImGuiCol_Text] = ImVec4(1.f, 0.5f, 0.f, 1.f);
                break;
            case RED:
                style.Colors[ImGuiCol_Text] = ImVec4(1.f, 0.f, 0.f, 1.f);
                break;
            case WHITE:
            default:
                style.Colors[ImGuiCol_Text] = ImVec4(1.f, 1.f, 1.f, 1.f);
                break;
        }
        ImGui::Text(message.content.c_str());
        style.Colors[ImGuiCol_Text] = ImVec4(1.f, 1.f, 1.f, 1.f);
    }

    if (ImGui::GetScrollMaxY() > 0.0f && autoScroll)
    {
        ImGui::SetScrollHereY(1.0f);
    }

    ImGui::EndChild();
    ImGui::End();
}
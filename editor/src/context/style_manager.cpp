#include "context/style_manager.h"

unsigned int UI::Style::popStyleCountMenuBar = 0;
unsigned int UI::Style::popStyleCount = 0;

void UI::Style::ApplyGeneralStyle()
{
	ImGui::StyleColorsDark();

	ImGuiStyle& style = ImGui::GetStyle();

	style.WindowRounding = 5.3f;
	style.FrameRounding = 2.3f;
	style.ScrollbarRounding = 0.0f;
	style.GrabRounding = 2.3f;

	ImVec4* colors = style.Colors;

    // BUTTON
	colors[ImGuiCol_Button]					    = Color::EnigmaDarkColor;
    colors[ImGuiCol_ButtonHovered]              = Color::MenuBarHovered;
    colors[ImGuiCol_ButtonActive]               = Color::EnigmaBaseColorDarker;

    // MENU BAR
    colors[ImGuiCol_MenuBarBg]                  = Color::MenuBarBackGround;

    // HEADER
    colors[ImGuiCol_Header]                     = Color::EnigmaGrey3; // Ex : Transform, MeshRenderer, etc...
	colors[ImGuiCol_HeaderHovered]			    = Color::EnigmaGrey3; // Like over but overeaded
	colors[ImGuiCol_HeaderActive]               = Color::EnigmaGrey4; // Like over but clicked

    // FRAME
	colors[ImGuiCol_FrameBg]				    = Color::EnigmaGrey1;
	colors[ImGuiCol_FrameBgHovered]			    = Color::EnigmaGrey2;
	colors[ImGuiCol_FrameBgActive]			    = Color::EnigmaBaseColorDarker;

    // WINDOW
	colors[ImGuiCol_WindowBg]				    = Color::EnigmaDarkColor;
	colors[ImGuiCol_TitleBg]                    = Color::EnigmaDarkColor;
    colors[ImGuiCol_TitleBgActive]              = Color::EnigmaDarkColor;
    colors[ImGuiCol_TitleBgCollapsed]           = Color::EnigmaDarkColor;
    colors[ImGuiCol_Tab]                        = Color::EnigmaDarkColor; // Other docked window and whole window is focused
    colors[ImGuiCol_TabActive]                  = Color::EnigmaBaseColorFull;
    colors[ImGuiCol_TabHovered]                 = Color::EnigmaBaseColorLighter;
    colors[ImGuiCol_TabSelected]                = Color::EnigmaBaseColorFull;
    colors[ImGuiCol_TabSelectedOverline]        = Color::EnigmaBaseColorLighter;
	colors[ImGuiCol_TabDimmed]                  = Color::EnigmaDarkColor; // Other docked window but whole window is not focused
    colors[ImGuiCol_TabDimmedSelected]          = Color::EnigmaBaseColorFull;
    colors[ImGuiCol_TabDimmedSelectedOverline]  = Color::EnigmaBaseColorLighter;

	// CHECK BOX
	colors[ImGuiCol_CheckMark]                  = Color::EnigmaBaseColorLighter;
    
    // DOCKING
    colors[ImGuiCol_DockingEmptyBg]             = Color::EnigmaDarkColor; // block of chere we can dock windows
	colors[ImGuiCol_DockingPreview]             = Color::EnigmaBaseColorDarker; // Preview where the window will be docked
    colors[ImGuiCol_NavWindowingHighlight]      = Color::EnigmaBaseColorLighter; //  Lines between blocks of docking

    // DRAG AND DROP
	colors[ImGuiCol_DragDropTarget]             = Color::EnigmaBaseColorLighter;

    // SLIDER
    colors[ImGuiCol_SliderGrab]                 = Color::EnigmaBaseColorFull;
    colors[ImGuiCol_SliderGrabActive]           = Color::EnigmaBaseColorLighter;
}

void UI::Style::PushStyle(ImGuiCol col, ImVec4 color)
{
	ImGui::PushStyleColor(col, color);
	popStyleCount++;
}

void UI::Style::PushStyleFrom(ImGuiCol col, ImGuiCol colToCopy)
{
    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4 color = style.Colors[colToCopy];
    ImGui::PushStyleColor(col, color);
	popStyleCount++;
}


void UI::Style::PopStyle(int n)
{
	ImGui::PopStyleColor(n);
	popStyleCount -= n;
}

void UI::Style::PopStyle()
{
	ImGui::PopStyleColor(popStyleCount);
	popStyleCount = 0;
}

void UI::Style::SetEnigmaFont()
{
    ImGuiIO& io = ImGui::GetIO();

    float fontSize = 22.0f;
    ImFont* hierarchyFont = io.Fonts->AddFontFromFileTTF("internal_assets/fonts/Inter-Regular.ttf", fontSize);

    ImGui::PushFont(hierarchyFont);
}

void UI::Style::SetConfigFile(const char* filePath)
{
    ImGuiIO& io = ImGui::GetIO();
	io.IniFilename = filePath;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
}

void UI::Style::PushMenuBarStyle()
{
	PushStyle(ImGuiCol_Button, Color::MenuBarBackGround);
    popStyleCountMenuBar = 1;
}

void UI::Style::PushInGameStyle()
{
    PushStyle(ImGuiCol_MenuBarBg, Color::EnigmaBaseColorFull);
    PushStyle(ImGuiCol_Button, Color::EnigmaBaseColorFull);
    PushStyle(ImGuiCol_ButtonHovered, Color::EnigmaBaseColorLighter);
    popStyleCountMenuBar = 3;
}

void UI::Style::PopMenuBarStyle()
{
	PopStyle(popStyleCountMenuBar);
    popStyleCountMenuBar = 0;
}

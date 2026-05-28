#pragma once
#include "emath/emath.h"
#include "imgui.h"

namespace UI
{
	struct Color
	{
		inline static ImVec4 MenuBarBackGround = ImVec4(0.14f, 0.14f, 0.14f, 1.f);
		inline static ImVec4 MenuBarHovered = ImVec4(0.282353f, 0.282353f, 0.282353f, 1.f);
		inline static ImVec4 EnigmaBaseColorLighter = ImVec4(0.047058f, 0.50196f, 0.3372549f, 0.6f);
		inline static ImVec4 EnigmaBaseColorFull = ImVec4(0.023529f, 0.25098f, 0.16862745f, 0.9f);
		inline static ImVec4 EnigmaBaseColorDarker = ImVec4(0.023529f, 0.25098f, 0.16862745f, 0.5f);
		inline static ImVec4 EnigmaDarkColor = ImVec4(0.06f, 0.06f, 0.06f, 0.94f);
		inline static ImVec4 EnigmaGrey1 = ImVec4(0.1f, 0.1f, 0.1f, 1.f);
		inline static ImVec4 EnigmaGrey2 = ImVec4(0.2f, 0.2f, 0.2f, 1.f);
		inline static ImVec4 EnigmaGrey3 = ImVec4(0.3f, 0.3f, 0.3f, 1.f);
		inline static ImVec4 EnigmaGrey4 = ImVec4(0.4f, 0.4f, 0.4f, 1.f);
		inline static ImVec4 EnigmaGrey5 = ImVec4(0.5f, 0.5f, 0.5f, 1.f);
	};

	struct Style
	{
		static void ApplyGeneralStyle();
		static void PushStyle(ImGuiCol col, ImVec4 color);
		static void PushStyleFrom(ImGuiCol col, ImGuiCol colToCopy);
		static void PopStyle(int n);
		static void PopStyle();
		static void SetEnigmaFont();
		static void SetConfigFile(const char* filePath);
		static void PushMenuBarStyle();
		static void PushInGameStyle();
		static void PopMenuBarStyle();

		static unsigned int popStyleCountMenuBar;
		static unsigned int popStyleCount;
	};
}
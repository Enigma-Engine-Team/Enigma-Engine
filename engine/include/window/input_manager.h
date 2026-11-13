#pragma once

#include "emath/emath.h"
#include "input.h"
#include "window.h"
#include "../utilities/macro.h"

class ENIGMA_API InputManager
{
public:

	static void SetCurrentWindow(Window* _window) { window = _window; };
	static void Update();

	static bool GetKey(const int key) { return window->GetKey(key) == EKeyState::PRESSED; }
	static bool GetKeyUp(const int key) { return window->GetKey(key) == EKeyState::RELEASED && previousKeys[key]; }
	static bool GetKeyDown(const int key) { return window->GetKey(key) == EKeyState::PRESSED && !previousKeys[key]; }
	static bool GetMouseButton(const int key) { return window->GetMouseButton(key) == EKeyState::PRESSED; }
	static bool GetMouseButtonUp(const int key) { return window->GetMouseButton(key) == EKeyState::RELEASED && previousMouse[key]; }
	static bool GetMouseButtonDown(const int key) { return window->GetMouseButton(key) == EKeyState::PRESSED && !previousMouse[key]; }

	static void MousePositionCallback(double xPos, double yPos);
	static void MouseScrollCallback(double xOffset, double yOffset);

	static bool IsCursorHidden();
	static void ShowCursor();
	static void HideCursor();
	static void SetCursorMode(ECursorMode cursorMode);

	static Math::Vector2D GetInversedGameMousePos();
	static Math::Vector2D ToScreenMousePos(Math::Vector2D pos);
	static Math::Vector2D ToGamePos(Math::Vector2D pos);
	static void UpdateGameRect(Math::Vector2D realtivePos, float width, float height);
	static Math::Vector2D GetMouseDelta();
	static void ResetMouseDelta();
	static Math::Vector2D GetMousePos() {return mousePos;};
	static Math::Vector2D GetGameMousePos() {return gameMousePos;};
	static Math::Vector2D GetMouseScroll() {return mouseScroll;};
	static bool IsScrolling() { return isScrolling; };
	static void SetIsScrolling(bool scrolling) { isScrolling = scrolling; };

	static void BlockInput();
	static void UnblockInput();
	static bool IsInputBlocked();

private:
	static inline Window* window = nullptr;
	static inline ECursorState cursorState = ECursorState::ENABLED;
	static inline Math::Vector2D mousePos = { 0.f, 0.f };
	static inline Math::Vector2D gameMousePos = { 0.f, 0.f };
	static inline Math::Vector2D mouseScroll = { 0.f, 0.f };
	static inline Math::Vector2D lastMousePos = { 0.f, 0.f };
	static inline std::array<bool, GLFW_KEY_LAST + 1> currentKeys {};
	static inline std::array<bool, GLFW_KEY_LAST + 1> previousKeys {};
	static inline std::array<bool, GLFW_MOUSE_BUTTON_LAST + 1> currentMouse {};
	static inline std::array<bool, GLFW_MOUSE_BUTTON_LAST + 1> previousMouse {};
	static inline float gameWidth = 0.f;
	static inline float gameHeight = 0.f;
	static inline bool isScrolling = false;
	static inline bool inputBlocked = false;
};
#pragma once

#include "emath/emath.h"
#include "input.h"
#include <unordered_map>
#include "window.h"
#include "../utilities/macro.h"

class ENIGMA_API InputManager
{
public:

	static void SetCurrentWindow(Window* _window) { window = _window; };

	static bool GetKeyUp(int key);
	static bool GetKeyDown(int key);
	static bool GetMouseButtonClicked(int key);
	static bool GetKeyClicked(int key);
	static bool GetMouseButtonDown(int key);
	static bool GetMouseButtonUp(int key);
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

private :
	static Window* window;
	static Math::Vector2D mousePos;
	static Math::Vector2D gameMousePos;
	static Math::Vector2D mouseScroll;
	static Math::Vector2D lastMousePos;
	static bool isScrolling;
	static std::unordered_map<int, bool> previousKeys;
	static float gameWidth;
	static float gameHeight;
	static ECursorState cursorState;
	static bool inputBlocked;
};
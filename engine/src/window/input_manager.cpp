#include "window/input_manager.h"
#include <iostream>

Math::Vector2D InputManager::mousePos = { 0.f, 0.f };
Math::Vector2D InputManager::gameMousePos = { 0.f, 0.f };
Math::Vector2D InputManager::mouseScroll = { 0.f, 0.f };
Math::Vector2D InputManager::lastMousePos = { 0.f, 0.f };
float InputManager::gameWidth = 0.f;
float InputManager::gameHeight = 0.f;
ECursorState InputManager::cursorState = ECursorState::ENABLED;
bool InputManager::isScrolling = false;
bool InputManager::inputBlocked = false;
Window* InputManager::window = nullptr;
std::unordered_map<int, bool> InputManager::previousKeys{};

bool InputManager::GetKeyUp(int key)
{
	return window->GetKey(key) == EKeyState::RELEASED;
}

bool InputManager::GetKeyDown(int key)
{
	return window->GetKey(key) == EKeyState::PRESSED;
}

bool InputManager::GetMouseButtonClicked(int key)
{
	bool current = window->GetMouseButton(key) == EKeyState::PRESSED;
	bool previous = previousKeys[key];

	previousKeys[key] = current;

	return current && !previous;
}

bool InputManager::GetKeyClicked(int key)
{
	bool current = window->GetKey(key) == EKeyState::PRESSED;
	bool previous = previousKeys[key];

	previousKeys[key] = current;

	return current && !previous;
}

bool InputManager::GetMouseButtonDown(int key)
{
	return window->GetMouseButton(key) == EKeyState::PRESSED;
}

bool InputManager::GetMouseButtonUp(int key)
{
	return window->GetMouseButton(key) == EKeyState::RELEASED;
}

void InputManager::MousePositionCallback(double xPos, double yPos)
{
	mousePos.x = static_cast<float>(xPos);
	mousePos.y = static_cast<float>(yPos);
}

void InputManager::MouseScrollCallback(double xOffset, double yOffset)
{
	mouseScroll.x = static_cast<float>(xOffset);
	mouseScroll.y = static_cast<float>(yOffset);

	isScrolling = true;
}

void InputManager::ShowCursor()
{
	window->SetCursorState(ECursorState::ENABLED);
	cursorState = ECursorState::ENABLED;
	ResetMouseDelta();
}

bool InputManager::IsCursorHidden()
{
	return cursorState == ECursorState::DISABLED;
}

void InputManager::HideCursor()
{
	window->SetCursorState(ECursorState::DISABLED);
	cursorState = ECursorState::DISABLED;
	ResetMouseDelta();
}

void InputManager::SetCursorMode(ECursorMode cursorMode)
{
	window->SetCursorMode(cursorMode);
}

Math::Vector2D InputManager::GetInversedGameMousePos()
{
	return Math::Vector2D(gameMousePos.x, gameHeight - gameMousePos.y);
}

void InputManager::UpdateGameRect(Math::Vector2D relativePos, float width, float height)
{
	gameWidth = width;
	gameHeight = height;
	gameMousePos = relativePos;
}

Math::Vector2D InputManager::ToScreenMousePos(Math::Vector2D pos)
{
	Math::Vector2D ratio = gameMousePos / mousePos;
	return pos * ratio;
}

Math::Vector2D InputManager::ToGamePos(Math::Vector2D pos)
{
	Math::Vector2D ratio = Math::Vector2D(1920.f, 1080.f) / Math::Vector2D(gameWidth, gameHeight);
	return pos / ratio;
}


Math::Vector2D InputManager::GetMouseDelta()
{
	Math::Vector2D delta = mousePos - lastMousePos;
	lastMousePos = mousePos;
	return delta;
}

void InputManager::ResetMouseDelta()
{
	lastMousePos = mousePos;
}

void InputManager::BlockInput()
{
	inputBlocked = true;
}

void InputManager::UnblockInput()
{
	inputBlocked = false;
}

bool InputManager::IsInputBlocked()
{
	return inputBlocked;
}
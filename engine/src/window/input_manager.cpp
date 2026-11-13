#include "window/input_manager.h"
#include <iostream>

void InputManager::Update()
{
	if (window == nullptr)
	{
		std::cerr << "InputManager::Update: window is nullptr" << std::endl;
		return;
	}

	if (IsCursorHidden())
		ResetMouseDelta();

	if (isScrolling)
	{
		mouseScroll = { 0.f, 0.f };
		isScrolling = false;
	}

	previousKeys = currentKeys;
	previousMouse = currentMouse;

	for (int i = 0; i < GLFW_KEY_LAST + 1; i++)
		currentKeys[i] = window->GetKey(i) == EKeyState::PRESSED;
	for (int i = 0; i < GLFW_MOUSE_BUTTON_LAST + 1; i++)
		currentMouse[i] = window->GetMouseButton(i) == EKeyState::PRESSED;
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
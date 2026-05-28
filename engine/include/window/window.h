#pragma once

#include <array>
#include <GLFW/glfw3.h>
#include <functional>
#include "../utilities/macro.h"

enum EKeyState
{
	RELEASED = GLFW_RELEASE,
	PRESSED = GLFW_PRESS,
	REPEAT = GLFW_REPEAT
};

enum ECursorState
{
	ENABLED = GLFW_CURSOR_NORMAL,
	DISABLED = GLFW_CURSOR_DISABLED
};

enum ECursorMode
{
	CLASSIC,
	HORIZONTAL_RESIZE
};

class ENIGMA_API Window
{
public:
	Window() = default;
	void Create(int width, int height, const char* title);
	void Destroy();

	void SwapBuffers();
	void PollEvents();
	void SetFrameBufferSizeCallBack(std::function<void(int, int)> callback);
	void SetWindowSizeCallBack(std::function<void(int, int)> callback);
	void SetCursorPosCallBack(std::function<void(double, double)> callback);
	void SetScrollCallBack(std::function<void(double, double)> callback);
	void SetWindowShouldClose(bool shouldClose);
	void SetCursorState(ECursorState cursorState);
	void SetCursorMode(ECursorMode cursorMode);
	void SetWindowIcon(const char* path);
	std::array<int, 2> GetFrameBufferSize();
	std::array<int, 2> GetWindowSize();
	bool ShouldClose();
	int GetKey(int key);
	int GetMouseButton(int key);
	static float GetTime();

	GLFWwindow* GetWindow() { return window; };

private:
	static void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
	static void WindowSizeCallback(GLFWwindow* window, int width, int height);
	static void CursorPosCallback(GLFWwindow* window, double xPos, double yPos);
	static void ScrollCallback(GLFWwindow* window, double xDelta, double yDelta);

	GLFWwindow* window = nullptr;
	GLFWcursor* resizeHCursor = nullptr;
	GLFWcursor* arrowCursor = nullptr;
	std::function<void(int, int)> framebufferCallback;
	std::function<void(int, int)> windowCallback;
	std::function<void(double, double)> cursorCallback;
	std::function<void(double, double)> scrollCallback;
};
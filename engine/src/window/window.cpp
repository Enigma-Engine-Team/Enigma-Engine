#include "window/window.h"
#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "renderer/stb_image.h"

void Window::Create(int width, int height, const char* title)
{
	if (!glfwInit())
		glfwTerminate();

	window = glfwCreateWindow(width, height, title, NULL, NULL);
	if (!window)
		Destroy();

	glfwMakeContextCurrent(window);
	glfwSwapInterval(0);
	glfwSetWindowUserPointer(window, this);
	glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);
	glfwSetWindowSizeCallback(window, WindowSizeCallback);
	glfwSetCursorPosCallback(window, CursorPosCallback);
	glfwSetScrollCallback(window, ScrollCallback);
	glfwSetInputMode(window, GLFW_CURSOR, ECursorState::ENABLED);

	resizeHCursor = glfwCreateStandardCursor(GLFW_HRESIZE_CURSOR);
	arrowCursor = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
}

void Window::Destroy()
{
	glfwDestroyCursor(resizeHCursor);
	glfwDestroyCursor(arrowCursor);
	glfwDestroyWindow(window);
	glfwTerminate();
}

void Window::SwapBuffers()
{
	glfwSwapBuffers(window);
}

void Window::PollEvents()
{
	glfwPollEvents();
}

void Window::SetFrameBufferSizeCallBack(std::function<void(int, int)> callback)
{
	framebufferCallback = callback;
}

void Window::SetWindowSizeCallBack(std::function<void(int, int)> callback)
{
	windowCallback = callback;
}

void Window::SetCursorPosCallBack(std::function<void(double, double)> callback)
{
	cursorCallback = callback;
}

void Window::SetScrollCallBack(std::function<void(double, double)> callback)
{
	scrollCallback = callback;
}

void Window::SetWindowShouldClose(bool shouldClose)
{
	glfwSetWindowShouldClose(window, shouldClose);
}

void Window::SetCursorState(ECursorState cursorState)
{
	glfwSetInputMode(window, GLFW_CURSOR, cursorState);
}

void Window::SetCursorMode(ECursorMode cursorMode)
{
	switch (cursorMode)
	{
	case ECursorMode::HORIZONTAL_RESIZE:
		glfwSetCursor(window, resizeHCursor);
		break;
	case ECursorMode::CLASSIC:
		glfwSetCursor(window, arrowCursor);
		break;
	default:
		glfwSetCursor(window, arrowCursor);
		break;
	}
}

void Window::SetWindowIcon(const char* path)
{
	int width, height, channels;
	unsigned char* pixels = stbi_load(path, &width, &height, &channels, 4);

	if (!pixels)
		return;

	GLFWimage image {};
	image.width = width;
	image.height = height;
	image.pixels = pixels;

	glfwSetWindowIcon(window, 1, &image);

	stbi_image_free(pixels);
}

std::array<int, 2> Window::GetFrameBufferSize()
{
	int w, h;
	glfwGetFramebufferSize(window, &w, &h);
	return { w, h };
}

std::array<int, 2> Window::GetWindowSize()
{
	int w, h;
	glfwGetWindowSize(window, &w, &h);
	return { w, h };
}

bool Window::ShouldClose()
{
	return glfwWindowShouldClose(window);
}

float Window::GetTime()
{
	return static_cast<float>(glfwGetTime());
}

int Window::GetKey(int key)
{
	return glfwGetKey(window, key);
}

int Window::GetMouseButton(int key)
{
	return glfwGetMouseButton(window, key);
}

void Window::FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	Window* self = static_cast<Window*>(glfwGetWindowUserPointer(window));

	if (self && self->framebufferCallback)
		self->framebufferCallback(width, height);
}

void Window::WindowSizeCallback(GLFWwindow* window, int width, int height)
{
	Window* self = static_cast<Window*>(glfwGetWindowUserPointer(window));

	if (self && self->windowCallback)
		self->windowCallback(width, height);
}

void Window::CursorPosCallback(GLFWwindow* window, double xPos, double yPos)
{
	Window* self = static_cast<Window*>(glfwGetWindowUserPointer(window));

	if (self && self->cursorCallback)
		self->cursorCallback(xPos, yPos);
}

void Window::ScrollCallback(GLFWwindow* window, double xDelta, double yDelta)
{
	Window* self = static_cast<Window*>(glfwGetWindowUserPointer(window));

	if (self && self->scrollCallback)
		self->scrollCallback(xDelta, yDelta);
}

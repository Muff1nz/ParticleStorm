#include "Window.h"


void Window::InitWindow(int height, int width) {
	this->fullscreen = false;

	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

	monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(monitor);
	glfwWindowHint(GLFW_RED_BITS, mode->redBits);
	glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
	glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
	glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

	screenWidth = mode->width;
	screenHeight = mode->height;
	refreshRate = mode->refreshRate;

	if (fullscreen) {
		window = glfwCreateWindow(mode->width, mode->height, "Particle Storm", glfwGetPrimaryMonitor(), nullptr);
		glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);

		this->windowHeight = mode->height;
		this->windowWidth = mode->width;
	} else {
		this->windowHeight = height;
		this->windowWidth = width;
		window = glfwCreateWindow(width, height, "Particle Storm", nullptr, nullptr);
	}

	glfwGetWindowPos(window, &posX, &posy);

	isDisposed = false;
}

void Window::Dispose() {
	if (isDisposed)
		return;

	glfwDestroyWindow(window);
	glfwTerminate();

	isDisposed = true;
}

void Window::UpdateMetaData() {
	if (!fullscreen) {
		glfwGetFramebufferSize(window, &windowWidth, &windowHeight);
		glfwGetWindowPos(window, &posX, &posy);
	}
}

void Window::ToggleFullscreen() {
	fullscreen = !fullscreen;
	if (fullscreen)
		glfwSetWindowMonitor(window, monitor, 0, 0, screenWidth, screenHeight, refreshRate);
	else
		glfwSetWindowMonitor(window, nullptr, posX, posy, windowWidth, windowHeight, refreshRate);
}

void Window::SetSize(int windowWidth, int windowHeight) {
	this->windowWidth = windowWidth;
	this->windowHeight = windowHeight;
	
	if (fullscreen)
		return;
	
	glfwSetWindowSize(window, windowWidth, windowWidth);
}

int Window::GetHeight() const {
	return fullscreen ? screenHeight : windowHeight;
}

int Window::GetWidth() const {
	return fullscreen ? screenWidth : windowWidth;
}

bool Window::IsFullscreen() const {
	return fullscreen;
}

bool Window::IsMinimized() const {
	int h, w;
	glfwGetFramebufferSize(window, &h, &w);
	return h == 0 || w == 0;
}

GLFWwindow* Window::GetWindow() const {
	return window;
}

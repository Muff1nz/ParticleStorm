#include "Window.h"


void Window::InitWindow(int height, int width, bool fullscreen) {
	this->height = height;
	this->width = width;
	this->fullscreen = fullscreen;

	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	if (fullscreen) {
		auto monitor = glfwGetPrimaryMonitor();

		const GLFWvidmode* mode = glfwGetVideoMode(monitor);
		glfwWindowHint(GLFW_RED_BITS, mode->redBits);
		glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
		glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
		glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

		//TODO: Detect actual screen resolution
		window = glfwCreateWindow(2560, 1440, "Particle Storm", glfwGetPrimaryMonitor(), nullptr);

		glfwSetWindowMonitor(window, monitor, 0, 0, 2560, 1440, mode->refreshRate);
	} else
		window = glfwCreateWindow(width, height, "Particle Storm", nullptr, nullptr);
}

int Window::GetHeight() {
	return height;
}

int Window::GetWidth() {
	return width;
}

int Window::IsFullscreen() {
	return fullscreen;
}

GLFWwindow* Window::GetWindow() {
	return window;
}

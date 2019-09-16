#include "Window.h"


void Window::InitWindow(int height, int width, bool fullscreen) {
	this->fullscreen = fullscreen;

	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
	if (fullscreen) {
		auto monitor = glfwGetPrimaryMonitor();

		const GLFWvidmode* mode = glfwGetVideoMode(monitor);
		glfwWindowHint(GLFW_RED_BITS, mode->redBits);
		glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
		glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
		glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

		//TODO: Detect actual screen resolution
		window = glfwCreateWindow(mode->width, mode->height, "Particle Storm", glfwGetPrimaryMonitor(), nullptr);
		glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);

		this->height = mode->height;
		this->width = mode->width;
	} else {
		this->height = height;
		this->width = width;
		window = glfwCreateWindow(width, height, "Particle Storm", nullptr, nullptr);
	}

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
	glfwGetFramebufferSize(window, &width, &height);
}

int Window::GetHeight() const {
	return height;
}

int Window::GetWidth() const {
	return width;
}

int Window::IsFullscreen() const {
	return fullscreen;
}

GLFWwindow* Window::GetWindow() const {
	return window;
}

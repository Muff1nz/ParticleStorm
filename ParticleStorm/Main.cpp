#include <SDL2/SDL.h>
#include <string> 
#include <iostream>
#include <thread>
#include <cctype>
#include "SessionManager.h"


#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <vec4.hpp>
#include <mat4x4.hpp>

#include <iostream>
#include "RenderEngineVulkan.h"

void VulkanGLFWTest() {
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	GLFWwindow* window = glfwCreateWindow(800, 600, "Vulkan window", nullptr, nullptr);

	uint32_t extensionCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

	std::cout << extensionCount << " extensions supported" << std::endl;

	glm::mat4 matrix;
	glm::vec4 vec;
	auto test = matrix * vec;

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
	}

	glfwDestroyWindow(window);

	glfwTerminate();
}

void PrintMenu() {
	std::cout << "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n";
	std::cout << "ParticleStorm Main Menu:\n";	
	std::cout << "Sandbox(1):\n";
	std::cout << "Benchmark(2)\n";
	std::cout << "VulkanGLFWTest(3)\n";
	std::cout << "RenderEngineVulkanTest(4)\n";
	std::cout << "Quit(X)\n";
}

void MainMenu() {
	SessionManager session;

	char userInput = 'N';
	do {
		PrintMenu();
		userInput = getchar();
		std::cin.ignore();
		switch (std::toupper(userInput)) {
		case '1':
			session.Sandbox();
			break;
		case '2':
			session.Benchmark();
			break;
		case '3':
			VulkanGLFWTest();
			break;
		case '4':
			session.VulkanTest();
			break;
		case 'X':
			break;
		default:
			std::cout << "\nInvalid option!\n";
			break;
		}
	} while (std::toupper(userInput) != 'X');
}


int main(int argc, char* args[]) {
	MainMenu();
	return 0;
}



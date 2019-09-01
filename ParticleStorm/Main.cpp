#include <iostream>
#include <cctype>
#include "SessionManager.h"


void PrintMenu() {
	std::cout << "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n";
	std::cout << "ParticleStorm Main Menu:\n";	
	std::cout << "Sandbox(1):\n";
	std::cout << "Physics Benchmark(2)\n";
	std::cout << "Graphics Benchmark(3)\n";
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
			session.PhysBench();
			break;
		case '3':
			session.GraphBench();
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
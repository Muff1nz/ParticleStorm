#include <iostream>
#include <cctype>
#include "SessionManager.h"


void PrintMenu() {
	std::cout << "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n";
	std::cout << "ParticleStorm Main Menu:\n";	
	std::cout << "Sandbox(1):\n";
	std::cout << "Benchmark(2)\n";
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
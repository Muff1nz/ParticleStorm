#include <iostream>
#include "Test.h"
#include "Main.h"
using namespace std;


void TestFunc(Test* test) {
	test->changeMe = 7;
}

int main() {
	cout << "IS THIS C++?";
	cout << std::endl;

	Test test = Test();
	cout << test.GetSecretString() + " " + std::to_string(test.changeMe);
	TestFunc(&test);
	cout << test.GetSecretString() + " " + std::to_string(test.changeMe);
	getchar();
	return 1;
}




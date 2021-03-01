
#include <iostream>

using namespace std;

int main (int argc, char** argv)
{
	cout << "Welcome to the Greeter!\n"
	     << "Enter your name: "
	     << flush;

	string name;
	cin >> name;

	cout << "Hello, " << name << "!\n"
	     << "Good-bye!" << endl;

	return 0;
}


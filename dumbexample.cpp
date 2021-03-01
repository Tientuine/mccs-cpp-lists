#include <iostream>

using namespace std;

int main (int argc, char** argv)
{
	typedef int* int_ptr;

	int x = 5; // allocated on "the stack"

	int_ptr y = new int {3}; // allocated on "the heap"

	cout << "x = " << x << endl;
	cout << "y = " << *y << endl;

	delete y;

	return 0; // oh, snap! memory leak, bruh!
}


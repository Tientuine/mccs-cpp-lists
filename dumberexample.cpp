#include <iostream>

using namespace std;

class DopeClass
{
public:
	DopeClass (int n) : _data(new int[n]) {}

	~DopeClass () { delete[] _data; }

	int firstVal() { return _data[0]; }

private:
	int* _data;
};

int main (int argc, char** argv)
{
	DopeClass x {50}; // allocated on "the stack"

	DopeClass* y = new DopeClass {100}; // allocated on "the heap"

	cout << "x = " << x.firstVal() << endl;
	cout << "y = " << y->firstVal() << endl;

	delete y;

	return 0; // oh, shit! memory leak, brah!
}


#include <iostream>
using namespace std;

void func()
{
	cerr << "EMPTY" << endl;
}

template <class A, class ...B> void func(A argHead, B... argTail)
{
	cerr << "A: " << argHead << endl;
	func(argTail...);
}


int main(void)
{
	func(1,2,3,4,5,6);
	return 0;
}

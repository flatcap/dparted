#include <iostream>
#include <functional>

class X;
typedef std::function<int(X&, double)> fn_proto;

class X {
public:
	int x;
	int wibble (double d) { return x*d; }
};

int main()
{
	fn_proto e = &X::wibble;

	X x = {33};

	std::cout << "e() = " << e(x,2.3) << '\n';
}


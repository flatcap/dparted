#include <functional>
#include <iostream>

template <class T, class R, class... Args>
void connect(const T& t, std::function<R(const T&, Args...)> f)
{
	std::function<R(Args...)> fun = [&t,f](Args... args){ f(t,args...); };
	fun(42, 3.14f);
}

class X
{
public:
	void f(int, float) const
	{
		std::cout << "X::f\n";
	}
};

int main()
{
	std::function<void(X const&, int, float)> f = &X::f;
	X x;
	connect(x, f);
}


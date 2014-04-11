#include <iostream>
using namespace std;

void output()
{
	cout << '\n';
}

template <typename T, typename ...P>
void output(T t, P ...p)
{
	cout << t << ' ';
	if (sizeof...(p)) { output(p...); }
	else { cout << '\n'; }
}

template <typename T>
T sum(T t)
{
	return(t);
}

template <typename T, typename ...P>
T sum(T t, P ...p)
{
	if (sizeof...(p)) {
		t += sum(p...);
	}
	return(t);
}

int main()
{
	output();
	output('5');
	output('5', 2);
	output('5', 2, "cows");
	output('5', 2, "cows", -1);
	output('5', 2, "cows", -1, 0.5f);
	output('5', 2, "cows", -1, 0.5f, 16.3);
	cout << endl;

	cout << sum(1) << '\n'
		<< sum(1, 2) << '\n'
		<< sum(1, 2, 3) << '\n'
		<< sum(1, 2, 3, 4) << '\n'
		<< sum(1, 2, 3, 4, 5) << '\n';
	cout << endl;

	cout << sum(0.1) << '\n'
		<< sum(0.1, 0.2) << '\n'
		<< sum(0.1, 0.2, 0.3) << '\n';
	cout << endl;
}


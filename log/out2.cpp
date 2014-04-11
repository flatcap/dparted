
void output()
{
	cout << '\n';
}

template <typename T, typename ...P>
void output(T t, P ...p)
{
	cout << t << ' ';
	output(p...);
}


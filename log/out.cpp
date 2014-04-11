template<typename T>
void output(T t)
{
	cout << t << endl;
}

template <typename T, typename ...P>
void output(T t, P ...p)
{
	cout << t << ' ';
	output(p...);
}


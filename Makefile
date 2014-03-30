
main:	tree.cpp
	g++ -g -std=c++11 -Wall -Wextra tree.cpp -o main $(shell pkg-config --cflags --libs gtkmm-3.0)
	main&

clean:
	rm -f main

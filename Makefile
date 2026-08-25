build:
	g++ -std=c++23 -o net main.cpp socket.cpp -I.
debug-build:
	g++ -std=c++23 -O0 -g -o net main.cpp
run:
	./net

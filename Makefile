build:
	clang++ -std=c++23 -o net main.cpp -g
debug-build:
	clang++ -std=c++23 -O0 -g -o net main.cpp
run:
	./net

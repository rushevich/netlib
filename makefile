all: build
deps:
	cmake -B build
build: deps
	cmake --build build

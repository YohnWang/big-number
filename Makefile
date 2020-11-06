
all: a.exe
	./a.exe
a.exe:main.cpp  bgn.cpp Makefile bgn.h test.cpp
	g++ -std=c++17 -O3 -I . -m64 -Wno-literal-suffix -Werror=return-type test.cpp bgn.cpp -o a.exe
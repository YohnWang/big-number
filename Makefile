
all: a.exe
	./a.exe
a.exe:main.cpp  bgn.cpp Makefile bgn.h
	g++ -O3 -I . -m64 -Wno-literal-suffix -Werror=return-type main.cpp bgn.cpp -o a.exe
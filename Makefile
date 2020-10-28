
all: a.exe
	./a.exe
a.exe: bgn.cpp Makefile
	g++  -Wno-literal-suffix -Werror=return-type  bgn.cpp -o a.exe
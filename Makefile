
all: a.exe
	./a.exe
a.exe: bgn.cpp Makefile
	g++ -O2 -m64 -Wno-literal-suffix -Werror=return-type  bgn.cpp -o a.exe
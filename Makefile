default: mandelbrox

mandelbrox: mandelbrox.c Makefile
	cc -pthread -O3 -o mandelbrox mandelbrox.c -lm

static: mandelbrox.c Makefile
	cc -pthread -O3 -static -o mandelbrox-static mandelbrox.c -lm

clean:
	@rm -vf mandelbrox mandelbrox-static

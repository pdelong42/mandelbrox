default: mandelbrox

mandelbrox: mandelbrox.c
	cc -O3 -o mandelbrox mandelbrox.c -lm

static: mandelbrox.c
	cc -O3 -static -o mandelbrox-static mandelbrox.c -lm

clean:
	@rm -vf mandelbrox mandelbrox-static

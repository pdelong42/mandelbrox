default: mandelbrox

mandelbrox: mandelbrox.c
	cc -o mandelbrox mandelbrox.c -lm

static: mandelbrox.c
	cc -static -o mandelbrox-static mandelbrox.c -lm

clean:
	@rm -vf mandelbrox mandelbrox-static

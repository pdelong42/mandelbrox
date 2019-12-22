default: mandelbrox

mandelbrox: mandelbrox.c
	cc -o mandelbrox mandelbrox.c

static: mandelbrox.c
	cc -static -o mandelbrox-static mandelbrox.c

clean:
	@rm -vf mandelbrox mandelbrox-static

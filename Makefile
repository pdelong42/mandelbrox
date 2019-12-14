default: mandelbrox

mandelbrox: mandelbrox.c
	cc -o mandelbrox mandelbrox.c

clean:
	rm -f mandelbrox

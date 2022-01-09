all: build
build:
	gcc -Wcpp main.c -o UnixLs

clean:
	-rm -f UnixLs
CC=gcc
CFLAGS=--std=c11 -Wall -Werror -Wextra -g3 -Ilibs/equation -Ilibs/libtiff -Llibs/equation -Llibs/libtiff/.libs -Wno-error=unused-command-line-argument

all: build

build: libbuild imgread.o
	$(CC) $(CFLAGS) imgread.o -ltiff -lequation -o imgread

libbuild:
	make -C libs/equation library

clean:
	make -C libs/equation clean
	rm -vf *.o
	rm -vf *.so
	rm -vf imgread

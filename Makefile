CC=gcc
CFLAGS=--std=c11 -Wall -Werror -Wextra -g3

all: build

build: libbuild imgread.o
	$(CC) $(CFLAGS) imgread.o -ltiff -L. -lequation -o imgread

libbuild:
	make -C libs/equation library
	mv libs/equation/libequation.so ./

clean:
	make -C libs/equation clean
	rm -vf *.o
	rm -vf *.so
	rm -vf imgread

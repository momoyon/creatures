CC=gcc
CFLAGS=-Wall -Wextra -I./include -I./raylib-5.0_win64_mingw-w64/include/ -Wswitch-enum -Werror=switch-enum -Wno-char-subscripts -Wno-sign-compare -Wno-type-limits
LDFLAGS=-L./raylib-5.0_win64_mingw-w64/lib
LIBS=-lraylib -lgdi32 -lwinmm

creatures: src/*.c
	$(CC) $(CFLAGS) -O2 -o $@ src/*.c $(LDFLAGS) $(LIBS)

debug: src/*.c
	$(CC) $(CFLAGS) -ggdb -DDEBUG=1 -o creatures-debug src/*.c $(LDFLAGS) $(LIBS)

all: creatures debug test_ipv4

clean:
	rm *.exe

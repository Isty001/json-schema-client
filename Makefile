SOURCES = main.c $(shell find src -name '*.c') lib/*.c
TEST_SOURCES = $(shell find src test -name '*.c' ! -name curl.c) lib/*.c

default:
	make compile src="$(SOURCES)" out="client.o"

run:
	make
	./client.o

compile:
	gcc -Wall -Wextra -std=gnu11 -g $(src) -o $(out) -lncurses -lmenu -lform -lcurl -lpcre -D _GNU_SOURCE

test:
	make compile src="$(TEST_SOURCES) -D TEST" out="test.o"
	./test.o

test-valgrind:
	make test
	valgrind --track-origins=yes --leak-check=full --show-reachable=yes ./test.o

.PHONY: test

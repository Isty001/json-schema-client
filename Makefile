COMMON_SOURCES = $(shell find src -name '*.c' ! -name 'curl.c') lib/*.c
APP = main.c $(COMMON_SOURCES) src/curl.c
TEST_SOURCES = $(COMMON_SOURCES) test/*.c test/mock/*.c
TEST_NCURSES_SOURCES = $(COMMON_SOURCES) test/ncurses/*.c test/mock/*.c

default:
	make compile src="$(APP)" out="client.o"

run:
	make
	./client.o

compile:
	gcc -Wall -Wextra -std=gnu11 -g $(src) -o $(out) -lncurses -lmenu -lform -lcurl -lpcre -D _GNU_SOURCE

compile-test:
	make compile src="$(TEST_SOURCES) -D TEST" out="test.o"

test:
	make compile-test
	./test.o

test-valgrind:
	make test
	valgrind --track-origins=yes --leak-check=full --show-reachable=yes ./test.o

test-ncurses:
	make compile src="$(TEST_NCURSES_SOURCES) -D TEST" out="test_ncurses.o"
	./test_ncurses.o

.PHONY: test

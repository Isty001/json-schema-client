SOURCES = main.c $(shell find src -name '*.c') lib/*.c
TEST_SOURCES = $(shell find src test -name '*.c' ! -name curl.c) lib/*.c

compile:
	gcc -Wall -Wextra -g $(src) -o $(out) -lncurses -lmenu -lform -lcurl -lpcre -D _GNU_SOURCE

run:
	make compile src="$(SOURCES)" out="client.o"
	./client.o

test:
	make compile src="$(TEST_SOURCES) -D TEST" out="test.o"
	./test.o

test-valgrind:
	make test
	valgrind --track-origins=yes --leak-check=full --show-leak-kinds=all ./test.o

.PHONY: test

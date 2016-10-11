SOURCES = main.c $(shell find src -name '*.c') lib/*.c
TEST_SOURCES = $(shell find src test -name '*.c' ! -name curl.c) lib/*.c

compile = $(shell gcc -Wall -Wextra -g $1 -o $2 -lncurses -lmenu -lform -lcurl -lpcre -D _GNU_SOURCE)

run: $(call compile,$(SOURCES),client.o)
	./client.o

test: $(call compile,$(TEST_SOURCES) -D TEST,test.o)
	./test.o

test-valgrind:
	make test
	valgrind --track-origins=yes --leak-check=full --show-leak-kinds=all ./test.o

.PHONY: test

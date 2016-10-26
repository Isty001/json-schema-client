COMMON_SOURCES = $(shell find src -name '*.c' ! -name 'curl.c') lib/*.c
APP = main.c $(COMMON_SOURCES) src/curl.c

TEST_SOURCES = $(COMMON_SOURCES) test/*.c test/mock/*.c
TEST_NCURSES_SOURCES = $(COMMON_SOURCES) test/ncurses/*.c test/mock/*.c

STORAGE_DIR = $(shell realpath ~/schema-client/storage)
STORAGE_FILE = storage.txt

EXECUTABLE_TARGET = /usr/bin

ENVIRONMENT = -D _GNU_SOURCE -D STORAGE_DIR=\"$(STORAGE_DIR)\" -D STORAGE_FILE=\"$(STORAGE_FILE)\"


default:
	make compile src="$(APP)" out="schema-client"
	make install

run:
	make
	schema-client

setup-env:
	mkdir -m 777 -p $(STORAGE_DIR)
	touch $(STORAGE_DIR)/storage.txt

compile:
	make setup-env
	gcc -Wall -Wextra -std=gnu11 -g $(src) -o $(out) -lncurses -lmenu -lform -lcurl -lpcre $(ENVIRONMENT)

compile-test:
	make compile src="$(TEST_SOURCES) -D TEST" out="test.o"

install:
	sudo cp schema-client $(EXECUTABLE_TARGET)

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

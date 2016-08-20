CC=gcc
CC_FLAGS=-Wall -Wextra -Wpedantic -std=gnu11 -g
#CC_FLAGS=-Wall -Wextra -Wpedantic -std=gnu11 -g -ftest-coverage -fprofile-arcs

.PHONY: test_utf8
test_utf8: test/main.c utf8.o
	$(CC) $^ -o $@ $(CC_FLAGS) -Iinclude
	./test_utf8

utf8.o: src/utf8.c include/utf8.h
	$(CC) $< -c -o $@ $(CC_FLAGS) -Iinclude

.PHONY: clean
clean:
	rm -f utf8.o test_utf8

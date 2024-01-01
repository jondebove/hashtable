.POSIX:

CFLAGS = -Wall -Wextra
TESTS = hashtable_test

.PHONY: all clean

all: $(TESTS)

hashtable_test: hashtable_test.c hashtable.h

clean:
	-rm -f $(TESTS)

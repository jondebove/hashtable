
CFLAGS = -std=c99 -O2 -Wall -Wextra

hashtable_test: hashtable_test.c hashtable.h

.PHONY: clean
clean:
	-$(RM) hashtable_test

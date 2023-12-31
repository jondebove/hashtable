#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "hashtable.h"

struct entry {
	int key;
	int val;
	HASH_ENTRY(entry) entries;
};

HASH_TABLE(table, entry);

#define hash(k) ((unsigned int)k)
#define equal(e, k) (e->key == k)

int main(void)
{
	struct entry *e, *ep;
	unsigned int i;
	int k;

	unsigned int shift = 3;
	struct table *ht;					/* Hash table */
	ht = malloc(HASH_TABLE_SIZE(table, shift));
	HASH_INIT(ht, shift);					/* Initialization */
	assert(HASH_SIZE(ht) == 8);

	for (i = 0; i < 10; i++) {
		e = malloc(sizeof(struct entry));
		e->key = i * i;
		HASH_INSERT(ht, e, hash(e->key), entries);	/* Insertion */
	}

	HASH_FOREACH(ep, i, ht, entries)			/* Traversal */
		ep->val = i;

	k = 9;
	HASH_SEARCH_FOREACH(ep, hash(k), ht, entries)		/* Search */
		if (equal(ep, k))
			break;

	if (ep) {
		HASH_REMOVE(ep, entries);			/* Deletion */
		free(ep);
	}

	HASH_FOREACH(ep, i, ht, entries)			/* Traversal */
		printf("key=%d, val=%d\n", ep->key, ep->val);

	HASH_FOREACH_SAFE(ep, i, ht, entries, e)		/* Hash table deletion */
		free(ep);
	free(ht);

	return EXIT_SUCCESS;
}

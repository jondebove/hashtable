#include <stdio.h>
#include <stdlib.h>

#include "hashtable.h"

struct entry {
	int key;
	int val;
	struct hashnode node;
};
#define hash(k) ((uint64_t)(k))
#define entry_of(ptr) \
	((struct entry *)((char *)(ptr) - offsetof(struct entry, node)))

int main(void)
{
	struct entry *e;
	int i, k;

	/* Hash table */
	struct hashtable ht;

	/* Initialization */
	struct hashnode buckets[32];
	if (!hashtable_init(&ht, 32, buckets)) {
		fprintf(stderr, "error while initializing hashtable\n");
		return EXIT_FAILURE;
	}

	/* Insertion */
	for (i = 0; i < 10; i++) {
		e = malloc(sizeof(*e));
		e->key = i * i;
		e->val = i;
		hashtable_insert(&ht, &e->node, hash(e->key));
	}

	/* Search */
	struct hashnode *hn;
	k = 9;
	for (hn = hashtable_first(&ht, hash(k)); hn; hn = hn->next) {
		e = entry_of(hn);
		if (e->key == k) {
			break;
		}
	}

	/* Deletion */
	if (hn) {
		hashtable_remove(&ht, hn, k);
		free(e);
	}

	/* Traversal */
	struct hashiter hi;
	for (hn = hashiter_first(&hi, &ht); hn; hn = hashiter_next(&hi)) {
		e = entry_of(hn);
		printf("key=%d, val=%d\n", e->key, e->val);
	}

	/* Hash table deletion */
	for (hn = hashiter_first(&hi, &ht); hn; hn = hashiter_next(&hi)) {
		free(entry_of(hn));
	}

	return EXIT_SUCCESS;
}

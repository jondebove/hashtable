#include <stdio.h>
#include <stdlib.h>

#include "hashtable.h"

struct entry {
	int key;
	int val;
	struct hashnode node;
};
#define hash(k) ((uint64_t)(k))

int main(void)
{
	struct entry *e;
	int i, k;

	/* Hash table */
	struct hashtable ht;

	/* Initialization */
	struct hashnode *nodes[32];
	if (!hashtable_init(&ht, 32, nodes)) {
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
	for (hn = hashtable_first(&ht, hash(k)); hn; hn = hashtable_next(hn)) {
		e = HASH_CONTAINEROF(hn, struct entry, node);
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
		e = HASH_CONTAINEROF(hn, struct entry, node);
		printf("key=%d, val=%d\n", e->key, e->val);
	}

	/* Hash table deletion */
	for (hn = hashiter_first(&hi, &ht); hn; hn = hashiter_next(&hi)) {
		e = HASH_CONTAINEROF(hn, struct entry, node);
		free(e);
	}

	return EXIT_SUCCESS;
}

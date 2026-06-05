/*
 * BSD 2-Clause License
 * 
 * Copyright (c) 2026, Jonathan Debove
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <assert.h>
#include <stddef.h>
#include <stdint.h>

struct hashnode {
	struct hashnode *next;
};

struct hashtable {
	int bits;
	size_t count;
	struct hashnode **table;
};

struct hashiter {
	struct hashnode *const *base;
	struct hashnode *const *head;
	struct hashnode *next;
};

static inline
struct hashtable *hashtable_init(struct hashtable *ht,
		size_t n, struct hashnode *table[static n])
{
	if (n < 2) return NULL;

	for (ht->bits = 0; (n >>= 1); ht->bits++);
	ht->count = 0;
	for (n = ((size_t)1 << ht->bits); n--; ) table[n] = NULL;
	ht->table = table;
	return ht;
}

#define HASH_CONTAINEROF(ptr, type, field) \
	((type *)((char *)(ptr) - offsetof(type, field)))

static inline
struct hashnode **hashtable_head(struct hashtable const *ht, uint64_t hash)
{
	return &ht->table[(hash * 0x93c467e37db0c7a3U) >> (64 - ht->bits)];
}

static inline
struct hashnode *hashtable_first(struct hashtable const *ht, uint64_t hash)
{
	return *hashtable_head(ht, hash);
}

static inline
struct hashnode *hashtable_next(struct hashnode const *hn)
{
	return hn->next;
}

static inline
void hashtable_insert(struct hashtable *ht, struct hashnode *hn, uint64_t hash)
{
	struct hashnode **head = hashtable_head(ht, hash);
	hn->next = *head;
	*head = hn;
	ht->count++;
}

static inline
void hashtable_remove(struct hashtable *ht, struct hashnode *hn, uint64_t hash)
{
	struct hashnode **head = hashtable_head(ht, hash);
	struct hashnode *n = *head;
	assert(n);
	if (n == hn) {
		(*head) = hn->next;
	} else {
		for (; n->next != hn; n = n->next) assert(n);
		n->next = hn->next;
	}
	ht->count--;
}

static inline
struct hashnode *hashiter_next(struct hashiter *hi)
{
	struct hashnode *hn = hi->next;
	if (hn) {
		for (hi->next = hn->next; !hi->next && hi->head != hi->base;
				hi->next = *(--hi->head));
	}
	return hn;
}

static inline
struct hashnode *hashiter_first(struct hashiter *hi, struct hashtable const *ht)
{
	hi->next = NULL;
	hi->base = ht->table;
	for (hi->head = hi->base + ((size_t)1 << ht->bits);
			!hi->next && hi->head != hi->base;
			hi->next = *(--hi->head));
	return hashiter_next(hi);
}

#endif	/* HASHTABLE_H */

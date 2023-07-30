/*
 * BSD 2-Clause License
 * 
 * Copyright (c) 2023, Jonathan Debove
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

#include <sys/queue.h>

/*
 * Hash table definitions.
 */
#define HASH_TABLE(name, type)						\
struct name {								\
	LIST_HEAD(, type) *ht_table;					\
	unsigned int ht_size;						\
}

#define HASH_ENTRY(type)						\
struct {								\
	LIST_ENTRY(type) he_list;					\
	unsigned int he_hash;						\
}

/*
 * Hash table functions.
 */
#define HASH_INIT(htab, buffer, size) do {				\
	(htab)->ht_table = (void *)(buffer);				\
	(htab)->ht_size = (size) / sizeof(*(htab)->ht_table);		\
	unsigned int h_idx;						\
	for (h_idx = 0; h_idx < (htab)->ht_size; h_idx++) {		\
		LIST_INIT(&(htab)->ht_table[h_idx]);			\
	}								\
} while (0)

#define HASH_INDEX(htab, hash)						\
	(((hash) * 0xb504f32dU) % ((htab)->ht_size))

#define HASH_MOVE(hdst, hsrc, type, field) do {				\
	unsigned int h_idx;						\
	struct type *h_elm, *h_nxt;					\
	HASH_FOREACH_SAFE(h_elm, h_idx, (hsrc), field, h_nxt) {		\
		HASH_REMOVE(h_elm, field);				\
		HASH_INSERT((hdst), h_elm,				\
				h_elm->field.he_hash, field);		\
	}								\
} while (0)

#define	HASH_INSERT(htab, elm, hash, field) do {			\
	(elm)->field.he_hash = (hash);					\
	unsigned int h_idx;						\
	h_idx = HASH_INDEX((htab), (elm)->field.he_hash);		\
	LIST_INSERT_HEAD(&(htab)->ht_table[h_idx],			\
			(elm), field.he_list);				\
} while (0)

#define HASH_REMOVE(elm, field)	do {					\
	LIST_REMOVE((elm), field.he_list);				\
} while (0)

/*
 * Hash table for loops.
 */
#define	HASH_FOREACH(var, idx, htab, field)				\
	for ((idx) = 0;	(idx) < (htab)->ht_size; (idx)++)		\
	LIST_FOREACH((var), &(htab)->ht_table[idx], field.he_list)

#define	HASH_SEARCH_FOREACH(var, hash, htab, field)			\
	for (unsigned int h_hash = (hash),				\
			h_idx = HASH_INDEX((htab), h_hash),		\
			h_once = 1; h_once; h_once--)			\
		LIST_FOREACH((var), &(htab)->ht_table[h_idx],		\
				field.he_list)				\
			if (h_hash == (var)->field.he_hash)

#ifndef LIST_FOREACH_SAFE
#define	LIST_FOREACH_SAFE(var, htab, field, nxt)			\
	for ((var) = LIST_FIRST((htab)); (var) && 			\
			((nxt) = LIST_NEXT((var), field), (var));	\
			(var) = (nxt))
#endif /* LIST_FOREACH_SAFE */

#define	HASH_FOREACH_SAFE(var, idx, htab, field, nxt)			\
	for ((idx) = 0; (idx) < (htab)->ht_size; (idx)++)		\
		LIST_FOREACH_SAFE((var), &(htab)->ht_table[idx],	\
				field.he_list, (nxt))

#define	HASH_SEARCH_FOREACH_SAFE(var, hash, htab, field, nxt)		\
	for (unsigned int h_hash = (hash),				\
			h_idx = HASH_INDEX((htab), h_hash),		\
			h_once = 1; h_once; h_once--)			\
		LIST_FOREACH_SAFE((var), &(htab)->ht_table[h_idx],	\
				field.he_list, (nxt))			\
			if (h_hash == (var)->field.he_hash)

/*
 * Hash table access methods.
 */
#define HASH_BUFFER(htab) ((void *)(htab)->ht_table)

#define HASH_SIZE(htab) ((htab)->ht_size)

#define HASH_SIZEOF(htab) ((htab)->ht_size * sizeof(*(htab)->ht_table))

#define HASH_HASH(elm, field) ((elm)->field.he_hash)

#define HASH_EMPTY(empty, htab) do {					\
	for ((empty) = 0; (empty) < (htab)->ht_size; (empty)++)		\
		if (!LIST_EMPTY(&(htab)->ht_table[empty]))		\
			break;						\
	(empty) = (empty) == (htab)->ht_size;				\
} while (0)

/*
 * Simple Hash table definitions.
 */
#define SHASH_TABLE(name, type)						\
struct name {								\
	SLIST_HEAD(, type) *sht_table;					\
	unsigned int sht_size;						\
}

#define SHASH_ENTRY(type)						\
struct {								\
	SLIST_ENTRY(type) she_list;					\
	unsigned int she_hash;						\
}

/*
 * Hash table functions.
 */
#define SHASH_INIT(htab, buffer, size) do {				\
	(htab)->sht_table = (void *)(buffer);				\
	(htab)->sht_size = (size) / sizeof(*(htab)->sht_table);		\
	unsigned int sh_idx;						\
	for (sh_idx = 0; sh_idx < (htab)->sht_size; sh_idx++) {		\
		SLIST_INIT(&(htab)->sht_table[sh_idx]);			\
	}								\
} while (0)

#define SHASH_INDEX(htab, hash)						\
	(((hash) * 0xb504f32dU) % ((htab)->sht_size))

#define SHASH_MOVE(hdst, hsrc, type, field) do {			\
	unsigned int sh_idx;						\
	struct type *sh_elm, *sh_nxt;					\
	SHASH_FOREACH_SAFE(sh_elm, sh_idx, (hsrc), field, sh_nxt) {	\
		SHASH_REMOVE((hsrc), sh_elm, type, field);		\
		SHASH_INSERT((hdst), sh_elm,				\
				sh_elm->field.she_hash, field);		\
	}								\
} while (0)

#define	SHASH_INSERT(htab, elm, hash, field) do {			\
	(elm)->field.she_hash = (hash);					\
	unsigned int sh_idx;						\
	sh_idx = SHASH_INDEX((htab), (elm)->field.she_hash);		\
	SLIST_INSERT_HEAD(&(htab)->sht_table[sh_idx],			\
			(elm), field.she_list);				\
} while (0)

#define SHASH_REMOVE(htab, elm, type, field) do {			\
	unsigned int sh_idx;						\
	sh_idx = SHASH_INDEX((htab), (elm)->field.she_hash);		\
	SLIST_REMOVE(&(htab)->sht_table[sh_idx],			\
				(elm), type, field.she_list);		\
} while (0)

/*
 * Simple Hash table for loops.
 */
#define	SHASH_FOREACH(var, idx, htab, field)				\
	for ((idx) = 0;	(idx) < (htab)->sht_size; (idx)++)		\
	SLIST_FOREACH((var), &(htab)->sht_table[idx], field.she_list)

#define	SHASH_SEARCH_FOREACH(var, hash, htab, field)			\
	for (unsigned int sh_hash = (hash),				\
			sh_idx = SHASH_INDEX((htab), sh_hash),		\
			sh_once = 1; sh_once; sh_once--)		\
		SLIST_FOREACH((var), &(htab)->sht_table[sh_idx],	\
				field.she_list)				\
			if (sh_hash == (var)->field.she_hash)

#ifndef SLIST_FOREACH_SAFE
#define	SLIST_FOREACH_SAFE(var, htab, field, nxt)			\
	for ((var) = SLIST_FIRST((htab)); (var) && 			\
			((nxt) = SLIST_NEXT((var), field), (var));	\
			(var) = (nxt))
#endif /* SLIST_FOREACH_SAFE */

#define	SHASH_FOREACH_SAFE(var, idx, htab, field, nxt)			\
	for ((idx) = 0; (idx) < (htab)->sht_size; (idx)++)		\
		SLIST_FOREACH_SAFE((var), &(htab)->sht_table[idx],	\
				field.she_list, (nxt))

#define	SHASH_SEARCH_FOREACH_SAFE(var, hash, htab, field, nxt)		\
	for (unsigned int sh_hash = (hash),				\
			sh_idx = SHASH_INDEX((htab), sh_hash),		\
			sh_once = 1; sh_once; sh_once--)		\
		SLIST_FOREACH_SAFE((var), &(htab)->sht_table[sh_idx],	\
				field.she_list, (nxt))			\
			if (sh_hash == (var)->field.she_hash)

/*
 * Hash table access methods.
 */
#define SHASH_BUFFER(htab) ((void *)(htab)->sht_table)

#define SHASH_SIZE(htab) ((htab)->sht_size)

#define SHASH_SIZEOF(htab) ((htab)->sht_size * sizeof(*(htab)->sht_table))

#define SHASH_HASH(elm, field) ((elm)->field.she_hash)

#define SHASH_EMPTY(empty, htab) do {					\
	for ((empty) = 0; (empty) < (htab)->sht_size; (empty)++)	\
		if (!SLIST_EMPTY(&(htab)->sht_table[empty]))		\
			break;						\
	(empty) = (empty) == (htab)->sht_size;				\
} while (0)

#endif	/* HASHTABLE_H */

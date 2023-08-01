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

#include <limits.h>
#include <sys/queue.h>

#if UINT_MAX == 0xffffU
#	define HASH_MULT 0x9e3bU
#	define HASH_BITS 16U
#elif UINT_MAX == 0xffffffffU
#	define HASH_MULT 0x9e3779b1U
#	define HASH_BITS 32U
#elif UINT_MAX == 0xffffffffffffffffU
#	define HASH_MULT 0x9e3779b97f4a7c55U
#	define HASH_BITS 64U
#else
#	define HASH_MULT 1U
#	define HASH_BITS 0U
#endif

/*
 * Hash table definitions.
 */
#define HASH_TABLE(name, type)						\
struct name {								\
	LIST_HEAD(, type) *ht_table;					\
	unsigned int ht_size;						\
	unsigned char ht_shift;						\
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
	for ((htab)->ht_shift = 0; HASH_BITS > 0 &&			\
			(1U << (htab)->ht_shift) < (htab)->ht_size;	\
			(htab)->ht_shift++);				\
	(htab)->ht_shift = HASH_BITS - (htab)->ht_shift;		\
	unsigned int h__idx;						\
	for (h__idx = 0; h__idx < (htab)->ht_size; h__idx++) {		\
		LIST_INIT(&(htab)->ht_table[h__idx]);			\
	}								\
} while (0)

#define HASH_INDEX(htab, hash)						\
	((((hash) * HASH_MULT) >> (htab)->ht_shift) % (htab)->ht_size)

#define HASH_MOVE(hdst, hsrc, type, field) do {				\
	unsigned int h__idx;						\
	struct type *h__elm, *h__nxt;					\
	HASH_FOREACH_SAFE(h__elm, h__idx, (hsrc), field, h__nxt) {	\
		HASH_REMOVE(h__elm, field);				\
		HASH_INSERT((hdst), h__elm,				\
				h__elm->field.he_hash, field);		\
	}								\
} while (0)

#define HASH_INSERT(htab, elm, hash, field) do {			\
	(elm)->field.he_hash = (hash);					\
	unsigned int h__idx;						\
	h__idx = HASH_INDEX((htab), (elm)->field.he_hash);		\
	LIST_INSERT_HEAD(&(htab)->ht_table[h__idx],			\
			(elm), field.he_list);				\
} while (0)

#define HASH_REMOVE(elm, field)	do {					\
	LIST_REMOVE((elm), field.he_list);				\
} while (0)

/*
 * Hash table for loops.
 */
#define HASH_FOREACH(var, idx, htab, field)				\
	for ((idx) = 0;	(idx) < (htab)->ht_size; (idx)++)		\
	LIST_FOREACH((var), &(htab)->ht_table[idx], field.he_list)

#define HASH_SEARCH_FOREACH(var, hash, htab, field)			\
	for (unsigned int h__hash = (hash),				\
			h__idx = HASH_INDEX((htab), h__hash),		\
			h__once = 1; h__once; h__once--)		\
		LIST_FOREACH((var), &(htab)->ht_table[h__idx],		\
				field.he_list)				\
			if (h__hash == (var)->field.he_hash)

#ifndef LIST_FOREACH_SAFE
#define LIST_FOREACH_SAFE(var, head, field, nxt)			\
	for ((var) = LIST_FIRST((head)); (var) && 			\
			((nxt) = LIST_NEXT((var), field), (var));	\
			(var) = (nxt))
#endif /* LIST_FOREACH_SAFE */

#define HASH_FOREACH_SAFE(var, idx, htab, field, nxt)			\
	for ((idx) = 0; (idx) < (htab)->ht_size; (idx)++)		\
		LIST_FOREACH_SAFE((var), &(htab)->ht_table[idx],	\
				field.he_list, (nxt))

#define HASH_SEARCH_FOREACH_SAFE(var, hash, htab, field, nxt)		\
	for (unsigned int h__hash = (hash),				\
			h__idx = HASH_INDEX((htab), h__hash),		\
			h__once = 1; h__once; h__once--)		\
		LIST_FOREACH_SAFE((var), &(htab)->ht_table[h__idx],	\
				field.he_list, (nxt))			\
			if (h__hash == (var)->field.he_hash)

/*
 * Hash table access methods.
 */
#define HASH_BUFFER(htab) ((void *)(htab)->ht_table)

#define HASH_SIZE(htab) ((htab)->ht_size)

#define HASH_HASH(elm, field) ((elm)->field.he_hash)

/*
 * Simple Hash table definitions.
 */
#define SHASH_TABLE(name, type)						\
struct name {								\
	SLIST_HEAD(, type) *sht_table;					\
	unsigned int sht_size;						\
	unsigned char sht_shift;					\
}

#define SHASH_ENTRY(type)						\
struct {								\
	SLIST_ENTRY(type) she_list;					\
	unsigned int she_hash;						\
}

/*
 * Simple hash table functions.
 */
#define SHASH_INIT(htab, buffer, size) do {				\
	(htab)->sht_table = (void *)(buffer);				\
	(htab)->sht_size = (size) / sizeof(*(htab)->sht_table);		\
	for ((htab)->sht_shift = 0; HASH_BITS > 0 &&			\
			(1U << (htab)->sht_shift) < (htab)->sht_size;	\
			(htab)->sht_shift++);				\
	(htab)->sht_shift = HASH_BITS - (htab)->sht_shift;		\
	unsigned int sh__idx;						\
	for (sh__idx = 0; sh__idx < (htab)->sht_size; sh__idx++) {	\
		SLIST_INIT(&(htab)->sht_table[sh__idx]);		\
	}								\
} while (0)

#define SHASH_INDEX(htab, hash)						\
	((((hash) * HASH_MULT) >> (htab)->sht_shift) % (htab)->sht_size)

#define SHASH_MOVE(hdst, hsrc, type, field) do {			\
	unsigned int sh__idx;						\
	struct type *sh__elm, *sh__nxt;					\
	SHASH_FOREACH_SAFE(sh__elm, sh__idx, (hsrc), field, sh__nxt) {	\
		SHASH_REMOVE((hsrc), sh__elm, type, field);		\
		SHASH_INSERT((hdst), sh__elm,				\
				sh__elm->field.she_hash, field);	\
	}								\
} while (0)

#define SHASH_INSERT(htab, elm, hash, field) do {			\
	(elm)->field.she_hash = (hash);					\
	unsigned int sh__idx;						\
	sh__idx = SHASH_INDEX((htab), (elm)->field.she_hash);		\
	SLIST_INSERT_HEAD(&(htab)->sht_table[sh__idx],			\
			(elm), field.she_list);				\
} while (0)

#define SHASH_REMOVE(htab, elm, type, field) do {			\
	unsigned int sh__idx;						\
	sh__idx = SHASH_INDEX((htab), (elm)->field.she_hash);		\
	SLIST_REMOVE(&(htab)->sht_table[sh__idx],			\
				(elm), type, field.she_list);		\
} while (0)

/*
 * Simple Hash table for loops.
 */
#define SHASH_FOREACH(var, idx, htab, field)				\
	for ((idx) = 0;	(idx) < (htab)->sht_size; (idx)++)		\
	SLIST_FOREACH((var), &(htab)->sht_table[idx], field.she_list)

#define SHASH_SEARCH_FOREACH(var, hash, htab, field)			\
	for (unsigned int sh__hash = (hash),				\
			sh__idx = SHASH_INDEX((htab), sh__hash),	\
			sh__once = 1; sh__once; sh__once--)		\
		SLIST_FOREACH((var), &(htab)->sht_table[sh__idx],	\
				field.she_list)				\
			if (sh__hash == (var)->field.she_hash)

#ifndef SLIST_FOREACH_SAFE
#define SLIST_FOREACH_SAFE(var, head, field, nxt)			\
	for ((var) = SLIST_FIRST((head)); (var) && 			\
			((nxt) = SLIST_NEXT((var), field), (var));	\
			(var) = (nxt))
#endif /* SLIST_FOREACH_SAFE */

#define SHASH_FOREACH_SAFE(var, idx, htab, field, nxt)			\
	for ((idx) = 0; (idx) < (htab)->sht_size; (idx)++)		\
		SLIST_FOREACH_SAFE((var), &(htab)->sht_table[idx],	\
				field.she_list, (nxt))

#define SHASH_SEARCH_FOREACH_SAFE(var, hash, htab, field, nxt)		\
	for (unsigned int sh__hash = (hash),				\
			sh__idx = SHASH_INDEX((htab), sh__hash),	\
			sh__once = 1; sh__once; sh__once--)		\
		SLIST_FOREACH_SAFE((var), &(htab)->sht_table[sh__idx],	\
				field.she_list, (nxt))			\
			if (sh__hash == (var)->field.she_hash)

/*
 * Simple hash table access methods.
 */
#define SHASH_BUFFER(htab) ((void *)(htab)->sht_table)

#define SHASH_SIZE(htab) ((htab)->sht_size)

#define SHASH_HASH(elm, field) ((elm)->field.she_hash)

#endif	/* HASHTABLE_H */

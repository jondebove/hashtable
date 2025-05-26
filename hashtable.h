/*
 * BSD 2-Clause License
 * 
 * Copyright (c) 2023-2025, Jonathan Debove
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
#include <stddef.h>
#include <sys/queue.h>

#if UINT_MAX == 0xffffU
#	define HASH_MULT 0x93b5U
#	define HASH_BITS 16U
#elif UINT_MAX == 0xffffffffU
#	define HASH_MULT 0x93c467e3U
#	define HASH_BITS 32U
#elif UINT_MAX == 0xffffffffffffffffU
#	define HASH_MULT 0x93c467e37db0c7a3U
#	define HASH_BITS 64U
#else
#	error UINT_WIDTH different from 16, 32 or 64 not implemented.
#endif

/*
 * Hash table definitions.
 */
#define HASH_TABLE(name, type)						\
struct name {								\
	unsigned int ht_shift;						\
	LIST_HEAD(, type) ht_table[];					\
}

#define HASH_ENTRY(type)						\
struct {								\
	unsigned int he_hash;						\
	LIST_ENTRY(type) he_list;					\
}

/*
 * Hash table functions.
 */
#define HASH_TABLE_SIZE(name, shift)					\
	(sizeof(struct name) + (1U << (shift)) *			\
	 sizeof(((struct name *)0)->ht_table[0]))

#define HASH_INIT(htab, shift) do {					\
	(htab)->ht_shift = (shift);					\
	for (unsigned int h__idx = HASH_SIZE(htab); h__idx--;)		\
		LIST_INIT(&(htab)->ht_table[h__idx]);			\
} while (0)

#define HASH_INDEX(htab, hash) ((unsigned int)				\
	(((hash) * HASH_MULT) >> (HASH_BITS - HASH_SHIFT(htab))))

#define HASH_MOVE(hdst, hsrc, type, field) do {				\
	struct type *h__elm, *h__nxt;					\
	HASH_FOREACH_SAFE(h__elm, (hsrc), field, h__nxt) {		\
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
#define HASH_FOREACH(var, htab, field)					\
	for (unsigned int h__idx = HASH_SIZE(htab); h__idx--;)		\
		LIST_FOREACH((var), &(htab)->ht_table[h__idx],		\
				field.he_list)

#define HASH_SEARCH_FOREACH(var, hash, htab, field)			\
	for (unsigned int h__hash = (hash),				\
			h__idx = HASH_INDEX((htab), h__hash),		\
			h__once = 1; h__once; h__once = 0)		\
		LIST_FOREACH((var), &(htab)->ht_table[h__idx],		\
				field.he_list)				\
			if (h__hash == (var)->field.he_hash)

#ifndef LIST_FOREACH_SAFE
#define LIST_FOREACH_SAFE(var, head, field, nxt)			\
	for ((var) = LIST_FIRST((head)); (var) && 			\
			((nxt) = LIST_NEXT((var), field), (var));	\
			(var) = (nxt))
#endif /* LIST_FOREACH_SAFE */

#define HASH_FOREACH_SAFE(var, htab, field, nxt)			\
	for (unsigned int h__idx = HASH_SIZE(htab); h__idx--;)		\
		LIST_FOREACH_SAFE((var), &(htab)->ht_table[h__idx],	\
				field.he_list, (nxt))

#define HASH_SEARCH_FOREACH_SAFE(var, hash, htab, field, nxt)		\
	for (unsigned int h__hash = (hash),				\
			h__idx = HASH_INDEX((htab), h__hash),		\
			h__once = 1; h__once; h__once = 0)		\
		LIST_FOREACH_SAFE((var), &(htab)->ht_table[h__idx],	\
				field.he_list, (nxt))			\
			if (h__hash == (var)->field.he_hash)

/*
 * Hash table access methods.
 */
#define HASH_SHIFT(htab) ((htab)->ht_shift)

#define HASH_SIZE(htab) (1U << (htab)->ht_shift)

#define HASH_HASH(elm, field) ((elm)->field.he_hash)

/*
 * Simple hash table definitions.
 */
#define SHASH_TABLE(name, type)						\
struct name {								\
	unsigned int sht_shift;						\
	SLIST_HEAD(, type) sht_table[];					\
}

#define SHASH_ENTRY(type)						\
struct {								\
	unsigned int she_hash;						\
	SLIST_ENTRY(type) she_list;					\
}

/*
 * Simple hash table functions.
 */
#define SHASH_TABLE_SIZE(name, shift)					\
	(sizeof(struct name) + (1U << (shift)) *			\
	 sizeof(((struct name *)0)->sht_table[0]))

#define SHASH_INIT(htab, shift) do {					\
	(htab)->sht_shift = (shift);					\
	for (unsigned int sh__idx = SHASH_SIZE(htab); sh__idx--;)	\
		SLIST_INIT(&(htab)->sht_table[sh__idx]);		\
} while (0)

#define SHASH_INDEX(htab, hash) ((unsigned int)				\
	(((hash) * HASH_MULT) >> (HASH_BITS - SHASH_SHIFT(htab))))

#define SHASH_MOVE(hdst, hsrc, type, field) do {			\
	struct type *sh__elm, *sh__nxt;					\
	SHASH_FOREACH_SAFE(sh__elm, (hsrc), field, sh__nxt) {		\
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
 * Simple hash table for loops.
 */
#define SHASH_FOREACH(var, htab, field)					\
	for (unsigned int sh__idx = SHASH_SIZE(htab); sh__idx--;)	\
		SLIST_FOREACH((var), &(htab)->sht_table[sh__idx],	\
				field.she_list)

#define SHASH_SEARCH_FOREACH(var, hash, htab, field)			\
	for (unsigned int sh__hash = (hash),				\
			sh__idx = SHASH_INDEX((htab), sh__hash),	\
			sh__once = 1; sh__once; sh__once = 0)		\
		SLIST_FOREACH((var), &(htab)->sht_table[sh__idx],	\
				field.she_list)				\
			if (sh__hash == (var)->field.she_hash)

#ifndef SLIST_FOREACH_SAFE
#define SLIST_FOREACH_SAFE(var, head, field, nxt)			\
	for ((var) = SLIST_FIRST((head)); (var) && 			\
			((nxt) = SLIST_NEXT((var), field), (var));	\
			(var) = (nxt))
#endif /* SLIST_FOREACH_SAFE */

#define SHASH_FOREACH_SAFE(var, htab, field, nxt)			\
	for (unsigned int sh__idx = SHASH_SIZE(htab); sh__idx--;)	\
		SLIST_FOREACH_SAFE((var), &(htab)->sht_table[sh__idx],	\
				field.she_list, (nxt))

#define SHASH_SEARCH_FOREACH_SAFE(var, hash, htab, field, nxt)		\
	for (unsigned int sh__hash = (hash),				\
			sh__idx = SHASH_INDEX((htab), sh__hash),	\
			sh__once = 1; sh__once; sh__once = 0)		\
		SLIST_FOREACH_SAFE((var), &(htab)->sht_table[sh__idx],	\
				field.she_list, (nxt))			\
			if (sh__hash == (var)->field.she_hash)

/*
 * Simple hash table access methods.
 */
#define SHASH_SHIFT(htab) ((htab)->sht_shift)

#define SHASH_SIZE(htab) (1U << (htab)->sht_shift)

#define SHASH_HASH(elm, field) ((elm)->field.she_hash)

#endif	/* HASHTABLE_H */

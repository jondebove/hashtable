# hashtable - implementations of hash tables.

The `"hashtable.h"` header file provides a set of macros that define and
operate on the following data structures:

- SHASH hash table based on an array of singly-linked lists.
- HASH  hash table based on an array of doubly-linked lists.

Both structures support the following functionality:

- O(1) insertion of a new entry.
- O(1) search for an entry.
- O(1) Removal of an entry.
- Forward traversal through the hash table.

Both use internally the lists implemented in the "sys/queue.h" BSD
header file (respectively SLIST and LIST).

## Interface

    #include "hashtable.h"

    /* Creation */
    HASH_ENTRY(TYPE);
    HASH_TABLE(TABLENAME, TYPE);
    void HASH_INIT(HASH_TABLE *htab, void *buffer, unsigned int size);

    /* Access */
    void HASH_EMPTY(unsigned int empty, HASH_TABLE *htab);
    void *HASH_BUFFER(HASH_TABLE *htab);
    unsigned int HASH_SIZE(HASH_TABLE *htab);

    /* Insertion */
    void HASH_INSERT(HASH_TABLE *htab, struct TYPE *elm,
                    unsigned int hash, HASH_ENTRY NAME);
    void HASH_MOVE(HASH_TABLE *dst, HASH_TABLE *src, TYPE, HASH_ENTRY NAME);

    /* Traversal */
    HASH_FOREACH(struct TYPE *var, unsigned int idx,
                    HASH_TABLE *htab, HASH_ENTRY NAME);
    HASH_FOREACH_SAFE(struct TYPE *var, unsigned int idx,
                    HASH_TABLE *htab, HASH_ENTRY NAME, struct TYPE *nxt);

    HASH_SEARCH_FOREACH(struct TYPE *var, unsigned int hash,
                    HASH_TABLE *htab, HASH_ENTRY NAME);
    HASH_SEARCH_FOREACH_SAFE(struct TYPE *var, unsigned int hash,
                    HASH_TABLE *htab, HASH_ENTRY NAME, struct TYPE *nxt);

    /* Removal */
    void HASH_REMOVE(struct TYPE *elm, HASH_ENTRY NAME);

    /* Same interface for SHASH except for SHASH_REMOVE */

    void SHASH_REMOVE(HASH_TABLE *htab, struct TYPE *elm,
                    TYPE, HASH_ENTRY NAME);

## Details

In the macros definitions, `TYPE` is the name of a user-defined
structure, that must contain a fild of type `HASH_ENTRY`, named `NAME`.
The argument `TABLENAME` is the name of a user-defined structure that
must be declared using the macro `HASH_TABLE()`.

### Creation

The macros do not allocate memory for the entries or the hash table.
The user must handle the memory by himself and pass a memory buffer
using the macro `HASH_INIT()`.

To change the size of the hash table, a new one shall shall be created
and the entries moved with the macro `HASH_MOVE()`.

### Insertion

The macro `HASH_INSERT()` inserts an element associated with `hash`.
It does not test if the element is already inserted and allows multiple
elements with same key.
In case this feature is unwanted, the user may search the element in
prior insertion and do proper action.

### Traversal

Complete hash table traversal may be done with the macros
`HASH_FOREACH()` and `HASH_FOREACH_SAFE()`.
The latter allows for removal of the element while iterating.

`HASH_SEARCH_FOREACH()` and `HASH_SEARCH_FOREACH_SAFE()` iterate the
elements with specified `hash`.
Testing if the key matchs is left to the user.

### Removal

`HASH_REMOVE()` removes the element from the hash table.

`SHASH_REMOVE()` is the only `SHASH` macro with an interface different
from the `HASH` macros.
Due to the usage of singly-linked lists, it needs two additional
arguments: the hash table and the entry type.

## Examples

For details on the use of these macros, see the example in the file
`"hashtable_test.c"`.

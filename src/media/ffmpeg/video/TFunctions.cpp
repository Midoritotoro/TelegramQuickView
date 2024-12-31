#include "TFunctions.h"
#include "Config.h"

[[nodiscard]] void* tfind(
    const void* vkey,
    void* const* vrootp,
    int (*compar)(const void*,
        const void*))
{
    node_t* const* rootp = (node_t* const*)vrootp;

    assert(vkey != NULL);
    assert(compar != NULL);

    if (rootp == NULL)
        return NULL;

    while (*rootp != NULL) {		/* T1: */
        int r;

        if ((r = (*compar)(vkey, (*rootp)->key)) == 0)	/* T2: */
            return *rootp;		/* key found */
        rootp = (r < 0) ?
            &(*rootp)->llink :		/* T3: follow left branch */
            &(*rootp)->rlink;		/* T4: follow right branch */
    }
    return NULL;
}

[[nodiscard]] void* tsearch(
    const void* vkey,
    void** vrootp,
    int (*compar)(const void*,
        const void*))
{
    node_t* q;
    node_t** rootp = (node_t**)vrootp;

    assert(vkey != NULL);
    assert(compar != NULL);

    if (rootp == NULL)
        return NULL;

    while (*rootp != NULL) {	/* Knuth's T1: */
        int r;

        if ((r = (*compar)(vkey, (*rootp)->key)) == 0)	/* T2: */
            return *rootp;		/* we found it! */

        rootp = (r < 0) ?
            &(*rootp)->llink :		/* T3: follow left branch */
            &(*rootp)->rlink;		/* T4: follow right branch */
    }

    q = (node_t*)malloc(sizeof(node_t));		/* T5: key not found */
    if (q != 0) {				/* make new node */
        *rootp = q;			/* link new node to old */
        q->key = (char*)(vkey);	/* initialize new node */
        q->llink = q->rlink = NULL;
    }
    return q;
}
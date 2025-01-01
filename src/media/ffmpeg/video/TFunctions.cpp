#include "TFunctions.h"

#include <cstdlib>
#include <cassert>

#include <search.h>


static struct
{
    const void** tab;
    size_t count;
} list = { NULL, 0 };
static const void* smallest;


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

void trecurse(
    const node_t* root,	/* Root of the tree to be walked */
    cmp_fn_t action, int level)
{
    assert(root != NULL);
    assert(action != NULL);

    if (root->llink == NULL && root->rlink == NULL)
        (*action)(root, leaf, level);
    else {
        (*action)(root, preorder, level);
        if (root->llink != NULL)
            trecurse(root->llink, action, level + 1);
        (*action)(root, postorder, level);
        if (root->rlink != NULL)
            trecurse(root->rlink, action, level + 1);
        (*action)(root, endorder, level);
    }
}

/* Walk the nodes of a tree */
void twalk(
    const void* vroot,
    cmp_fn_t action) /* Root of the tree to be walked */
{
    if (vroot != NULL && action != NULL)
        trecurse((const node_t*)vroot, action, 0);
}


int cmp_smallest(
    const void* a,
    const void* b)
{
    if (a == b)
        return 0;
    if (a == smallest)
        return -1;
    if (b == smallest)
        return +1;
    abort();
}

void tdestroy_recurse(
    node_t* root,
    void (*free_action)(void*))
{
    if (root->llink != NULL)
        tdestroy_recurse(root->llink, free_action);
    if (root->rlink != NULL)
        tdestroy_recurse(root->rlink, free_action);

    (*free_action) ((void*)root->key);
    free(root);
}

void tdestroy(void* vrootp, void (*freefct)(void*))
{
    node_t* root = (node_t*)vrootp;

    if (root != NULL)
        tdestroy_recurse(root, freefct);
}

/**
* Copyright (C) 2015 Happy Fish / YuQing
*
* libfastcommon may be copied only under the terms of the GNU General
* Public License V3, which may be found in the FastDFS source kit.
* Please visit the FastDFS Home Page http://www.fastken.com/ for more detail.
**/

//uniq_skiplist.h

#ifndef _UNIQ_SKIPLIST_H
#define _UNIQ_SKIPLIST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common_define.h"
#include "skiplist_common.h"
#include "fast_mblock.h"

typedef void (*uniq_skiplist_free_func)(void *ptr, const int delay_seconds);

typedef struct uniq_skiplist_node
{
    void *data;
    int level_index;
    volatile struct uniq_skiplist_node *links[0];
} UniqSkiplistNode;

typedef struct uniq_skiplist_factory
{
    int max_level_count;
    int delay_free_seconds;
    bool bidirection;       //if need reverse iteration for level 0
    skiplist_compare_func compare_func;
    uniq_skiplist_free_func free_func;
    UniqSkiplistNode *tail;  //the tail node for interator
    struct fast_mblock_man skiplist_allocator;
    struct fast_mblock_man *node_allocators;
} UniqSkiplistFactory;

typedef struct uniq_skiplist
{
    UniqSkiplistFactory *factory;
    int top_level_index;
    int element_count;
    UniqSkiplistNode *top;  //the top node
} UniqSkiplist;

typedef struct uniq_skiplist_iterator {
    volatile UniqSkiplistNode *current;
    volatile UniqSkiplistNode *tail;
} UniqSkiplistIterator;

#ifdef __cplusplus
extern "C" {
#endif

#define uniq_skiplist_count(sl) (sl)->element_count

#define uniq_skiplist_init_ex(factory, max_level_count, compare_func, \
        free_func, alloc_skiplist_once, min_alloc_elements_once, \
        delay_free_seconds) \
    uniq_skiplist_init_ex2(factory, max_level_count, compare_func, \
        free_func, alloc_skiplist_once, min_alloc_elements_once, \
        delay_free_seconds, false) \

#define uniq_skiplist_init(factory, max_level_count, compare_func, free_func) \
    uniq_skiplist_init_ex(factory, max_level_count,  \
            compare_func, free_func, 64 * 1024, \
            SKIPLIST_DEFAULT_MIN_ALLOC_ELEMENTS_ONCE, 0)

#define uniq_skiplist_delete(sl, data)  \
    uniq_skiplist_delete_ex(sl, data, true)

#define uniq_skiplist_replace(sl, data) \
    uniq_skiplist_replace_ex(sl, data, true)


int uniq_skiplist_init_ex2(UniqSkiplistFactory *factory,
        const int max_level_count, skiplist_compare_func compare_func,
        uniq_skiplist_free_func free_func, const int alloc_skiplist_once,
        const int min_alloc_elements_once, const int delay_free_seconds,
        const bool bidirection);

void uniq_skiplist_destroy(UniqSkiplistFactory *factory);

UniqSkiplist *uniq_skiplist_new(UniqSkiplistFactory *factory,
        const int level_count);

void uniq_skiplist_free(UniqSkiplist *sl);

int uniq_skiplist_insert(UniqSkiplist *sl, void *data);
int uniq_skiplist_delete_ex(UniqSkiplist *sl, void *data,
        const bool need_free);
int uniq_skiplist_replace_ex(UniqSkiplist *sl, void *data,
        const bool need_free_old);
void *uniq_skiplist_find(UniqSkiplist *sl, void *data);
int uniq_skiplist_find_all(UniqSkiplist *sl, void *data,
        UniqSkiplistIterator *iterator);
int uniq_skiplist_find_range(UniqSkiplist *sl, void *start_data,
        void *end_data, UniqSkiplistIterator *iterator);

UniqSkiplistNode *uniq_skiplist_find_ge_node(UniqSkiplist *sl, void *data);

static inline void *uniq_skiplist_find_ge(UniqSkiplist *sl, void *data)
{
    UniqSkiplistNode *node;
    node = uniq_skiplist_find_ge_node(sl, data);
    if (node == NULL) {
        return NULL;
    }

    return node->data;
}

static inline void uniq_skiplist_iterator(UniqSkiplist *sl,
        UniqSkiplistIterator *iterator)
{
    iterator->current = sl->top->links[0];
    iterator->tail = sl->factory->tail;
}

static inline void *uniq_skiplist_next(UniqSkiplistIterator *iterator)
{
    void *data;

    if (iterator->current == iterator->tail) {
        return NULL;
    }

    data = iterator->current->data;
    iterator->current = iterator->current->links[0];
    return data;
}

static inline int uniq_skiplist_iterator_count(UniqSkiplistIterator *iterator)
{
    volatile UniqSkiplistNode *current;
    int count;

    count = 0;
    current = iterator->current;
    while (current != iterator->tail) {
        ++count;
        current = current->links[0];
    }

    return count;
}

static inline void *uniq_skiplist_get_first(UniqSkiplist *sl)
{
    if (sl->top->links[0] != sl->factory->tail) {
        return sl->top->links[0]->data;
    } else {
        return NULL;
    }
}

static inline bool uniq_skiplist_empty(UniqSkiplist *sl)
{
    return sl->top->links[0] == sl->factory->tail;
}

#define LEVEL0_DOUBLE_CHAIN_NEXT_LINK(node)  node->links[0]
#define LEVEL0_DOUBLE_CHAIN_PREV_LINK(node)  node->links[node->level_index + 1]
#define LEVEL0_DOUBLE_CHAIN_TAIL(sl)  LEVEL0_DOUBLE_CHAIN_PREV_LINK(sl->top)

#define UNIQ_SKIPLIST_LEVEL0_TAIL_NODE(sl)    ((UniqSkiplistNode *) \
        LEVEL0_DOUBLE_CHAIN_TAIL(sl))

#define UNIQ_SKIPLIST_LEVEL0_PREV_NODE(node)  ((UniqSkiplistNode *) \
        LEVEL0_DOUBLE_CHAIN_PREV_LINK(node))

#define UNIQ_SKIPLIST_LEVEL0_NEXT_NODE(node)  ((UniqSkiplistNode *) \
        LEVEL0_DOUBLE_CHAIN_NEXT_LINK(node))

#ifdef __cplusplus
}
#endif

#endif

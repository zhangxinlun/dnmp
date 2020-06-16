/**
* Copyright (C) 2015 Happy Fish / YuQing
*
* libfastcommon may be copied only under the terms of the GNU General
* Public License V3, which may be found in the FastDFS source kit.
* Please visit the FastDFS Home Page http://www.fastken.com/ for more detail.
**/

//flat_skiplist.c

#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include "logger.h"
#include "flat_skiplist.h"

int flat_skiplist_init_ex(FlatSkiplist *sl, const int level_count,
        skiplist_compare_func compare_func, skiplist_free_func free_func,
        const int min_alloc_elements_once)
{
    int bytes;
    int element_size;
    int i;
    int alloc_elements_once;
    int result;
    struct fast_mblock_man *top_mblock;

    if (level_count <= 0) {
        logError("file: "__FILE__", line: %d, "
                "invalid level count: %d",
                __LINE__, level_count);
        return EINVAL;
    }

    if (level_count > 30) {
        logError("file: "__FILE__", line: %d, "
                "level count: %d is too large",
                __LINE__, level_count);
        return E2BIG;
    }

    bytes = sizeof(FlatSkiplistNode *) * level_count;
    sl->tmp_previous = (FlatSkiplistNode **)malloc(bytes);
    if (sl->tmp_previous == NULL) {
        logError("file: "__FILE__", line: %d, "
                "malloc %d bytes fail, errno: %d, error info: %s",
                __LINE__, bytes, errno, STRERROR(errno));
        return errno != 0 ? errno : ENOMEM;
    }

    bytes = sizeof(struct fast_mblock_man) * level_count;
    sl->mblocks = (struct fast_mblock_man *)malloc(bytes);
    if (sl->mblocks == NULL) {
        logError("file: "__FILE__", line: %d, "
                "malloc %d bytes fail, errno: %d, error info: %s",
                __LINE__, bytes, errno, STRERROR(errno));
        return errno != 0 ? errno : ENOMEM;
    }
    memset(sl->mblocks, 0, bytes);

    alloc_elements_once = min_alloc_elements_once;
    if (alloc_elements_once <= 0) {
        alloc_elements_once = SKIPLIST_DEFAULT_MIN_ALLOC_ELEMENTS_ONCE;
    }
    else if (alloc_elements_once > 1024) {
        alloc_elements_once = 1024;
    }

    for (i=level_count-1; i>=0; i--) {
        element_size = sizeof(FlatSkiplistNode) +
            sizeof(FlatSkiplistNode *) * (i + 1);
        if ((result=fast_mblock_init_ex(sl->mblocks + i,
            element_size, alloc_elements_once, NULL, NULL, false)) != 0)
        {
            return result;
        }
        if (i % 2 == 0 && alloc_elements_once < 64 * 1024) {
            alloc_elements_once *= 2;
        }
    }

    sl->top_level_index = level_count - 1;
    top_mblock = sl->mblocks + sl->top_level_index;
    sl->top = (FlatSkiplistNode *)fast_mblock_alloc_object(top_mblock);
    if (sl->top == NULL) {
        return ENOMEM;
    }
    memset(sl->top, 0, top_mblock->info.element_size);

    sl->tail = (FlatSkiplistNode *)fast_mblock_alloc_object(sl->mblocks + 0);
    if (sl->tail == NULL) {
        return ENOMEM;
    }
    memset(sl->tail, 0, sl->mblocks[0].info.element_size);

    sl->tail->prev = sl->top;
    for (i=0; i<level_count; i++) {
        sl->top->links[i] = sl->tail;
    }

    sl->level_count = level_count;
    sl->compare_func = compare_func;
    sl->free_func = free_func;

    srand(time(NULL));
    return 0;
}

void flat_skiplist_destroy(FlatSkiplist *sl)
{
    int i;
    FlatSkiplistNode *node;
    FlatSkiplistNode *deleted;

    if (sl->mblocks == NULL) {
        return;
    }

    if (sl->free_func != NULL) {
        node = sl->top->links[0];
        while (node != sl->tail) {
            deleted = node;
            node = node->links[0];
            sl->free_func(deleted->data);
        }
    }

    for (i=0; i<sl->level_count; i++) {
        fast_mblock_destroy(sl->mblocks + i);
    }

    free(sl->mblocks);
    sl->mblocks = NULL;
}

static inline int flat_skiplist_get_level_index(FlatSkiplist *sl)
{
    int i;

    for (i=0; i<sl->top_level_index; i++) {
        if (rand() < RAND_MAX / 2) {
            break;
        }
    }

    return i;
}

int flat_skiplist_insert(FlatSkiplist *sl, void *data)
{
    int i;
    int level_index;
    FlatSkiplistNode *node;
    FlatSkiplistNode *previous;

    level_index = flat_skiplist_get_level_index(sl);
    node = (FlatSkiplistNode *)fast_mblock_alloc_object(sl->mblocks + level_index);
    if (node == NULL) {
        return ENOMEM;
    }
    node->data = data;

    previous = sl->top;
    for (i=sl->top_level_index; i>level_index; i--) {
        while (previous->links[i] != sl->tail && sl->compare_func(data,
                    previous->links[i]->data) < 0)
        {
            previous = previous->links[i];
        }
    }

    while (i >= 0) {
        while (previous->links[i] != sl->tail && sl->compare_func(data,
                    previous->links[i]->data) < 0)
        {
            previous = previous->links[i];
        }

        sl->tmp_previous[i] = previous;
        i--;
    }

    //set previous links of level 0
    node->prev = previous;
    previous->links[0]->prev = node;

    //thread safe for one write with many read model
    for (i=0; i<=level_index; i++) {
        node->links[i] = sl->tmp_previous[i]->links[i];
        sl->tmp_previous[i]->links[i] = node;
    }

    return 0;
}

static FlatSkiplistNode *flat_skiplist_get_previous(FlatSkiplist *sl, void *data,
        int *level_index)
{
    int i;
    int cmp;
    FlatSkiplistNode *previous;

    previous = sl->top;
    for (i=sl->top_level_index; i>=0; i--) {
        while (previous->links[i] != sl->tail) {
            cmp = sl->compare_func(data, previous->links[i]->data);
            if (cmp > 0) {
                break;
            }
            else if (cmp == 0) {
                *level_index = i;
                return previous;
            }

            previous = previous->links[i];
        }
    }

    return NULL;
}

static FlatSkiplistNode *flat_skiplist_get_first_larger_or_equal(
        FlatSkiplist *sl, void *data)
{
    int i;
    int cmp;
    FlatSkiplistNode *previous;
    FlatSkiplistNode *current;

    previous = sl->top;
    for (i=sl->top_level_index; i>=0; i--) {
        while (previous->links[i] != sl->tail) {
            cmp = sl->compare_func(data, previous->links[i]->data);
            if (cmp > 0) {
                break;
            }
            else if (cmp == 0) {
                current = previous->links[i]->links[0];
                while ((current != sl->tail) && (sl->compare_func(
                            data, current->data) == 0))
                {
                    current = current->links[0];
                }
                return current->prev;
            }

            previous = previous->links[i];
        }
    }

    return previous;
}

static FlatSkiplistNode *flat_skiplist_get_first_larger(
        FlatSkiplist *sl, void *data)
{
    int i;
    int cmp;
    FlatSkiplistNode *previous;

    previous = sl->top;
    for (i=sl->top_level_index; i>=0; i--) {
        while (previous->links[i] != sl->tail) {
            cmp = sl->compare_func(data, previous->links[i]->data);
            if (cmp > 0) {
                break;
            }
            else if (cmp == 0) {
                previous = previous->links[i]->prev;
                while ((previous != sl->top) && (sl->compare_func(
                                data, previous->data) == 0))
                {
                    previous = previous->prev;
                }

                return previous;
            }

            previous = previous->links[i];
        }
    }

    return previous;
}

int flat_skiplist_delete(FlatSkiplist *sl, void *data)
{
    int i;
    int level_index;
    FlatSkiplistNode *previous;
    FlatSkiplistNode *deleted;

    previous = flat_skiplist_get_previous(sl, data, &level_index);
    if (previous == NULL) {
        return ENOENT;
    }

    deleted = previous->links[level_index];
    for (i=level_index; i>=0; i--) {
        while (previous->links[i] != sl->tail && previous->links[i] != deleted) {
            previous = previous->links[i];
        }

        assert(previous->links[i] == deleted);
        previous->links[i] = previous->links[i]->links[i];
    }

    deleted->links[0]->prev = previous;

    if (sl->free_func != NULL) {
        sl->free_func(deleted->data);
    }
    fast_mblock_free_object(sl->mblocks + level_index, deleted);
    return 0;
}

int flat_skiplist_delete_all(FlatSkiplist *sl, void *data, int *delete_count)
{
    *delete_count = 0;
    while (flat_skiplist_delete(sl, data) == 0) {
        (*delete_count)++;
    }

    return *delete_count > 0 ? 0 : ENOENT;
}

void *flat_skiplist_find(FlatSkiplist *sl, void *data)
{
    int level_index;
    FlatSkiplistNode *previous;

    previous = flat_skiplist_get_previous(sl, data, &level_index);
    return (previous != NULL) ? previous->links[level_index]->data : NULL;
}

int flat_skiplist_find_all(FlatSkiplist *sl, void *data, FlatSkiplistIterator *iterator)
{
    int level_index;
    FlatSkiplistNode *previous;
    FlatSkiplistNode *last;

    previous = flat_skiplist_get_previous(sl, data, &level_index);
    if (previous == NULL) {
        iterator->top = sl->top;
        iterator->current = sl->top;
        return ENOENT;
    }

    previous = previous->links[level_index];
    last = previous->links[0];
    while (last != sl->tail && sl->compare_func(data, last->data) == 0) {
        last = last->links[0];
    }

    do {
        previous = previous->prev;
    } while (previous != sl->top && sl->compare_func(data, previous->data) == 0);

    iterator->top = previous;
    iterator->current = last->prev;
    return 0;
}

void *flat_skiplist_find_ge(FlatSkiplist *sl, void *data)
{
    FlatSkiplistNode *node;
    node = flat_skiplist_get_first_larger_or_equal(sl, data);
    if (node == sl->top) {
        return NULL;
    }
    return node->data;
}

int flat_skiplist_find_range(FlatSkiplist *sl, void *start_data, void *end_data,
        FlatSkiplistIterator *iterator)
{
    if (sl->compare_func(start_data, end_data) > 0) {
        iterator->current = sl->top;
        iterator->top = sl->top;
        return EINVAL;
    }

    iterator->current = flat_skiplist_get_first_larger_or_equal(sl, start_data);
    if (iterator->current == sl->top) {
        iterator->top = sl->top;
        return ENOENT;
    }

    iterator->top = flat_skiplist_get_first_larger(sl, end_data);
    return iterator->current != iterator->top ? 0 : ENOENT;
}

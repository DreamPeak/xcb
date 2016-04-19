/*
 * Copyright (c) 2013-2016, Dalian Futures Information Technology Co., Ltd.
 *
 * Gaohang Wu  <wugaohang at dce dot com dot cn>
 * Xiaoye Meng <mengxiaoye at dce dot com dot cn>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

/*
 * adapted from Tahseen Ur Rehman's RadixTree implementation
 * (http://code.google.com/p/radixtree)
 */

#ifndef PTRIE_INCLUDED
#define PTRIE_INCLUDED

#include "dlist.h"

/* exported types */
typedef struct ptrie_t      *ptrie_t;
typedef struct ptrie_node_t *ptrie_node_t;

/* FIXME: exported functions */
extern ptrie_t      ptrie_new(void);
extern void         ptrie_free(ptrie_t *pp);
extern ptrie_node_t ptrie_node_parent(ptrie_node_t node);
extern char        *ptrie_node_key(ptrie_node_t node);
extern dlist_t      ptrie_node_value(ptrie_node_t node);
extern ptrie_node_t ptrie_get_root(ptrie_t ptrie);
extern ptrie_node_t ptrie_set_index(ptrie_t ptrie, const char *index);
extern ptrie_node_t ptrie_get_index(ptrie_t ptrie, const char *index);
extern int          ptrie_insert(ptrie_node_t node, const char *key, void *value);
extern ptrie_node_t ptrie_find(ptrie_node_t node, const char *key);
extern int          ptrie_remove(ptrie_node_t node, const char *key, void *value);
extern ptrie_node_t ptrie_search_prefix(ptrie_node_t node, const char *key, void *value);
extern void         ptrie_lock(ptrie_t ptrie);
extern void         ptrie_unlock(ptrie_t ptrie);
extern void         ptrie_rwlock_rdlock(ptrie_t ptrie);
extern void         ptrie_rwlock_wrlock(ptrie_t ptrie);
extern void         ptrie_rwlock_unlock(ptrie_t ptrie);

#endif /* PTRIE_INCLUDED */


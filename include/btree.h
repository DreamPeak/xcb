/*
 * Copyright (c) 2013-2016, Dalian Futures Information Technology Co., Ltd.
 *
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

#ifndef BTREE_INCLUDED
#define BTREE_INCLUDED

/* FIXME: exported types */
typedef struct btree_t *btree_t;

/* FIXME: exported functions */
extern btree_t       btree_new(int t, int cmp(const void *x, const void *y));
extern void          btree_free(btree_t* bp);
extern unsigned long btree_length(btree_t btree);
extern void         *btree_insert(btree_t btree, const void *key, void *value);
extern void         *btree_find(btree_t btree, const void *key);
extern void         *btree_remove(btree_t btree, const void *key);

#endif /* BTREE_INCLUDED */


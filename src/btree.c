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

#include <string.h>
#include "mem.h"
#include "btree.h"

/* FIXME */
struct node {
	unsigned char	leaf:1;
	/* the number of keys currently stored in the node */
	int		n;
	struct {
		const void	*key;
		void		*value;
	}		*bindings;
	union {
		/* pointers to the node's children */
		struct node	**children;
		struct {
			struct node	*prev, *next;
		}		pn;
	}		u;
	struct node	*parent;
};
struct btree_t {
	/* minimum degree */
	int		t;
	unsigned long	length;
	struct node	*root, *sentinel;
	int		(*cmp)(const void *x, const void *y);
};

#define BSEARCH(x, skey, i, k) \
	{ \
		int lo = 0, hi = x->n - 1; \
		while (lo <= hi) { \
			i = (lo + hi) >> 1; \
			if ((k = (*btree->cmp)(skey, x->bindings[i].key)) == 0) \
				break; \
			k > 0 ? (lo = i + 1) : (hi = i - 1); \
		} \
	}

/* FIXME */
static struct node *node_new(int t) {
	struct node *node;

	if (NEW0(node) == NULL)
		return NULL;
	if ((node->bindings = ALLOC((2 * t - 1) * sizeof *node->bindings)) == NULL) {
		FREE(node);
		return NULL;
	}
	memset(node->bindings, '\0', (2 * t - 1) * sizeof *node->bindings);
	return node;
}

/* FIXME */
static void node_free(struct node *node) {
	if (node) {
		FREE(node->bindings);
		if (node->leaf == 0)
			FREE(node->u.children);
		FREE(node);
	}
}

static int cmpdefault(const void *x, const void *y) {
	return strcmp((char *)x, (char *)y);
}

static int btree_split_child(btree_t btree, struct node *x, int i) {
	struct node *y, *z;
	int j;

	if (x == NULL)
		return -1;
	y = x->u.children[i];
	if ((z = node_new(btree->t)) == NULL)
		return -1;
	if (y->leaf == 0) {
		if ((z->u.children = ALLOC(2 * btree->t * sizeof *z->u.children)) == NULL) {
			node_free(z);
			return -1;
		}
		memset(z->u.children, '\0', 2 * btree->t * sizeof *z->u.children);
		for (j = 0; j < btree->t; ++j) {
			z->u.children[j] = y->u.children[j + btree->t];
			y->u.children[j + btree->t] = NULL;
		}
	} else {
		z->u.pn.prev = y;
		z->u.pn.next = y->u.pn.next;
		y->u.pn.next->u.pn.prev = z;
		y->u.pn.next = z;
	}
	z->leaf   = y->leaf;
	z->n      = btree->t - 1;
	for (j = 0; j < btree->t - 1; ++j) {
		z->bindings[j].key   = y->bindings[j + btree->t].key;
		z->bindings[j].value = y->bindings[j + btree->t].value;
		y->bindings[j + btree->t].key   = NULL;
		y->bindings[j + btree->t].value = NULL;
	}
	z->parent = x;
	for (j = x->n; j > i; --j)
		x->u.children[j + 1] = x->u.children[j];
	x->u.children[i + 1] = z;
	/* FIXME */
	for (j = x->n - 1; j > i - 1; --j) {
		x->bindings[j + 1].key   = x->bindings[j].key;
		x->bindings[j + 1].value = x->bindings[j].value;
	}
	x->bindings[i].key   = y->bindings[btree->t - 1].key;
	x->bindings[i].value = y->bindings[btree->t - 1].value;
	if (y->leaf == 0) {
		y->bindings[btree->t - 1].key   = NULL;
		y->bindings[btree->t - 1].value = NULL;
		y->n = btree->t - 1;
	} else
		y->n = btree->t;
	++x->n;
	return 0;
}

static void *btree_put_nonfull(btree_t btree, struct node *x, const void *key, void *value) {
	/* avoid gcc warning */
	int i = 0, j = 0, k = 0;

	if (x == NULL)
		return NULL;
	while (x->leaf == 0) {
		BSEARCH(x, key, i, k);
		if (k > 0)
			++i;
		if (x->u.children[i]->n == 2 * btree->t - 1) {
			if (btree_split_child(btree, x, i) == -1)
				return NULL;
			if ((*btree->cmp)(key, x->bindings[i].key) > 0)
				++i;
		}
		x = x->u.children[i];
	}
	i = -1, k = 1;
	BSEARCH(x, key, i, k);
	if (k == 0) {
		void *prev;

		prev = x->bindings[i].value;
		x->bindings[i].value = value;
		return prev;
	} else if (k < 0)
		--i;
	/* FIXME */
	for (j = x->n - 1; j > i; --j) {
		x->bindings[j + 1].key   = x->bindings[j].key;
		x->bindings[j + 1].value = x->bindings[j].value;
	}
	x->bindings[i + 1].key   = key;
	x->bindings[i + i].value = value;
	++x->n;
	++btree->length;
	return NULL;
}

btree_t btree_new(int t, int cmp(const void *x, const void *y)) {
	struct node *x, *y;
	btree_t btree;

	/* 2-3-4 tree when t == 2 */
	if (t < 2)
		return NULL;
	if ((x = node_new(t)) == NULL)
		return NULL;
	if ((y = node_new(t)) == NULL) {
		node_free(x);
		return NULL;
	}
	x->leaf      = 1;
	y->leaf      = 1;
	x->n         = 0;
	y->n         = 0;
	x->u.pn.next = x->u.pn.prev = y;
	y->u.pn.next = y->u.pn.prev = x;
	if (NEW(btree) == NULL) {
		node_free(y);
		node_free(x);
		return NULL;
	}
	btree->t        = t;
	btree->length   = 0;
	btree->root     = x;
	btree->sentinel = y;
	btree->cmp      = cmp ? cmp : cmpdefault;
	return btree;
}

/* FIXME */
void btree_free(btree_t *bp) {
	struct node *x;

	if (bp == NULL || *bp == NULL)
		return;
	x = (*bp)->sentinel->u.pn.next;
	while (x != (*bp)->sentinel) {
		struct node *y = x->parent, *z = x;

		while (y) {
			int i;
			struct node *tmp = y;

			for (i = 0; i < y->n; ++i)
				y->u.children[i]->parent = NULL;
			y = y->parent;
			node_free(tmp);
		}
		x = x->u.pn.next;
		node_free(z);
	}
	node_free(x);
	FREE(*bp);
}

unsigned long btree_length(btree_t btree) {
	if (btree == NULL)
		return 0;
	return btree->length;
}

void *btree_insert(btree_t btree, const void *key, void *value) {
	struct node *x;

	if (btree == NULL || key == NULL)
		return NULL;
	x = btree->root;
	if (x->n == 2 * btree->t - 1) {
		struct node *y;

		if ((y = node_new(btree->t)) == NULL)
			return NULL;
		y->leaf = 0;
		y->n    = 0;
		if ((y->u.children = ALLOC(2 * btree->t * sizeof *y->u.children)) == NULL) {
			node_free(y);
			return NULL;
		}
		memset(y->u.children, '\0', 2 * btree->t * sizeof *y->u.children);
		y->u.children[0] = x;
		x->parent = y;
		btree->root = y;
		if (btree_split_child(btree, y, 0) == -1)
			return NULL;
		return btree_put_nonfull(btree, y, key, value);
	} else
		return btree_put_nonfull(btree, x, key, value);
}

void *btree_find(btree_t btree, const void *key) {
	struct node *x;
	/* avoid gcc warning */
	int i = 0, k = 0;

	if (btree == NULL || key == NULL)
		return NULL;
	x = btree->root;
	while (x->leaf == 0) {
		BSEARCH(x, key, i, k);
		if (k > 0)
			++i;
		else if (k == 0)
			return x->bindings[i].value;
		x = x->u.children[i];
	}
	i = -1, k = 1;
	BSEARCH(x, key, i, k);
	if (k == 0)
		return x->bindings[i].value;
	return NULL;
}

/* FIXME */
void *btree_remove(btree_t btree, const void *key) {
	return NULL;
}


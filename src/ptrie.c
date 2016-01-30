/*
 * Copyright (c) 2013-2016, Dalian Futures Information Technology Co., Ltd.
 *
 * Gaohang Wu
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

#include <pthread.h>
#include <string.h>
#include "mem.h"
#include "ptrie.h"

/* FIXME */
struct ptrie_t {
	ptrie_node_t		root, indices;
	pthread_mutex_t		lock;
	pthread_rwlock_t	rwlock;
};
struct ptrie_node_t {
	char			*key;
	dlist_t			value;
	ptrie_node_t		parent, prev, next, children;
};

#define STEP_DOWN(nmatches, newkey, x) \
	while (nmatches < strlen(newkey) && nmatches == strlen(x->key) && x->children) { \
		ptrie_node_t y = x->children; \
		char *subkey = newkey + nmatches; \
		int flag = 0; \
		while (y) { \
			if (subkey[0] == y->key[0]) { \
				flag = 1; \
				newkey = subkey; \
				x = y; \
				nmatches = get_nmatches(newkey, x->key); \
				break; \
			} \
			y = y->next; \
		} \
		if (!flag) break; \
	}

/* FIXME */
static ptrie_node_t node_new(const char *key) {
	ptrie_node_t node;

	if (NEW0(node) == NULL)
		return NULL;
	if ((node->key = mem_strndup(key, strlen(key))) == NULL) {
		FREE(node);
		return NULL;
	}
	return node;
}

/* FIXME */
static void node_free(ptrie_node_t node) {
	if (node) {
		FREE(node->key);
		if (node->value)
			dlist_free(&node->value);
		FREE(node);
	}
}

static unsigned get_nmatches(const char *x, const char *y) {
	unsigned n = 0, xlen = strlen(x), ylen = strlen(y);

	while (n < xlen && n < ylen) {
		if (x[n] != y[n])
			break;
		++n;
	}
	return n;
}

/* FIXME */
static void merge(ptrie_node_t x, ptrie_node_t y) {
	if (x == NULL || y == NULL)
		return;
	if (RESIZE(x->key, strlen(x->key) + strlen(y->key) + 1) == NULL)
		return;
	strcat(x->key, y->key);
	x->value    = y->value;
	y->value    = NULL;
	x->children = y->children;
	node_free(y);
}

ptrie_t ptrie_new(void) {
	ptrie_node_t node;
	ptrie_t ptrie;
	pthread_mutexattr_t mattr;
	pthread_rwlockattr_t rwattr;

	if ((node = node_new("")) == NULL)
		return NULL;
	node->value = dlist_new(NULL, NULL);
	if (NEW0(ptrie) == NULL) {
		node_free(node);
		return NULL;
	}
	ptrie->root = node;
	pthread_mutexattr_init(&mattr);
	pthread_mutexattr_settype(&mattr, PTHREAD_MUTEX_ADAPTIVE_NP);
	pthread_mutex_init(&ptrie->lock, &mattr);
	pthread_mutexattr_destroy(&mattr);
	pthread_rwlockattr_init(&rwattr);
	pthread_rwlockattr_setkind_np(&rwattr, PTHREAD_RWLOCK_PREFER_WRITER_NONRECURSIVE_NP);
	pthread_rwlock_init(&ptrie->rwlock, &rwattr);
	pthread_rwlockattr_destroy(&rwattr);
	return ptrie;
}

/* FIXME */
void ptrie_free(ptrie_t *pp) {
	ptrie_node_t x;

	if (pp == NULL || *pp == NULL)
		return;
	pthread_mutex_destroy(&(*pp)->lock);
	pthread_rwlock_destroy(&(*pp)->rwlock);
	x = (*pp)->root;
	while (x)
		if (x->children)
			x = x->children;
		else {
			ptrie_node_t y = x;

			x = x->parent;
			if (y->next == NULL)
				x->children = NULL;
			else {
				y->next->prev = NULL;
				x->children = y->next;
                        }
			node_free(y);
		}
	FREE(*pp);
}

char *ptrie_node_key(ptrie_node_t node) {
	if (node == NULL)
		return NULL;
	return node->key;
}

dlist_t ptrie_node_value(ptrie_node_t node) {
	if (node == NULL)
		return NULL;
	return node->value;
}

ptrie_node_t ptrie_node_parent(ptrie_node_t node) {
	if (node == NULL)
		return NULL;
	return node->parent;
}

/* FIXME */
ptrie_node_t ptrie_get_root(ptrie_t ptrie) {
	if (ptrie == NULL)
		return NULL;
	return ptrie->root;
}

/* FIXME */
ptrie_node_t ptrie_set_index(ptrie_t ptrie, const char *index) {
	ptrie_node_t node;

	if (ptrie == NULL || index == NULL)
		return NULL;
	if ((node = node_new(index)) == NULL)
		return NULL;
	node->value  = dlist_new(NULL, NULL);
	node->parent = ptrie->root;
	if (ptrie->indices) {
		node->next = ptrie->indices;
		ptrie->indices->prev = node;
	}
	ptrie->root->children = ptrie->indices = node;
	return node;
}

/* FIXME */
ptrie_node_t ptrie_get_index(ptrie_t ptrie, const char *index) {
	ptrie_node_t node;

	if (ptrie == NULL || index == NULL)
		return NULL;
	for (node = ptrie->indices; node; node = node->next)
		if (!strcmp(node->key, index))
			return node;
	return NULL;
}

int ptrie_insert(ptrie_node_t node, const char *key, void *value) {
	char *newkey = (char *)key;
	ptrie_node_t x = node, y, z;
	unsigned nmatches;

	if (node == NULL || key == NULL)
		return -1;
	nmatches = get_nmatches(newkey, x->key);
	STEP_DOWN(nmatches, newkey, x);
	/* exact match */
	if (nmatches == strlen(newkey) && nmatches == strlen(x->key)) {
		if (dlist_find(x->value, value))
			return -1;
		dlist_insert_tail(x->value, value);
	/* split the current node */
	} else if (nmatches > 0 && nmatches < strlen(x->key)) {
		if ((y = node_new(x->key)) == NULL)
			return -1;
		y->value    = x->value;
		y->parent   = x;
		y->children = x->children;
		if (nmatches < strlen(newkey)) {
			if ((z = node_new(newkey)) == NULL) {
				node_free(y);
				return -1;
			}
			z->value  = dlist_new(NULL, NULL);
			dlist_insert_tail(z->value, value);
			z->parent = x;
			z->prev   = y;
			y->next   = z;
			x->value  = dlist_new(NULL, NULL);
		} else {
			x->value  = dlist_new(NULL, NULL);
			dlist_insert_tail(x->value, value);
		}
		/* FIXME */
		x->key[nmatches] = '\0';
		x->children = y;
	/* add node as the child of the current node */
	} else {
		if ((y = node_new(newkey)) == NULL)
			return -1;
		y->value = dlist_new(NULL, NULL);
		dlist_insert_tail(y->value, value);
		y->parent = x;
		if (x->children) {
			y->next  = x->children;
			x->children->prev = y;
		}
		x->children = y;
	}
	return 0;
}

ptrie_node_t ptrie_find(ptrie_node_t node, const char *key) {
	char *newkey = (char *)key;
	ptrie_node_t x = node;
	unsigned nmatches;

	if (node == NULL || key == NULL)
		return NULL;
	nmatches = get_nmatches(newkey, x->key);
	STEP_DOWN(nmatches, newkey, x);
	return nmatches == strlen(x->key) ? x : NULL;
}

int ptrie_remove(ptrie_node_t node, const char *key, void *value) {
	char *newkey = (char *)key;
	ptrie_node_t x = node;
	unsigned nmatches;

	if (node == NULL || key == NULL)
		return -1;
	nmatches = get_nmatches(newkey, x->key);
	STEP_DOWN(nmatches, newkey, x);
	/* only the exact match case is worth considering */
	if (nmatches == strlen(newkey) && nmatches == strlen(x->key)) {
		dlist_node_t dnode;

		if ((dnode = dlist_find(x->value, value))) {
			dlist_remove(x->value, dnode);
			if (dlist_length(x->value) == 0) {
				if (x->children == NULL) {
					ptrie_node_t w = x->parent;

					if (x->prev)
						x->prev->next = x->next;
					if (x->next)
						x->next->prev = x->prev;
					if (w->children == x)
						w->children = x->next;
					node_free(x);
					if (w != node && w->children && w->children->prev == NULL &&
						w->children->next == NULL && dlist_length(w->value) == 0)
						merge(w, w->children);
				} else if (x != node && x->children && x->children->prev == NULL &&
					x->children->next == NULL)
					merge(x, x->children);
			}
			return 0;
		}
	}
	return -1;
}

ptrie_node_t ptrie_search_prefix(ptrie_node_t node, const char *key, void *value) {
	char *newkey = (char *)key;
	ptrie_node_t x = node;
	unsigned nmatches;

	if (node == NULL || key == NULL)
		return NULL;
	nmatches = get_nmatches(newkey, x->key);
	/* step down */
	while (nmatches < strlen(newkey) && nmatches == strlen(x->key)) {
		if (dlist_find(x->value, value))
			return x;
		if (x->children) {
			ptrie_node_t y = x->children;
			char *subkey = newkey + nmatches;
			int flag = 0;

			while (y) {
				if (subkey[0] == y->key[0]) {
					flag = 1;
					newkey = subkey;
					x = y;
					nmatches = get_nmatches(newkey, x->key);
					break;
				}
				y = y->next;
			}
			if (!flag)
				break;
		} else
			break;
	}
	if (nmatches == strlen(newkey) && nmatches < strlen(x->key)) {
		dlist_t queue = dlist_new(NULL, NULL);

		dlist_insert_tail(queue, x);
		while (dlist_length(queue)) {
			x = dlist_remove_head(queue);
			if (dlist_find(x->value, value)) {
				dlist_free(&queue);
				return x;
			}
			if (x->children) {
				x = x->children;
				dlist_insert_tail(queue, x);
				while (x->next) {
					dlist_insert_tail(queue, x);
					x = x->next;
				}
			}
		}
	}
	return NULL;
}


void ptrie_lock(ptrie_t ptrie) {
	if (ptrie == NULL)
		return;
	pthread_mutex_lock(&ptrie->lock);
}

void ptrie_unlock(ptrie_t ptrie) {
	if (ptrie == NULL)
		return;
	pthread_mutex_unlock(&ptrie->lock);
}

void ptrie_rwlock_rdlock(ptrie_t ptrie) {
	if (ptrie == NULL)
		return;
	pthread_rwlock_rdlock(&ptrie->rwlock);
}

void ptrie_rwlock_wrlock(ptrie_t ptrie) {
	if (ptrie == NULL)
		return;
	pthread_rwlock_wrlock(&ptrie->rwlock);
}

void ptrie_rwlock_unlock(ptrie_t ptrie) {
	if (ptrie == NULL)
		return;
	pthread_rwlock_unlock(&ptrie->rwlock);
}


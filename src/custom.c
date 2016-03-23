/*
 * Copyright (c) 2013-2016, Dalian Futures Information Technology Co., Ltd.
 *
 * Gaohang Wu
 * Bo Wang
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

#include "fmacros.h"
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <time.h>
#include "mem.h"
#include "dlist.h"
#include "table.h"
#include "ptrie.h"
#include "btree.h"
#include "dstr.h"
#include "utils.h"
#include "logger.h"
#include "net.h"
#include "db.h"
#include "commons.h"

/* FIXME */
struct crss {
	client	c;
	dstr	rid, match, start, stop;
	int	cancel;
};

/* FIXME */
extern int last_quote;
extern table_t cache;
extern ptrie_t subscribers;
extern int persistence;
extern db_t *db;
extern db_readoptions_t *db_ro;
extern table_t idxfmts;
extern const char *password;
extern const char *dpip;
extern int dpport;

/* FIXME */
extern dstr get_indices(void);

/* FIXME */
static table_t rids = NULL;

void s_command(client c) {
	dstr pkey, skey;
	int i;
	dlist_t dlist;

	if (dstr_length(c->argv[0]) == 1) {
		add_reply_error(c, "index can't be empty\r\n");
		return;
	}
	pkey = dstr_new(c->argv[0] + 1);
	skey = dstr_new(pkey);
	if (c->argc > 1)
		for (i = 1; i < c->argc; ++i) {
			skey = dstr_cat(skey, ",");
			skey = dstr_cat(skey, c->argv[i]);
		}
	if (last_quote) {
		btree_t btree;

		table_lock(cache);
		if ((btree = table_get_value(cache, pkey))) {
			btree_node_t node = btree_find(btree, skey, &i);

			while (node) {
				int n = btree_node_n(node);

				for (; i < n; ++i) {
					dstr key   = (dstr)btree_node_key(node, i);
					dstr value = (dstr)btree_node_value(node, i);
					dstr *fields = NULL, *fields2 = NULL;
					int nfield = 0, nfield2 = 0, j;
					dstr res, contracts = NULL;

					/* FIXME */
					if (dstr_length(key) < dstr_length(skey) ||
						memcmp(key, skey, dstr_length(skey)))
						break;
					fields = dstr_split_len(key, dstr_length(key), ",", 1, &nfield);
					res = dstr_new(fields[0]);
					if (nfield > 1) {
						contracts = dstr_new(fields[1]);
						for (j = 2; j < nfield; ++j) {
							contracts = dstr_cat(contracts, ",");
							contracts = dstr_cat(contracts, fields[j]);
						}
					}
					fields2 = dstr_split_len(value, dstr_length(value), ",", 1, &nfield2);
					res = dstr_cat(res, ",");
					res = dstr_cat(res, fields2[0]);
					if (contracts) {
						res = dstr_cat(res, ",");
						res = dstr_cat(res, contracts);
					}
					for (j = 1; j < nfield2; ++j) {
						res = dstr_cat(res, ",");
						res = dstr_cat(res, fields2[j]);
					}
					res = dstr_cat(res, "\r\n");
					pthread_spin_lock(&c->lock);
					if (!(c->flags & CLIENT_CLOSE_ASAP)) {
						if (net_try_write(c->fd, res, dstr_length(res),
							10, NET_NONBLOCK) == -1) {
							xcb_log(XCB_LOG_WARNING, "Writing to client '%p': %s",
								c, strerror(errno));
							if (++c->eagcount >= 3) {
								client_free_async(c);
								pthread_spin_unlock(&c->lock);
								dstr_free_tokens(fields2, nfield2);
								dstr_free(contracts);
								dstr_free(res);
								dstr_free_tokens(fields, nfield);
								table_unlock(cache);
								dstr_free(skey);
								dstr_free(pkey);
								return;
							}
						} else if (c->eagcount)
							c->eagcount = 0;
					}
					pthread_spin_unlock(&c->lock);
					dstr_free_tokens(fields2, nfield2);
					dstr_free(contracts);
					dstr_free(res);
					dstr_free_tokens(fields, nfield);
				}
				node = btree_node_next(node);
				i = 0;
			}
		}
		table_unlock(cache);
	}
	ptrie_rwlock_wrlock(subscribers);
	dlist = ptrie_node_value(ptrie_get_root(subscribers));
	if (dlist_find(dlist, c)) {
		add_reply_error(c, "subscribe/unsubscribe is symmetrical. already subscribed 'ALL'\r\n");
		dstr_free(skey);
	} else {
		ptrie_node_t node, node2;

		if ((node = ptrie_get_index(subscribers, pkey)) == NULL)
			node = ptrie_set_index(subscribers, pkey);
		if ((node2 = ptrie_search_prefix(node, skey, c))) {
			dlist_iter_t diter;
			dlist_node_t dnode;
			dstr path = dstr_new("");

			dlist = dlist_new(NULL, NULL);
			while (ptrie_node_parent(node2)) {
				dlist_insert_head(dlist, ptrie_node_key(node2));
				node2 = ptrie_node_parent(node2);
			}
			diter = dlist_iter_new(dlist, DLIST_START_HEAD);
			while ((dnode = dlist_next(diter)))
				path = dstr_cat(path, dlist_node_value(dnode));
			add_reply_error_format(c, "subscribe/unsubscribe is symmetrical. "
				"already subscribed '%s'\r\n", path);
			dlist_iter_free(&diter);
			dlist_free(&dlist);
			dstr_free(path);
			dstr_free(skey);
		/* FIXME */
		} else if (ptrie_insert(node, skey, c) == 0)
			dlist_insert_sort(c->subscribers, skey);
	}
	ptrie_rwlock_unlock(subscribers);
	dstr_free(pkey);
}

/* FIXME */
void sall_command(client c) {
	ptrie_rwlock_wrlock(subscribers);
	if (dlist_length(c->subscribers) > 0)
		add_reply_error_format(c, "subscribe/unsubscribe is symmetrical. already subscribed '%s'\r\n",
			dlist_node_value(dlist_head(c->subscribers)));
	else {
		dlist_t dlist = ptrie_node_value(ptrie_get_root(subscribers));

		if (dlist_find(dlist, c) == NULL)
			dlist_insert_tail(dlist, c);
	}
	ptrie_rwlock_unlock(subscribers);
}

void u_command(client c) {
	dstr pkey, skey;
	dlist_t dlist;

	if (dstr_length(c->argv[0]) == 1) {
		add_reply_error(c, "index can't be empty\r\n");
		return;
	}
	pkey = dstr_new(c->argv[0] + 1);
	skey = dstr_new(pkey);
	if (c->argc > 1) {
		int i;

		for (i = 1; i < c->argc; ++i) {
			skey = dstr_cat(skey, ",");
			skey = dstr_cat(skey, c->argv[i]);
		}
	}
	ptrie_rwlock_wrlock(subscribers);
	dlist = ptrie_node_value(ptrie_get_root(subscribers));
	if (dlist_find(dlist, c))
		add_reply_error(c, "subscribe/unsubscribe is symmetrical. unsubscribe 'ALL' first");
	else {
		ptrie_node_t node, node2;

		if ((node = ptrie_get_index(subscribers, pkey)) == NULL)
			node = ptrie_set_index(subscribers, pkey);
		if ((node2 = ptrie_search_prefix(node, skey, c))) {
			dlist_iter_t diter;
			dlist_node_t dnode;
			dstr path = dstr_new("");

			dlist = dlist_new(NULL, NULL);
			while (ptrie_node_parent(node2)) {
				dlist_insert_head(dlist, ptrie_node_key(node2));
				node2 = ptrie_node_parent(node2);
			}
			diter = dlist_iter_new(dlist, DLIST_START_HEAD);
			while ((dnode = dlist_next(diter)))
				path = dstr_cat(path, dlist_node_value(dnode));
			if (strcmp(skey, path))
				add_reply_error_format(c, "subscribe/unsubscribe is symmetrical. "
					"unsubscribe '%s' first", path);
			/* FIXME */
			else if (ptrie_remove(node, skey, c) == 0) {
				dstr s = dlist_remove(c->subscribers, dlist_find(c->subscribers, skey));

				dstr_free(s);
			}
			dlist_iter_free(&diter);
			dlist_free(&dlist);
			dstr_free(path);
		}
	}
	ptrie_rwlock_unlock(subscribers);
	dstr_free(skey);
	dstr_free(pkey);
	add_reply_string(c, "\r\n", 2);
}

/* FIXME */
void uall_command(client c) {
	ptrie_rwlock_wrlock(subscribers);
	if (dlist_length(c->subscribers) > 0)
		add_reply_error_format(c, "subscribe/unsubscribe is symmetrical, unsubscribe '%s' first\r\n",
			dlist_node_value(dlist_head(c->subscribers)));
	else {
		dlist_t dlist = ptrie_node_value(ptrie_get_root(subscribers));

		dlist_remove(dlist, dlist_find(dlist, c));
	}
	ptrie_rwlock_unlock(subscribers);
}

/* FIXME */
static void kfree(const void *key) {
	dstr_free((dstr)key);
}

/* FIXME */
static void *q_thread(void *data) {
	struct crss *crss = (struct crss *)data;
	char buf[64];
	dstr rid, res = NULL;
	db_iterator_t *it;
	const char *key, *value;
	size_t klen, vlen;

	snprintf(buf, sizeof buf, "%p,", crss->c);
	rid = dstr_new(buf);
	rid = dstr_cat(rid, crss->rid);
	it = db_iterator_create(db, db_ro);
	db_iterator_seek(it, crss->start, dstr_length(crss->start));
	/* seek failed */
	if (!db_iterator_valid(it)) {
		res = dstr_new(crss->rid);
		res = dstr_cat(res, ",1\r\n\r\n");
		pthread_spin_lock(&crss->c->lock);
		if (net_try_write(crss->c->fd, res, dstr_length(res), 10, NET_NONBLOCK) == -1)
			xcb_log(XCB_LOG_WARNING, "Writing to client '%p': %s", crss->c, strerror(errno));
		pthread_spin_unlock(&crss->c->lock);
		goto end;
	}
	key = db_iterator_key(it, &klen);
	while (memcmp(key, crss->stop, dstr_length(crss->stop)) <= 0) {
		char *p;

		if (crss->cancel)
			break;
		if (!strcmp(crss->match, "") || ((p = strstr(key, crss->match)) && *(p - 1) == ',')) {
			value = db_iterator_value(it, &vlen);
			res = dstr_new(crss->rid);
			res = dstr_cat(res, ",0,");
			res = dstr_cat_len(res, key, klen);
			res = dstr_cat(res, ",");
			res = dstr_cat_len(res, value, vlen);
			res = dstr_cat(res, "\r\n");
			pthread_spin_lock(&crss->c->lock);
			if (net_try_write(crss->c->fd, res, dstr_length(res), 10, NET_NONBLOCK) == -1) {
				xcb_log(XCB_LOG_WARNING, "Writing to client '%p': %s",
					crss->c, strerror(errno));
				pthread_spin_unlock(&crss->c->lock);
				goto end;
			}
			pthread_spin_unlock(&crss->c->lock);
			dstr_free(res);
		}
		db_iterator_next(it);
		if (!db_iterator_valid(it) || crss->cancel)
			break;
		key = db_iterator_key(it, &klen);
	}
	res = dstr_new(crss->rid);
	res = dstr_cat(res, ",1\r\n\r\n");
	pthread_spin_lock(&crss->c->lock);
	if (net_try_write(crss->c->fd, res, dstr_length(res), 10, NET_NONBLOCK) == -1)
		xcb_log(XCB_LOG_WARNING, "Writing to client '%p': %s", crss->c, strerror(errno));
	pthread_spin_unlock(&crss->c->lock);

end:
	db_iterator_destroy(&it);
	dstr_free(res);
	table_lock(rids);
	table_remove(rids, rid);
	table_unlock(rids);
	dstr_free(rid);
	client_decr(crss->c);
	dstr_free(crss->rid);
	dstr_free(crss->match);
	dstr_free(crss->start);
	dstr_free(crss->stop);
	FREE(crss);
	return NULL;
}

/* FIXME */
void q_command(client c) {
	struct tm tm;
	char *end;
	char buf[64];
	dstr rid;
	struct crss *crss;

	if (!persistence) {
		add_reply_error(c, "database not open\r\n");
		return;
	}
	if (dstr_length(c->argv[0]) == 1) {
		add_reply_error(c, "index can't be empty\r\n");
		return;
	}
	if ((end = strptime(c->argv[3], "%F %T", &tm)) && *end == '\0')
		c->argv[3] = dstr_cat(c->argv[3], ".000");
	else if ((end = strptime(c->argv[3], "%F %R", &tm)) && *end == '\0')
		c->argv[3] = dstr_cat(c->argv[3], ":00.000");
	else {
		add_reply_error(c, "invalid time format, "
			"please use 'YYYY-mm-dd HH:MM:SS' or 'YYYY-mm-dd HH:MM'.\r\n");
		return;
	}
	if ((end = strptime(c->argv[4], "%F %T", &tm)) && *end == '\0')
		c->argv[4] = dstr_cat(c->argv[4], ".999");
	else if ((end = strptime(c->argv[4], "%F %R", &tm)) && *end == '\0')
		c->argv[4] = dstr_cat(c->argv[4], ":59.999");
	else {
		add_reply_error(c, "invalid time format, "
			"please use 'YYYY-mm-dd HH:MM:SS' or 'YYYY-mm-dd HH:MM'.\r\n");
		return;
	}
	snprintf(buf, sizeof buf, "%p,", c);
	rid = dstr_new(buf);
	rid = dstr_cat(rid, c->argv[1]);
	if (rids == NULL)
		rids = table_new(cmpstr, hashmurmur2, kfree, NULL);
	table_lock(rids);
	if ((crss = table_get_value(rids, rid))) {
		add_reply_error_format(c, "query with rid '%s' already exists\r\n", c->argv[1]);
		dstr_free(rid);
	} else if (NEW(crss)) {
		pthread_t thread;

		crss->c      = c;
		crss->rid    = dstr_new(c->argv[1]);
		crss->match  = dstr_new(c->argv[2]);
		crss->start  = dstr_new(c->argv[0] + 1);
		crss->start  = dstr_cat(crss->start, ",");
		crss->start  = dstr_cat(crss->start, c->argv[3]);
		crss->stop   = dstr_new(c->argv[0] + 1);
		crss->stop   = dstr_cat(crss->stop, ",");
		crss->stop   = dstr_cat(crss->stop, c->argv[4]);
		crss->cancel = 0;
		if (pthread_create(&thread, NULL, q_thread, crss) != 0) {
			add_reply_error(c, strerror(errno));
			add_reply_string(c, "\r\n", 2);
			dstr_free(crss->rid);
			dstr_free(crss->match);
			dstr_free(crss->start);
			dstr_free(crss->stop);
			FREE(crss);
			dstr_free(rid);
		} else {
			client_incr(crss->c);
			table_insert(rids, rid, crss);
		}
	} else {
		add_reply_error(c, "error allocating memory for crss\r\n");
		dstr_free(rid);
	}
	table_unlock(rids);
}

/* FIXME */
void qc_command(client c) {
	char buf[64];
	dstr rid;
	struct crss *crss;

	snprintf(buf, sizeof buf, "%p,", c);
	rid = dstr_new(buf);
	rid = dstr_cat(rid, c->argv[1]);
	if (rids == NULL)
		rids = table_new(cmpstr, hashmurmur2, kfree, NULL);
	table_lock(rids);
	if ((crss = table_get_value(rids, rid))) {
		crss->cancel = 1;
		xcb_log(XCB_LOG_DEBUG, "Query with rid '%s' got cancelled", c->argv[1]);
	}
	table_unlock(rids);
	add_reply_string(c, "\r\n", 2);
	dstr_free(rid);
}

/* FIXME */
void indices_command(client c) {
	dstr res = get_indices();

	add_reply_string_format(c, "%s,%s\r\n\r\n", c->argv[1], res);
	dstr_free(res);
}

/* FIXME */
void index_command(client c) {
	const char *fmt;

	table_lock(idxfmts);
	if ((fmt = table_get_value(idxfmts, c->argv[1])))
		add_reply_string_format(c, "%s\r\n", fmt);
	else
		add_reply_error(c, "format unavailable");
	table_unlock(idxfmts);
	add_reply_string(c, "\r\n", 2);
}

/* FIXME */
static int cmppass(const char *a, const char *b) {
	char bufa[512], bufb[512];
	int alen = strlen(a), blen = strlen(b), i, diff = 0;

	if (alen > sizeof bufa || blen > sizeof bufb)
		return 1;
	memset(bufa, '\0', sizeof bufa);
	memset(bufb, '\0', sizeof bufb);
	memcpy(bufa, a, alen);
	memcpy(bufb, b, blen);
	for (i = 0; i < sizeof bufa; ++i)
		diff |= bufa[i] ^ bufb[i];
	diff |= alen ^ blen;
	return diff;
}

/* FIXME */
void auth_command(client c) {
	if (password == NULL)
		add_reply_error(c, "no password is set");
	else if (cmppass(password, c->argv[1]) == 0) {
		c->authenticated = 1;
		add_reply_string(c, "OK\r\n", 4);
	} else {
		c->authenticated = 0;
		add_reply_error(c, "invalid password");
	}
	add_reply_string(c, "\r\n", 2);
}


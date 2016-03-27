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
#ifdef __linux__
#define CONFIG_HAVE_BACKTRACE 1
#define CONFIG_HAVE_EPOLL     1
#include <execinfo.h>
#endif
#include <fcntl.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
/* FIXME */
#include "macros.h"
#undef MIN
#undef MAX
#include "mem.h"
#include "dlist.h"
#include "table.h"
#include "ptrie.h"
#include "btree.h"
#include "dstr.h"
#include "utils.h"
#include "logger.h"
#include "config.h"
#include "net.h"
#include "event.h"
#include "thrpool.h"
#include "pgmsock.h"
#include "db.h"
#include "basics.h"
#include "commons.h"

/* FIXME */
int last_quote = 0;
table_t cache;
ptrie_t subscribers;
dlist_t clients_to_close;
table_t idxfmts;
event_loop el;
int persistence = 1;
db_t *db;
db_readoptions_t *db_ro;
const char *password;
const char *dpip;
int dpport;

/* FIXME */
void client_free(client c);
static void tcp_accept_handler(event_loop el, int fd, int mask, void *data);
static void help_command(client c);
static void config_command(client c);
static void show_command(client c);
static void monitor_command(client c);
static void database_command(client c);
static void shutdown_command(client c);
extern void s_command(client c);
extern void sall_command(client c);
extern void u_command(client c);
extern void uall_command(client c);
extern void q_command(client c);
extern void qc_command(client c);
extern void indices_command(client c);
extern void index_command(client c);
extern void auth_command(client c);

/* FIXME */
static table_t cmds;
static struct cmd commands[] = {
	{"help",	help_command,		"Display this text",				1},
	{"?",		help_command,		"Synonym for 'help'",				1},
	{"config",	config_command,		"Get or set configurations",			-3},
	{"show",	show_command,		"Show clients",					2},
	{"monitor",	monitor_command,	"Monitor on or off",				2},
	{"database",	database_command,	"Database operations",				-2},
	{"shutdown",	shutdown_command,	"Shut down xcb-pb2",				1},
	{"quit",	NULL,			"Quit connecting to xcb-pb2",			1}
};
static table_t ctm_cmds;
static struct cmd ctm_commands[] = {
	{"S",		s_command,		"Subscribe",					-1},
	{"SALL",	sall_command,		"Subscribe all",				1},
	{"U",		u_command,		"Unsubscribe",					-1},
	{"UALL",	uall_command,		"Unsubscribe all",				1},
	{"Q",		q_command,		"Query",					5},
	{"QC",		qc_command,		"Query cancellation",				2},
	{"INDICES",	indices_command,	"Indices",					2},
	{"INDEX",	index_command,		"Index",					2},
	{"AUTH",	auth_command,		"Authenticate",					2},
	{"QUIT",	NULL,			"Quit connecting to xcb-pb2",			1}
};
static struct config *cfg;
static pthread_mutex_t cfg_lock = PTHREAD_ADAPTIVE_MUTEX_INITIALIZER_NP;
static const char *cfg_path;
static dlist_t clients;
static dlist_t monitors;
static thrpool_t tp;
static table_t indices;
static int indices_changed = 0;
static db_options_t *db_o;
static db_writeoptions_t *db_wo;
static db_writebatch_t *db_wb;
static pthread_mutex_t wb_lock = PTHREAD_ADAPTIVE_MUTEX_INITIALIZER_NP;
static int cp_time = 6;
static int rotate = 7;
static int dirty = 0;
static dlist_t pgm_recv_cfgs;
static dlist_t pgm_receivers;
static char neterr[256];
static int tcpsock = -1;
static int ctmsock = -1;
static int log_reload = 0;
static int shut_down = 0;
static int cronloops = 0;

static void sig_hup_handler(int sig) {
	NOT_USED(sig);

	xcb_log(XCB_LOG_WARNING, "SIGHUP received, scheduling reload...");
	log_reload = 1;
}

#ifdef CONFIG_HAVE_BACKTRACE
static void sig_segv_handler(int sig, siginfo_t *info, void *secret) {
	ucontext_t *uc = (ucontext_t *)secret;
	int fd, trace_size = 0;
	void *trace[100];
	struct sigaction sa;
	NOT_USED(info);

	xcb_log(XCB_LOG_WARNING, "\n\n=== XCUBE BUG REPORT START: Cut & paste starting from here ===\n"
		"    xcb-pb2 crashed by signal: %d\n--- STACK TRACE", sig);
	close_logger();
	if ((fd = open("/var/log/xcb/xcb-pb2.log", O_APPEND | O_CREAT | O_WRONLY, 0644)) == -1)
		return;
	trace_size = backtrace(trace, 100);
#ifdef __x86_64__
	trace[1] = (void *)uc->uc_mcontext.gregs[16];
#endif
	backtrace_symbols_fd(trace, trace_size, fd);
	write(fd, "\n=== XCUBE BUG REPORT END. Make sure to include from START to END. ===\n\n", 72);
	close(fd);
	sigemptyset(&sa.sa_mask);
	sa.sa_flags   = SA_NODEFER | SA_ONSTACK | SA_RESETHAND;
	sa.sa_handler = SIG_DFL;
	sigaction(sig, &sa, NULL);
	kill(getpid(), sig);
}
#endif

static void sig_term_handler(int sig) {
	NOT_USED(sig);

	xcb_log(XCB_LOG_WARNING, "SIGTERM received, scheduling shutdown...");
	shut_down = 1;
}

static void setup_signal_handlers(void) {
	struct sigaction sa;

	sigemptyset(&sa.sa_mask);
	sa.sa_flags   = 0;
	sa.sa_handler = sig_hup_handler;
	sigaction(SIGHUP, &sa, NULL);
#ifdef CONFIG_HAVE_BACKTRACE
	sigemptyset(&sa.sa_mask);
	sa.sa_flags     = SA_NODEFER | SA_RESETHAND | SA_SIGINFO;
	sa.sa_sigaction = sig_segv_handler;
	sigaction(SIGSEGV, &sa, NULL);
	sigaction(SIGBUS, &sa, NULL);
	sigaction(SIGFPE, &sa, NULL);
	sigaction(SIGILL, &sa, NULL);
#endif
	sigemptyset(&sa.sa_mask);
	sa.sa_flags   = 0;
	sa.sa_handler = sig_term_handler;
	sigaction(SIGTERM, &sa, NULL);
}

static void usage(void) {
	fprintf(stderr, "Usage: ./xcb-pb2 [-f] path/to/xcb-pb2.conf\n");
	fprintf(stderr, "       ./xcb-pb2 -h or --help\n");
	exit(1);
}

/* FIXME */
static void kfree(const void *key) {
	dstr_free((dstr)key);
}

/* FIXME */
static void vfree(void *value) {
	dstr_free((dstr)value);
}

/* FIXME */
static void bfree(void *value) {
	btree_t btree = (btree_t)value;

	btree_free(&btree);
}

/* FIXME */
static void msgfree(void *value) {
	msg_decr((struct msg *)value);
}

/* FIXME */
static int prepare_for_shutdown(void) {
	xcb_log(XCB_LOG_WARNING, "User requested shutdown...");
	if (persistence) {
		pthread_mutex_lock(&wb_lock);
		db_close(&db);
		pthread_mutex_unlock(&wb_lock);
	}
	if (tcpsock != -1)
		close(tcpsock);
	if (ctmsock != -1)
		close(ctmsock);
	xcb_log(XCB_LOG_WARNING, "xcb-pb2 is now ready to exit, bye bye...");
	close_logger();
	return 0;
}

dstr get_indices(void) {
	dlist_t dlist = dlist_new(cmpstr, vfree);
	dstr res = dstr_new("INDICES");

	/* split and sort */
	table_lock(indices);
	if (table_length(indices) > 0) {
		table_iter_t iter = table_iter_new(indices);
		table_node_t node;

		while ((node = table_next(iter))) {
			dstr apps = (dstr)table_node_value(node);
			dstr *fields = NULL;
			int nfield = 0, i;

			fields = dstr_split_len(apps, dstr_length(apps), ",", 1, &nfield);
			for (i = 0; i < nfield; ++i)
				if (dlist_find(dlist, fields[i]) == NULL)
					dlist_insert_sort(dlist, dstr_new(fields[i]));
			dstr_free_tokens(fields, nfield);
		}
		table_iter_free(&iter);
	}
	table_unlock(indices);
	/* assemble */
	if (dlist_length(dlist) > 0) {
		dlist_iter_t iter = dlist_iter_new(dlist, DLIST_START_HEAD);
		dlist_node_t node;

		while ((node = dlist_next(iter))) {
			dstr index = (dstr)dlist_node_value(node);

			res = dstr_cat(res, ",");
			res = dstr_cat(res, index);
		}
		dlist_iter_free(&iter);
	}
	dlist_free(&dlist);
	res = dstr_cat(res, "\r\n");
	return res;
}

static int server_cron(event_loop el, unsigned long id, void *data) {
	dlist_iter_t iter;
	dlist_node_t node;
	NOT_USED(el);
	NOT_USED(id);
	NOT_USED(data);

	if (log_reload) {
		close_logger();
		if (init_logger("/var/log/xcb/xcb-pb2.log", __LOG_DEBUG) == 0) {
			const char *tmp;

			pthread_mutex_lock(&cfg_lock);
			if ((tmp = variable_retrieve(cfg, "general", "log_level"))) {
				if (!strcasecmp(tmp, "debug"))
					set_logger_level(__LOG_DEBUG);
				else if (!strcasecmp(tmp, "info"))
					set_logger_level(__LOG_INFO);
				else if (!strcasecmp(tmp, "notice"))
					set_logger_level(__LOG_NOTICE);
				else if (!strcasecmp(tmp, "warning"))
					set_logger_level(__LOG_WARNING);
			}
			pthread_mutex_unlock(&cfg_lock);
			log_reload = 0;
		}
		/* FIXME */
		if (last_quote) {
			table_lock(cache);
			table_clear(cache);
			table_unlock(cache);
		}
	}
	if (shut_down) {
		if (prepare_for_shutdown() == 0)
			exit(0);
		xcb_log(XCB_LOG_WARNING, "SIGTERM received, but errors trying to shutdown the server");
	}
	/* close clients asynchronously */
	if (dlist_length(clients_to_close) > 0) {
		iter = dlist_iter_new(clients_to_close, DLIST_START_HEAD);
		while ((node = dlist_next(iter))) {
			client c = (client)dlist_node_value(node);

			if (c->refcount == 0)
				client_free(c);
		}
		dlist_iter_free(&iter);
	}
	/* triggered approximately every one second */
	if (cronloops % 10 == 0)
		if (persistence && dirty) {
			char *dberr = NULL;

			pthread_mutex_lock(&wb_lock);
			db_write(db, db_wo, db_wb, &dberr);
			if (dberr) {
				xcb_log(XCB_LOG_WARNING, "Writing data batch: %s", dberr);
				db_free(dberr);
			}
			db_writebatch_clear(db_wb);
			pthread_mutex_unlock(&wb_lock);
			dirty = 0;
		}
	/* triggered approximately every 20 seconds */
	if (cronloops % 200 == 0) {
		/* heartbeat */
		dlist_lock(clients);
		if (dlist_length(clients) > 0) {
			dstr res = dstr_new("HEARTBEAT|");
			dstr ip = getipv4();

			res = dstr_cat(res, ip);
			res = dstr_cat(res, "\r\n");
			iter = dlist_iter_new(clients, DLIST_START_HEAD);
			while ((node = dlist_next(iter))) {
				client c = (client)dlist_node_value(node);

				if (c->sock != ctmsock) {
					pthread_spin_lock(&c->lock);
					if (net_try_write(c->fd, res, dstr_length(res),
						10, NET_NONBLOCK) == -1)
						xcb_log(XCB_LOG_WARNING, "Writing to client '%s:%d': %s",
							c->ip, c->port, strerror(errno));
					pthread_spin_unlock(&c->lock);
				}
			}
			dlist_iter_free(&iter);
			dstr_free(ip);
			dstr_free(res);
		}
		dlist_unlock(clients);
	}
	/* FIXME */
	if (indices_changed) {
		dlist_lock(clients);
		if (dlist_length(clients) > 0) {
			dstr res = get_indices();

			iter = dlist_iter_new(clients, DLIST_START_HEAD);
			while ((node = dlist_next(iter))) {
				client c = (client)dlist_node_value(node);

				pthread_spin_lock(&c->lock);
				if (net_try_write(c->fd, res, dstr_length(res), 10, NET_NONBLOCK) == -1)
					xcb_log(XCB_LOG_WARNING, "Writing to client '%s:%d': %s",
						c->ip, c->port, strerror(errno));
				pthread_spin_unlock(&c->lock);
			}
			dstr_free(res);
			dlist_iter_free(&iter);
		}
		dlist_unlock(clients);
		indices_changed = 0;
	}
	++cronloops;
	return 100;
}

/* FIXME */
static void *rc_thread(void *data) {
	char *datestr = (char *)data;
	db_iterator_t *it;

	it = db_iterator_create(db, db_ro);
	db_iterator_seek_to_first(it);
	while (db_iterator_valid(it)) {
		const char *key;
		size_t klen;
		dstr *fields = NULL;
		int nfield = 0;

		key = db_iterator_key(it, &klen);
		fields = dstr_split_len(key, klen, ",", 1, &nfield);
		if (nfield > 1 && memcmp(datestr, fields[1], strlen(datestr)) > 0)
			db_delete(db, db_wo, key, klen, NULL);
		dstr_free_tokens(fields, nfield);
		db_iterator_next(it);
	}
	db_iterator_destroy(&it);
	db_compact_range(db, NULL, 0, NULL, 0);
	FREE(datestr);
	return NULL;
}

/* FIXME */
static int rotate_n_compact(event_loop el, unsigned long id, void *data) {
	struct timeval tv;
	time_t t;
	char *datestr;
	struct tm lt;
	NOT_USED(el);
	NOT_USED(id);
	NOT_USED(data);

	gettimeofday(&tv, NULL);
	t = tv.tv_sec - rotate * 24 * 60 * 60;
	if ((datestr = ALLOC(64))) {
		pthread_t thread;

		strftime(datestr, 64, "%F %T", localtime_r(&t, &lt));
		if (pthread_create(&thread, NULL, rc_thread, datestr) != 0){
			xcb_log(XCB_LOG_ERROR, "Error initializing rc thread");
			FREE(datestr);
		}
	}
	localtime_r(&tv.tv_sec, &lt);
	if (lt.tm_hour >= cp_time)
		return (24 - lt.tm_hour + cp_time) * 60 * 60 * 1000 - tv.tv_usec / 1000;
	else
		return (cp_time - lt.tm_hour) * 60 * 60 * 1000 - tv.tv_usec / 1000;
}

/* FIXME */
static void init_pgm_recv_cfg(char *cat, struct pgm_cfg *pgm_recv_cfg) {
	struct variable *var = variable_browse(cfg, cat);

	while (var) {
		if (!strcasecmp(var->name, "network")) {
			if (strcmp(var->value, ""))
				pgm_recv_cfg->network = var->value;
		} else if (!strcasecmp(var->name, "port")) {
			if (strcmp(var->value, ""))
				pgm_recv_cfg->port = atoi(var->value);
		} else
			xcb_log(XCB_LOG_WARNING, "Unknown variable '%s' in category '%s'"
				" of xcb-pb2.conf", var->name, cat);
		var = var->next;
	}
}

static int send_quote(void *data, void *data2) {
	struct msg *msg = (struct msg *)data;
	dstr skey = (dstr)data2;
	dstr *fields = NULL, *fields2 = NULL;
	int nfield = 0, nfield2 = 0;
	dstr key, value, index, res;
	ptrie_node_t node;
	dlist_t dlist;
	dlist_iter_t diter;
	dlist_node_t dnode;

	/* FIXME */
	fields = dstr_split_len(msg->data, strlen(msg->data), "|", 1, &nfield);
	key   = dstr_new(fields[0]);
	value = dstr_new(fields[1]);
	fields2 = dstr_split_len(key, dstr_length(key), ",", 1, &nfield2);
	index = dstr_new(fields2[0]);
	res = dstr_new(key);
	res = dstr_cat(res, ",");
	res = dstr_cat(res, value);
	res = dstr_cat(res, "\r\n");
	/* in case of multiple subscribers */
	ptrie_rwlock_rdlock(subscribers);
	if ((node = ptrie_get_index(subscribers, index)) == NULL) {
		dlist = ptrie_node_value(ptrie_get_root(subscribers));
		if (dlist_length(dlist) > 0) {
			diter = dlist_iter_new(dlist, DLIST_START_HEAD);
			while ((dnode = dlist_next(diter))) {
				client c = (client)dlist_node_value(dnode);

				pthread_spin_lock(&c->lock);
				if (!(c->flags & CLIENT_CLOSE_ASAP)) {
					if (net_try_write(c->fd, res, dstr_length(res),
						10, NET_NONBLOCK) == -1) {
						xcb_log(XCB_LOG_WARNING, "Writing to client '%s:%d': %s",
							c->ip, c->port, strerror(errno));
						if (++c->eagcount >= 3)
							client_free_async(c);
					} else if (c->eagcount)
						c->eagcount = 0;
				}
				pthread_spin_unlock(&c->lock);
			}
			dlist_iter_free(&diter);
		}
	} else {
		node = ptrie_find(node, skey);
		while (node) {
			dlist = ptrie_node_value(node);
			if (dlist_length(dlist) > 0) {
				diter = dlist_iter_new(dlist, DLIST_START_HEAD);
				while ((dnode = dlist_next(diter))) {
					client c = (client)dlist_node_value(dnode);

					pthread_spin_lock(&c->lock);
					if (!(c->flags & CLIENT_CLOSE_ASAP)) {
						if (net_try_write(c->fd, res, dstr_length(res),
							10, NET_NONBLOCK) == -1) {
							xcb_log(XCB_LOG_WARNING,
								"Writing to client '%s:%d': %s",
								c->ip, c->port, strerror(errno));
							if (++c->eagcount >= 3)
								client_free_async(c);
						} else if (c->eagcount)
							c->eagcount = 0;
					}
					pthread_spin_unlock(&c->lock);
				}
				dlist_iter_free(&diter);
			}
			node = ptrie_node_parent(node);
		}
	}
	ptrie_rwlock_unlock(subscribers);
	dstr_free(res);
	dstr_free(index);
	dstr_free_tokens(fields2, nfield2);
	dstr_free(value);
	dstr_free(key);
	dstr_free_tokens(fields, nfield);
	dstr_free(skey);
	msg_decr(msg);
	return 0;
}

static int on_msgv(struct pgm_msgv_t *msgv, size_t len) {
	int i = 0;

	do {
		int j, aqdu_len = 0;
		struct pgm_sk_buff_t* pskb = msgv[i].msgv_skb[0];
		struct msg *msg;
		dstr *fields = NULL;
		int nfield = 0;

		for (j = 0; j < msgv[i].msgv_len; ++j)
			aqdu_len += msgv[i].msgv_skb[j]->len;
		++i;
		len -= aqdu_len;
		/* in case of multiple monitors */
		dlist_rwlock_rdlock(monitors);
		if (dlist_length(monitors) > 0) {
			dstr res = dstr_new("RX '");
			dlist_iter_t iter = dlist_iter_new(monitors, DLIST_START_HEAD);
			dlist_node_t node;

			res = dstr_cat_len(res, pskb->data, pskb->len);
			res = dstr_cat(res, "'\r\n");
			while ((node = dlist_next(iter))) {
				client c = (client)dlist_node_value(node);

				pthread_spin_lock(&c->lock);
				if (!(c->flags & CLIENT_CLOSE_ASAP)) {
					if (net_try_write(c->fd, res, dstr_length(res),
						10, NET_NONBLOCK) == -1) {
						xcb_log(XCB_LOG_WARNING, "Writing to client '%s:%d': %s",
							c->ip, c->port, strerror(errno));
						if (++c->eagcount >= 3)
							client_free_async(c);
					} else if (c->eagcount)
						c->eagcount = 0;
				}
				pthread_spin_unlock(&c->lock);
			}
			dlist_iter_free(&iter);
			dstr_free(res);
		}
		dlist_rwlock_unlock(monitors);
		if (NEW0(msg) == NULL)
			continue;
		if ((msg->data = mem_strndup(pskb->data, pskb->len)) == NULL) {
			FREEMSG(msg);
			continue;
		}
		msg->refcount = 1;
		xcb_log(XCB_LOG_DEBUG, "Data '%s' received", msg->data);
		fields = dstr_split_len(pskb->data, pskb->len, "|", 1, &nfield);
		/* indices */
		if (pskb->len >= 7 && !memcmp(pskb->data, "INDICES", 7)) {
			if (nfield == 3) {
				dstr network, apps, prev;

				network = dstr_new(fields[1]);
				apps    = dstr_new(fields[2]);
				table_lock(indices);
				if ((prev = table_get_value(indices, network)) == NULL) {
					table_insert(indices, network, apps);
					indices_changed = 1;
				} else if (dstr_length(prev) != dstr_length(apps) ||
					memcmp(prev, apps, dstr_length(prev))) {
					table_insert(indices, network, apps);
					indices_changed = 1;
					dstr_free(prev);
					dstr_free(network);
				} else {
					dstr_free(apps);
					dstr_free(network);
				}
				table_unlock(indices);
			}
		/* index */
		} else if (pskb->len >= 5 && !memcmp(pskb->data, "INDEX", 5)) {
			if (nfield == 3) {
				dstr index, format, prev;

				index  = dstr_new(fields[1]);
				format = dstr_new(fields[2]);
				table_lock(idxfmts);
				if ((prev = table_get_value(idxfmts, index)) == NULL)
					table_insert(idxfmts, index, format);
				else if (dstr_length(prev) != dstr_length(format) ||
					memcmp(prev, format, dstr_length(prev))) {
					table_insert(idxfmts, index, format);
					dstr_free(prev);
					dstr_free(index);
				} else {
					dstr_free(format);
					dstr_free(index);
				}
				table_unlock(idxfmts);
			}
		} else {
			dstr key = NULL, value = NULL, index = NULL, contracts = NULL;
			dstr *fields2 = NULL;
			int nfield2 = 0, i;
			dstr skey;

			if (nfield != 2)
				goto end;
			key   = dstr_new(fields[0]);
			value = dstr_new(fields[1]);
			fields2 = dstr_split_len(key, dstr_length(key), ",", 1, &nfield2);
			if (nfield2 < 2)
				goto end;
			index = dstr_new(fields2[0]);
			if (nfield2 > 2) {
				contracts = dstr_new(fields2[2]);
				for (i = 3; i < nfield2; ++i) {
					contracts = dstr_cat(contracts, ",");
					contracts = dstr_cat(contracts, fields2[i]);
				}
			}
			skey = dstr_new(index);
			if (contracts) {
				skey = dstr_cat(skey, ",");
				skey = dstr_cat(skey, contracts);
			}
			if (last_quote) {
				dstr ckey, cvalue;
				btree_t btree;

				ckey   = dstr_new(skey);
				cvalue = dstr_new(fields2[1]);
				cvalue = dstr_cat(cvalue, ",");
				cvalue = dstr_cat(cvalue, value);
				/* put latest quotes into cache */
				table_lock(cache);
				if ((btree = table_get_value(cache, index)) == NULL) {
					btree = btree_new(128, cmpstr, kfree, vfree);
					btree_insert(btree, ckey, cvalue);
					table_insert(cache, dstr_new(index), btree);
				} else {
					btree_node_t node;

					if ((node = btree_find(btree, ckey, &i)) == NULL)
						btree_insert(btree, ckey, cvalue);
					else {
						dstr key   = (dstr)btree_node_key(node, i);
						dstr value = (dstr)btree_node_value(node, i);

						if (strcmp(key, ckey))
							btree_insert(btree, ckey, cvalue);
						else if (dstr_length(value) == dstr_length(cvalue) &&
							!memcmp(value, cvalue, dstr_length(value))) {
							table_unlock(cache);
							dstr_free(cvalue);
							dstr_free(ckey);
							dstr_free(skey);
							goto end;
						} else {
							btree_insert(btree, ckey, cvalue);
							dstr_free(value);
							dstr_free(ckey);
						}
					}
				}
				table_unlock(cache);
			}
			msg_incr(msg);
			/* send quotes as soon as possible */
			thrpool_queue(tp, send_quote, msg, skey, msgfree, vfree);
			/* FIXME */
			if (persistence) {
				pthread_mutex_lock(&wb_lock);
				db_writebatch_put(db_wb, key, value);
				pthread_mutex_unlock(&wb_lock);
				dirty = 1;
			}

end:
			/* in case of multiple monitors */
			dlist_rwlock_rdlock(monitors);
			if (dlist_length(monitors) > 0) {
				dstr res = dstr_new("TX '");
				dlist_iter_t iter = dlist_iter_new(monitors, DLIST_START_HEAD);
				dlist_node_t node;

				if (key && value) {
					res = dstr_cat(res, key);
					res = dstr_cat(res, ",");
					res = dstr_cat(res, value);
				} else
					res = dstr_cat(res, msg->data);
				res = dstr_cat(res, "'\r\n");
				while ((node = dlist_next(iter))) {
					client c = (client)dlist_node_value(node);

					pthread_spin_lock(&c->lock);
					if (!(c->flags & CLIENT_CLOSE_ASAP)) {
						if (net_try_write(c->fd, res, dstr_length(res),
							10, NET_NONBLOCK) == -1) {
							xcb_log(XCB_LOG_WARNING,
								"Writing to client '%s:%d': %s",
								c->ip, c->port, strerror(errno));
							if (++c->eagcount >= 3)
								client_free_async(c);
						} else if (c->eagcount)
							c->eagcount = 0;
					}
					pthread_spin_unlock(&c->lock);
				}
				dlist_iter_free(&iter);
				dstr_free(res);
			}
			dlist_rwlock_unlock(monitors);
			dstr_free(contracts);
			dstr_free(index);
			dstr_free_tokens(fields2, nfield2);
			dstr_free(value);
			dstr_free(key);
		}
		dstr_free_tokens(fields, nfield);
		msg_decr(msg);
		/* FIXME */
		if (shut_down)
			break;
	} while (len > 0);
	return 0;
}

/* receiver thread */
static void *receiver_thread(void *data) {
	pgm_sock_t *pgm_receiver = (pgm_sock_t *)data;
	/* FIXME */
	int iov_len = 20;
	struct pgm_msgv_t msgv[iov_len];
#ifdef CONFIG_HAVE_EPOLL
	int efd, ev_len = 1;
	/* wait for maximum 1 event */
	struct epoll_event events[ev_len];

	/* FIXME */
	if ((efd = epoll_create(20)) == -1) {
		xcb_log(XCB_LOG_WARNING, "Opening epoll fd: %s", strerror(errno));
		return NULL;
	}
	if (pgm_epoll_ctl(pgm_receiver, efd, EPOLL_CTL_ADD, EPOLLIN) == -1) {
		xcb_log(XCB_LOG_WARNING, "Registering socket on epoll fd '%d': %s", efd, strerror(errno));
		return NULL;
	}
#else
	fd_set readfds;
	int nfds = 0;

#endif
	for (;;) {
		size_t len;
		pgm_error_t *pgm_err = NULL;
		int status;
		struct timeval tv;
#ifdef CONFIG_HAVE_EPOLL
		int timeout;
#endif

		status = pgm_recvmsgv(pgm_receiver, msgv, NELEMS(msgv), 0, &len, &pgm_err);
		switch (status) {
		case PGM_IO_STATUS_NORMAL:
			on_msgv(msgv, len);
			break;
		case PGM_IO_STATUS_TIMER_PENDING:
			{
				socklen_t optlen = sizeof tv;

				pgm_getsockopt(pgm_receiver, IPPROTO_PGM, PGM_TIME_REMAIN, &tv, &optlen);
			}
			goto block;
		case PGM_IO_STATUS_RATE_LIMITED:
			{
				socklen_t optlen = sizeof tv;

				pgm_getsockopt(pgm_receiver, IPPROTO_PGM, PGM_RATE_REMAIN, &tv, &optlen);
			}
			/* fall through */
		case PGM_IO_STATUS_WOULD_BLOCK:

block:
#ifdef CONFIG_HAVE_EPOLL
			timeout = status == PGM_IO_STATUS_WOULD_BLOCK
				? -1 : (tv.tv_sec * 1000 + tv.tv_usec / 1000);
			epoll_wait(efd, events, NELEMS(events), timeout);
#else
			FD_ZERO(&readfds);
			pgm_select_info(pgm_receiver, &readfds, NULL, &nfds);
			select(nfds, &readfds, NULL, NULL, status == PGM_IO_STATUS_RATE_LIMITED ? &tv : NULL);
#endif
			break;
		case PGM_IO_STATUS_RESET:
			{
				struct pgm_sk_buff_t* pskb = msgv[0].msgv_skb[0];

				pgm_free_skb(pskb);
			}
		default:
			if (pgm_err) {
				xcb_log(XCB_LOG_ERROR, "Receiving data: %s", pgm_err->message);
				pgm_error_free(pgm_err);
				pgm_err = NULL;
			}
			if (status == PGM_IO_STATUS_ERROR)
				break;
		}
		if (shut_down)
			break;
	}
#ifdef CONFIG_HAVE_EPOLL
	close(efd);
#endif
	return NULL;
}

int main(int argc, char **argv) {
	int ncmds, i;
	const char *tmp;
	pgm_error_t *pgm_err = NULL;
	char *cat;

	/* FIXME */
	signal(SIGPIPE, SIG_IGN);
	setup_signal_handlers();
	cmds = table_new(cmpstr, hashmurmur2, NULL, NULL);
	ncmds = sizeof commands / sizeof (struct cmd);
	for (i = 0; i < ncmds; ++i) {
		struct cmd *cmd = commands + i;

		table_insert(cmds, cmd->name, cmd);
	}
	ctm_cmds = table_new(cmpstr, hashmurmur2, NULL, NULL);
	ncmds = sizeof ctm_commands / sizeof (struct cmd);
	for (i = 0; i < ncmds; ++i) {
		struct cmd *cmd = ctm_commands + i;

		table_insert(ctm_cmds, cmd->name, cmd);
	}
	if (argc != 2 && argc != 3)
		usage();
	else if (!strcmp(argv[1], "-h") || !strcmp(argv[1], "--help"))
		usage();
	else if (argc == 3 && strcmp(argv[1], "-f"))
		usage();
	if (argc == 2 && daemon(1, 0) == -1)
		fprintf(stderr, "Error daemonizing: %s\n", strerror(errno));
	/* FIXME */
	if (init_logger("/var/log/xcb/xcb-pb2.log", __LOG_DEBUG) == -1) {
		fprintf(stderr, "Error initializing logger\n");
		exit(1);
	}
	cfg_path = argc == 2 ? argv[1] : argv[2];
	if ((cfg = config_load(cfg_path)) == NULL)
		exit(1);
	if ((tmp = variable_retrieve(cfg, "general", "log_level"))) {
		if (!strcasecmp(tmp, "info"))
			set_logger_level(__LOG_INFO);
		else if (!strcasecmp(tmp, "notice"))
			set_logger_level(__LOG_NOTICE);
		else if (!strcasecmp(tmp, "warning"))
			set_logger_level(__LOG_WARNING);
	}
	/* FIXME */
	if ((tmp = variable_retrieve(cfg, "general", "last_quote")))
		if (atoi(tmp) == 1)
			last_quote = 1;
	cache = table_new(cmpstr, hashmurmur2, kfree, bfree);
	subscribers = ptrie_new();
	clients_to_close = dlist_new(NULL, NULL);
	clients = dlist_new(NULL, NULL);
	monitors = dlist_new(NULL, NULL);
	tp = thrpool_new(16, 256, 200, NULL);
	indices = table_new(cmpstr, hashmurmur2, NULL, NULL);
	idxfmts = table_new(cmpstr, hashmurmur2, NULL, NULL);
	/* FIXME */
	if ((el = create_event_loop(1024 + 1000)) == NULL) {
		xcb_log(XCB_LOG_ERROR, "Error creating event loop");
		goto err;
	}
	create_time_event(el, 1, server_cron, NULL, NULL);
	/* FIXME */
	if ((tmp = variable_retrieve(cfg, "database", "persistence")))
		if (atoi(tmp) == 1)
			persistence = 1;
	if (persistence && (tmp = variable_retrieve(cfg, "database", "db_path")) && strcmp(tmp, "")) {
		char *dberr = NULL;
		struct timeval tv;
		struct tm lt;

		db_o = db_options_create();
		/* FIXME */
		makedir(tmp, 0777);
		db = db_open(db_o, tmp, &dberr);
		if (dberr) {
			xcb_log(XCB_LOG_ERROR, "Opening database: %s", dberr);
			db_free(dberr);
			goto err;
		}
		db_wo = db_writeoptions_create();
		/* db_writeoptions_set_sync(db_wo, 1); */
		db_wb = db_writebatch_create();
		db_ro = db_readoptions_create();
		if ((tmp = variable_retrieve(cfg, "database", "cp_time")) && strcmp(tmp, ""))
			if (atoi(tmp) >= 0 && atoi(tmp) <= 23)
				cp_time = atoi(tmp);
		gettimeofday(&tv, NULL);
		localtime_r(&tv.tv_sec, &lt);
		if (lt.tm_hour >= cp_time)
			create_time_event(el, (24 - lt.tm_hour + cp_time) * 60 * 60 * 1000 - tv.tv_usec / 1000,
				rotate_n_compact, NULL, NULL);
		else
			create_time_event(el, (cp_time - lt.tm_hour) * 60 * 60 * 1000 - tv.tv_usec / 1000,
				rotate_n_compact, NULL, NULL);
	}
	if (!pgm_init(&pgm_err)) {
		xcb_log(XCB_LOG_ERROR, "Error starting PGM engine: %s", pgm_err->message);
		pgm_error_free(pgm_err);
		goto err;
	}
	/* FIXME */
	pgm_recv_cfgs = dlist_new(NULL, NULL);
	pgm_receivers = dlist_new(NULL, NULL);
	cat = category_browse(cfg, NULL);
	while (cat) {
		if (!strcasecmp(cat, "pgm_receiver")) {
			struct pgm_cfg *pgm_recv_cfg;
			pgm_sock_t *pgm_receiver = NULL;
			pthread_t thread;

			if (NEW(pgm_recv_cfg) == NULL) {
				xcb_log(XCB_LOG_ERROR, "Error allocating memory for PGM cfg");
				goto err;
			}
			pgm_recv_cfg->network = NULL;
			pgm_recv_cfg->port    = 0;
			init_pgm_recv_cfg(cat, pgm_recv_cfg);
			if (pgm_recv_cfg->network == NULL) {
				xcb_log(XCB_LOG_ERROR, "PGM network can't be NULL");
				goto err;
			}
			if (pgm_recv_cfg->port == 0) {
				xcb_log(XCB_LOG_ERROR, "PGM port can't be zero");
				goto err;
			}
			dlist_insert_tail(pgm_recv_cfgs, pgm_recv_cfg);
			if ((pgm_receiver = pgmsock_create(pgm_recv_cfg->network, pgm_recv_cfg->port,
				PGMSOCK_RECEIVER)) == NULL)
				goto err;
			dlist_insert_tail(pgm_receivers, pgm_receiver);
			if (pthread_create(&thread, NULL, receiver_thread, pgm_receiver) != 0) {
				xcb_log(XCB_LOG_ERROR, "Error initializing receiver thread");
				goto err;
			}
		}
		cat = category_browse(cfg, cat);
	}
	if ((tmp = variable_retrieve(cfg, "general", "password")) && strcmp(tmp, ""))
		password = tmp;
	if ((tmp = variable_retrieve(cfg, "dispatcher", "ip")) && strcmp(tmp, ""))
		dpip = tmp;
	if ((tmp = variable_retrieve(cfg, "dispatcher", "port")) && strcmp(tmp, ""))
		dpport = atoi(tmp);
	if ((tmp = variable_retrieve(cfg, "general", "tcp_port")) && strcmp(tmp, ""))
		if ((tcpsock = net_tcp_server(NULL, atoi(tmp), neterr, sizeof neterr)) == -1) {
			xcb_log(XCB_LOG_ERROR, "Opening port '%s': %s", tmp, neterr);
			goto err;
		}
	if ((tmp = variable_retrieve(cfg, "general", "ctm_port")) && strcmp(tmp, ""))
		if ((ctmsock = net_tcp_server(NULL, atoi(tmp), neterr, sizeof neterr)) == -1) {
			xcb_log(XCB_LOG_ERROR, "Opening port '%s': %s", tmp, neterr);
			goto err;
		}
	if (tcpsock > 0 && create_file_event(el, tcpsock, EVENT_READABLE, tcp_accept_handler, NULL) == -1) {
		xcb_log(XCB_LOG_ERROR, "Unrecoverable error creating tcpsock '%d' file event", tcpsock);
		goto err;
	}
	if (ctmsock > 0 && create_file_event(el, ctmsock, EVENT_READABLE, tcp_accept_handler, NULL) == -1) {
		xcb_log(XCB_LOG_ERROR, "Unrecoverable error creating ctmsock '%d' file event", ctmsock);
		goto err;
	}
	xcb_log(XCB_LOG_NOTICE, "Server publisher started");
	start_event_loop(el, ALL_EVENTS);
	delete_event_loop(el);
	pgm_shutdown();
	return 0;

err:
	close_logger();
	exit(1);
}

/* FIXME */
static void remove_from_subscribers(client c) {
	dlist_t dlist;
	dlist_iter_t diter;
	dlist_node_t dnode;

	ptrie_rwlock_wrlock(subscribers);
	dlist = ptrie_node_value(ptrie_get_root(subscribers));
	if ((dnode = dlist_find(dlist, c)))
		dlist_remove(dlist, dnode);
	else {
		diter = dlist_iter_new(c->subscribers, DLIST_START_HEAD);
		while ((dnode = dlist_next(diter))) {
			dstr skey = (dstr)dlist_node_value(dnode);
			dstr *fields = NULL;
			int nfield = 0;
			ptrie_node_t node;

			fields = dstr_split_len(skey, dstr_length(skey), ",", 1, &nfield);
			node = ptrie_get_index(subscribers, fields[0]);
			ptrie_remove(node, skey, c);
			dstr_free_tokens(fields, nfield);
		}
		dlist_iter_free(&diter);
	}
	dlist_free(&c->subscribers);
	ptrie_rwlock_unlock(subscribers);
}

/* FIXME */
static void remove_from_monitors(client c) {
	dlist_node_t node;

	dlist_rwlock_wrlock(monitors);
	if ((node = dlist_find(monitors, c)))
		dlist_remove(monitors, node);
	dlist_rwlock_unlock(monitors);
}

/* FIXME */
static void remove_from_clients(client c) {
	dlist_node_t node;

	dlist_lock(clients);
	if ((node = dlist_find(clients, c)))
		dlist_remove(clients, node);
	dlist_unlock(clients);
}

static void client_reset(client c) {
	int i;

	for (i = 0; i < c->argc; ++i)
		dstr_free(c->argv[i]);
	c->argc = 0;
	c->cmd  = NULL;
}

void client_free(client c) {
	dlist_node_t node;

	remove_from_subscribers(c);
	remove_from_monitors(c);
	remove_from_clients(c);
	if (c->flags & CLIENT_CLOSE_ASAP && (node = dlist_find(clients_to_close, c)))
		dlist_remove(clients_to_close, node);
	delete_file_event(el, c->fd, EVENT_READABLE);
	delete_file_event(el, c->fd, EVENT_WRITABLE);
	ring_free(&c->reply);
	client_reset(c);
	FREE(c->argv);
	pthread_spin_destroy(&c->lock);
	close(c->fd);
	xcb_log(XCB_LOG_NOTICE, "Client '%s:%d' got freed", c->ip, c->port);
	FREE(c);
}

/* FIXME */
static int process_command(client c) {
	/* the quit command */
	if (!strcasecmp(c->argv[0], "quit")) {
		add_reply_string(c, "OK\r\n\r\n", 6);
		c->flags |= CLIENT_CLOSE_AFTER_REPLY;
		return -1;
	}
	if (c->sock == ctmsock) {
		dstr ctm_cmd = dstr_new_len(c->argv[0], 1);

		if (!strcmp(c->argv[0], "SALL") || !strcmp(c->argv[0], "UALL") ||
			!strcmp(c->argv[0], "QC") || !strcmp(c->argv[0], "INDICES") ||
			!strcmp(c->argv[0], "INDEX") || !strcmp(c->argv[0], "AUTH"))
			c->cmd = table_get_value(ctm_cmds, c->argv[0]);
		else {
			c->cmd = table_get_value(ctm_cmds, ctm_cmd);
			dstr_free(ctm_cmd);
		}
	} else
		c->cmd = table_get_value(cmds, c->argv[0]);
	if (c->cmd == NULL) {
		add_reply_error_format(c, "unknown command '%s'\r\n", c->argv[0]);
		return 0;
	} else if ((c->cmd->arity > 0 && c->cmd->arity != c->argc) || c->argc < -c->cmd->arity) {
		add_reply_error_format(c, "wrong number of arguments for command '%s'\r\n", c->argv[0]);
		return 0;
	}
	c->cmd->cproc(c);
	return 0;
}

void process_inbuf(client c) {
	while (c->inpos > 0) {
		char *newline;
		size_t len;

		if (c->flags & CLIENT_CLOSE_AFTER_REPLY)
			break;
		if ((newline = strstr(c->inbuf, "\r\n")) == NULL) {
			if (c->inpos == sizeof c->inbuf - 1) {
				add_reply_error(c, "protocol error: too big request\r\n");
				c->flags |= CLIENT_CLOSE_AFTER_REPLY;
				c->inpos = 0;
			}
			break;
		}
		len = newline - c->inbuf;
		if (c->argv)
			FREE(c->argv);
		/* FIXME */
		*newline = '\0';
		xcb_log(XCB_LOG_INFO, "Client '%s:%d' issued command '%s'", c->ip, c->port, c->inbuf);
		c->argv = c->sock == ctmsock ? dstr_split_len(c->inbuf, len, ",", 1, &c->argc)
			: dstr_split_args(c->inbuf, &c->argc);
		memmove(c->inbuf, c->inbuf + len + 2, sizeof c->inbuf - len - 2);
		c->inpos -= len + 2;
		if (c->argc == 0 || process_command(c) == 0)
			client_reset(c);
	}
}

static client client_new(int fd, int sock, char *ip, int port) {
	client c;

	if (NEW(c) == NULL)
		return NULL;
	/* FIXME */
	net_nonblock(fd, NULL, 0);
	net_tcp_nodelay(fd, NULL, 0);
	if (create_file_event(el, fd, EVENT_READABLE, read_from_client, c) == -1) {
		close(fd);
		client_free(c);
		return NULL;
	}
	c->fd            = fd;
	pthread_spin_init(&c->lock, 0);
	c->sock          = sock;
	strcpy(c->ip, ip);
	c->port          = port;
	c->flags         = 0;
	c->inpos         = 0;
	c->argc          = 0;
	c->argv          = NULL;
	c->outpos        = 0;
	c->reply         = ring_new();
	c->sentlen       = 0;
	c->subscribers   = dlist_new(cmpstr, vfree);
	c->eagcount      = 0;
	c->refcount      = 0;
	c->authenticated = 0;
	dlist_lock(clients);
	dlist_insert_tail(clients, c);
	dlist_unlock(clients);
	return c;
}

static void tcp_accept_handler(event_loop el, int fd, int mask, void *data) {
	char cip[128];
	int cport, cfd;
	client c;
	NOT_USED(el);
	NOT_USED(mask);
	NOT_USED(data);

	if ((cfd = net_tcp_accept(fd, cip, &cport, neterr, sizeof neterr)) == -1) {
		xcb_log(XCB_LOG_WARNING, "Accepting client connection: %s", neterr);
		return;
	}
	if ((c = client_new(cfd, fd, cip, cport)) == NULL) {
		xcb_log(XCB_LOG_WARNING, "Error registering fd '%d' event for the new client: %s",
			cfd, strerror(errno));
		close(cfd);
	} else if (c->sock != ctmsock) {
		/* heartbeat */
		dstr res = dstr_new("HEARTBEAT|");
		dstr ip = getipv4();
		dstr indices = get_indices();

		xcb_log(XCB_LOG_NOTICE, "Accepted client '%s:%d'", c->ip, c->port);
		res = dstr_cat(res, ip);
		res = dstr_cat(res, "\r\n");
		res = dstr_cat(res, indices);
		pthread_spin_lock(&c->lock);
		if (net_try_write(c->fd, res, dstr_length(res), 10, NET_NONBLOCK) == -1)
			xcb_log(XCB_LOG_WARNING, "Writing to client '%s:%d': %s",
				c->ip, c->port, strerror(errno));
		pthread_spin_unlock(&c->lock);
		dstr_free(indices);
		dstr_free(ip);
		dstr_free(res);
	} else {
		dstr res = get_indices();

		xcb_log(XCB_LOG_NOTICE, "Accepted client '%s:%d'", c->ip, c->port);
		pthread_spin_lock(&c->lock);
		if (net_try_write(c->fd, res, dstr_length(res), 10, NET_NONBLOCK) == -1)
			xcb_log(XCB_LOG_WARNING, "Writing to client '%s:%d': %s",
				c->ip, c->port, strerror(errno));
		pthread_spin_unlock(&c->lock);
		dstr_free(res);
	}
}

static void help_command(client c) {
	int ncmds, i;

	ncmds = sizeof commands / sizeof (struct cmd);
	for (i = 0; i < ncmds; ++i) {
		struct cmd *cmd = commands + i;

		add_reply_string_format(c, "%30.30s  %s\r\n", cmd->name, cmd->doc);
	}
	add_reply_string(c, "\r\n", 2);
}

/* FIXME */
static void config_command(client c) {
	if (!strcasecmp(c->argv[1], "get")) {
		pthread_mutex_lock(&cfg_lock);
		if (!strcasecmp(c->argv[2], "log_level"))
			add_reply_string_format(c, "log_level:%s\r\n",
				variable_retrieve(cfg, "general", "log_level"));
		else if (!strcasecmp(c->argv[2], "last_quote"))
			add_reply_string_format(c, "last_quote:%s\r\n",
				variable_retrieve(cfg, "general", "last_quote"));
		else
			add_reply_string(c, "-1\r\n", 4);
		pthread_mutex_unlock(&cfg_lock);
	} else if (!strcasecmp(c->argv[1], "set")) {
		struct category *category;

		pthread_mutex_lock(&cfg_lock);
		if (!strcasecmp(c->argv[2], "log_level") && c->argc >= 4) {
			category = category_get(cfg, "general");
			if (variable_update(category, "log_level", c->argv[3]) == 0) {
				add_reply_string(c, "OK\r\n", 4);
				if (!strcasecmp(c->argv[3], "debug"))
					set_logger_level(__LOG_DEBUG);
				else if (!strcasecmp(c->argv[3], "info"))
					set_logger_level(__LOG_INFO);
				else if (!strcasecmp(c->argv[3], "notice"))
					set_logger_level(__LOG_NOTICE);
				else if (!strcasecmp(c->argv[3], "warning"))
					set_logger_level(__LOG_WARNING);
			} else
				add_reply_string(c, "-1\r\n", 4);
		} else if (!strcasecmp(c->argv[2], "last_quote") && c->argc >= 4) {
			category = category_get(cfg, "general");
			if (variable_update(category, "last_quote", c->argv[3]) == 0) {
				add_reply_string(c, "OK\r\n", 4);
				if (atoi(c->argv[3]) == 0)
					__sync_bool_compare_and_swap(&last_quote, 1, 0);
				else if (atoi(c->argv[3]) == 1)
					__sync_bool_compare_and_swap(&last_quote, 0, 1);
			} else
				add_reply_string(c, "-1\r\n", 4);
		} else
			add_reply_string(c, "-1\r\n", 4);
		pthread_mutex_unlock(&cfg_lock);
	} else
		add_reply_error_format(c, "unknown action '%s'", c->argv[1]);
	add_reply_string(c, "\r\n", 2);
}

static void show_command(client c) {
	if (!strcasecmp(c->argv[1], "clients")) {
		dlist_lock(clients);
		if (dlist_length(clients) > 0) {
			dlist_iter_t iter = dlist_iter_new(clients, DLIST_START_HEAD);
			dlist_node_t node;

			while ((node = dlist_next(iter))) {
				client ct = (client)dlist_node_value(node);

				add_reply_string_format(c, "%30.30s  %d\r\n", ct->ip, ct->port);
			}
			dlist_iter_free(&iter);
		}
		dlist_unlock(clients);
	} else
		add_reply_error_format(c, "unknown property '%s'", c->argv[1]);
	add_reply_string(c, "\r\n", 2);
}

/* FIXME */
static void monitor_command(client c) {
	if (!strcasecmp(c->argv[1], "on")) {
		dlist_rwlock_wrlock(monitors);
		if (dlist_find(monitors, c) == NULL)
			dlist_insert_tail(monitors, c);
		dlist_rwlock_unlock(monitors);
	} else if (!strcasecmp(c->argv[1], "off")) {
		remove_from_monitors(c);
		add_reply_string(c, "\r\n", 2);
	} else
		add_reply_error_format(c, "unknown action '%s'\r\n", c->argv[1]);
}

/* FIXME */
static void database_command(client c) {
	db_iterator_t *it;
	int i;

	if (!persistence) {
		add_reply_error(c, "database not open\r\n");
		return;
	}
	if (!strcasecmp(c->argv[1], "head")) {
		it = db_iterator_create(db, db_ro);
		db_iterator_seek_to_first(it);
		if (!db_iterator_valid(it)) {
			add_reply_string(c, "-1\r\n\r\n", 6);
			return;
		}
		for (i = 0; i < 10; ++i) {
			const char *key, *value;
			size_t klen, vlen;

			key   = db_iterator_key(it, &klen);
			value = db_iterator_value(it, &vlen);
			add_reply_string(c, key, klen);
			add_reply_string(c, ",", 1);
			add_reply_string(c, value, vlen);
			add_reply_string(c, "\r\n", 2);
			db_iterator_next(it);
			if (!db_iterator_valid(it))
				break;
		}
		db_iterator_destroy(&it);
	} else if (!strcasecmp(c->argv[1], "tail")) {
		dlist_t dlist = dlist_new(NULL, vfree);
		dlist_iter_t iter;
		dlist_node_t node;

		it = db_iterator_create(db, db_ro);
		db_iterator_seek_to_last(it);
		if (!db_iterator_valid(it)) {
			add_reply_string(c, "-1\r\n\r\n", 6);
			return;
		}
		for (i = 0; i < 10; ++i) {
			const char *key, *value;
			size_t klen, vlen;
			dstr ds;

			key   = db_iterator_key(it, &klen);
			value = db_iterator_value(it, &vlen);
			ds = dstr_new_len(key, klen);
			ds = dstr_cat(ds, ",");
			ds = dstr_cat_len(ds, value, vlen);
			dlist_insert_head(dlist, ds);
			db_iterator_prev(it);
			if (!db_iterator_valid(it))
				break;
		}
		iter = dlist_iter_new(dlist, DLIST_START_HEAD);
		while ((node = dlist_next(iter))) {
			dstr ds = (dstr)dlist_node_value(node);

			add_reply_string(c, ds, dstr_length(ds));
			add_reply_string(c, "\r\n", 2);
		}
		dlist_iter_free(&iter);
		db_iterator_destroy(&it);
		dlist_free(&dlist);
	} else if (!strcasecmp(c->argv[1], "put")) {
		if (c->argc >= 4) {
			char *dberr = NULL;

			db_put(db, db_wo, c->argv[2], dstr_length(c->argv[2]),
				c->argv[3], dstr_length(c->argv[3]), &dberr);
			if (dberr) {
				add_reply_error_format(c, "database put: %s", dberr);
				db_free(dberr);
			} else
				add_reply_string(c, "OK\r\n", 4);
		}
	} else if (!strcasecmp(c->argv[1], "delete")) {
		if (c->argc >= 3) {
			char *dberr = NULL;

			db_delete(db, db_wo, c->argv[2], dstr_length(c->argv[2]), &dberr);
			if (dberr) {
				add_reply_error_format(c, "database delete: %s", dberr);
				db_free(dberr);
			} else
				add_reply_string(c, "OK\r\n", 4);
		}
	} else if (!strcasecmp(c->argv[1], "compact")) {
		db_compact_range(db, NULL, 0, NULL, 0);
		add_reply_string(c, "OK\r\n", 4);
	} else if (!strcasecmp(c->argv[1], "props")) {
		if (c->argc >= 3) {
			char buf[64], *prop;

			if (!strcasecmp(c->argv[2], "stats") || !strcasecmp(c->argv[2], "sstables"))
				snprintf(buf, sizeof buf, "leveldb.%s", c->argv[2]);
			else if (!strcmp(c->argv[2], "0") || !strcmp(c->argv[2], "1") ||
				!strcmp(c->argv[2], "2") || !strcmp(c->argv[2], "3") ||
				!strcmp(c->argv[2], "4") || !strcmp(c->argv[2], "5") ||
				!strcmp(c->argv[2], "6"))
				snprintf(buf, sizeof buf, "leveldb.num-files-at-level%s", c->argv[2]);
			else {
				add_reply_error_format(c, "unknown property '%s'\r\n", c->argv[2]);
				return;
			}
			prop = db_property_value(db, buf);
			add_reply_string(c, prop, strlen(prop));
			add_reply_string(c, "\r\n", 2);
			db_free(prop);
		}
	} else
		add_reply_error_format(c, "unknown action '%s'", c->argv[1]);
	add_reply_string(c, "\r\n", 2);
}

static void shutdown_command(client c) {
	if (prepare_for_shutdown() == 0)
		exit(0);
	add_reply_error(c, "errors trying to shutdown the server\r\n");
}


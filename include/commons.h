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

#ifndef COMMONS_INCLUDED
#define COMMONS_INCLUDED

#include <pthread.h>
#include <time.h>
#include "dlist.h"
#include "ring.h"
#include "dstr.h"
#include "event.h"

/* FIXME */
#define CLIENT_CLOSE_AFTER_REPLY (1 << 0)
#define CLIENT_CLOSE_ASAP        (1 << 1)

/* FIXME */
struct pgm_cfg {
	const char		*network;
	int			port;
};

/* FIXME */
typedef struct client {
	int			fd;
	pthread_spinlock_t	lock;
	int			sock;
	char			ip[128];
	int			port;
	struct cmd		*cmd;
	int			flags;
	int			inpos;
	char			inbuf[64 * 1024];
	int			argc;
	dstr			*argv;
	int			outpos;
	char			outbuf[16 * 1024];
	ring_t			reply;
	int			sentlen;
	dlist_t			subscribers;
	int			eagcount;
	int			refcount;
	int			authenticated;
} *client;

/* FIXME */
typedef void cmd_proc(client c);
struct cmd {
	char			*name;
	cmd_proc		*cproc;
	char			*doc;
	int			arity;
};

/* FIXME */
#define NANOSLEEP(nanosec) \
	do { \
		struct timespec ts; \
		ts.tv_sec  = 0; \
		ts.tv_nsec = (nanosec); \
		nanosleep(&ts, NULL); \
	} while (0);

/* FIXME */
extern void client_incr(client c);
extern void client_decr(client c);
extern void client_free_async(client c);
extern void add_reply_string(client c, const char *str, size_t len);
extern void add_reply_string_format(client c, const char *fmt, ...);
extern void add_reply_error(client c, const char *err);
extern void add_reply_error_format(client c, const char *fmt, ...);
extern void read_from_client(event_loop el, int fd, int mask, void *data);

#endif /* COMMONS_INCLUDED */


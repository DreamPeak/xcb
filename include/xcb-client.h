/*
 * Copyright (c) 2013-2017, Dalian Futures Information Technology Co., Ltd.
 *
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

#ifndef XCB_CLIENT_INCLUDED
#define XCB_CLIENT_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include <time.h>

#define XCB_ERR         -1
#define XCB_OK           0

#define XCB_ERR_IO       1 /* error in read or write */
#define XCB_ERR_EOF      3 /* end of file            */
#define XCB_ERR_PROTOCOL 4 /* protocol error         */
#define XCB_ERR_OOM      5 /* out of memory          */
#define XCB_ERR_OTHER    2 /* everything else...     */

/* FIXME: exported types */
typedef struct xcb_context_t *xcb_context_t;
typedef const char *reply_subscribe_t;
typedef const char *reply_query_t;
typedef const char *reply_indices_t;
typedef const char *reply_index_t;
typedef void (*xcb_callback_connect_t)(xcb_context_t c);
typedef void (*xcb_callback_subscribe_t)(reply_subscribe_t r);
typedef void (*xcb_callback_query_t)(reply_query_t r);
typedef void (*xcb_callback_indices_t)(reply_indices_t);
typedef void (*xcb_callback_index_t)(reply_index_t r);
struct xcb_context_t {
	int				err;
	char				errstr[128];
	int				fd;
	int				inpos;
	char				inbuf[8 * 1024 * 1024];
	xcb_callback_connect_t		xcb_callback_connect;
	xcb_callback_subscribe_t	xcb_callback_subscribe;
	xcb_callback_query_t		xcb_callback_query;
	xcb_callback_indices_t		xcb_callback_indices;
	xcb_callback_index_t		xcb_callback_index;
};

/* connect to XCUBE and create a client context */
extern xcb_context_t xcb_connect(const char *addr, int port);
/* free the client context */
extern void          xcb_free(xcb_context_t c);
/* wait for all background threads to complete */
extern void          xcb_join(xcb_context_t c);
/* register callback which is triggered when connected */
extern void          xcb_register_callback_connect(xcb_context_t c, xcb_callback_connect_t cb);
/* register callback which processes replies to subscribe */
extern void          xcb_register_callback_subscribe(xcb_context_t c, xcb_callback_subscribe_t cb);
/* register callback which processes replies to query */
extern void          xcb_register_callback_query(xcb_context_t c, xcb_callback_query_t cb);
/* register callback which processes indices */
extern void          xcb_register_callback_indices(xcb_context_t c, xcb_callback_indices_t cb);
/* register callback which processes index format */
extern void          xcb_register_callback_index(xcb_context_t c, xcb_callback_index_t cb);
/* subscribe an index according to a certain pattern */
extern int           xcb_subscribe(xcb_context_t c, const char *index, const char *pattern);
/* subscribe all indices */
extern int           xcb_subscribe_all(xcb_context_t c);
/* unsubscribe an index according to a certain pattern */
extern int           xcb_unsubscribe(xcb_context_t c, const char *index, const char *pattern);
/* unsubscribe all indices */
extern int           xcb_unsubscribe_all(xcb_context_t c);
/* query an index according to a certain pattern, with session id, starting and ending time */
extern int           xcb_query(xcb_context_t c, const char *sid, const char *index, const char *pattern,
			struct tm *start, struct tm *end);
/* cancel a query using its session id */
extern int           xcb_query_cancel(xcb_context_t c, const char *sid);
/* return all indices supported by XCUBE */
extern int           xcb_indices(xcb_context_t c, const char *sid);
/* return index format */
extern int           xcb_index(xcb_context_t c, const char *index);

#ifdef __cplusplus
}
#endif

#endif /* XCB_CLIENT_INCLUDED */


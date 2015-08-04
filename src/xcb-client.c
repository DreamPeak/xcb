/*
 * Copyright (c) 2013-2015, Dalian Futures Information Technology Co., Ltd.
 *
 * Bo Wang
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
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <poll.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include "macros.h"
#include "mem.h"
#include "dstr.h"
#include "net.h"
#include "xcb-client.h"

/* FIXME */
static pthread_t cthread = (pthread_t)-1;
static pthread_t rthread = (pthread_t)-1;
static struct info {
	xcb_context_t	c;
	dstr		ip;
	int		port;
} info;

/* FIXME */
static xcb_context_t xcb_context_init(void) {
	xcb_context_t c;

	if (NEW0(c) == NULL)
		return NULL;
	c->err       = 0;
	c->errstr[0] = '\0';
	c->inpos     = 0;
	return c;
}

/* FIXME */
static void xcb_set_error(xcb_context_t c, int type, const char *str) {
	c->err = type;
	if (str) {
		size_t len = strlen(str);

		len = len < sizeof c->errstr - 1 ? len : sizeof c->errstr - 1;
		memcpy(c->errstr, str, len);
		c->errstr[len] = '\0';
	} else {
		/* assert(type == XCB_ERR_IO); */
		strerror_r(errno, c->errstr, sizeof c->errstr);
	}
}

/* FIXME */
static void xcb_set_error_from_errno(xcb_context_t c, int type, const char *prefix) {
	char buf[128];
	size_t len = 0;

	if (prefix)
		len = snprintf(buf, sizeof buf, "%s: ", prefix);
	strerror_r(errno, buf + len, sizeof buf - len);
	xcb_set_error(c, type, buf);
}

static void *read_thread(void *data) {
	xcb_context_t c = (xcb_context_t)data;
	struct pollfd rfd[1];

	rfd[0].fd     = c->fd;
	rfd[0].events = POLLIN;
	for (;;) {
		int nread;

		if (poll(rfd, 1, -1) == -1) {
			xcb_set_error_from_errno(c, XCB_ERR_IO, "poll(2)");
			break;
		}
		if ((nread = read(c->fd, c->inbuf + c->inpos, sizeof c->inbuf - c->inpos)) == -1) {
			if (errno == EAGAIN || errno == EINTR)
				nread = 0;
			else {
				xcb_set_error(c, XCB_ERR_IO, NULL);
				break;
			}
		} else if (nread == 0) {
			xcb_set_error(c, XCB_ERR_EOF, "Server closed the connection");
			break;
		}
		if (nread)
			c->inpos += nread;
		else
			continue;
		if (c->inpos >= sizeof c->inbuf) {
			xcb_set_error(c, XCB_ERR_IO, "Client reached max input buffer length");
			break;
		}
		c->inbuf[c->inpos] = '\0';
		while (c->inpos > 0) {
			char *newline;
			size_t len;
			dstr *fields = NULL;
			int nfield = 0;

			if ((newline = strstr(c->inbuf, "\r\n")) == NULL) {
				if (c->inpos == sizeof c->inbuf - 1) {
					xcb_set_error(c, XCB_ERR_PROTOCOL, "Too big reply");
					goto end;
				}
				break;
			}
			len = newline - c->inbuf;
			fields = dstr_split_len(c->inbuf, len, ",", 1, &nfield);
			if (nfield > 1) {
				/* FIXME */
				if (!strcasecmp(fields[1], "indices")) {
					if (c->xcb_callback_indices)
						c->xcb_callback_indices(c->inbuf);
				} else if (!strcasecmp(fields[1], "timestamp")) {
					if (c->xcb_callback_index)
						c->xcb_callback_index(c->inbuf);
				} else if (!strcmp(fields[1], "0") || !strcmp(fields[1], "1")) {
					if (c->xcb_callback_query)
						c->xcb_callback_query(c->inbuf);
				} else if (c->xcb_callback_subscribe)
					c->xcb_callback_subscribe(c->inbuf);
			}
			dstr_free_tokens(fields, nfield);
			memmove(c->inbuf, c->inbuf + len + 2, sizeof c->inbuf - len - 2);
			c->inpos -= len + 2;
		}
	}

end:
	rthread = (pthread_t)-1;
	return NULL;
}

static int xcb_connect_tcp(xcb_context_t c, const char *addr, int port) {
	char _port[6];
	struct addrinfo hints, *servinfo, *p;
	int rv;

	snprintf(_port, 6, "%d", port);
	memset(&hints, '\0', sizeof hints);
	hints.ai_family   = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	if ((rv = getaddrinfo(addr, _port, &hints, &servinfo)) != 0) {
		xcb_set_error(c, XCB_ERR_OTHER, gai_strerror(rv));
		return XCB_ERR;
	}
	for (p = servinfo; p; p = p->ai_next) {
		int s, flags, rcvbuf = 8 * 1024 * 1024, on = 1;

		/* FIXME */
		if ((s = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
			continue;
		if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &on, sizeof on) == -1) {
			xcb_set_error_from_errno(c, XCB_ERR_IO, "setsockopt(SO_REUSEADDR)");
			close(s);
			goto err;
		}
		if ((flags = fcntl(s, F_GETFL)) == -1) {
			xcb_set_error_from_errno(c, XCB_ERR_IO, "fcntl(F_GETFL)");
			close(s);
			goto err;
		}
		if (fcntl(s, F_SETFL, flags | O_NONBLOCK) == -1) {
			xcb_set_error_from_errno(c, XCB_ERR_IO, "fcntl(F_SETFL)");
			close(s);
			goto err;
		}
		if (connect(s, p->ai_addr, p->ai_addrlen) == -1) {
			if (errno == EHOSTUNREACH) {
				close(s);
				continue;
			} else if (errno == EINPROGRESS) {
				/* FIXME: This is OK. */
			} else {
				xcb_set_error_from_errno(c, XCB_ERR_IO, NULL);
				close(s);
				goto err;
			}
		}
		if (setsockopt(s, SOL_SOCKET, SO_RCVBUF, &rcvbuf, sizeof rcvbuf) == -1) {
			xcb_set_error_from_errno(c, XCB_ERR_IO, "setsockopt(SO_RCVBUF)");
			close(s);
			goto err;
		}
		if (setsockopt(s, IPPROTO_TCP, TCP_NODELAY, &on, sizeof on) == -1) {
			xcb_set_error_from_errno(c, XCB_ERR_IO, "setsockopt(TCP_NODELAY)");
			close(s);
			goto err;
		}
		c->fd = s;
		if (rthread == (pthread_t)-1)
			if (pthread_create(&rthread, NULL, read_thread, c) != 0) {
				xcb_set_error_from_errno(c, XCB_ERR_OTHER, "pthread_create");
				close(s);
				goto err;
			}
		if (c->xcb_callback_connect)
			c->xcb_callback_connect(c);
		rv = XCB_OK;
		goto end;
	}
	if (p == NULL) {
		char buf[128];

		snprintf(buf, sizeof buf, "Can't create socket: %s", strerror(errno));
		xcb_set_error(c, XCB_ERR_OTHER, buf);
	}

err:
	rv = XCB_ERR;
end:
	freeaddrinfo(servinfo);
	return rv;
}

/* FIXME */
static void *check_thread(void *data) {
	struct info *ifp = (struct info *)data;

	signal(SIGPIPE, SIG_IGN);
	for (;;) {

start:
		sleep(5);
		if (write(ifp->c->fd, "\r\n", 2) == -1) {
			int count;

			for (count = 0; count < 3; ++count) {
				sleep(5);
				if (write(ifp->c->fd, "\r\n", 2) == -1)
					xcb_set_error(ifp->c, XCB_ERR_IO, NULL);
				else
					goto start;
			}
			close(ifp->c->fd);
			while (1)
				if (xcb_connect_tcp(ifp->c, ifp->ip, ifp->port) == XCB_ERR)
					sleep(10);
				else
					break;
		}
	}
	return NULL;
}

/* FIXME */
xcb_context_t xcb_connect(const char *addr, int port) {
	xcb_context_t c = xcb_context_init();

	if (c == NULL)
		goto end;
	if (xcb_connect_tcp(c, addr, port) == XCB_ERR)
		goto end;
	info.c    = c;
	info.ip   = dstr_new(addr);
	info.port = port;
	if (pthread_create(&cthread, NULL, check_thread, &info) != 0)
		xcb_set_error_from_errno(c, XCB_ERR_OTHER, "pthread_create");

end:
	return c;
}

/* FIXME */
void xcb_free(xcb_context_t c) {
	if (c->fd > 0)
		close(c->fd);
	FREE(c);
}

/* FIXME */
void xcb_join(xcb_context_t c) {
	NOT_USED(c);

	pthread_join(cthread, NULL);
	pthread_join(rthread, NULL);
}

/* FIXME */
void xcb_register_callback_connect(xcb_context_t c, xcb_callback_connect_t cb) {
	if (cb)
		c->xcb_callback_connect = cb;
}

/* FIXME */
void xcb_register_callback_subscribe(xcb_context_t c, xcb_callback_subscribe_t cb) {
	if (cb)
		c->xcb_callback_subscribe = cb;
}

/* FIXME */
void xcb_register_callback_query(xcb_context_t c, xcb_callback_query_t cb) {
	if (cb)
		c->xcb_callback_query = cb;
}

/* FIXME */
void xcb_register_callback_indices(xcb_context_t c, xcb_callback_indices_t cb) {
	if (cb)
		c->xcb_callback_indices = cb;
}

/* FIXME */
void xcb_register_callback_index(xcb_context_t c, xcb_callback_index_t cb) {
	if (cb)
		c->xcb_callback_index = cb;
}

/* FIXME */
int xcb_subscribe(xcb_context_t c, const char *index, const char *pattern) {
	dstr s;

	if (index == NULL) {
		xcb_set_error(c, XCB_ERR_OTHER, "Index can't be empty");
		return XCB_ERR;
	}
	s = dstr_new("S");
	s = dstr_cat(s, index);
	if (pattern) {
		s = dstr_cat(s, ",");
		s = dstr_cat(s, pattern);
	}
	s = dstr_cat(s, "\r\n");
	if (net_try_write(c->fd, s, dstr_length(s), 10, NET_NONBLOCK) == -1) {
		xcb_set_error(c, XCB_ERR_IO, NULL);
		dstr_free(s);
		return XCB_ERR;
	}
	dstr_free(s);
	return XCB_OK;
}

/* FIXME */
int xcb_subscribe_all(xcb_context_t c) {
	if (net_try_write(c->fd, "SALL\r\n", 6, 10, NET_NONBLOCK) == -1) {
		xcb_set_error(c, XCB_ERR_IO, NULL);
		return XCB_ERR;
	}
	return XCB_OK;
}

/* FIXME */
int xcb_unsubscribe(xcb_context_t c, const char *index, const char *pattern) {
	dstr s;

	if (index == NULL) {
		xcb_set_error(c, XCB_ERR_OTHER, "Index can't be empty");
		return XCB_ERR;
	}
	s = dstr_new("U");
	s = dstr_cat(s, index);
	if (pattern) {
		s = dstr_cat(s, ",");
		s = dstr_cat(s, pattern);
	}
	s = dstr_cat(s, "\r\n");
	if (net_try_write(c->fd, s, dstr_length(s), 10, NET_NONBLOCK) == -1) {
		xcb_set_error(c, XCB_ERR_IO, NULL);
		dstr_free(s);
		return XCB_ERR;
	}
	dstr_free(s);
	return XCB_OK;
}

/* FIXME */
int xcb_unsubscribe_all(xcb_context_t c) {
	if (net_try_write(c->fd, "UALL\r\n", 6, 10, NET_NONBLOCK) == -1) {
		xcb_set_error(c, XCB_ERR_IO, NULL);
		return XCB_ERR;
	}
	return XCB_OK;
}

/* FIXME */
int xcb_query(xcb_context_t c, const char *sid, const char *index, const char *pattern,
	struct tm *start, struct tm *end) {
	dstr s;
	char buf[128];

	if (sid == NULL || index == NULL) {
		xcb_set_error(c, XCB_ERR_OTHER, "Session ID or index can't be empty");
		return XCB_ERR;
	}
	s = dstr_new("Q");
	s = dstr_cat(s, index);
	s = dstr_cat(s, ",");
	s = dstr_cat(s, sid);
	s = dstr_cat(s, ",");
	s = dstr_cat(s, pattern);
	s = dstr_cat(s, ",");
	strftime(buf, sizeof buf, "%F %T", start);
	s = dstr_cat(s, ",");
	strftime(buf, sizeof buf, "%F %T", end);
	s = dstr_cat(s, "\r\n");
	if (net_try_write(c->fd, s, dstr_length(s), 10, NET_NONBLOCK) == -1) {
		xcb_set_error(c, XCB_ERR_IO, NULL);
		dstr_free(s);
		return XCB_ERR;
	}
	dstr_free(s);
	return XCB_OK;
}

/* FIXME */
int xcb_query_cancel(xcb_context_t c, const char *sid) {
	dstr s;

	if (sid == NULL) {
		xcb_set_error(c, XCB_ERR_OTHER, "Session ID can't be empty");
		return XCB_ERR;
	}
	s = dstr_new("QC,");
	s = dstr_cat(s, sid);
	s = dstr_cat(s, "\r\n");
	if (net_try_write(c->fd, s, dstr_length(s), 10, NET_NONBLOCK) == -1) {
		xcb_set_error(c, XCB_ERR_IO, NULL);
		dstr_free(s);
		return XCB_ERR;
	}
	dstr_free(s);
	return XCB_OK;
}

/* FIXME */
int xcb_indices(xcb_context_t c, const char *sid) {
	dstr s;

	if (sid == NULL) {
		xcb_set_error(c, XCB_ERR_OTHER, "Session ID can't be empty");
		return XCB_ERR;
	}
	s = dstr_new("INDICES,");
	s = dstr_cat(s, sid);
	s = dstr_cat(s, "\r\n");
	if (net_try_write(c->fd, s, dstr_length(s), 10, NET_NONBLOCK) == -1) {
		xcb_set_error(c, XCB_ERR_IO, NULL);
		dstr_free(s);
		return XCB_ERR;
	}
	dstr_free(s);
	return XCB_OK;
}

/* FIXME */
int xcb_index(xcb_context_t c, const char *index) {
	dstr s;

	if (index == NULL) {
		xcb_set_error(c, XCB_ERR_OTHER, "Index can't be empty");
		return XCB_ERR;
	}
	s = dstr_new("INDEX,");
	s = dstr_cat(s, index);
	s = dstr_cat(s, "\r\n");
	if (net_try_write(c->fd, s, dstr_length(s), 10, NET_NONBLOCK) == -1) {
		xcb_set_error(c, XCB_ERR_IO, NULL);
		dstr_free(s);
		return XCB_ERR;
	}
	dstr_free(s);
	return XCB_OK;
}


/*
 * Copyright (c) 2013-2017, Dalian Futures Information Technology Co., Ltd.
 *
 * Gaohang Wu  <wugaohang at dce dot com dot cn>
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
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <time.h>
#include <unistd.h>
#include "macros.h"
#include "mem.h"
#include "dlist.h"
#include "dstr.h"
#include "net.h"
#include "basics.h"

/* FIXME */
#define PAGEALIGN(len) ((len) + sysconf(_SC_PAGESIZE) - 1) & ~(sysconf(_SC_PAGESIZE) - 1)

/* FIXME */
static dstr ip;
static int port;

static void usage(void) {
	fprintf(stderr,
		"Usage: xcb-sim [OPTION]... <FILE>\n"
		"  -h <hostname> Server hostname (default: 127.0.0.1)\n"
		"  -p <port>     Server port (default: 33330)\n"
		"  -?            Output this help and exit\n");
	exit(1);
}

/* FIXME */
static void vfree(void *value) {
	FREE(value);
}

int main(int argc, char **argv) {
	int opt, i = 1, fd, sock;
	struct stat sb;
	char *addr, *p, *q;
	char neterr[256];
	int pfd[2];
	dlist_t dlist = dlist_new(NULL, vfree);

	/* FIXME */
	ip = dstr_new("127.0.0.1");
	port = 33329;
	while ((opt = getopt(argc, argv, "h:p:?")) != -1)
		switch (opt) {
		case 'h':
			dstr_free(ip);
			ip = dstr_new(optarg);
			i += 2;
			break;
		case 'p':
			port = atoi(optarg);
			i += 2;
			break;
		case '?':
		default:
			usage();
		}
	if ((fd = open(argv[i], O_RDONLY)) == -1) {
		fprintf(stderr, "Error opening file: %s\n", strerror(errno));
		exit(1);
	}
	fstat(fd, &sb);
	if ((addr = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0)) == MAP_FAILED) {
		fprintf(stderr, "Error mmaping file: %s\n", strerror(errno));
		exit(1);
	}
	close(fd);
	if ((sock = net_udp_nonblock_connect(ip, port, neterr, sizeof neterr)) == -1) {
		fprintf(stderr, "Connecting %s:%d: %s\n", ip, port, neterr);
		exit(1);
	}
	if (pipe(pfd) != 0) {
		fprintf(stderr, "Error creating pipe: %s\n", strerror(errno));
		exit(1);
	}
	p = addr;
	q = memchr(p, '\n', sb.st_size);
	while (q) {
		dstr *fields = NULL;
		int nfield = 0;

		/* FIXME */
		fields = dstr_split_len(p, q - p, ",", 1, &nfield);
		if (nfield > 13) {
			int len = PAGEALIGN(sizeof (Quote));
			Quote *quote;

			if ((quote = POSIXALIGN(sysconf(_SC_PAGESIZE), len))) {
				int time, date;
				struct tm tm;
				struct iovec iov;

				memset(quote, '\0', len);
				quote->thyquote.m_nLen   = sizeof (tHYQuote);
				RMCHR(fields[2], ':');
				RMCHR(fields[2], '.');
				time = atoi(fields[2]);
				date = atoi(fields[0]);
				tm.tm_sec  = time % 100000   / 1000;
				tm.tm_min  = time % 10000000 / 100000;
				tm.tm_hour = time / 10000000;
				tm.tm_mday = date % 100;
				tm.tm_mon  = date / 100 % 100 - 1;
				tm.tm_year = date / 10000 - 1900;
				quote->thyquote.m_nTime  = mktime(&tm);
				strncat(quote->thyquote.m_cHYDM, fields[1],
					sizeof quote->thyquote.m_cHYDM - 1);
				quote->thyquote.m_dJSP   = atof(fields[5]);
				quote->thyquote.m_dJKP   = atof(fields[4]);
				quote->thyquote.m_nCCL   = atoi(fields[9]);
				quote->thyquote.m_dZXJ   = atof(fields[3]);
				quote->thyquote.m_nCJSL  = atoi(fields[8]);
				quote->thyquote.m_dZDJ   = atof(fields[7]);
				quote->thyquote.m_dZGJ   = atof(fields[6]);
				quote->thyquote.m_dMRJG1 = atof(fields[10]);
				quote->thyquote.m_dMCJG1 = atof(fields[12]);
				quote->thyquote.m_nMRSL1 = atoi(fields[11]);
				quote->thyquote.m_nMCSL1 = atoi(fields[13]);
				quote->m_nMSec           = time % 1000;
				iov.iov_base = quote;
				iov.iov_len  = len;
				/* zero copy */
				if (vmsplice(pfd[1], &iov, 1, SPLICE_F_GIFT) == -1) {
					fprintf(stderr, "Error vmsplicing: %s\n", strerror(errno));
					exit(1);
				}

again:
				if (splice(pfd[0], NULL, sock, NULL, len, SPLICE_F_MOVE) == -1) {
					if (errno == EAGAIN)
						goto again;
					else {
						fprintf(stderr, "Error splicing: %s\n", strerror(errno));
						exit(1);
					}
				}
				dlist_insert_tail(dlist, quote);
			} else
				fprintf(stderr, "Error allocating memory for quote\n");
		}
		dstr_free_tokens(fields, nfield);
		p = q + 1;
		q = memchr(p, '\n', sb.st_size - (q - addr));
		if (q == NULL) {
			p = addr;
			q = memchr(p, '\n', sb.st_size);
		}
		if (dlist_length(dlist) >= 2000) {
			dlist_free(&dlist);
			dlist = dlist_new(NULL, vfree);
		}
	}
	close(pfd[0]);
	close(pfd[1]);
	close(sock);
	munmap(addr, sb.st_size);
	return 0;
}


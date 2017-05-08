/*
 * Copyright (c) 2013-2017, Dalian Futures Information Technology Co., Ltd.
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

#include "fmacros.h"
#include <alloca.h>
#include <errno.h>
#include <ifaddrs.h>
#include <netdb.h>
#include <string.h>
#include "utils.h"

/* FIXME */
int cmpstr(const void *x, const void *y) {
	return strcmp((char *)x, (char *)y);
}

/* FIXME: Peter J. Weinberger hash */
unsigned hashpjw(const void *key) {
	const char *ptr = key;
	unsigned int val = 0, tmp;

	while (*ptr != '\0') {
		val = (val << 4) + *ptr++;
		if ((tmp = val & (unsigned int)0xf0000000) != 0)
			val = (val ^ (tmp >> 24)) ^ tmp;
	}
	return val;
}

/* FIXME */
unsigned hashdjb2(const void *key) {
	const char *ptr = key;
	int c;
	unsigned hash = 5381;

	while ((c = *ptr++))
		/* hash * 33 + c */
		hash = ((hash << 5) + hash) + c;
	return hash;
}

/* FIXME */
unsigned hashmurmur2(const void *key) {
	const uint32_t m = 0x5bd1e995;
	const uint32_t r = 24;
	size_t len = strlen(key);
	const unsigned char *data = (const unsigned char *)key;
	uint32_t hash = 5381 ^ len;

	while (len >= 4) {
		uint32_t k = *(uint32_t *)data;

		k *= m;
		k ^= k >> r;
		k *= m;
		hash *= m;
		hash ^= k;
		data += 4;
		len  -= 4;
	}
	switch (len) {
	case 3: hash ^= data[2] << 16;
	case 2: hash ^= data[1] << 8;
	case 1: hash ^= data[0]; hash *= m;
	};
	hash ^= hash >> 13;
	hash *= m;
	hash ^= hash >> 15;
	return (unsigned)hash;
}

/* FIXME */
unsigned intlen(int32_t i) {
	if (i >= 1000000000) return 10;
	if (i >= 100000000 ) return 9;
	if (i >= 10000000  ) return 8;
	if (i >= 1000000   ) return 7;
	if (i >= 100000    ) return 6;
	if (i >= 10000     ) return 5;
	if (i >= 1000      ) return 4;
	if (i >= 100       ) return 3;
	if (i >= 10        ) return 2;
	if (i >= 1         ) return 1;
	return 0;
}

/* FIXME */
dstr getipv4(void) {
	struct ifaddrs *ifaddr, *ifa;
	char host[NI_MAXHOST];
	dstr res = NULL;

	if (getifaddrs(&ifaddr) == -1)
		return NULL;
	for (ifa = ifaddr; ifa; ifa = ifa->ifa_next)
		if (ifa->ifa_addr->sa_family == AF_INET && strcmp(ifa->ifa_name, "lo"))
			break;
	if (ifa && getnameinfo(ifa->ifa_addr, sizeof (struct sockaddr_in), host, NI_MAXHOST,
		NULL, 0, NI_NUMERICHOST) == 0)
		res = dstr_new(host);
	freeifaddrs(ifaddr);
	return res;
}

/* FIXME */
int makedir(const char *path, mode_t mode) {
	char *p, *tmp = strdupa(path), *fullpath = alloca(strlen(path) + 1);
	int count = 0, pcount = 0, i;
	char **pieces;

	for (p = tmp; *p; ++p)
		if (*p == '/')
			++count;
	pieces = alloca(count * sizeof *pieces);
	for (p = tmp; *p; ++p)
		if (*p == '/') {
			*p = '\0';
			pieces[pcount++] = p + 1;
		}
	*fullpath = '\0';
	for (i = 0; i < pcount - 1; ++i) {
		strcat(fullpath, "/");
		strcat(fullpath, pieces[i]);
		if (mkdir(fullpath, mode) != 0 && errno != EEXIST)
			return errno;
	}
	return 0;
}


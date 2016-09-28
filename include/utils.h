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

#ifndef UTILS_INCLUDED
#define UTILS_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include <inttypes.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "dstr.h"

/* exported functions */
extern int      cmpstr(const void *x, const void *y);
extern unsigned hashpjw(const void *key);
extern unsigned hashdjb2(const void *key);
extern unsigned hashmurmur2(const void *key);
extern unsigned intlen(int32_t i);
extern dstr     getipv4(void);
extern int      makedir(const char *path, mode_t mode);

#ifdef __cplusplus
}
#endif

#endif /* UTILS_INCLUDED */


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

#include <stdio.h>
#include <time.h>
#include "macros.h"
#include "module.h"
#include "basics.h"

static char *app = "hello";
static char *desc = "Hello World";
static char *fmt = "HELLO,timestamp,contract,lastprice,bidprice1,askprice1";

static int hello_exec(void *data, void *data2) {
	RAII_VAR(struct msg *, msg, (struct msg *)data, msg_decr);
	Quote *quote = (Quote *)msg->data;
	time_t t = (time_t)quote->thyquote.m_nTime;
	struct tm lt;
	char datestr[64], res[512];
	NOT_USED(data2);

	strftime(datestr, sizeof datestr, "%F %T", localtime_r(&t, &lt));
	snprintf(res, sizeof res, "HELLO,%s.%03d,%s|%.2f,%.2f,%.2f",
		datestr,
		quote->m_nMSec,
		quote->thyquote.m_cHYDM,
		quote->thyquote.m_dZXJ,
		quote->thyquote.m_dMRJG1,
		quote->thyquote.m_dMCJG1);
	out2rmp(res);
	return 0;
}

static int load_module(void) {
	if (msgs_hook(&default_msgs, hello_exec, NULL) == -1)
		return MODULE_LOAD_FAILURE;
	return register_application(app, hello_exec, desc, fmt, mod_info->self);
}

static int unload_module(void) {
	msgs_unhook(&default_msgs, hello_exec);
	return unregister_application(app);
}

static int reload_module(void) {
	/* FIXME */
	return MODULE_LOAD_SUCCESS;
}

MODULE_INFO(load_module, unload_module, reload_module, "Hello World Application");


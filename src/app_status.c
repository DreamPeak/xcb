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
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/time.h>
#include <time.h>
#include "macros.h"
#include "mem.h"
#include "utils.h"
#include "logger.h"
#include "config.h"
#include "module.h"
#include "basics.h"
#include "ctp.h"

/* FIXME */
static char *app = "status";
static char *desc = "Instrument Status";
static ctp_tdspi_t *tdspi;
static ctp_tdapi_t *tdapi;
static struct config *cfg;
static const char *front_ip, *front_port, *brokerid, *userid, *passwd;
static struct timeval tv;
static int prev_hour = 0;

static inline void load_config(void) {
	/* FIXME */
	if ((cfg = config_load("/etc/xcb/status.conf"))) {
		char *cat = category_browse(cfg, NULL);

		while (cat) {
			if (!strcasecmp(cat, "general")) {
				struct variable *var = variable_browse(cfg, cat);

				while (var) {
					if (!strcasecmp(var->name, "front_ip")) {
						if (strcasecmp(var->value, ""))
							front_ip = var->value;
					} else if (!strcasecmp(var->name, "front_port")) {
						if (strcasecmp(var->value, ""))
							front_port = var->value;
					} else if (!strcasecmp(var->name, "brokerid")) {
						if (strcasecmp(var->value, ""))
							brokerid = var->value;
					} else if (!strcasecmp(var->name, "userid")) {
						if (strcasecmp(var->value, ""))
							userid = var->value;
					} else if (!strcasecmp(var->name, "passwd")) {
						if (strcasecmp(var->value, ""))
							passwd = var->value;
					} else
						xcb_log(XCB_LOG_WARNING, "Unknown variable '%s' in "
							"category '%s' of status.conf", var->name, cat);
					var = var->next;
				}
			}
			cat = category_browse(cfg, cat);
		}
	}
}

static void on_front_connected(void) {
	struct CThostFtdcReqUserLoginField req;
	int res;

	memset(&req, '\0', sizeof req);
	strncat(req.BrokerID, brokerid, sizeof req.BrokerID - 1);
	strncat(req.UserID, userid, sizeof req.UserID - 1);
	strncat(req.Password, passwd, sizeof req.Password - 1);
	res = ctp_tdapi_login_user(tdapi, &req, 1);
	xcb_log(XCB_LOG_NOTICE, "CTP version: '%s'", ctp_tdapi_get_version(tdapi));
	xcb_log(XCB_LOG_NOTICE, "Login %s for user '%s'", res == 0 ? "succeeded" : "failed", userid);
}

static void on_front_disconnected(int reason) {
	xcb_log(XCB_LOG_NOTICE, "Front disconnected: reason=%d", reason);
}

static void on_error(struct CThostFtdcRspInfoField *rspinfo, int rid, int islast) {
	if (rspinfo && rspinfo->ErrorID != 0)
		xcb_log(XCB_LOG_ERROR, "Error occurred: errorid=%d", rspinfo->ErrorID);
}

static void on_instrument_status(struct CThostFtdcInstrumentStatusField *status) {
	int time, hour;
	struct tm lt;
	char datestr[64], res[512];

	RMCHR(status->EnterTime, ':');
	time = atoi(status->EnterTime);
	hour = time / 10000;
	if (tv.tv_sec == 0 || hour == 9 || hour == 21)
		gettimeofday(&tv, NULL);
	if (prev_hour == 23 && hour == 0)
		tv.tv_sec += 24 * 60 * 60;
	prev_hour = hour;
	localtime_r(&tv.tv_sec, &lt);
	lt.tm_sec  = time % 100;
	lt.tm_min  = time / 100 % 100;
	lt.tm_hour = hour;
	strftime(datestr, sizeof datestr, "%F %T", &lt);
	snprintf(res, sizeof res, "STATUS,%s.000,%s|%c",
		datestr,
		status->InstrumentID,
		status->InstrumentStatus);
	out2rmp(res);
}

static int status_exec(void *data, void *data2) {
	RAII_VAR(struct msg *, msg, (struct msg *)data, msg_decr);
	NOT_USED(data2);

	/* do nothing */
	return 0;
}

static int load_module(void) {
	char path[256], front[1024];

	load_config();
	if (front_ip == NULL || front_port == NULL || brokerid == NULL || userid == NULL || passwd == NULL) {
		xcb_log(XCB_LOG_ERROR, "front_ip, front_port, brokerid, userid or passwd can't be empty");
		return MODULE_LOAD_FAILURE;
	}
	tdspi = ctp_tdspi_create();
	ctp_tdspi_on_front_connected(tdspi, on_front_connected);
	ctp_tdspi_on_front_disconnected(tdspi, on_front_disconnected);
	ctp_tdspi_on_error(tdspi, on_error);
	ctp_tdspi_on_instrument_status(tdspi, on_instrument_status);
	/* FIXME */
	snprintf(path, sizeof path, "/var/log/xcb/%s/", userid);
	makedir(path, 0777);
	tdapi = ctp_tdapi_create(path);
	ctp_tdapi_register_spi(tdapi, tdspi);
	/* FIXME */
	snprintf(front, sizeof front, "tcp://%s:%s", front_ip, front_port);
	ctp_tdapi_register_front(tdapi, front);
	ctp_tdapi_subscribe_public(tdapi, THOST_TERT_RESUME);
	ctp_tdapi_init(tdapi);
	return register_application(app, status_exec, desc, NULL, mod_info->self);
}

static int unload_module(void) {
	/* FIXME */
	ctp_tdapi_register_spi(tdapi, NULL);
	ctp_tdapi_destroy(tdapi);
	ctp_tdspi_destroy(tdspi);
	config_destroy(cfg);
	return unregister_application(app);
}

static int reload_module(void) {
	/* FIXME */
	return MODULE_LOAD_SUCCESS;
}

MODULE_INFO(load_module, unload_module, reload_module, "Instrument Status Application");


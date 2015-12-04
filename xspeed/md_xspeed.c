/*
 * Copyright (c) 2013-2015, Dalian Futures Information Technology Co., Ltd.
 *
 * Gaohang Wu
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
#include "macros.h"
#include "mem.h"
#include "dstr.h"
#include "logger.h"
#include "config.h"
#include "module.h"
#include "basics.h"
#include "xspeed.h"

static char *app = "xspeed";
static char *desc = "Xspeed API";
static xspeed_mdspi_t *mdspi;
static xspeed_mdapi_t *mdapi;
static struct config *cfg;
static const char *front_ip, *front_port, *userid, *passwd, *contracts;
static int reqid = 1;

static inline void load_config(void) {
	/* FIXME */
	if ((cfg = config_load("/etc/xcb/xspeed.conf"))) {
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
					} else if (!strcasecmp(var->name, "userid")) {
						if (strcasecmp(var->value, ""))
							userid = var->value;
					} else if (!strcasecmp(var->name, "passwd")) {
						if (strcasecmp(var->value, ""))
							passwd = var->value;
					} else if (!strcasecmp(var->name, "reqid")) {
						if (strcasecmp(var->value, ""))
							reqid = atoi(var->value);
					} else if (!strcasecmp(var->name, "contracts")) {
						if (strcasecmp(var->value, ""))
							contracts= var->value;
					} else
						xcb_log(XCB_LOG_WARNING, "Unknown variable '%s' in "
							"category '%s' of xspeed.conf", var->name, cat);
					var = var->next;
				}
			}
			cat = category_browse(cfg, cat);
		}
	}
}

static void on_front_connected(void) {
	struct DFITCUserLoginField req;
	int res;

	memset(&req, '\0', sizeof req);
	req.lRequestID = reqid;
	/* FIXME */
	strcpy(req.accountID, userid);
	strcpy(req.passwd, passwd);
	res = xspeed_mdapi_login_user(mdapi, &req);
	xcb_log(XCB_LOG_NOTICE, "Login %s for user '%s'", res == 0 ? "succeeded" : "failed", userid);
}

static void on_front_disconnected(int reason) {
	xcb_log(XCB_LOG_NOTICE, "Front disconnected: reason=%d", reason);
}

static void on_error(struct DFITCErrorRtnField *rspinfo) {
	if (rspinfo && rspinfo->nErrorID != 0)
		xcb_log(XCB_LOG_ERROR, "Error occurred: errorid=%d", rspinfo->nErrorID);
}

static void on_user_login(struct DFITCUserLoginInfoRtnField *userlogin,
	struct DFITCErrorRtnField *rspinfo) {
	if (rspinfo && rspinfo->nErrorID != 0)
		xcb_log(XCB_LOG_ERROR, "Error occurred: errorid=%d", rspinfo->nErrorID);
	else if (contracts) {
		dstr *fields = NULL;
		int nfield = 0, i;

		fields = dstr_split_len(contracts, strlen(contracts), ",", 1, &nfield);
		for (i = 0; i < nfield; ++i)
			if (xspeed_mdapi_subscribe_market_data(mdapi, &fields[i], 1, 0) == 0)
				xcb_log(XCB_LOG_INFO, "Subscribe contract '%s' succeeded", fields[i]);
			else
				xcb_log(XCB_LOG_INFO, "Subscribe contract '%s' failed", fields[i]);
		dstr_free_tokens(fields, nfield);
	}
}

static void on_subscribe_market_data(struct DFITCSpecificInstrumentField *instrument,
	struct DFITCErrorRtnField *rspinfo) {
	if (rspinfo && rspinfo->nErrorID!= 0)
		xcb_log(XCB_LOG_ERROR, "Error occurred: errorid=%d", rspinfo->nErrorID);
}

static void on_deep_market_data(struct DFITCDepthMarketDataField *deepmd) {
	Quote *quote;

	if (deepmd == NULL)
		return;
	if (NEW0(quote)) {
		/* FIXME */
		quote->thyquote.m_nLen   = sizeof (tHYQuote);
		RMCHR(deepmd->UpdateTime, ':');
		quote->thyquote.m_nTime  = atoi(deepmd->UpdateTime) * 1000 + deepmd->UpdateMillisec;
		strcpy(quote->thyquote.m_cHYDM, deepmd->instrumentID);
		quote->thyquote.m_dZJSJ  = deepmd->preSettlementPrice;
		quote->thyquote.m_dJJSJ  = deepmd->settlementPrice;
		quote->thyquote.m_dCJJJ  = deepmd->AveragePrice;
		quote->thyquote.m_dZSP   = deepmd->preClosePrice;
		quote->thyquote.m_dJSP   = deepmd->closePrice;
		quote->thyquote.m_dJKP   = deepmd->openPrice;
		quote->thyquote.m_nZCCL  = deepmd->preOpenInterest;
		quote->thyquote.m_nCCL   = deepmd->openInterest;
		quote->thyquote.m_dZXJ   = deepmd->lastPrice;
		quote->thyquote.m_nCJSL  = deepmd->Volume;
		quote->thyquote.m_dCJJE  = deepmd->turnover;
		quote->thyquote.m_dZGBJ  = deepmd->upperLimitPrice;
		quote->thyquote.m_dZDBJ  = deepmd->lowerLimitPrice;
		quote->thyquote.m_dZGJ   = deepmd->highestPrice;
		quote->thyquote.m_dZDJ   = deepmd->lowestPrice;
		quote->thyquote.m_dZXSD  = deepmd->preDelta;
		quote->thyquote.m_dJXSD  = deepmd->currDelta;
		quote->thyquote.m_dMRJG1 = deepmd->BidPrice1;
		quote->thyquote.m_dMCJG1 = deepmd->AskPrice1;
		quote->thyquote.m_nMRSL1 = deepmd->BidVolume1;
		quote->thyquote.m_nMCSL1 = deepmd->AskVolume1;
		process_quote(quote);
	} else
		xcb_log(XCB_LOG_WARNING, "Error allocating memory for quote");
}

static int xspeed_exec(void *data, void *data2) {
	RAII_VAR(struct msg *, msg, (struct msg *)data, msg_decr);

	/* do nothing */
	return 0;
}

static int load_module(void) {
	char front[1024];

	load_config();
	if (front_ip == NULL || front_port == NULL || userid == NULL || passwd == NULL) {
		xcb_log(XCB_LOG_ERROR, "front_ip, front_port, userid or passwd can't be empty");
		return MODULE_LOAD_FAILURE;
	}
	mdspi = xspeed_mdspi_create();
	xspeed_mdspi_on_front_connected(mdspi, on_front_connected);
	xspeed_mdspi_on_front_disconnected(mdspi, on_front_disconnected);
	xspeed_mdspi_on_error(mdspi, on_error);
	xspeed_mdspi_on_user_login(mdspi, on_user_login);
	xspeed_mdspi_on_subscribe_market_data(mdspi, on_subscribe_market_data);
	xspeed_mdspi_on_deep_market_data(mdspi, on_deep_market_data);
	mdapi = xspeed_mdapi_create();
	/* FIXME */
	snprintf(front, sizeof front, "tcp://%s:%s", front_ip, front_port);
	xspeed_mdapi_init(mdapi, front, mdspi);
	return register_application(app, xspeed_exec, desc, NULL, mod_info->self);
}

static int unload_module(void) {
	/* FIXME */
	xspeed_mdapi_destroy(mdapi);
	xspeed_mdspi_destroy(mdspi);
	config_destroy(cfg);
	return unregister_application(app);
}

static int reload_module(void) {
	/* FIXME */
	return MODULE_LOAD_SUCCESS;
}

MODULE_INFO(load_module, unload_module, reload_module, "Xspeed API Application");


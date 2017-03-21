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

#include <float.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include "macros.h"
#include "mem.h"
#include "dstr.h"
#include "utils.h"
#include "logger.h"
#include "config.h"
#include "module.h"
#include "basics.h"
#include "ctp.h"

/* FIXME */
static char *app = "ctp";
static char *desc = "Comprehensive Transaction Platform API";
static ctp_mdspi_t *mdspi;
static ctp_mdapi_t *mdapi;
static struct config *cfg;
static const char *front_ip, *front_port, *brokerid, *userid, *passwd;
static dstr contracts;

static inline void load_config(void) {
	/* FIXME */
	if ((cfg = config_load("/etc/xcb/ctp.conf"))) {
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
					} else if (!strcasecmp(var->name, "contracts")) {
						if (strcasecmp(var->value, "")) {
							if (dstr_length(contracts) > 0)
								contracts = dstr_cat(contracts, ",");
							contracts = dstr_cat(contracts, var->value);
						}
					} else
						xcb_log(XCB_LOG_WARNING, "Unknown variable '%s' in "
							"category '%s' of ctp.conf", var->name, cat);
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
	res = ctp_mdapi_login_user(mdapi, &req, 1);
	xcb_log(XCB_LOG_NOTICE, "CTP version: '%s'", ctp_mdapi_get_version(mdapi));
	xcb_log(XCB_LOG_NOTICE, "Login %s for user '%s'", res == 0 ? "succeeded" : "failed", userid);
}

static void on_front_disconnected(int reason) {
	xcb_log(XCB_LOG_NOTICE, "Front disconnected: reason=%d", reason);
}

static void on_error(struct CThostFtdcRspInfoField *rspinfo, int rid, int islast) {
	if (rspinfo && rspinfo->ErrorID != 0)
		xcb_log(XCB_LOG_ERROR, "Error occurred: errorid=%d", rspinfo->ErrorID);
}

static void on_user_login(struct CThostFtdcRspUserLoginField *userlogin,
	struct CThostFtdcRspInfoField *rspinfo, int rid, int islast) {
	/* FIXME */
	if (rspinfo && rspinfo->ErrorID != 0) {
		xcb_log(XCB_LOG_ERROR, "Error occurred: errorid=%d", rspinfo->ErrorID);
		return;
	}
	if (islast && contracts) {
		dstr *fields = NULL;
		int nfield = 0, i;

		fields = dstr_split_len(contracts, strlen(contracts), ",", 1, &nfield);
		for (i = 0; i < nfield; ++i)
			if (ctp_mdapi_subscribe_market_data(mdapi, &fields[i], 1) == 0)
				xcb_log(XCB_LOG_INFO, "Subscribe contract '%s' succeeded", fields[i]);
			else
				xcb_log(XCB_LOG_INFO, "Subscribe contract '%s' failed", fields[i]);
		dstr_free_tokens(fields, nfield);
	}
}

static void on_subscribe_market_data(struct CThostFtdcSpecificInstrumentField *instrument,
	struct CThostFtdcRspInfoField *rspinfo, int rid, int islast) {
	if (rspinfo && rspinfo->ErrorID != 0)
		xcb_log(XCB_LOG_ERROR, "Error occurred: errorid=%d", rspinfo->ErrorID);
}

static void on_deep_market_data(struct CThostFtdcDepthMarketDataField *deepmd) {
	Quote *quote;

	if (deepmd == NULL)
		return;
	if (NEW0(quote)) {
		/* FIXME */
		quote->thyquote.m_nLen   = sizeof (tHYQuote);
		RMCHR(deepmd->UpdateTime, ':');
		quote->thyquote.m_nTime  = atoi(deepmd->UpdateTime) * 1000 + deepmd->UpdateMillisec;
		strcpy(quote->thyquote.m_cJYS, deepmd->ExchangeID);
		strcpy(quote->thyquote.m_cHYDM, deepmd->InstrumentID);
		if (deepmd->PreSettlementPrice != DBL_MAX)
			quote->thyquote.m_dZJSJ  = deepmd->PreSettlementPrice;
		if (deepmd->SettlementPrice != DBL_MAX)
			quote->thyquote.m_dJJSJ  = deepmd->SettlementPrice;
		if (deepmd->AveragePrice != DBL_MAX)
			quote->thyquote.m_dCJJJ  = deepmd->AveragePrice;
		if (deepmd->PreClosePrice != DBL_MAX)
			quote->thyquote.m_dZSP   = deepmd->PreClosePrice;
		if (deepmd->ClosePrice != DBL_MAX)
			quote->thyquote.m_dJSP   = deepmd->ClosePrice;
		if (deepmd->OpenPrice != DBL_MAX)
			quote->thyquote.m_dJKP   = deepmd->OpenPrice;
		if (deepmd->PreOpenInterest != DBL_MAX)
			quote->thyquote.m_nZCCL  = deepmd->PreOpenInterest;
		if (deepmd->OpenInterest != DBL_MAX)
			quote->thyquote.m_nCCL   = deepmd->OpenInterest;
		if (deepmd->LastPrice != DBL_MAX)
			quote->thyquote.m_dZXJ   = deepmd->LastPrice;
		quote->thyquote.m_nCJSL  = deepmd->Volume;
		if (deepmd->Turnover != DBL_MAX)
			quote->thyquote.m_dCJJE  = deepmd->Turnover;
		if (deepmd->UpperLimitPrice != DBL_MAX)
			quote->thyquote.m_dZGBJ  = deepmd->UpperLimitPrice;
		if (deepmd->LowerLimitPrice != DBL_MAX)
			quote->thyquote.m_dZDBJ  = deepmd->LowerLimitPrice;
		if (deepmd->HighestPrice != DBL_MAX)
			quote->thyquote.m_dZGJ   = deepmd->HighestPrice;
		if (deepmd->LowestPrice != DBL_MAX)
			quote->thyquote.m_dZDJ   = deepmd->LowestPrice;
		if (deepmd->PreDelta != DBL_MAX)
			quote->thyquote.m_dZXSD  = deepmd->PreDelta;
		if (deepmd->CurrDelta != DBL_MAX)
			quote->thyquote.m_dJXSD  = deepmd->CurrDelta;
		if (deepmd->BidPrice1 != DBL_MAX)
			quote->thyquote.m_dMRJG1 = deepmd->BidPrice1;
		if (deepmd->AskPrice1 != DBL_MAX)
			quote->thyquote.m_dMCJG1 = deepmd->AskPrice1;
		quote->thyquote.m_nMRSL1 = deepmd->BidVolume1;
		quote->thyquote.m_nMCSL1 = deepmd->AskVolume1;
		process_quote(quote);
	} else
		xcb_log(XCB_LOG_WARNING, "Error allocating memory for quote");
}

static int ctp_exec(void *data, void *data2) {
	RAII_VAR(struct msg *, msg, (struct msg *)data, msg_decr);
	NOT_USED(data2);

	/* do nothing */
	return 0;
}

static int load_module(void) {
	char path[256], front[1024];

	contracts = dstr_new("");
	load_config();
	if (front_ip == NULL || front_port == NULL || brokerid == NULL || userid == NULL || passwd == NULL) {
		xcb_log(XCB_LOG_ERROR, "front_ip, front_port, brokerid, userid or passwd can't be empty");
		return MODULE_LOAD_FAILURE;
	}
	mdspi = ctp_mdspi_create();
	ctp_mdspi_on_front_connected(mdspi, on_front_connected);
	ctp_mdspi_on_front_disconnected(mdspi, on_front_disconnected);
	ctp_mdspi_on_error(mdspi, on_error);
	ctp_mdspi_on_user_login(mdspi, on_user_login);
	ctp_mdspi_on_subscribe_market_data(mdspi, on_subscribe_market_data);
	ctp_mdspi_on_deep_market_data(mdspi, on_deep_market_data);
	/* FIXME */
	snprintf(path, sizeof path, "/var/log/xcb/%s/", userid);
	makedir(path, 0777);
	mdapi = ctp_mdapi_create(path, 0, 0);
	ctp_mdapi_register_spi(mdapi, mdspi);
	/* FIXME */
	snprintf(front, sizeof front, "tcp://%s:%s", front_ip, front_port);
	ctp_mdapi_register_front(mdapi, front);
	ctp_mdapi_init(mdapi);
	return register_application(app, ctp_exec, desc, NULL, mod_info->self);
}

static int unload_module(void) {
	/* FIXME */
	ctp_mdapi_register_spi(mdapi, NULL);
	ctp_mdapi_destroy(mdapi);
	ctp_mdspi_destroy(mdspi);
	config_destroy(cfg);
	dstr_free(contracts);
	return unregister_application(app);
}

static int reload_module(void) {
	/* FIXME */
	return MODULE_LOAD_SUCCESS;
}

MODULE_INFO(load_module, unload_module, reload_module, "Comprehensive Transaction Platform API Application");


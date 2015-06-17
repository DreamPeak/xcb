/*
 * Copyright (c) 2013-2015, Dalian Futures Information Technology Co., Ltd.
 *
 * Bo Wang <futurewb at dce dot com dot cn>
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
#include "logger.h"
#include "config.h"
#include "module.h"
#include "utilities.h"
#include "basics.h"
#include "femas.h"

/* FIXME */
static char *app = "femas";
static char *desc = "FEMAS API";
static femas_mdspi_t *mdspi;
static femas_mdapi_t *mdapi;
static struct config *cfg;
static const char *front_ip, *front_port, *brokerid, *userid, *passwd, *contracts;
static const char *tradingday;

static inline void load_config(void) {
	/* FIXME */
	if ((cfg = config_load("/etc/xcb/femas.conf"))) {
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
						if (strcasecmp(var->value, ""))
							contracts = var->value;
					} else
						xcb_log(XCB_LOG_WARNING, "Unknown variable '%s' in "
							"category '%s' of femas.conf", var->name, cat);
					var = var->next;
				}
			}
			cat = category_browse(cfg, cat);
		}
	}
}

static void on_front_connected(void) {
	struct CUstpFtdcReqUserLoginField req;
	int res;

	memset(&req, 0, sizeof req);
	/* FIXME */
	strcpy(req.TradingDay, tradingday);
	xcb_log(XCB_LOG_WARNING, "trading day is %s", tradingday);
	strcpy(req.UserID, userid);
	strcpy(req.BrokerID, brokerid);
	if (passwd != NULL)
		strcpy(req.Password, passwd);
	strcpy(req.UserProductInfo, "demo v1.0");
	res = femas_mdapi_user_login(mdapi, &req, 0);
	xcb_log(XCB_LOG_NOTICE, "Login %s for user '%s'", res == 0 ? "succeeded" : "failed", userid);
}

static void on_front_disconnected(int reason) {
	xcb_log(XCB_LOG_NOTICE, "Front disconnected: reason=%d", reason);
}

static void on_user_login(struct CUstpFtdcRspUserLoginField *userlogin,
	struct CUstpFtdcRspInfoField *rspinfo, int rid, int islast) {
	/* FIXME */
	if (rspinfo && rspinfo->ErrorID != 0) {
		xcb_log(XCB_LOG_ERROR, "Error occurred: errorid=%d", rspinfo->ErrorID);
	} else if (islast && contracts) {
		dstr *fields = NULL;
		int nfield = 0, i;

		fields = dstr_split_len(contracts, strlen(contracts), ",", 1, &nfield);
		for (i = 0; i < nfield; ++i)
			if (femas_mdapi_subscribe_market_data(mdapi, &fields[i], 1) == 0)
				xcb_log(XCB_LOG_INFO, "Subscribe contract '%s' succeeded", fields[i]);
			else
				xcb_log(XCB_LOG_INFO, "Subscribe contract '%s' failed", fields[i]);
		dstr_free_tokens(fields, nfield);
	}
}

static void on_error(struct CUstpFtdcRspInfoField *rspinfo, int rid, int islast) {
	if (rspinfo && rspinfo->ErrorID != 0)
		xcb_log(XCB_LOG_ERROR, "Error occurred: errorid=%d", rspinfo->ErrorID);
}

static void on_subscribe_market_data(struct CUstpFtdcSpecificInstrumentField *instrument,
	struct CUstpFtdcRspInfoField *rspinfo, int rid, int islast) {
	if (rspinfo && rspinfo->ErrorID != 0)
		xcb_log(XCB_LOG_ERROR, "Error occurred: errorid=%d", rspinfo->ErrorID);
}

static void on_deep_market_data(struct CUstpFtdcDepthMarketDataField *deepmd) {
	Quote *quote;

	if (deepmd == NULL) {
		xcb_log(XCB_LOG_WARNING, "deepmd is NULL!");
		return;
	}
	if (NEW0(quote)) {
		/* FIXME */
		quote->thyquote.m_nLen	 = sizeof(tHYQuote);
		RMCHR(deepmd->UpdateTime, ':');
		quote->thyquote.m_nTime  = atoi(deepmd->UpdateTime) * 1000 + deepmd->UpdateMillisec;
		strcpy(quote->thyquote.m_cHYDM, deepmd->InstrumentID);
		quote->thyquote.m_dZJSJ  = deepmd->PreSettlementPrice;
		quote->thyquote.m_dJJSJ  = deepmd->SettlementPrice;
		quote->thyquote.m_dCJJJ  = 0;
		quote->thyquote.m_dZSP   = deepmd->PreClosePrice;
		quote->thyquote.m_dJSP   = deepmd->ClosePrice;
		quote->thyquote.m_dJKP   = deepmd->OpenPrice;
		quote->thyquote.m_nZCCL  = deepmd->PreOpenInterest;
		quote->thyquote.m_nCCL   = deepmd->OpenInterest;
		quote->thyquote.m_dZXJ   = deepmd->LastPrice;
		quote->thyquote.m_nCJSL  = deepmd->Volume;
		quote->thyquote.m_dCJJE  = deepmd->Turnover;
		quote->thyquote.m_dZGBJ  = deepmd->UpperLimitPrice;
		quote->thyquote.m_dZDBJ  = deepmd->LowerLimitPrice;
		quote->thyquote.m_dZGJ   = deepmd->HighestPrice;
		quote->thyquote.m_dZDJ   = deepmd->LowestPrice;
		quote->thyquote.m_dZXSD  = deepmd->PreDelta;
		quote->thyquote.m_dJXSD  = deepmd->CurrDelta;
		quote->thyquote.m_dMRJG1 = deepmd->BidPrice1;
		quote->thyquote.m_dMCJG1 = deepmd->AskPrice1;
		quote->thyquote.m_nMRSL1 = deepmd->BidVolume1;
		quote->thyquote.m_nMCSL1 = deepmd->AskVolume1;
		quote->thyquote.m_dMRJG2 = deepmd->BidPrice2;
		quote->thyquote.m_dMCJG2 = deepmd->AskPrice2;
		quote->thyquote.m_nMRSL2 = deepmd->BidVolume2;
		quote->thyquote.m_nMCSL2 = deepmd->AskVolume2;
		quote->thyquote.m_dMRJG3 = deepmd->BidPrice3;
		quote->thyquote.m_dMCJG3 = deepmd->AskPrice3;
		quote->thyquote.m_nMRSL3 = deepmd->BidVolume3;
		quote->thyquote.m_nMCSL3 = deepmd->AskVolume3;
		quote->thyquote.m_dMRJG4 = deepmd->BidPrice4;
		quote->thyquote.m_dMCJG4 = deepmd->AskPrice4;
		quote->thyquote.m_nMRSL4 = deepmd->BidVolume4;
		quote->thyquote.m_nMCSL4 = deepmd->AskVolume4;
		quote->thyquote.m_dMRJG5 = deepmd->BidPrice5;
		quote->thyquote.m_dMCJG5 = deepmd->AskPrice5;
		quote->thyquote.m_nMRSL5 = deepmd->BidVolume5;
		quote->thyquote.m_nMCSL5 = deepmd->AskVolume5;
		process_quote(quote);
	} else
		xcb_log(XCB_LOG_WARNING, "Error allocating memory for quote");
}

static int femas_exec(void *data, void *data2) {
	RAII_VAR(struct msg *, msg, (struct msg *)data, msg_decr);

	/* do nothing */
	return 0;
}

static int load_module(void) {
	char path[256], front[1024];

	load_config();
	if (front_ip == NULL || front_port == NULL || userid == NULL) {
		xcb_log(XCB_LOG_ERROR, "front_ip, front_port, userid can't be empty");
		return MODULE_LOAD_FAILURE;
	}
	mdspi = femas_mdspi_create();
	femas_mdspi_on_front_connected(mdspi, on_front_connected);
	femas_mdspi_on_front_disconnected(mdspi, on_front_disconnected);
	femas_mdspi_on_user_login(mdspi, on_user_login);
	femas_mdspi_on_error(mdspi, on_error);
	femas_mdspi_on_subscribe_market_data(mdspi, on_subscribe_market_data);
	femas_mdspi_on_deep_market_data(mdspi, on_deep_market_data);
	/* FIXME */
	snprintf(path, sizeof path, "/var/log/xcb/%s/", userid);
	makedir(path, 0777);
	mdapi = femas_mdapi_create(path);
	femas_mdapi_register_spi(mdapi, mdspi);
	/* FIXME */
	snprintf(front, sizeof front, "tcp://%s:%s", front_ip, front_port);
	femas_mdapi_register_front(mdapi, front);
	femas_mdapi_init(mdapi);
	tradingday = femas_mdapi_get_tradingday(mdapi);
	return register_application(app, femas_exec, desc, NULL, mod_info->self);
}

static int unload_module(void) {
	/* FIXME */
	femas_mdapi_register_spi(mdapi, NULL);
	femas_mdapi_destroy(mdapi);
	femas_mdspi_destroy(mdspi);
	config_destroy(cfg);
	return unregister_application(app);
}

static int reload_module(void) {
	/* FIXME */
	return MODULE_LOAD_SUCCESS;
}

MODULE_INFO(load_module, unload_module, reload_module, "FEMAS API Application");


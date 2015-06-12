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
#include "xspeed2.h"

/* FIXME */
static char *app = "xspeed2";
static char *desc = "XSpeed Level2 API";
static xspeed_l2spi_t *l2spi;
static xspeed_l2api_t *l2api;
static struct config *cfg;
static const char *front_ip, *front_port, *userid, *passwd, *reqid = "1", *contracts;

static inline void load_config(void) {
	/* FIXME */
	if ((cfg = config_load("/etc/xcb/xspeed2.conf"))) {
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
							reqid = var->value;
					} else if (!strcasecmp(var->name, "contracts")) {
						if (strcasecmp(var->value, ""))
							contracts = var->value;
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
	struct DFITCUserLoginField req;
	int res;

	memset(&req, 0, sizeof req);
	strcpy(req.accountID, userid);
	strcpy(req.passwd, passwd);
	req.lRequestID = atoi(reqid);
	res = xspeed_l2api_user_login(l2api, &req);
	xcb_log(XCB_LOG_NOTICE, "Login %s for user '%s'", res == 0 ? "succeeded" : "failed", userid);
}

static void on_front_disconnected(int reason) {
	xcb_log(XCB_LOG_NOTICE, "Front disconnected: reason='%d'", reason);
}

static void on_user_login(struct ErrorRtnField *rspinfo) {
	/* FIXME */
	if (rspinfo && rspinfo->ErrorID != 0) {
		xcb_log(XCB_LOG_ERROR, "Error occurred: errorid=%d", rspinfo->ErrorID);
		return;
	}
	if (contracts) {
		dstr *fields = NULL;
		int nfield = 0, i;

		fields = dstr_split_len(contracts, strlen(contracts), ",", 1, &nfield);
		for (i = 0; i < nfield; ++i)
			if ((xspeed_l2api_subscribe_market_data(l2api, &fields[i], 1)) == 0)
				xcb_log(XCB_LOG_INFO, "Subscribe contract '%s' succeeded", fields[i]);
			else
				xcb_log(XCB_LOG_INFO, "Subscribe contract '%s' failed", fields[i]);
		dstr_free_tokens(fields, nfield);
	} else {
		if ((xspeed_l2api_subscribe_all(l2api)) == 0)
			xcb_log(XCB_LOG_INFO, "Subscribe all succeeded");
		else
			xcb_log(XCB_LOG_INFO, "Subscribe all failed");
	}
}

static void on_subscribe_market_data(struct ErrorRtnField *rspinfo) {
	if (rspinfo && rspinfo->ErrorID != 0)
		xcb_log(XCB_LOG_ERROR, "Error occurred: errorid=%d", rspinfo->ErrorID);
}

static void on_subscribe_all(struct ErrorRtnField *rspinfo) {
	if (rspinfo && rspinfo->ErrorID != 0)
		xcb_log(XCB_LOG_ERROR, "Error occurred: errorid=%d", rspinfo->ErrorID);
}

static void on_best_and_deep(struct MDBestAndDeep *deepmd) {
	Quote *quote;

	if (deepmd == NULL) {
		xcb_log(XCB_LOG_WARNING, "deepmd is NULL!");
		return;
	}
	if (NEW0(quote)) {
		/* FIXME */
		quote->thyquote.m_nLen   = sizeof(tHYQuote);
		quote->thyquote.m_nTime  = atoi(deepmd->GenTime);
		strcpy(quote->thyquote.m_cJYS, deepmd->Exchange);
		strcpy(quote->thyquote.m_cHYDM, deepmd->Contract);
		quote->thyquote.m_bTPBZ  = deepmd->SuspensionSign;
		quote->thyquote.m_dZJSJ  = deepmd->LastClearPrice;
		quote->thyquote.m_dJJSJ  = deepmd->ClearPrice;
		quote->thyquote.m_dCJJJ  = deepmd->AvgPrice;
		quote->thyquote.m_dZSP   = deepmd->LastClose;
		quote->thyquote.m_dJSP   = deepmd->Close;
		quote->thyquote.m_dJKP   = deepmd->OpenPrice;
		quote->thyquote.m_nZCCL  = deepmd->LastOpenInterest;
		quote->thyquote.m_nCCL   = deepmd->OpenInterest;
		quote->thyquote.m_dZXJ   = deepmd->LastPrice;
		quote->thyquote.m_nCJSL  = deepmd->MatchTotQty;
		quote->thyquote.m_dCJJE  = deepmd->Turnover;
		quote->thyquote.m_dZGBJ  = deepmd->RiseLimit;
		quote->thyquote.m_dZDBJ  = deepmd->FallLimit;
		quote->thyquote.m_dZGJ   = deepmd->HighPrice;
		quote->thyquote.m_dZDJ   = deepmd->LowPrice;
		quote->thyquote.m_dZXSD  = deepmd->PreDelta;
		quote->thyquote.m_dJXSD  = deepmd->CurrDelta;
		quote->thyquote.m_dMRJG1 = deepmd->BuyPriceOne;
		quote->thyquote.m_dMCJG1 = deepmd->SellPriceOne;
		quote->thyquote.m_nMRSL1 = deepmd->BuyQtyOne;
		quote->thyquote.m_nMCSL1 = deepmd->SellQtyOne;
		quote->thyquote.m_dMRJG2 = deepmd->BuyPriceTwo;
		quote->thyquote.m_dMCJG2 = deepmd->SellPriceTwo;
		quote->thyquote.m_nMRSL2 = deepmd->BuyQtyTwo;
		quote->thyquote.m_nMCSL2 = deepmd->SellQtyTwo;
		quote->thyquote.m_dMRJG3 = deepmd->BuyPriceThree;
		quote->thyquote.m_dMCJG3 = deepmd->SellPriceThree;
		quote->thyquote.m_nMRSL3 = deepmd->BuyQtyThree;
		quote->thyquote.m_nMCSL3 = deepmd->SellQtyThree;
		quote->thyquote.m_dMRJG4 = deepmd->BuyPriceFour;
		quote->thyquote.m_dMCJG4 = deepmd->SellPriceFour;
		quote->thyquote.m_nMRSL4 = deepmd->BuyQtyFour;
		quote->thyquote.m_nMCSL4 = deepmd->SellQtyFour;
		quote->thyquote.m_dMRJG5 = deepmd->BuyPriceFive;
		quote->thyquote.m_dMCJG5 = deepmd->SellPriceFive;
		quote->thyquote.m_nMRSL5 = deepmd->BuyQtyFive;
		quote->thyquote.m_nMCSL5 = deepmd->SellQtyFive;
		process_quote(quote);
	} else
		xcb_log(XCB_LOG_WARNING, "Error allocating memory for quote");
}

static int xspeed2_exec(void *data, void *data2) {
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
	l2spi = xspeed_l2spi_create();
	xspeed_l2spi_on_front_connected(l2spi, on_front_connected);
	xspeed_l2spi_on_front_disconnected(l2spi, on_front_disconnected);
	xspeed_l2spi_on_user_login(l2spi, on_user_login);
	xspeed_l2spi_on_subscribe_all(l2spi, on_subscribe_all);
	xspeed_l2spi_on_subscribe_market_data(l2spi, on_subscribe_market_data);
	xspeed_l2spi_on_best_and_deep(l2spi, on_best_and_deep);
	l2api = xspeed_l2api_create();
	/* FIXME */
	snprintf(front, sizeof front, "tcp://%s:%s", front_ip, front_port);
	/* FIXME */
	xspeed_l2api_connect(l2api, front, l2spi, 0);
	return register_application(app, xspeed2_exec, desc, NULL, mod_info->self);
}

static int unload_module(void) {
	/* FIXME */
	xspeed_l2api_connect(l2api, NULL, NULL, 0);
	xspeed_l2api_destory(l2api);
	xspeed_l2spi_destroy(l2spi);
	config_destroy(cfg);
	return unregister_application(app);
}

static int reload_module(void) {
	/* FIXME */
	return MODULE_LOAD_SUCCESS;
}

MODULE_INFO(load_module, unload_module, reload_module, "XSpeed Level2 API application");


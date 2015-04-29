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
#include "sec.h"

/* FIXME */
static char *app = "sec";
static char *desc = "Security level1 API";
static sec_mdspi_t *mdspi;
static sec_mdapi_t *mdapi;
static struct config *cfg;
static const char *front_ip, *front_port, *userid, *passwd, *reqid, *contracts;

static inline void load_config(void) {
	/* FIXME */
	if ((cfg = config_load("/etc/xcb/sec.conf"))) {
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
							"category '%s' of sec.conf", var->name, cat);
					var = var->next;
				}
			}
			cat = category_browse(cfg, cat);
		}
	}	
}

static void on_front_connected(void) {
	struct DFITCSECReqUserLoginField req;
	int res;

	memset(&req, 0, sizeof req);
	/* FIXME */
	strcpy(req.accountID, userid);
	strcpy(req.passWord, passwd);
	req.requestID = atoi(reqid);
	res = sec_mdapi_user_login(mdapi, &req);
	xcb_log(XCB_LOG_NOTICE, "Login %s for user '%s'", res == 0 ? "succeeded" : "failed", userid);
}

static void on_front_disconnected(int reason) {
	xcb_log(XCB_LOG_NOTICE, "Front disconnected: reason = '%d'", reason);
}

static void on_user_login(struct DFITCSECRspUserLoginField *userlogin, struct DFITCSECRspInfoField *rspinfo) {
	/* FIXME */
	if (rspinfo == NULL)
		if (contracts) {
			dstr *fields = NULL;
			int nfield = 0, i, requset;

			requset = atoi(reqid);
			fields = dstr_split_len(contracts, strlen(contracts), ",", 1, &nfield);
			for (i = 0; i < nfield; ++i)
				if (sec_mdapi_subscribe_market_data(mdapi, &fields[i], 1, requset) == 0)
					xcb_log(XCB_LOG_INFO, "Subscribe contract '%s' succeeded", fields[i]);
				else
					xcb_log(XCB_LOG_INFO, "Subscribe contract '%s' failed", fields[i]);
			dstr_free_tokens(fields, nfield);
		}
}

static void on_deep_market_data(struct DFITCStockDepthMarketDataField *deepmd) {
	Quote *quote;
	char contract[256];

	if (deepmd == NULL)
		return;
	if (NEW0(quote)) {
		/* FIXME */
		quote->thyquote.m_nLen   = sizeof (THYQuote);
		quote->thyquote.m_nTime  = atoi(deepmd->sharedDataField.updateTime);
		strcpy(quote->thyquote.m_cJYS, deepmd->staticDataField.exchangeID);
		strcpy(contract, deepmd->staticDataField.exchangeID);
		strcat(contract, deepmd->staticDataField.securityID);
		strncpy(quote->thyquote.m_cHYDM, contract, sizeof quote->thyquote.m_cHYDM - 1);
		quote->thyquote.m_bTPBZ  = 0;
		quote->thyquote.m_dZJSJ  = 0;
		quote->thyquote.m_dJJSJ  = 0;
		quote->thyquote.m_dCJJJ  = 0;
		quote->thyquote.m_dZSP   = deepmd->staticDataField.preClosePrice;
		quote->thyquote.m_dJSP   = 0;
		quote->thyquote.m_dJKP   = deepmd->staticDataField.openPrice;
		quote->thyquote.m_nZCCL  = 0;
		quote->thyquote.m_nCCL   = 0;
		quote->thyquote.m_dZXJ   = deepmd->sharedDataField.latestPrice;
		quote->thyquote.m_nCJSL  = deepmd->sharedDataField.tradeQty;
		quote->thyquote.m_dCJJE  = deepmd->sharedDataField.turnover;
		quote->thyquote.m_dZGBJ  = 0;
		quote->thyquote.m_dZDBJ  = 0;
		quote->thyquote.m_dZGJ   = deepmd->sharedDataField.highestPrice;
		quote->thyquote.m_dZDJ   = deepmd->sharedDataField.lowestPrice;
		quote->thyquote.m_dZXSD  = 0;
		quote->thyquote.m_dJXSD  = 0;
		quote->thyquote.m_dMRJG1 = deepmd->sharedDataField.bidPrice1;
		quote->thyquote.m_dMCJG1 = deepmd->sharedDataField.askPrice1;
		quote->thyquote.m_nMRSL1 = deepmd->sharedDataField.bidQty1;
		quote->thyquote.m_nMCSL1 = deepmd->sharedDataField.askQty1;
		quote->thyquote.m_dMRJG2 = deepmd->sharedDataField.bidPrice2;
		quote->thyquote.m_dMCJG2 = deepmd->sharedDataField.askPrice2;
		quote->thyquote.m_nMRSL2 = deepmd->sharedDataField.bidQty2;
		quote->thyquote.m_nMCSL2 = deepmd->sharedDataField.askQty2;
		quote->thyquote.m_dMRJG3 = deepmd->sharedDataField.bidPrice3;
		quote->thyquote.m_dMCJG3 = deepmd->sharedDataField.askPrice3;
		quote->thyquote.m_nMRSL3 = deepmd->sharedDataField.bidQty3;
		quote->thyquote.m_nMCSL3 = deepmd->sharedDataField.askQty3;
		quote->thyquote.m_dMRJG4 = deepmd->sharedDataField.bidPrice4;
		quote->thyquote.m_dMCJG4 = deepmd->sharedDataField.askPrice4;
		quote->thyquote.m_nMRSL4 = deepmd->sharedDataField.bidQty4;
		quote->thyquote.m_nMCSL4 = deepmd->sharedDataField.askQty4;
		quote->thyquote.m_dMRJG5 = deepmd->sharedDataField.bidPrice5;
		quote->thyquote.m_dMCJG5 = deepmd->sharedDataField.askPrice5;
		quote->thyquote.m_nMRSL5 = deepmd->sharedDataField.bidQty5;
		quote->thyquote.m_nMCSL5 = deepmd->sharedDataField.askQty5;
		process_quote(quote);
	} else
		xcb_log(XCB_LOG_WARNING, "Error allocating memory for quote");
}

static int sec_exec(void *data, void *data2) {
	RAII_VAR(struct msg *, msg, (struct msg *)data, msg_decr);

	/* do nothing */
	return 0;
}

static int load_module(void) {
	char front[1024];

	load_config();
	if (front_ip == NULL || front_port == NULL || userid == NULL || passwd == NULL) {
		xcb_log(XCB_LOG_ERROR, "protocol, front_ip, front_port, userid or passwd can't be empty");
		return MODULE_LOAD_FAILURE;
	}
	mdspi = sec_mdspi_create();
	sec_mdspi_on_front_connected(mdspi, on_front_connected);
	sec_mdspi_on_front_disconnected(mdspi, on_front_disconnected);
	sec_mdspi_on_user_login(mdspi, on_user_login);
	sec_mdspi_on_deep_market_data(mdspi, on_deep_market_data);
	mdapi = sec_mdapi_create();
	/* FIXME */
	snprintf(front, sizeof front, "tcp://%s:%s", front_ip, front_port);
	sec_mdapi_init(mdapi, front, mdspi);
	return register_application(app, sec_exec, desc, NULL, mod_info->self);
}

static int unload_module(void) {
	/* FIXME */
	sec_mdapi_release(mdapi);
	sec_mdspi_destroy(mdspi);
	config_destroy(cfg);
	return unregister_application(app);
}

static int reload_module(void) {
	/* FIXME */
	return MODULE_LOAD_SUCCESS;
}

MODULE_INFO(load_module, unload_module, reload_module, "Security level1 API");


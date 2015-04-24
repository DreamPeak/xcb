/*
 * Copyright (c) 2013-2015, Dalian Futures Information Technology Co., Ltd.
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
#include "mem.h"
#include "dstr.h"
#include "logger.h"
#include "config.h"
#include "module.h"
#include "utilities.h"
#include "basics.h"
#include "ctp.h"

/* FIXME */
static char *app = "ctp";
static char *desc = "Comprehensive Transaction Platform API";
static ctp_mdspi_t *mdspi;
static ctp_mdapi_t *mdapi;
static struct config *cfg;
static const char *front_ip, *front_port, *brokerid, *userid, *passwd, *contracts;

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
	struct CThostFtdcReqUserLoginField req;
	int res;

	memset(&req, 0, sizeof req);
	/* FIXME */
	strcpy(req.BrokerID, brokerid);
	strcpy(req.UserID, userid);
	strcpy(req.Password, passwd);
	res = ctp_mdapi_login_user(mdapi, &req, 1);
	xcb_log(XCB_LOG_NOTICE, "User '%s' login %s", userid, res == 0 ? "success" : "failure");
}

static void on_user_login(struct CThostFtdcRspUserLoginField *userlogin,
	struct CThostFtdcRspInfoField *rspinfo, int rid, int islast) {
	if (islast && rspinfo && rspinfo->ErrorID != 0 && contracts) {
		dstr *fields = NULL;
		int nfield = 0, i;

		fields = dstr_split_len(contracts, strlen(contracts), ",", 1, &nfield);
		for (i = 0; i < nfield; ++i)
			if (ctp_mdapi_subscribe_market_data(mdapi, &fields[i], 1) == 0)
				xcb_log(XCB_LOG_INFO, "Subscribe '%s' success", fields[i]);
			else
				xcb_log(XCB_LOG_INFO, "Subscribe '%s' failure", fields[i]);
		dstr_free_tokens(fields, nfield);
	}
}

static void on_deep_market_data(struct CThostFtdcDepthMarketDataField *deepmd) {
	Quote *quote;

	if (deepmd == NULL)
		return;
	if (NEW0(quote)) {
		/* FIXME */
		quote->thyquote.m_nLen   = sizeof (THYQuote);
		quote->thyquote.m_nTime  = atoi(deepmd->UpdateTime);
		if (!strcmp(deepmd->ExchangeID, "SHFE"))
			strcpy(quote->thyquote.m_cJYS, "SQ");
		else if (!strcmp(deepmd->ExchangeID, "DCE"))
			strcpy(quote->thyquote.m_cJYS, "DL");
		else if (!strcmp(deepmd->ExchangeID, "CZCE"))
			strcpy(quote->thyquote.m_cJYS, "ZZ");
		else if (!strcmp(deepmd->ExchangeID, "CFFEX"))
			strcpy(quote->thyquote.m_cJYS, "ZJ");
		strcpy(quote->thyquote.m_cHYDM, deepmd->InstrumentID);
		quote->thyquote.m_dZJSJ  = deepmd->PreSettlementPrice != DBL_MAX
						? deepmd->PreSettlementPrice : 0;
		quote->thyquote.m_dJJSJ  = deepmd->SettlementPrice != DBL_MAX ? deepmd->SettlementPrice : 0;
		quote->thyquote.m_dCJJJ  = deepmd->AveragePrice != DBL_MAX ? deepmd->AveragePrice : 0;
		quote->thyquote.m_dZSP   = deepmd->PreClosePrice != DBL_MAX ? deepmd->PreClosePrice : 0;
		quote->thyquote.m_dJSP   = deepmd->ClosePrice != DBL_MAX ? deepmd->ClosePrice : 0;
		quote->thyquote.m_dJKP   = deepmd->OpenPrice != DBL_MAX ? deepmd->OpenPrice : 0;
		quote->thyquote.m_nZCCL  = deepmd->PreOpenInterest != DBL_MAX ? deepmd->PreOpenInterest : 0;
		quote->thyquote.m_nCCL   = deepmd->OpenInterest != DBL_MAX ? deepmd->OpenInterest : 0;
		quote->thyquote.m_dZXJ   = deepmd->LastPrice != DBL_MAX ? deepmd->LastPrice : 0;
		quote->thyquote.m_nCJSL  = deepmd->Volume;
		quote->thyquote.m_dCJJE  = deepmd->Turnover != DBL_MAX ? deepmd->Turnover : 0;
		quote->thyquote.m_dZGBJ  = deepmd->UpperLimitPrice != DBL_MAX ? deepmd->UpperLimitPrice : 0;
		quote->thyquote.m_dZDBJ  = deepmd->LowerLimitPrice != DBL_MAX ? deepmd->LowerLimitPrice : 0;
		quote->thyquote.m_dZGJ   = deepmd->HighestPrice != DBL_MAX ? deepmd->HighestPrice : 0;
		quote->thyquote.m_dZDJ   = deepmd->LowestPrice != DBL_MAX ? deepmd->LowestPrice : 0;
		quote->thyquote.m_dZXSD  = deepmd->PreDelta != DBL_MAX ? deepmd->PreDelta : 0;
		quote->thyquote.m_dJXSD  = deepmd->CurrDelta != DBL_MAX ? deepmd->CurrDelta : 0;
		quote->thyquote.m_dMRJG1 = deepmd->BidPrice1 != DBL_MAX ? deepmd->BidPrice1 : 0;
		quote->thyquote.m_dMCJG1 = deepmd->AskPrice1 != DBL_MAX ? deepmd->AskPrice1 : 0;
		quote->thyquote.m_nMRSL1 = deepmd->BidVolume1;
		quote->thyquote.m_nMCSL1 = deepmd->AskVolume1;
		quote->m_nMSec           = deepmd->UpdateMillisec;
		put_quote(quote);
	}
}

static int ctp_exec(void *data, void *data2) {
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
	mdspi = ctp_mdspi_create();
	ctp_mdspi_on_front_connected(mdspi, on_front_connected);
	ctp_mdspi_on_user_login(mdspi, on_user_login);
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
	return unregister_application(app);
}

static int reload_module(void) {
	/* FIXME */
	return MODULE_LOAD_SUCCESS;
}

MODULE_INFO(load_module, unload_module, reload_module, "Comprehensive Transaction Platform API Application");


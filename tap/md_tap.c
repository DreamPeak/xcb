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

#include <ctype.h>
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
#include "tap.h"

#define DEFAULT_AUTHCODE     ("67EA896065459BECDFDB924B29CB7DF1946CED32E26C1EAC946CED\
32E26C1EAC946CED32E26C1EAC946CED32E26C1EAC5211AF9FEE541DDE41BCBAB68D525B0D111A0884D84\
7D57163FF7F329FA574E7946CED32E26C1EAC946CED32E26C1EAC733827B0CE853869ABD9B8F170E14F88\
47D3EA0BF4E191F5D97B3DFE4CCB1F01842DD2B3EA2F4B20CAD19B8347719B7E20EA1FA7A3D1BFEFF2229\
0F4B5C43E6C520ED5A40EC1D50ACDF342F46A92CCF87AEE6D73542C42EC17818349C7DEDAB0E4DB169777\
14F873D505029E27B3D57EB92D5BEDA0A710197EB67F94BB1892B30F58A3F211D9C3B3839BE2D73FD08DD\
776B9188654853DDA57675EBB7D6FBBFC")

/* FIXME */
static char *app = "tap";
static char *desc = "Trading Analysis Platform API";
static tap_mdspi_t *mdspi;
static tap_mdapi_t *mdapi;
static struct config *cfg;
static const char *front_ip, *front_port, *userid, *passwd, *contracts;

static inline void load_config(void) {
	/* FIXME */
	if ((cfg = config_load("/etc/xcb/tap.conf"))) {
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
					} else if (!strcasecmp(var->name, "userid"))
						userid = var->value;
					else if (!strcasecmp(var->name, "passwd"))
						passwd = var->value;
					else if (!strcasecmp(var->name, "contracts")) {
						if (strcasecmp(var->value, ""))
							contracts = var->value;
					} else
						xcb_log(XCB_LOG_WARNING, "Unknown variable '%s' in "
							"category '%s' of tap.conf", var->name, cat);
					var = var->next;
				}
			}
			cat = category_browse(cfg, cat);
		}
	}
}

static void on_login(TAPIINT32 error, const struct TapAPIQuotLoginRspInfo *info) {
	xcb_log(XCB_LOG_NOTICE, "Login %s for user '%s'",
		error == TAPIERROR_SUCCEED ? "succeeded" : "failed", userid);
}

static void on_api_ready(void) {
	if (contracts) {
		dstr *fields = NULL;
		int nfield = 0, i;

		fields = dstr_split_len(contracts, strlen(contracts), ",", 1, &nfield);
		for (i = 0; i < nfield; ++i) {
			TAPIUINT32 sid = 0;
			struct TapAPIContract contract;
			char *p, *q;

			memset(&contract, '\0', sizeof contract);
			/* FIXME */
			strcpy(contract.Commodity.ExchangeNo, "ZCE");
			contract.Commodity.CommodityType = TAPI_COMMODITY_TYPE_FUTURES;
			p = q = fields[i];
			while (!isdigit(*q))
				++q;
			strncpy(contract.Commodity.CommodityNo, p, q - p);
			strcpy(contract.ContractNo1, q);
			contract.CallOrPutFlag1 = TAPI_CALLPUT_FLAG_NONE;
			contract.CallOrPutFlag2 = TAPI_CALLPUT_FLAG_NONE;
			tap_mdapi_subscribe_quote(mdapi, &sid, &contract);
		}
		dstr_free_tokens(fields, nfield);
	}
}

static void on_subscribe_quote(TAPIUINT32 sid, TAPIINT32 error, TAPIYNFLAG islast,
	const struct TapAPIQuoteWhole *info) {
	if (error == TAPIERROR_SUCCEED) {
		Quote *quote;

		if (info == NULL)
			return;
		xcb_log(XCB_LOG_INFO, "Subscribe contract '%s%s' succeeded",
			info->Contract.Commodity.CommodityNo, info->Contract.ContractNo1);
		if (NEW0(quote)) {
			/* FIXME */
			char *time = (char *)info->DateTimeStamp + 11;

			quote->thyquote.m_nLen   = sizeof (tHYQuote);
			RMCHR(time, ':');
			RMCHR(time, '.');
			quote->thyquote.m_nTime  = atoi(time);
			strcpy(quote->thyquote.m_cHYDM, info->Contract.Commodity.CommodityNo);
			strcat(quote->thyquote.m_cHYDM, info->Contract.ContractNo1);
			quote->thyquote.m_dZJSJ  = info->QPreSettlePrice;
			quote->thyquote.m_dJJSJ  = info->QSettlePrice;
			quote->thyquote.m_dCJJJ  = info->QAveragePrice;
			quote->thyquote.m_dZSP   = info->QPreClosingPrice;
			quote->thyquote.m_dJSP   = info->QClosingPrice;
			quote->thyquote.m_dJKP   = info->QOpeningPrice;
			quote->thyquote.m_nZCCL  = info->QPrePositionQty;
			quote->thyquote.m_nCCL   = info->QPositionQty;
			quote->thyquote.m_dZXJ   = info->QLastPrice;
			quote->thyquote.m_dCJJE  = info->QTotalTurnover;
			quote->thyquote.m_dZGBJ  = info->QLimitUpPrice;
			quote->thyquote.m_dZDBJ  = info->QLimitDownPrice;
			quote->thyquote.m_dZGJ   = info->QHighPrice;
			quote->thyquote.m_dZDJ   = info->QLowPrice;
			quote->thyquote.m_dZXSD  = info->QPreDelta;
			quote->thyquote.m_dJXSD  = info->QCurrDelta;
			quote->thyquote.m_dMRJG1 = info->QBidPrice[0];
			quote->thyquote.m_dMCJG1 = info->QAskPrice[0];
			quote->thyquote.m_nMRSL1 = info->QBidQty[0];
			quote->thyquote.m_nMCSL1 = info->QAskQty[0];
			process_quote(quote);
		} else
			xcb_log(XCB_LOG_WARNING, "Error allocating memory for quote");
	} else if (info)
		xcb_log(XCB_LOG_INFO, "Subscribe contract '%s%s' failed",
			info->Contract.Commodity.CommodityNo, info->Contract.ContractNo1);
}

static void on_quote(const struct TapAPIQuoteWhole *info) {
	Quote *quote;

	if (info == NULL)
		return;
	if (NEW0(quote)) {
		/* FIXME */
		char *time = (char *)info->DateTimeStamp + 11;

		quote->thyquote.m_nLen   = sizeof (tHYQuote);
		RMCHR(time, ':');
		RMCHR(time, '.');
		quote->thyquote.m_nTime  = atoi(time);
		strcpy(quote->thyquote.m_cHYDM, info->Contract.Commodity.CommodityNo);
		strcat(quote->thyquote.m_cHYDM, info->Contract.ContractNo1);
		quote->thyquote.m_dZJSJ  = info->QPreSettlePrice;
		quote->thyquote.m_dJJSJ  = info->QSettlePrice;
		quote->thyquote.m_dCJJJ  = info->QAveragePrice;
		quote->thyquote.m_dZSP   = info->QPreClosingPrice;
		quote->thyquote.m_dJSP   = info->QClosingPrice;
		quote->thyquote.m_dJKP   = info->QOpeningPrice;
		quote->thyquote.m_nZCCL  = info->QPrePositionQty;
		quote->thyquote.m_nCCL   = info->QPositionQty;
		quote->thyquote.m_dZXJ   = info->QLastPrice;
		quote->thyquote.m_dCJJE  = info->QTotalTurnover;
		quote->thyquote.m_dZGBJ  = info->QLimitUpPrice;
		quote->thyquote.m_dZDBJ  = info->QLimitDownPrice;
		quote->thyquote.m_dZGJ   = info->QHighPrice;
		quote->thyquote.m_dZDJ   = info->QLowPrice;
		quote->thyquote.m_dZXSD  = info->QPreDelta;
		quote->thyquote.m_dJXSD  = info->QCurrDelta;
		quote->thyquote.m_dMRJG1 = info->QBidPrice[0];
		quote->thyquote.m_dMCJG1 = info->QAskPrice[0];
		quote->thyquote.m_nMRSL1 = info->QBidQty[0];
		quote->thyquote.m_nMCSL1 = info->QAskQty[0];
		process_quote(quote);
	} else
		xcb_log(XCB_LOG_WARNING, "Error allocating memory for quote");
}

static int tap_exec(void *data, void *data2) {
	RAII_VAR(struct msg *, msg, (struct msg *)data, msg_decr);
	NOT_USED(data2);

	/* do nothing */
	return 0;
}

static int load_module(void) {
	struct TapAPIApplicationInfo appinfo;
	TAPIINT32 iresult;
	struct TapAPIQuoteLoginAuth loginauth;

	load_config();
	if (front_ip == NULL || front_port == NULL || userid == NULL || passwd == NULL) {
		xcb_log(XCB_LOG_ERROR, "front_ip, front_port, userid or passwd can't be empty");
		return MODULE_LOAD_FAILURE;
	}
	mdspi = tap_mdspi_create();
	tap_mdspi_on_login(mdspi, on_login);
	tap_mdspi_on_api_ready(mdspi, on_api_ready);
	tap_mdspi_on_subscribe_quote(mdspi, on_subscribe_quote);
	tap_mdspi_on_quote(mdspi, on_quote);
	/* FIXME */
	strcpy(appinfo.AuthCode, DEFAULT_AUTHCODE);
	strcpy(appinfo.KeyOperationLogPath, "/var/log/xcb/");
	if ((mdapi = tap_mdapi_create(&appinfo, &iresult)) == NULL) {
		xcb_log(XCB_LOG_ERROR, "Error occurred: errorid=%d", iresult);
		return MODULE_LOAD_FAILURE;
	}
	tap_mdapi_set_spi(mdapi, mdspi);
	tap_mdapi_set_host_addr(mdapi, front_ip, atoi(front_port));
	memset(&loginauth, '\0', sizeof loginauth);
	strncat(loginauth.UserNo, userid, sizeof loginauth.UserNo - 1);
	loginauth.ISModifyPassword = APIYNFLAG_NO;
	strncat(loginauth.Password, passwd, sizeof loginauth.Password - 1);
	loginauth.ISDDA = APIYNFLAG_NO;
	tap_mdapi_login(mdapi, &loginauth);
	return register_application(app, tap_exec, desc, NULL, mod_info->self);
}

static int unload_module(void) {
	/* FIXME */
	tap_mdapi_destroy(mdapi);
	tap_mdspi_destroy(mdspi);
	config_destroy(cfg);
	return unregister_application(app);
}

static int reload_module(void) {
	/* FIXME */
	return MODULE_LOAD_SUCCESS;
}

MODULE_INFO(load_module, unload_module, reload_module, "Trading Analysis Platform API Application");


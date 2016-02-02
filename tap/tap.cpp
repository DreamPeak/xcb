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

#include <stddef.h>
#include "TapQuoteAPI.h"
#include "tap.h"

extern "C" {

/* FIXME */
struct tap_mdapi_t {
	ITapQuoteAPI			*rep;
};
struct tap_mdspi_t : public ITapQuoteAPINotify {
	tap_on_login			on_login_;
	tap_on_api_ready		on_api_ready_;
	tap_on_disconnect		on_disconnect_;
	tap_on_change_passwd		on_change_passwd_;
	tap_on_query_exchange		on_query_exchange_;
	tap_on_query_commodity		on_query_commodity_;
	tap_on_query_time_bucket	on_query_time_bucket_;
	tap_on_time_bucket		on_time_bucket_;
	tap_on_query_contract		on_query_contract_;
	tap_on_contract			on_contract_;
	tap_on_subscribe_quote		on_subscribe_quote_;
	tap_on_unsubscribe_quote	on_unsubscribe_quote_;
	tap_on_quote			on_quote_;
	tap_on_query_hisquote		on_query_hisquote_;
	void OnRspLogin(TAPIINT32 error, const TapAPIQuotLoginRspInfo *info) {
		if (on_login_)
			(*on_login_)(error, info);
	}
	void  OnAPIReady() {
		if (on_api_ready_)
			(*on_api_ready_)();
	}
	void OnDisconnect(TAPIINT32 reason) {
		if (on_disconnect_)
			(*on_disconnect_)(reason);
	}
	void OnRspChangePassword(TAPIUINT32 sid, TAPIINT32 error) {
		if (on_change_passwd_)
			(*on_change_passwd_)(sid, error);
	}
	void OnRspQryExchange(TAPIUINT32 sid, TAPIINT32 error, TAPIYNFLAG islast,
		const TapAPIExchangeInfo *info) {
		if (on_query_exchange_)
			(*on_query_exchange_)(sid, error, islast, info);
	}
	void OnRspQryCommodity(TAPIUINT32 sid, TAPIINT32 error, TAPIYNFLAG islast,
		const TapAPIQuoteCommodityInfo *info) {
		if (on_query_commodity_)
			(*on_query_commodity_)(sid, error, islast, info);
	}
	void OnRspQryTimeBucketOfCommodity(TAPIUINT32 sid, TAPIINT32 error, TAPIYNFLAG islast,
		const TapAPITimeBucketOfCommodityInfo *info) {
		if (on_query_time_bucket_)
			(*on_query_time_bucket_)(sid, error, islast, info);
	}
	void OnRtnTimeBucketOfCommodity(const TapAPITimeBucketOfCommodityInfo *info) {
		if (on_time_bucket_)
			(*on_time_bucket_)(info);
	}
	void OnRspQryContract(TAPIUINT32 sid, TAPIINT32 error, TAPIYNFLAG islast,
		const TapAPIQuoteContractInfo *info) {
		if (on_query_contract_)
			(*on_query_contract_)(sid, error, islast, info);
	}
	void OnRtnContract(const TapAPIQuoteContractInfo *info) {
		if (on_contract_)
			(*on_contract_)(info);
	}
	void OnRspSubscribeQuote(TAPIUINT32 sid, TAPIINT32 error, TAPIYNFLAG islast,
		const TapAPIQuoteWhole *info) {
		if (on_subscribe_quote_)
			(*on_subscribe_quote_)(sid, error, islast, info);
	}
	void OnRspUnSubscribeQuote(TAPIUINT32 sid, TAPIINT32 error, TAPIYNFLAG islast,
		const TapAPIContract *info) {
		if (on_unsubscribe_quote_)
			(*on_unsubscribe_quote_)(sid, error, islast, info);
	}
	void  OnRtnQuote(const TapAPIQuoteWhole *info) {
		if (on_quote_)
			(*on_quote_)(info);
	}
	void  OnRspQryHisQuote(TAPIUINT32 sid, TAPIINT32 error, TAPIYNFLAG islast,
		const TapAPIHisQuoteQryRsp *info) {
		if (on_query_hisquote_)
			(*on_query_hisquote_)(sid, error, islast, info);
	}
};

tap_mdapi_t *tap_mdapi_create(const struct TapAPIApplicationInfo *appinfo, TAPIINT32 *iresult) {
	tap_mdapi_t *mdapi = new tap_mdapi_t;
	if ((mdapi->rep = CreateTapQuoteAPI(appinfo, *iresult)) == NULL) {
		delete mdapi;
		return NULL;
	}
	return mdapi;
}

void tap_mdapi_destroy(tap_mdapi_t *mdapi) {
	if (mdapi) {
		FreeTapQuoteAPI(mdapi->rep);
		delete mdapi;
	}
}

TAPIINT32 tap_mdapi_set_log_path(const TAPICHAR *path) {
	return SetTapQuoteAPIDataPath(path);
}

TAPIINT32 tap_mdapi_set_log_level(TAPILOGLEVEL level) {
	return SetTapQuoteAPILogLevel(level);
}

/* FIXME */
TAPIINT32 tap_mdapi_set_spi(tap_mdapi_t *mdapi, tap_mdspi_t *mdspi) {
	if (mdapi)
		return mdapi->rep->SetAPINotify(mdspi);
	return -1;
}

/* FIXME */
TAPIINT32 tap_mdapi_set_host_addr(tap_mdapi_t *mdapi, const TAPICHAR *ip, TAPIUINT16 port) {
	if (mdapi)
		return mdapi->rep->SetHostAddress(ip, port);
	return -1;
}

/* FIXME */
TAPIINT32 tap_mdapi_login(tap_mdapi_t *mdapi, const struct TapAPIQuoteLoginAuth *loginauth) {
	if (mdapi)
		return mdapi->rep->Login(loginauth);
	return -1;
}

/* FIXME */
TAPIINT32 tap_mdapi_disconnect(tap_mdapi_t *mdapi) {
	if (mdapi)
		return mdapi->rep->Disconnect();
	return -1;
}

const TAPICHAR *tap_mdapi_get_version() {
	return GetTapQuoteAPIVersion();
}

/* FIXME */
TAPIINT32 tap_mdapi_get_servtime(tap_mdapi_t *mdapi, TAPIDATETIME *datetime) {
	if (mdapi)
		return mdapi->rep->GetServerTime(datetime);
	return -1;
}

/* FIXME */
TAPIINT32 tap_mdapi_change_passwd(tap_mdapi_t *mdapi, TAPIUINT32 *sid,
	const struct TapAPIChangePasswordReq *req) {
	if (mdapi)
		return mdapi->rep->ChangePassword(sid, req);
	return -1;
}

/* FIXME */
TAPIINT32 tap_mdapi_query_exchange(tap_mdapi_t *mdapi, TAPIUINT32 *sid) {
	if (mdapi)
		return mdapi->rep->QryExchange(sid);
	return -1;
}

/* FIXME */
TAPIINT32 tap_mdapi_query_commodity(tap_mdapi_t *mdapi, TAPIUINT32 *sid){
	if (mdapi)
		return mdapi->rep->QryCommodity(sid);
	return -1;
}

/* FIXME */
TAPIINT32 tap_mdapi_query_time_bucket(tap_mdapi_t *mdapi, TAPIUINT32 *sid,
	const struct TapAPICommodity *req) {
	if (mdapi)
		return mdapi->rep->QryTradingTimeBucketOfCommodity(sid, req);
	return -1;
}

/* FIXME */
TAPIINT32 tap_mdapi_query_contract(tap_mdapi_t *mdapi, TAPIUINT32 *sid,
	const struct TapAPICommodity *req) {
	if (mdapi)
		return mdapi->rep->QryContract(sid, req);
	return -1;
}

/* FIXME */
TAPIINT32 tap_mdapi_subscribe_quote(tap_mdapi_t *mdapi, TAPIUINT32 *sid,
	const struct TapAPIContract *contract) {
	if (mdapi)
		return mdapi->rep->SubscribeQuote(sid, contract);
	return -1;
}

/* FIXME */
TAPIINT32 tap_mdapi_unsubscribe_quote(tap_mdapi_t *mdapi, TAPIUINT32 *sid,
	const struct TapAPIContract *contract) {
	if (mdapi)
		return mdapi->rep->UnSubscribeQuote(sid, contract);
	return -1;
}

/* FIXME */
TAPIINT32 tap_mdapi_query_hisquote(tap_mdapi_t *mdapi, TAPIUINT32 *sid,
	const struct TapAPIHisQuoteQryReq *req) {
	if (mdapi)
		return mdapi->rep->QryHisQuote(sid, req);
	return -1;
}

/* FIXME */
const struct TapAPIQuoteWhole *tap_mdapi_get_fullquote(tap_mdapi_t *mdapi,
	const struct TapAPIContract *contract) {
	if (mdapi)
		return mdapi->rep->GetFullQuote(contract);
	return NULL;
}

tap_mdspi_t *tap_mdspi_create() {
	return new tap_mdspi_t;
}

void tap_mdspi_destroy(tap_mdspi_t *mdspi) {
	if (mdspi)
		delete mdspi;
}

void tap_mdspi_on_login(tap_mdspi_t *mdspi, tap_on_login func) {
	if (mdspi && func)
		mdspi->on_login_ = func;
}

void tap_mdspi_on_api_ready(tap_mdspi_t *mdspi, tap_on_api_ready func) {
	if (mdspi && func)
		mdspi->on_api_ready_ = func;
}

void tap_mdspi_on_disconnect(tap_mdspi_t *mdspi, tap_on_disconnect func) {
	if (mdspi && func)
		mdspi->on_disconnect_ = func;
}

void tap_mdspi_on_change_passwd(tap_mdspi_t *mdspi, tap_on_change_passwd func) {
	if (mdspi && func)
		mdspi->on_change_passwd_ = func;
}

void tap_mdspi_on_query_exchange(tap_mdspi_t *mdspi, tap_on_query_exchange func) {
	if (mdspi && func)
		mdspi->on_query_exchange_ = func;
}

void tap_mdspi_on_query_commodity(tap_mdspi_t *mdspi, tap_on_query_commodity func) {
	if (mdspi && func)
		mdspi->on_query_commodity_ = func;
}

void tap_mdspi_on_query_time_bucket(tap_mdspi_t *mdspi, tap_on_query_time_bucket func) {
	if (mdspi && func)
		mdspi->on_query_time_bucket_ = func;
}

void tap_mdspi_on_time_bucket(tap_mdspi_t *mdspi, tap_on_time_bucket func) {
	if (mdspi && func)
		mdspi->on_time_bucket_ = func;
}

void tap_mdspi_on_query_contract(tap_mdspi_t *mdspi, tap_on_query_contract func) {
	if (mdspi && func)
		mdspi->on_query_contract_ = func;
}

void tap_mdspi_on_contract(tap_mdspi_t *mdspi, tap_on_contract func) {
	if (mdspi && func)
		mdspi->on_contract_ = func;
}

void tap_mdspi_on_subscribe_quote(tap_mdspi_t *mdspi, tap_on_subscribe_quote func) {
	if (mdspi && func)
		mdspi->on_subscribe_quote_ = func;
}

void tap_mdspi_on_unsubscribe_quote(tap_mdspi_t *mdspi, tap_on_unsubscribe_quote func) {
	if (mdspi && func)
		mdspi->on_unsubscribe_quote_ = func;
}

void tap_mdspi_on_quote(tap_mdspi_t *mdspi, tap_on_quote func) {
	if (mdspi && func)
		mdspi->on_quote_ = func;
}

void tap_mdspi_on_query_hisquote(tap_mdspi_t *mdspi, tap_on_query_hisquote func) {
	if (mdspi && func)
		mdspi->on_query_hisquote_ = func;
}

}


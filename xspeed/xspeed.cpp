/*
 * Copyright (c) 2013-2017, Dalian Futures Information Technology Co., Ltd.
 *
 * Gaohang Wu  <wugaohang at dce dot com dot cn>
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
#include "DFITCMdApi.h"
#include "xspeed.h"

extern "C" {

/* FIXME */
struct xspeed_mdapi_t {
	DFITCXSPEEDMDAPI::DFITCMdApi		*rep;
};
struct xspeed_mdspi_t : public DFITCXSPEEDMDAPI::DFITCMdSpi {
	xspeed_on_front_connected		on_front_connected_		= NULL;
	xspeed_on_front_disconnected		on_front_disconnected_		= NULL;
	xspeed_on_error				on_error_			= NULL;
	xspeed_on_user_login			on_user_login_			= NULL;
	xspeed_on_user_logout			on_user_logout_			= NULL;
	xspeed_on_tradingday			on_tradingday_			= NULL;
	xspeed_on_subscribe_market_data		on_subscribe_market_data_	= NULL;
	xspeed_on_unsubscribe_market_data	on_unsubscribe_market_data_	= NULL;
	xspeed_on_deep_market_data		on_deep_market_data_		= NULL;
	xspeed_on_subscribe_quote_response	on_subscribe_quote_response_	= NULL;
	xspeed_on_unsubscribe_quote_response	on_unsubscribe_quote_response_	= NULL;
	xspeed_on_quote_response		on_quote_response_		= NULL;
	void OnFrontConnected() {
		if (on_front_connected_)
			(*on_front_connected_)();
	}
	void OnFrontDisconnected(int nReason) {
		if (on_front_disconnected_)
			(*on_front_disconnected_)(nReason);
	}
	void OnRspError(struct DFITCErrorRtnField *pRspInfo) {
		if (on_error_)
			(*on_error_)(pRspInfo);
	}
	void OnRspUserLogin(struct DFITCUserLoginInfoRtnField *pRspUserLogin,
		struct DFITCErrorRtnField *pRspInfo) {
		if (on_user_login_)
			(*on_user_login_)(pRspUserLogin, pRspInfo);
	}
	void OnRspUserLogout(struct DFITCUserLogoutInfoRtnField *pRspUsrLogout,
		struct DFITCErrorRtnField *pRspInfo) {
		if (on_user_logout_)
			(*on_user_logout_)(pRspUsrLogout, pRspInfo);
	}
	void OnRspTradingDay(struct DFITCTradingDayRtnField *pTradingDayRtnData) {
		if (on_tradingday_)
			(*on_tradingday_)(pTradingDayRtnData);
	}
	void OnRspSubMarketData(struct DFITCSpecificInstrumentField *pSpecificInstrument,
		struct DFITCErrorRtnField *pRspInfo) {
		if (on_subscribe_market_data_)
			(*on_subscribe_market_data_)(pSpecificInstrument, pRspInfo);
	}
	void OnRspUnSubMarketData(struct DFITCSpecificInstrumentField *pSpecificInstrument,
		struct DFITCErrorRtnField *pRspInfo) {
		if (on_unsubscribe_market_data_)
			(*on_unsubscribe_market_data_)(pSpecificInstrument, pRspInfo);
	}
	void OnMarketData(struct DFITCDepthMarketDataField *pMarketDataField) {
		if (on_deep_market_data_)
			(*on_deep_market_data_)(pMarketDataField);
	}
	void OnRspSubForQuoteRsp(struct DFITCSpecificInstrumentField *pSpecificInstrument,
		struct DFITCErrorRtnField *pRspInfo) {
		if (on_subscribe_quote_response_)
			(*on_subscribe_quote_response_)(pSpecificInstrument, pRspInfo);
	}
	void OnRspUnSubForQuoteRsp(struct DFITCSpecificInstrumentField *pSpecificInstrument,
		struct DFITCErrorRtnField *pRspInfo) {
		if (on_unsubscribe_quote_response_)
			(*on_unsubscribe_quote_response_)(pSpecificInstrument, pRspInfo);
	}
	void OnRtnForQuoteRsp(struct DFITCQuoteSubscribeRtnField *pForQuoteField) {
		if (on_quote_response_)
			(*on_quote_response_)(pForQuoteField);
	}
};

xspeed_mdapi_t *xspeed_mdapi_create(void) {
	xspeed_mdapi_t *mdapi = new xspeed_mdapi_t;
	mdapi->rep = DFITCXSPEEDMDAPI::DFITCMdApi::CreateDFITCMdApi();
	return mdapi;
}

void xspeed_mdapi_destroy(xspeed_mdapi_t *mdapi) {
	if (mdapi) {
		mdapi->rep->Release();
		delete mdapi;
	}
}

int xspeed_mdapi_init(xspeed_mdapi_t *mdapi, char *svraddr, xspeed_mdspi_t *mdspi) {
	if (mdapi)
		return mdapi->rep->Init(svraddr, mdspi);
	return -1;
}

int xspeed_mdapi_login_user(xspeed_mdapi_t *mdapi, struct DFITCUserLoginField *userlogin) {
	if (mdapi)
		return mdapi->rep->ReqUserLogin(userlogin);
	return -1;
}

int xspeed_mdapi_logout_user(xspeed_mdapi_t *mdapi, struct DFITCUserLogoutField *userlogout) {
	if (mdapi)
		return mdapi->rep->ReqUserLogout(userlogout);
	return -1;
}

int xspeed_mdapi_get_tradingday(xspeed_mdapi_t *mdapi, struct DFITCTradingDayField *tradingday) {
	if (mdapi)
		return mdapi->rep->ReqTradingDay(tradingday);
	return -1;
}

int xspeed_mdapi_subscribe_market_data(xspeed_mdapi_t *mdapi,
	char **instruments, int count, int rid) {
	if (mdapi)
		return mdapi->rep->SubscribeMarketData(instruments, count, rid);
	return -1;
}

int xspeed_mdapi_unsubscribe_market_data(xspeed_mdapi_t *mdapi,
	char **instruments, int count, int rid) {
	if (mdapi)
		return mdapi->rep->UnSubscribeMarketData(instruments, count, rid);
	return -1;
}

int xspeed_mdapi_subscribe_quote_response(xspeed_mdapi_t *mdapi,
	char **instruments, int count, int rid) {
	if (mdapi)
		return mdapi->rep->SubscribeForQuoteRsp(instruments, count, rid);
	return -1;
}

int xspeed_mdapi_unsubscribe_quote_response(xspeed_mdapi_t *mdapi,
	char **instruments, int count, int rid) {
	if (mdapi)
		return mdapi->rep->UnSubscribeForQuoteRsp(instruments, count, rid);
	return -1;
}

xspeed_mdspi_t *xspeed_mdspi_create(void) {
	return new xspeed_mdspi_t;
}

void xspeed_mdspi_destroy(xspeed_mdspi_t *mdspi) {
	if (mdspi)
		delete mdspi;
}

void xspeed_mdspi_on_front_connected(xspeed_mdspi_t *mdspi, xspeed_on_front_connected func) {
	if (mdspi && func)
		mdspi->on_front_connected_ = func;
}

void xspeed_mdspi_on_front_disconnected(xspeed_mdspi_t *mdspi, xspeed_on_front_disconnected func) {
	if (mdspi && func)
		mdspi->on_front_disconnected_ = func;
}

void xspeed_mdspi_on_error(xspeed_mdspi_t *mdspi, xspeed_on_error func) {
	if (mdspi && func)
		mdspi->on_error_ = func;
}

void xspeed_mdspi_on_user_login(xspeed_mdspi_t *mdspi, xspeed_on_user_login func) {
	if (mdspi && func)
		mdspi->on_user_login_ = func;
}

void xspeed_mdspi_on_user_logout(xspeed_mdspi_t *mdspi, xspeed_on_user_logout func) {
	if (mdspi && func)
		mdspi->on_user_logout_ = func;
}

void xspeed_mdspi_on_tradingday(xspeed_mdspi_t *mdspi, xspeed_on_tradingday func) {
	if (mdspi && func)
		mdspi->on_tradingday_ = func;
}

void xspeed_mdspi_on_subscribe_market_data(xspeed_mdspi_t *mdspi,
	xspeed_on_subscribe_market_data func) {
	if (mdspi && func)
		mdspi->on_subscribe_market_data_ = func;
}

void xspeed_mdspi_on_unsubscribe_market_data(xspeed_mdspi_t *mdspi,
	xspeed_on_unsubscribe_market_data func) {
	if (mdspi && func)
		mdspi->on_unsubscribe_market_data_ = func;
}

void xspeed_mdspi_on_deep_market_data(xspeed_mdspi_t *mdspi, xspeed_on_deep_market_data func) {
	if (mdspi && func)
		mdspi->on_deep_market_data_ = func;
}

void xspeed_mdspi_on_subscribe_quote_response(xspeed_mdspi_t *mdspi,
	xspeed_on_subscribe_quote_response func) {
	if (mdspi && func)
		mdspi->on_subscribe_quote_response_ = func;
}

void xspeed_mdspi_on_unsubscribe_quote_response(xspeed_mdspi_t *mdspi,
	xspeed_on_unsubscribe_quote_response func) {
	if (mdspi && func)
		mdspi->on_unsubscribe_quote_response_ = func;
}

void xspeed_mdspi_on_quote_response(xspeed_mdspi_t *mdspi, xspeed_on_quote_response func) {
	if (mdspi && func)
		mdspi->on_quote_response_ = func;
}

}


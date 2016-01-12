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
#include "ThostFtdcMdApi.h"
#include "ctp.h"

extern "C" {

/* FIXME */
struct ctp_mdapi_t {
	CThostFtdcMdApi				*rep;
};
struct ctp_mdspi_t : public CThostFtdcMdSpi {
	ctp_on_front_connected			on_front_connected_;
	ctp_on_front_disconnected		on_front_disconnected_;
	ctp_on_heartbeat_warning		on_heartbeat_warning_;
	ctp_on_error				on_error_;
	ctp_on_user_login			on_user_login_;
	ctp_on_user_logout			on_user_logout_;
	ctp_on_subscribe_market_data		on_subscribe_market_data_;
	ctp_on_unsubscribe_market_data		on_unsubscribe_market_data_;
	ctp_on_subscribe_quote_response		on_subscribe_quote_response_;
	ctp_on_unsubscribe_quote_response	on_unsubscribe_quote_response_;
	ctp_on_deep_market_data			on_deep_market_data_;
	ctp_on_quote_response			on_quote_response_;
	/* make gcc happy */
	virtual ~ctp_mdspi_t() {};
	void OnFrontConnected() {
		if (on_front_connected_)
			(*on_front_connected_)();
	}
	void OnFrontDisconnected(int nReason) {
		if (on_front_disconnected_)
			(*on_front_disconnected_)(nReason);
	}
	void OnHeartBeatWarning(int nTimeLapse) {
		if (on_heartbeat_warning_)
			(*on_heartbeat_warning_)(nTimeLapse);
	}
	void OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
		if (on_error_)
			(*on_error_)(pRspInfo, nRequestID, bIsLast ? 1 : 0);
	}
	void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,
		CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
		if (on_user_login_)
			(*on_user_login_)(pRspUserLogin, pRspInfo, nRequestID, bIsLast ? 1 : 0);
	}
	void OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout,
		CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
		if (on_user_logout_)
			(*on_user_logout_)(pUserLogout, pRspInfo, nRequestID, bIsLast ? 1 : 0);
	}
	void OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument,
		CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
		if (on_subscribe_market_data_)
			(*on_subscribe_market_data_)(pSpecificInstrument,
				pRspInfo, nRequestID, bIsLast ? 1 : 0);
	}
	void OnRspUnSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument,
		CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
		if (on_unsubscribe_market_data_)
			(*on_unsubscribe_market_data_)(pSpecificInstrument,
				pRspInfo, nRequestID, bIsLast ? 1 : 0);
	}
	void OnRspSubForQuoteRsp(CThostFtdcSpecificInstrumentField *pSpecificInstrument,
		CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
		if (on_subscribe_quote_response_)
			(*on_subscribe_quote_response_)(pSpecificInstrument,
				pRspInfo, nRequestID, bIsLast ? 1 : 0);
	}
	void OnRspUnSubForQuoteRsp(CThostFtdcSpecificInstrumentField *pSpecificInstrument,
		CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
		if (on_unsubscribe_quote_response_)
			(*on_unsubscribe_quote_response_)(pSpecificInstrument,
				pRspInfo, nRequestID, bIsLast ? 1 : 0);
	}
	void OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData) {
		if (on_deep_market_data_)
			(*on_deep_market_data_)(pDepthMarketData);
	}
	void OnRtnForQuoteRsp(CThostFtdcForQuoteRspField *pForQuoteRsp) {
		if (on_quote_response_)
			(*on_quote_response_)(pForQuoteRsp);
	}
};

ctp_mdapi_t *ctp_mdapi_create(const char *flowpath, int isudp, int ismulticast) {
	ctp_mdapi_t *mdapi = new ctp_mdapi_t;
	mdapi->rep = CThostFtdcMdApi::CreateFtdcMdApi(flowpath,
		isudp ? true : false, ismulticast ? true : false);
	return mdapi;
}

void ctp_mdapi_destroy(ctp_mdapi_t *mdapi) {
	if (mdapi) {
		mdapi->rep->Release();
		delete mdapi;
	}
}

void ctp_mdapi_register_front(ctp_mdapi_t *mdapi, char *frontaddr) {
	if (mdapi)
		mdapi->rep->RegisterFront(frontaddr);
}

void ctp_mdapi_register_name_server(ctp_mdapi_t *mdapi, char *nsaddr) {
	if (mdapi)
		mdapi->rep->RegisterNameServer(nsaddr);
}

/* FIXME */
void ctp_mdapi_register_fens_user(ctp_mdapi_t *mdapi, struct CThostFtdcFensUserInfoField *fensuser) {
	if (mdapi)
		mdapi->rep->RegisterFensUserInfo(fensuser);
}

void ctp_mdapi_register_spi(ctp_mdapi_t *mdapi, ctp_mdspi_t *mdspi) {
	if (mdapi && mdspi)
		mdapi->rep->RegisterSpi(mdspi);
}

void ctp_mdapi_init(ctp_mdapi_t *mdapi) {
	if (mdapi)
		mdapi->rep->Init();
}

/* FIXME */
int ctp_mdapi_join(ctp_mdapi_t *mdapi) {
	if (mdapi)
		return mdapi->rep->Join();
	return -1;
}

/* FIXME */
int ctp_mdapi_login_user(ctp_mdapi_t *mdapi, struct CThostFtdcReqUserLoginField *userlogin, int rid) {
	if (mdapi)
		return mdapi->rep->ReqUserLogin(userlogin, rid);
	return -1;
}

/* FIXME */
int ctp_mdapi_logout_user(ctp_mdapi_t *mdapi, struct CThostFtdcUserLogoutField *userlogout, int rid) {
	if (mdapi)
		return mdapi->rep->ReqUserLogout(userlogout, rid);
	return -1;
}

/* FIXME */
const char *ctp_mdapi_get_today(ctp_mdapi_t *mdapi) {
	if (mdapi)
		return mdapi->rep->GetTradingDay();
	return NULL;
}

/* FIXME */
int ctp_mdapi_subscribe_market_data(ctp_mdapi_t *mdapi, char **instruments, int count) {
	if (mdapi)
		return mdapi->rep->SubscribeMarketData(instruments, count);
	return -1;
}

/* FIXME */
int ctp_mdapi_unsubscribe_market_data(ctp_mdapi_t *mdapi, char **instruments, int count) {
	if (mdapi)
		return mdapi->rep->UnSubscribeMarketData(instruments, count);
	return -1;
}

/* FIXME */
int ctp_mdapi_subscribe_quote_response(ctp_mdapi_t *mdapi, char **instruments, int count) {
	if (mdapi)
		return mdapi->rep->SubscribeForQuoteRsp(instruments, count);
	return -1;
}

/* FIXME */
int ctp_mdapi_unsubscribe_quote_response(ctp_mdapi_t *mdapi, char **instruments, int count) {
	if (mdapi)
		return mdapi->rep->UnSubscribeForQuoteRsp(instruments, count);
	return -1;
}

ctp_mdspi_t *ctp_mdspi_create() {
	return new ctp_mdspi_t;
}

void ctp_mdspi_destroy(ctp_mdspi_t *mdspi) {
	if (mdspi)
		delete mdspi;
}

void ctp_mdspi_on_front_connected(ctp_mdspi_t *mdspi, ctp_on_front_connected func) {
	if (mdspi && func)
		mdspi->on_front_connected_ = func;
}

void ctp_mdspi_on_front_disconnected(ctp_mdspi_t *mdspi, ctp_on_front_disconnected func) {
	if (mdspi && func)
		mdspi->on_front_disconnected_ = func;
}

void ctp_mdspi_on_heartbeat_warning(ctp_mdspi_t *mdspi, ctp_on_heartbeat_warning func) {
	if (mdspi && func)
		mdspi->on_heartbeat_warning_ = func;
}

void ctp_mdspi_on_error(ctp_mdspi_t *mdspi, ctp_on_error func) {
	if (mdspi && func)
		mdspi->on_error_ = func;
}

void ctp_mdspi_on_user_login(ctp_mdspi_t *mdspi, ctp_on_user_login func) {
	if (mdspi && func)
		mdspi->on_user_login_ = func;
}

void ctp_mdspi_on_user_logout(ctp_mdspi_t *mdspi, ctp_on_user_logout func) {
	if (mdspi && func)
		mdspi->on_user_logout_ = func;
}

void ctp_mdspi_on_subscribe_market_data(ctp_mdspi_t *mdspi, ctp_on_subscribe_market_data func) {
	if (mdspi && func)
		mdspi->on_subscribe_market_data_ = func;
}

void ctp_mdspi_on_unsubscribe_market_data(ctp_mdspi_t *mdspi, ctp_on_unsubscribe_market_data func) {
	if (mdspi && func)
		mdspi->on_unsubscribe_market_data_ = func;
}

void ctp_mdspi_on_subscribe_quote_response(ctp_mdspi_t *mdspi, ctp_on_subscribe_quote_response func) {
	if (mdspi && func)
		mdspi->on_subscribe_quote_response_ = func;
}

void ctp_mdspi_on_unsubscribe_quote_response(ctp_mdspi_t *mdspi, ctp_on_unsubscribe_quote_response func) {
	if (mdspi && func)
		mdspi->on_unsubscribe_quote_response_ = func;
}

void ctp_mdspi_on_deep_market_data(ctp_mdspi_t *mdspi, ctp_on_deep_market_data func) {
	if (mdspi && func)
		mdspi->on_deep_market_data_ = func;
}

void ctp_mdspi_on_quote_response(ctp_mdspi_t *mdspi, ctp_on_quote_response func) {
	if (mdspi && func)
		mdspi->on_quote_response_ = func;
}

}


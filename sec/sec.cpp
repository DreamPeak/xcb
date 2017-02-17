/*
 * Copyright (c) 2013-2017, Dalian Futures Information Technology Co., Ltd.
 *
 * Bo Wang
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
#include "DFITCSECMdApi.h"
#include "sec.h"

extern "C" {

/* FIXME */
struct sec_mdapi_t {
	DFITCSECMdApi				*rep;
};
struct sec_mdspi_t : public DFITCSECMdSpi {
	sec_on_front_connected			on_front_connected_		= NULL;
	sec_on_front_disconnected		on_front_disconnected_		= NULL;
	sec_on_error				on_error_			= NULL;
	sec_on_stk_user_login			on_stk_user_login_		= NULL;
	sec_on_stk_user_logout			on_stk_user_logout_		= NULL;
	sec_on_stk_subscribe_market_data	on_stk_subscribe_market_data_	= NULL;
	sec_on_stk_unsubscribe_market_data	on_stk_unsubscribe_market_data_	= NULL;
	sec_on_stk_deep_market_data		on_stk_deep_market_data_	= NULL;
	sec_on_sop_user_login			on_sop_user_login_		= NULL;
	sec_on_sop_user_logout			on_sop_user_logout_		= NULL;
	sec_on_sop_subscribe_market_data	on_sop_subscribe_market_data_	= NULL;
	sec_on_sop_unsubscribe_market_data	on_sop_unsubscribe_market_data_	= NULL;
	sec_on_sop_deep_market_data		on_sop_deep_market_data_	= NULL;
	void OnFrontConnected() {
		if (on_front_connected_)
			(*on_front_connected_)();
	}
	void OnFrontDisconnected(int nReason) {
		if (on_front_disconnected_)
			(*on_front_disconnected_)(nReason);
	}
	void OnRspError(struct DFITCSECRspInfoField *pRspInfo) {
		if (on_error_)
			(*on_error_)(pRspInfo);
	}
	void OnRspStockUserLogin(struct DFITCSECRspUserLoginField *pRspUserLogin,
		struct DFITCSECRspInfoField *pRspInfo) {
		if (on_stk_user_login_)
			(*on_stk_user_login_)(pRspUserLogin, pRspInfo);
	}
	void OnRspStockUserLogout(struct DFITCSECRspUserLogoutField *pRspUsrLogout,
		struct DFITCSECRspInfoField *pRspInfo) {
		if (on_stk_user_logout_)
			(*on_stk_user_logout_)(pRspUsrLogout, pRspInfo);
	}
	void OnRspStockSubMarketData(struct DFITCSECSpecificInstrumentField *pSpecificInstrument,
		struct DFITCSECRspInfoField *pRspInfo) {
		if (on_stk_subscribe_market_data_)
			(*on_stk_subscribe_market_data_)(pSpecificInstrument, pRspInfo);
	}
	void OnRspStockUnSubMarketData(struct DFITCSECSpecificInstrumentField *pSpecificInstrument,
		struct DFITCSECRspInfoField *pRspInfo) {
		if (on_stk_unsubscribe_market_data_)
			(*on_stk_unsubscribe_market_data_)(pSpecificInstrument, pRspInfo);
	}
	void OnStockMarketData(struct DFITCStockDepthMarketDataField *pMarketDataField) {
		if (on_stk_deep_market_data_)
			(*on_stk_deep_market_data_)(pMarketDataField);
	}
	void OnRspSOPUserLogin(struct DFITCSECRspUserLoginField *pRspUserLogin,
		struct DFITCSECRspInfoField *pRspInfo) {
		if (on_sop_user_login_)
			(*on_sop_user_login_)(pRspUserLogin, pRspInfo);
	}
	void OnRspSOPUserLogout(struct DFITCSECRspUserLogoutField *pRspUsrLogout,
		struct DFITCSECRspInfoField *pRspInfo) {
		if (on_sop_user_logout_)
			(*on_sop_user_logout_)(pRspUsrLogout, pRspInfo);
	}
	void OnRspSOPSubMarketData(struct DFITCSECSpecificInstrumentField *pSpecificInstrument,
		struct DFITCSECRspInfoField *pRspInfo) {
		if (on_sop_subscribe_market_data_)
			(*on_sop_subscribe_market_data_)(pSpecificInstrument, pRspInfo);
	}
	void OnRspSOPUnSubMarketData(struct DFITCSECSpecificInstrumentField *pSpecificInstrument,
		struct DFITCSECRspInfoField *pRspInfo) {
		if (on_sop_unsubscribe_market_data_)
			(*on_sop_unsubscribe_market_data_)(pSpecificInstrument, pRspInfo);
	}
	void OnSOPMarketData(struct DFITCSOPDepthMarketDataField *pMarketDataField) {
		if (on_sop_deep_market_data_)
			(*on_sop_deep_market_data_)(pMarketDataField);
	}
};

sec_mdapi_t *sec_mdapi_create(void) {
	sec_mdapi_t *mdapi = new sec_mdapi_t;
	mdapi->rep = DFITCSECMdApi::CreateDFITCMdApi(NULL);
	return mdapi;
}

void sec_mdapi_destroy(sec_mdapi_t *mdapi) {
	if (mdapi)
		mdapi->rep->Release();
}

int sec_mdapi_init(sec_mdapi_t *mdapi, char *svraddr, sec_mdspi_t *mdspi) {
	if (mdapi && mdspi)
		return mdapi->rep->Init(svraddr, mdspi);
	return -1;
}

int sec_mdapi_stk_user_login(sec_mdapi_t *mdapi, struct DFITCSECReqUserLoginField *userlogin) {
	if (mdapi)
		return mdapi->rep->ReqStockUserLogin(userlogin);
	return -1;
}

int sec_mdapi_stk_user_logout(sec_mdapi_t *mdapi, struct DFITCSECReqUserLogoutField *userlogout) {
	if (mdapi)
		return mdapi->rep->ReqStockUserLogout(userlogout);
	return -1;
}

int sec_mdapi_stk_subscribe_market_data(sec_mdapi_t *mdapi,
	char **instruments, int count, int requsets) {
	if (mdapi)
		return mdapi->rep->SubscribeStockMarketData(instruments, count, requsets);
	return -1;
}

int sec_mdapi_stk_unsubscribe_market_data(sec_mdapi_t *mdapi,
	char **instruments, int count, int requsets) {
	if (mdapi)
		return mdapi->rep->UnSubscribeStockMarketData(instruments, count, requsets);
	return -1;
}

int sec_mdapi_sop_user_login(sec_mdapi_t *mdapi, struct DFITCSECReqUserLoginField *userlogin) {
	if (mdapi)
		return mdapi->rep->ReqSOPUserLogin(userlogin);
	return -1;
}

int sec_mdapi_sop_user_logout(sec_mdapi_t *mdapi, struct DFITCSECReqUserLogoutField *userlogout) {
	if (mdapi)
		return mdapi->rep->ReqSOPUserLogout(userlogout);
	return -1;
}

int sec_mdapi_sop_subscribe_market_data(sec_mdapi_t *mdapi,
	char **instruments, int count, int requsets) {
	if (mdapi)
		return mdapi->rep->SubscribeSOPMarketData(instruments, count, requsets);
	return -1;
}

int sec_mdapi_sop_unsubscribe_market_data(sec_mdapi_t *mdapi,
	char **instruments, int count, int requsets) {
	if (mdapi)
		return mdapi->rep->UnSubscribeSOPMarketData(instruments, count, requsets);
	return -1;
}

sec_mdspi_t *sec_mdspi_create() {
	return new sec_mdspi_t;
}

void sec_mdspi_destroy(sec_mdspi_t *mdspi) {
	if (mdspi)
		delete mdspi;
}

void sec_mdspi_on_front_connected(sec_mdspi_t *mdspi, sec_on_front_connected func) {
	if (mdspi && func)
		mdspi->on_front_connected_ = func;
}

void sec_mdspi_on_front_disconnected(sec_mdspi_t *mdspi, sec_on_front_disconnected func) {
	if (mdspi && func)
		mdspi->on_front_disconnected_ = func;
}

void sec_mdspi_on_error(sec_mdspi_t *mdspi, sec_on_error func) {
	if (mdspi && func)
		mdspi->on_error_ = func;
}

void sec_mdspi_on_stk_user_login(sec_mdspi_t *mdspi, sec_on_stk_user_login func) {
	if (mdspi && func)
		mdspi->on_stk_user_login_ = func;
}

void sec_mdspi_on_stk_user_logout(sec_mdspi_t *mdspi, sec_on_stk_user_logout func) {
	if (mdspi && func)
		mdspi->on_stk_user_logout_ = func;
}

void sec_mdspi_on_stk_subscribe_market_data(sec_mdspi_t *mdspi, sec_on_stk_subscribe_market_data func) {
	if (mdspi && func)
		mdspi->on_stk_subscribe_market_data_ = func;
}

void sec_mdspi_on_stk_unsubscribe_market_data(sec_mdspi_t *mdspi,
	sec_on_stk_unsubscribe_market_data func) {
	if (mdspi && func)
		mdspi->on_stk_unsubscribe_market_data_ = func;
}

void sec_mdspi_on_stk_deep_market_data(sec_mdspi_t *mdspi, sec_on_stk_deep_market_data func) {
	if (mdspi && func)
		mdspi->on_stk_deep_market_data_ = func;
}

void sec_mdspi_on_sop_user_login(sec_mdspi_t *mdspi, sec_on_sop_user_login func) {
	if (mdspi && func)
		mdspi->on_sop_user_login_ = func;
}

void sec_mdspi_on_sop_user_logout(sec_mdspi_t *mdspi, sec_on_sop_user_logout func) {
	if (mdspi && func)
		mdspi->on_sop_user_logout_ = func;
}

void sec_mdspi_on_sop_subscribe_market_data(sec_mdspi_t *mdspi,
	sec_on_sop_subscribe_market_data func) {
	if (mdspi && func)
		mdspi->on_sop_subscribe_market_data_ = func;
}

void sec_mdspi_on_sop_unsubscribe_market_data(sec_mdspi_t *mdspi,
	sec_on_sop_unsubscribe_market_data func) {
	if (mdspi && func)
		mdspi->on_sop_unsubscribe_market_data_ = func;
}

void sec_mdspi_on_sop_deep_market_data(sec_mdspi_t *mdspi, sec_on_sop_deep_market_data func) {
	if (mdspi && func)
		mdspi->on_sop_deep_market_data_ = func;
}

}


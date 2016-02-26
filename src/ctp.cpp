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
#include "ThostFtdcTraderApi.h"
#include "ctp.h"

extern "C" {

/* FIXME */
struct ctp_tdapi_t {
	CThostFtdcTraderApi		*rep;
};
struct ctp_tdspi_t : public CThostFtdcTraderSpi {
	ctp_on_front_connected		on_front_connected_;
	ctp_on_front_disconnected	on_front_disconnected_;
	ctp_on_heartbeat_warning	on_heartbeat_warning_;
	ctp_on_authenticate		on_authenticate_;
	ctp_on_user_login		on_user_login_;
	ctp_on_user_logout		on_user_logout_;
	/* make gcc happy */
	virtual ~ctp_tdspi_t() {};
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
	void OnRspAuthenticate(CThostFtdcRspAuthenticateField *pRspAuthenticateField,
		CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
		if (on_authenticate_)
			(*on_authenticate_)(pRspAuthenticateField, pRspInfo, nRequestID, bIsLast ? 1 : 0);
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
};

ctp_tdapi_t *ctp_tdapi_create(const char *flowpath) {
	ctp_tdapi_t *tdapi = new ctp_tdapi_t;
	tdapi->rep = CThostFtdcTraderApi::CreateFtdcTraderApi(flowpath);
	return tdapi;
}

void ctp_tdapi_destroy(ctp_tdapi_t *tdapi) {
	if (tdapi) {
		tdapi->rep->Release();
		delete tdapi;
	}
}

void ctp_tdapi_register_front(ctp_tdapi_t *tdapi, char *frontaddr) {
	if (tdapi)
		tdapi->rep->RegisterFront(frontaddr);
}

void ctp_tdapi_register_name_server(ctp_tdapi_t *tdapi, char *nsaddr) {
	if (tdapi)
		tdapi->rep->RegisterNameServer(nsaddr);
}

/* FIXME */
void ctp_tdapi_register_fens_user(ctp_tdapi_t *tdapi, CThostFtdcFensUserInfoField *fensuser) {
	if (tdapi)
		tdapi->rep->RegisterFensUserInfo(fensuser);
}

void ctp_tdapi_register_spi(ctp_tdapi_t *tdapi, ctp_tdspi_t *tdspi) {
	if (tdapi && tdspi)
		tdapi->rep->RegisterSpi(tdspi);
}

void ctp_tdapi_subscribe_private(ctp_tdapi_t *tdapi, THOST_TE_RESUME_TYPE resumetype) {
	if (tdapi)
		tdapi->rep->SubscribePrivateTopic(resumetype);
}

void ctp_tdapi_subscribe_public(ctp_tdapi_t *tdapi, THOST_TE_RESUME_TYPE resumetype) {
	if (tdapi)
		tdapi->rep->SubscribePublicTopic(resumetype);
}

void ctp_tdapi_init(ctp_tdapi_t *tdapi) {
	if (tdapi)
		tdapi->rep->Init();
}

/* FIXME */
int ctp_tdapi_join(ctp_tdapi_t *tdapi) {
	if (tdapi)
		return tdapi->rep->Join();
	return 0;
}

/* FIXME */
int ctp_tdapi_authenticate(ctp_tdapi_t *tdapi, CThostFtdcReqAuthenticateField *auth, int rid) {
	if (tdapi)
		tdapi->rep->ReqAuthenticate(auth, rid);
	return 0;
}

/* FIXME */
int ctp_tdapi_login_user(ctp_tdapi_t *tdapi, CThostFtdcReqUserLoginField *userlogin, int rid) {
	if (tdapi)
		return tdapi->rep->ReqUserLogin(userlogin, rid);
	return 0;
}

/* FIXME */
int ctp_tdapi_logout_user(ctp_tdapi_t *tdapi, CThostFtdcUserLogoutField *userlogout, int rid) {
	if (tdapi)
		return tdapi->rep->ReqUserLogout(userlogout, rid);
	return 0;
}

/* FIXME */
const char *ctp_tdapi_get_version(ctp_tdapi_t *tdapi) {
	if (tdapi)
		return tdapi->rep->GetApiVersion();
	return NULL;
}

/* FIXME */
const char *ctp_tdapi_get_tradingday(ctp_tdapi_t *tdapi) {
	if (tdapi)
		return tdapi->rep->GetTradingDay();
	return NULL;
}

ctp_tdspi_t *ctp_tdspi_create() {
	return new ctp_tdspi_t;
}

void ctp_tdspi_destroy(ctp_tdspi_t *tdspi) {
	if (tdspi)
		delete tdspi;
}

void ctp_tdspi_on_front_connected(ctp_tdspi_t *tdspi, ctp_on_front_connected func) {
	if (tdspi && func)
		tdspi->on_front_connected_ = func;
}

void ctp_tdspi_on_front_disconnected(ctp_tdspi_t *tdspi, ctp_on_front_disconnected func) {
	if (tdspi && func)
		tdspi->on_front_disconnected_ = func;
}

void ctp_tdspi_on_heartbeat_warning(ctp_tdspi_t *tdspi, ctp_on_heartbeat_warning func) {
	if (tdspi && func)
		tdspi->on_heartbeat_warning_ = func;
}

void ctp_tdspi_on_authenticate(ctp_tdspi_t *tdspi, ctp_on_authenticate func) {
	if (tdspi && func)
		tdspi->on_authenticate_ = func;
}

void ctp_tdspi_on_user_login(ctp_tdspi_t *tdspi, ctp_on_user_login func) {
	if (tdspi && func)
		tdspi->on_user_login_ = func;
}

void ctp_tdspi_on_user_logout(ctp_tdspi_t *tdspi, ctp_on_user_logout func) {
	if (tdspi && func)
		tdspi->on_user_logout_ = func;
}

}


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
	ctp_on_error			on_error_;
	ctp_on_authenticate		on_authenticate_;
	ctp_on_user_login		on_user_login_;
	ctp_on_user_logout		on_user_logout_;
	ctp_on_update_user_password	on_update_user_password_;
	ctp_on_update_account_password	on_update_account_password_;
	ctp_on_confirm_settlement	on_confirm_settlement_;
	ctp_on_insert_order		on_insert_order_;
	ctp_on_order_action		on_order_action_;
	ctp_on_order			on_order_;
	ctp_on_trade			on_trade_;
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
	void OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
		if (on_error_)
			(*on_error_)(pRspInfo, nRequestID, bIsLast ? 1 : 0);
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
	void OnRspUserPasswordUpdate(CThostFtdcUserPasswordUpdateField *pUserPasswordUpdate,
		CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
		if (on_update_user_password_)
			(*on_update_user_password_)(pUserPasswordUpdate,
				pRspInfo, nRequestID, bIsLast ? 1 : 0);
	}
	void OnRspTradingAccountPasswordUpdate(
		CThostFtdcTradingAccountPasswordUpdateField *pTradingAccountPasswordUpdate,
		CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
		if (on_update_account_password_)
			(*on_update_account_password_)(pTradingAccountPasswordUpdate,
				pRspInfo, nRequestID, bIsLast ? 1 : 0);
	}
	void OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm,
		CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
		if (on_confirm_settlement_)
			(*on_confirm_settlement_)(pSettlementInfoConfirm,
				pRspInfo, nRequestID, bIsLast ? 1 : 0);
	}
	void OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder,
		CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
		if (on_insert_order_)
			(*on_insert_order_)(pInputOrder, pRspInfo, nRequestID, bIsLast ? 1 : 0);
	}
	void OnRspOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction,
		CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
		if (on_order_action_)
			(*on_order_action_)(pInputOrderAction, pRspInfo, nRequestID, bIsLast ? 1 : 0);
	}
	void OnRtnOrder(CThostFtdcOrderField *pOrder) {
		if (on_order_)
			(*on_order_)(pOrder);
	}
	void OnRtnTrade(CThostFtdcTradeField *pTrade) {
		if (on_trade_)
			(*on_trade_)(pTrade);
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

/* FIXME */
int ctp_tdapi_confirm_settlement(ctp_tdapi_t *tdapi,
	struct CThostFtdcSettlementInfoConfirmField *stmtconfirm, int rid) {
	if (tdapi)
		return tdapi->rep->ReqSettlementInfoConfirm(stmtconfirm, rid);
	return 0;
}

/* FIXME */
int ctp_tdapi_update_user_password(ctp_tdapi_t *tdapi,
	struct CThostFtdcUserPasswordUpdateField *password, int rid) {
	if (tdapi)
		return tdapi->rep->ReqUserPasswordUpdate(password, rid);
	return 0;
}

/* FIXME */
int ctp_tdapi_update_account_password(ctp_tdapi_t *tdapi,
	struct CThostFtdcTradingAccountPasswordUpdateField *password, int rid) {
	if (tdapi)
		return tdapi->rep->ReqTradingAccountPasswordUpdate(password, rid);
	return 0;
}

/* FIXME */
int ctp_tdapi_insert_order(ctp_tdapi_t *tdapi, struct CThostFtdcInputOrderField *order, int rid) {
	if (tdapi)
		return tdapi->rep->ReqOrderInsert(order, rid);
	return 0;
}

/* FIXME */
int ctp_tdapi_order_action(ctp_tdapi_t *tdapi,
	struct CThostFtdcInputOrderActionField *orderaction, int rid) {
	if (tdapi)
		return tdapi->rep->ReqOrderAction(orderaction, rid);
	return 0;
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

void ctp_tdspi_on_update_user_password(ctp_tdspi_t *tdspi, ctp_on_update_user_password func) {
	if (tdspi && func)
		tdspi->on_update_user_password_ = func;
}

void ctp_tdspi_on_update_account_password(ctp_tdspi_t *tdspi, ctp_on_update_account_password func) {
	if (tdspi && func)
		tdspi->on_update_account_password_ = func;
}

void ctp_tdspi_on_confirm_settlement(ctp_tdspi_t *tdspi, ctp_on_confirm_settlement func) {
	if (tdspi && func)
		tdspi->on_confirm_settlement_ = func;
}

void ctp_tdspi_on_insert_order(ctp_tdspi_t *tdspi, ctp_on_insert_order func) {
	if (tdspi && func)
		tdspi->on_insert_order_ = func;
}

void ctp_tdspi_on_order_action(ctp_tdspi_t *tdspi, ctp_on_order_action func) {
	if (tdspi && func)
		tdspi->on_order_action_ = func;
}

void ctp_tdspi_on_order(ctp_tdspi_t *tdspi, ctp_on_order func) {
	if (tdspi && func)
		tdspi->on_order_ = func;
}

void ctp_tdspi_on_trade(ctp_tdspi_t *tdspi, ctp_on_trade func) {
	if (tdspi && func)
		tdspi->on_trade_ = func;
}

}


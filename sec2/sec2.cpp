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
#include "SECL2Api.h"
#include "sec2.h"

extern "C" {

/* FIXME */
struct sec2_mdapi_t {
	SECL2Api			*rep;
};
struct sec2_mdspi_t : public SECL2Spi {
	sec2_on_connected		on_connected_;
	sec2_on_disconnected		on_disconnected_;
	sec2_on_user_login		on_user_login_;
	sec2_on_user_logout		on_user_logout_;
	sec2_on_subscribe_market_data	on_subscribe_market_data_;
	sec2_on_unsubscribe_market_data	on_unsubscribe_market_data_;
	sec2_on_subscribe_all		on_subscribe_all_;
	sec2_on_unsubscribe_all		on_unsubscribe_all_;
	sec2_on_modify_password		on_modify_password_;
	sec2_on_heartbeat_lost		on_heartbeat_lost_;
	sec2_on_sh3113			on_sh3113_;
	sec2_on_sh3115			on_sh3115_;
	sec2_on_sh3201			on_sh3201_;
	sec2_on_sh3202			on_sh3202_;
	/* make gcc happy */
	virtual ~sec2_mdspi_t() {};
	void OnConnected() {
		if (on_connected_)
			(*on_connected_)();
	}
	void OnDisconnected(int nReason) {
		if (on_disconnected_)
			(*on_disconnected_)(nReason);
	}
	void OnRspUserLogin(struct ErrorRtnField *pErrorField) {
		if (on_user_login_)
			(*on_user_login_)(pErrorField);
	}
	void OnRspUserLogout(struct ErrorRtnField *pErrorField) {
		if (on_user_logout_)
			(*on_user_logout_)(pErrorField);
	}
	void OnRspSubscribeMarketData(struct ErrorRtnField *pErrorField) {
		if (on_subscribe_market_data_)
			(*on_subscribe_market_data_)(pErrorField);
	}
	void OnRspUnSubscribeMarketData(struct ErrorRtnField *pErrorField) {
		if (on_unsubscribe_market_data_)
			(*on_unsubscribe_market_data_)(pErrorField);
	}
	void OnRspSubscribeAll(struct ErrorRtnField *pErrorField) {
		if (on_subscribe_all_)
			(*on_subscribe_all_)(pErrorField);
	}
	void OnRspUnSubscribeAll(struct ErrorRtnField *pErrorField) {
		if (on_unsubscribe_all_)
			(*on_unsubscribe_all_)(pErrorField);
	}
	void OnRspModifyPassword(struct ErrorRtnField *pErrorField) {
		if (on_modify_password_)
			(*on_modify_password_)(pErrorField);
	}
	void OnHeartBeatLost(struct ErrorRtnField *pErrorField) {
		if (on_heartbeat_lost_)
			(*on_heartbeat_lost_)(pErrorField);
	}
	void OnSH3113(struct SHFAST30_UA3113_t *quote) {
		if (on_sh3113_)
			on_sh3113_(quote);
	}
	void OnSH3115(struct SHFAST30_UA3115_t *quote) {
		if (on_sh3115_)
			on_sh3115_(quote);
	}
	void OnSH3201(struct SHFAST30_UA3201_t *quote) {
		if (on_sh3201_)
			on_sh3201_(quote);
	}
	void OnSH3202(struct SHFAST30_UA3202_t *quote) {
		if (on_sh3202_)
			on_sh3202_(quote);
	}
};

sec2_mdapi_t *sec2_mdapi_create() {
	sec2_mdapi_t *mdapi = new sec2_mdapi_t;
	mdapi->rep = SECL2Api::CreateDFITCMdApi();
	return mdapi;
}

void sec2_mdapi_destroy(sec2_mdapi_t *mdapi) {
	if (mdapi) {
		SECL2Api::DestoryDFITCMdApi(mdapi->rep);
		delete mdapi;
	}
}

/* FIXME */
int sec2_mdapi_connect(sec2_mdapi_t *mdapi, char *addr, sec2_mdspi_t *mdspi, unsigned isudp) {
	if (mdapi)
		return mdapi->rep->Connect(addr, mdspi, isudp);
	return -1;
}

/* FIXME */
int sec2_mdapi_login_user(sec2_mdapi_t *mdapi, struct SECUserLoginField *userlogin) {
	if (mdapi)
		return mdapi->rep->ReqUserLogin(userlogin);
	return -1;
}

/* FIXME */
int sec2_mdapi_logout_user(sec2_mdapi_t *mdapi, struct SECUserLogoutField *userlogout) {
	if (mdapi)
		return mdapi->rep->ReqUserLogout(userlogout);
	return -1;
}

/* FIXME */
int sec2_mdapi_subscribe_market_data(sec2_mdapi_t *mdapi, char **instruments, int count) {
	if (mdapi)
		return mdapi->rep->SubscribeMarketData(instruments, count);
	return -1;
}

/* FIXME */
int sec2_mdapi_unsubscribe_market_data(sec2_mdapi_t *mdapi, char **instruments, int count) {
	if (mdapi)
		return mdapi->rep->UnSubscribeMarketData(instruments, count);
	return -1;
}

/* FIXME */
int sec2_mdapi_subscribe_all(sec2_mdapi_t *mdapi) {
	if (mdapi)
		return mdapi->rep->SubscribeAll();
	return -1;
}

/* FIXME */
int sec2_mdapi_unsubscribe_all(sec2_mdapi_t *mdapi) {
	if (mdapi)
		return mdapi->rep->UnSubscribeAll();
	return -1;
}

/* FIXME */
int sec2_mdapi_modify_password(sec2_mdapi_t *mdapi, struct SECPasswdChangeField *pcf) {
	if (mdapi)
		return mdapi->rep->ReqChangePassword(pcf);
	return -1;
}

sec2_mdspi_t *sec2_mdspi_create() {
	return new sec2_mdspi_t;
}

void sec2_mdspi_destroy(sec2_mdspi_t *mdspi) {
	if (mdspi)
		delete mdspi;
}

void sec2_mdspi_on_connected(sec2_mdspi_t *mdspi, sec2_on_connected func) {
	if (mdspi && func)
		mdspi->on_connected_ = func;
}

void sec2_mdspi_on_disconnected(sec2_mdspi_t *mdspi, sec2_on_disconnected func) {
	if (mdspi && func)
		mdspi->on_disconnected_ = func;
}

void sec2_mdspi_on_user_login(sec2_mdspi_t *mdspi, sec2_on_user_login func) {
	if (mdspi && func)
		mdspi->on_user_login_ = func;
}

void sec2_mdspi_on_user_logout(sec2_mdspi_t *mdspi, sec2_on_user_logout func) {
	if (mdspi && func)
		mdspi->on_user_logout_ = func;
}

void sec2_mdspi_on_subscribe_market_data(sec2_mdspi_t *mdspi, sec2_on_subscribe_market_data func) {
	if (mdspi && func)
		mdspi->on_subscribe_market_data_ = func;
}

void sec2_mdspi_on_unsubscribe_market_data(sec2_mdspi_t *mdspi, sec2_on_unsubscribe_market_data func) {
	if (mdspi && func)
		mdspi->on_unsubscribe_market_data_ = func;
}

void sec2_mdspi_on_subscribe_all(sec2_mdspi_t *mdspi, sec2_on_subscribe_all func) {
	if (mdspi && func)
		mdspi->on_subscribe_all_ = func;
}

void sec2_mdspi_on_unsubscribe_all(sec2_mdspi_t *mdspi, sec2_on_unsubscribe_all func) {
	if (mdspi && func)
		mdspi->on_unsubscribe_all_ = func;
}

void sec2_mdspi_on_modify_password(sec2_mdspi_t *mdspi, sec2_on_modify_password func) {
	if (mdspi && func)
		mdspi->on_modify_password_ = func;
}

void sec2_mdspi_on_heartbeat_lost(sec2_mdspi_t *mdspi, sec2_on_heartbeat_lost func) {
	if (mdspi && func)
		mdspi->on_heartbeat_lost_ = func;
}

void sec2_mdspi_on_sh3113(sec2_mdspi_t *mdspi, sec2_on_sh3113 func) {
	if (mdspi && func)
		mdspi->on_sh3113_ = func;
}

void sec2_mdspi_on_sh3115(sec2_mdspi_t *mdspi, sec2_on_sh3115 func) {
	if (mdspi && func)
		mdspi->on_sh3115_ = func;
}

void sec2_mdspi_on_sh3201(sec2_mdspi_t *mdspi, sec2_on_sh3201 func) {
	if (mdspi && func)
		mdspi->on_sh3201_ = func;
}

void sec2_mdspi_on_sh3202(sec2_mdspi_t *mdspi, sec2_on_sh3202 func) {
	if (mdspi && func)
		mdspi->on_sh3202_ = func;
}

}


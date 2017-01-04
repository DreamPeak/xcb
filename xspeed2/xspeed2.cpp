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

#include "DFITCL2Api.h"
#include "xspeed2.h"

extern "C" {

/* FIXME */
struct xspeed_l2api_t {
	DFITCL2Api				*rep;
};
struct xspeed_l2spi_t : public DFITCL2Spi {
	xspeed_on_front_connected		on_front_connected_;
	xspeed_on_front_disconnected		on_front_disconnected_;
	xspeed_on_user_login			on_user_login_;
	xspeed_on_user_logout			on_user_logout_;
	xspeed_on_subscribe_market_data		on_subscribe_market_data_;
	xspeed_on_unsubscribe_market_data	on_unsubscribe_market_data_;
	xspeed_on_subscribe_all			on_subscribe_all_;
	xspeed_on_unsubscribe_all		on_unsubscribe_all_;
	xspeed_on_best_and_deep			on_best_and_deep_;
	/* make gcc happy */
	virtual ~xspeed_l2spi_t() {};
	void OnConnected() {
		if (on_front_connected_)
			(*on_front_connected_)();
	}
	void OnDisconnected(int pReason) {
		if (on_front_disconnected_)
			(*on_front_disconnected_)(pReason);
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
	void OnBestAndDeep(struct MDBestAndDeep *pQuote) {
		if (on_best_and_deep_)
			(*on_best_and_deep_)(pQuote);
	}
};

xspeed_l2api_t  *xspeed_l2api_create(void) {
	xspeed_l2api_t *l2api = new xspeed_l2api_t;

	l2api->rep = DFITCL2Api::CreateDFITCMdApi();
	return l2api;
}

void xspeed_l2api_destory(xspeed_l2api_t *l2api) {
	if (l2api) {
		DFITCL2Api::DestoryDFITCMdApi(l2api->rep);
		delete l2api;
	}
}

int xspeed_l2api_connect(xspeed_l2api_t *l2api, char *svraddr,
	xspeed_l2spi_t *l2spi, unsigned int quote_type) {
	if (l2api && l2spi)
		return l2api->rep->Connect(svraddr, l2spi, quote_type);
	return 1;
}

int xspeed_l2api_user_login(xspeed_l2api_t *l2api, struct DFITCUserLoginField *userlogin) {
	if (l2api)
		return l2api->rep->ReqUserLogin(userlogin);
	return 1;
}

int xspeed_l2api_user_logout(xspeed_l2api_t *l2api, struct DFITCUserLogoutField *userlogout) {
	if (l2api)
		return l2api->rep->ReqUserLogout(userlogout);
	return 1;
}

int xspeed_l2api_subscribe_market_data(xspeed_l2api_t *l2api, char **instruments, int count) {
	if (l2api)
		return l2api->rep->SubscribeMarketData(instruments, count);
	return 1;
}

int xspeed_l2api_unsubscribe_market_data(xspeed_l2api_t *l2api, char **instruments, int count) {
	if (l2api)
		return l2api->rep->UnSubscribeMarketData(instruments, count);
	return 1;
}

int xspeed_l2api_subscribe_all(xspeed_l2api_t *l2api) {
	if (l2api)
		return l2api->rep->SubscribeAll();
	return 1;
}

int xspeed_l2api_unsubscribe_all(xspeed_l2api_t *l2api) {
	if (l2api)
		return l2api->rep->UnSubscribeAll();
	return 1;
}

xspeed_l2spi_t *xspeed_l2spi_create(void) {
	return new xspeed_l2spi_t;
}

void xspeed_l2spi_destroy(xspeed_l2spi_t *l2spi) {
	if (l2spi)
		delete l2spi;
}

void xspeed_l2spi_on_front_connected(xspeed_l2spi_t *l2spi, xspeed_on_front_connected func) {
	if (l2spi && func)
		l2spi->on_front_connected_ = func;
}

void xspeed_l2spi_on_front_disconnected(xspeed_l2spi_t *l2spi, xspeed_on_front_disconnected func) {
	if (l2spi && func)
		l2spi->on_front_disconnected_ = func;
}

void xspeed_l2spi_on_user_login(xspeed_l2spi_t *l2spi, xspeed_on_user_login func) {
	if (l2spi && func)
		l2spi->on_user_login_ = func;
}

void xspeed_l2spi_on_user_logout(xspeed_l2spi_t *l2spi, xspeed_on_user_logout func) {
	if (l2spi && func)
		l2spi->on_user_logout_ = func;
}

void xspeed_l2spi_on_subscribe_market_data(xspeed_l2spi_t *l2spi,
	xspeed_on_subscribe_market_data func) {
	if (l2spi && func)
		l2spi->on_subscribe_market_data_ = func;
}

void xspeed_l2spi_on_unsubscribe_market_data(xspeed_l2spi_t *l2spi,
	xspeed_on_unsubscribe_market_data func) {
	if (l2spi && func)
		l2spi->on_unsubscribe_market_data_ = func;
}

void xspeed_l2spi_on_subscribe_all(xspeed_l2spi_t *l2spi, xspeed_on_subscribe_all func) {
	if (l2spi && func)
		l2spi->on_subscribe_all_ = func;
}

void xspeed_l2spi_on_unsubscribe_all(xspeed_l2spi_t *l2spi, xspeed_on_unsubscribe_all func) {
	if (l2spi && func)
		l2spi->on_unsubscribe_all_ = func;
}

void xspeed_l2spi_on_best_and_deep(xspeed_l2spi_t *l2spi, xspeed_on_best_and_deep func) {
	if (l2spi && func)
		l2spi->on_best_and_deep_ = func;
}

}


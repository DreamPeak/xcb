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

#ifndef XSPEED_INCLUDED
#define XSPEED_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include "DFITCApiDataType.h"
#include "DFITCApiStruct.h"

/* FIXME: exported types */
typedef struct xspeed_mdapi_t xspeed_mdapi_t;
typedef struct xspeed_mdspi_t xspeed_mdspi_t;
typedef void (*xspeed_on_front_connected)(void);
typedef void (*xspeed_on_front_disconnected)(int reason);
typedef void (*xspeed_on_error)(struct DFITCErrorRtnField *rspinfo);
typedef void (*xspeed_on_user_login)(struct DFITCUserLoginInfoRtnField *userlogin,
		struct DFITCErrorRtnField *rspinfo);
typedef void (*xspeed_on_user_logout)(struct DFITCUserLogoutInfoRtnField *userlogout,
		struct DFITCErrorRtnField *rspinfo);
typedef void (*xspeed_on_tradingday)(struct DFITCTradingDayRtnField *tradingday);
typedef void (*xspeed_on_subscribe_market_data)(struct DFITCSpecificInstrumentField *instrument,
		struct DFITCErrorRtnField *rspinfo);
typedef void (*xspeed_on_unsubscribe_market_data)(struct DFITCSpecificInstrumentField *instrument,
		struct DFITCErrorRtnField *rspinfo);
typedef void (*xspeed_on_deep_market_data)(struct DFITCDepthMarketDataField * deepmd);
typedef void (*xspeed_on_subscribe_quote_response)(struct DFITCSpecificInstrumentField *instrument,
		struct DFITCErrorRtnField *rspinfo);
typedef void (*xspeed_on_unsubscribe_quote_response)(struct DFITCSpecificInstrumentField *instrument,
		struct DFITCErrorRtnField *rspinfo);
typedef void (*xspeed_on_quote_response)(struct DFITCQuoteSubscribeRtnField *quotersp);

/* exported functions */
extern xspeed_mdapi_t *xspeed_mdapi_create(void);
extern void            xspeed_mdapi_destroy(xspeed_mdapi_t *mdapi);
extern int             xspeed_mdapi_init(xspeed_mdapi_t *mdapi, char *svraddr, xspeed_mdspi_t *mdspi);
extern int             xspeed_mdapi_login_user(xspeed_mdapi_t *mdapi,
			struct DFITCUserLoginField *userlogin);
extern int             xspeed_mdapi_logout_user(xspeed_mdapi_t *mdapi,
			struct DFITCUserLogoutField *userlogout);
extern int             xspeed_mdapi_get_tradingday(xspeed_mdapi_t *mdapi,
			struct DFITCTradingDayField *tradingday);
extern int             xspeed_mdapi_subscribe_market_data(xspeed_mdapi_t *mdapi,
			char **instruments, int count, int rid);
extern int             xspeed_mdapi_unsubscribe_market_data(xspeed_mdapi_t *mdapi,
			char **instruments, int count, int rid);
extern int             xspeed_mdapi_subscribe_quote_response(xspeed_mdapi_t *mdapi,
			char **instruments, int count, int rid);
extern int             xspeed_mdapi_unsubscribe_quote_response(xspeed_mdapi_t *mdapi,
			char **instruments, int count, int rid);
extern xspeed_mdspi_t *xspeed_mdspi_create(void);
extern void            xspeed_mdspi_destroy(xspeed_mdspi_t *mdspi);
extern void            xspeed_mdspi_on_front_connected(xspeed_mdspi_t *mdspi,
			xspeed_on_front_connected func);
extern void            xspeed_mdspi_on_front_disconnected(xspeed_mdspi_t *mdspi,
			xspeed_on_front_disconnected func);
extern void            xspeed_mdspi_on_error(xspeed_mdspi_t *mdspi, xspeed_on_error func);
extern void            xspeed_mdspi_on_user_login(xspeed_mdspi_t *mdspi, xspeed_on_user_login func);
extern void            xspeed_mdspi_on_user_logout(xspeed_mdspi_t *mdspi, xspeed_on_user_logout func);
extern void            xspeed_mdspi_on_tradingday(xspeed_mdspi_t *mdspi, xspeed_on_tradingday func);
extern void            xspeed_mdspi_on_subscribe_market_data(xspeed_mdspi_t *mdspi,
			xspeed_on_subscribe_market_data func);
extern void            xspeed_mdspi_on_unsubscribe_market_data(xspeed_mdspi_t *mdspi,
			xspeed_on_unsubscribe_market_data func);
extern void            xspeed_mdspi_on_deep_market_data(xspeed_mdspi_t *mdspi,
			xspeed_on_deep_market_data func);
extern void            xspeed_mdspi_on_subscribe_quote_response(xspeed_mdspi_t *mdspi,
			xspeed_on_subscribe_quote_response func);
extern void            xspeed_mdspi_on_unsubscribe_quote_response(xspeed_mdspi_t *mdspi,
			xspeed_on_unsubscribe_quote_response func);
extern void            xspeed_mdspi_on_quote_response(xspeed_mdspi_t *mdspi,
			xspeed_on_quote_response func);

#ifdef __cplusplus
}
#endif

#endif /* XSPEED_INCLUDED */


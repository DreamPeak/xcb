/*
 * Copyright (c) 2013-2015, Dalian Futures Information Technology Co., Ltd.
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

#ifndef CTP_INCLUDED
#define CTP_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include "ThostFtdcUserApiDataType.h"
#include "ThostFtdcUserApiStruct.h"

/* FIXME: exported types */
typedef struct ctp_mdapi_t ctp_mdapi_t;
typedef struct ctp_mdspi_t ctp_mdspi_t;
typedef void (*ctp_on_front_connected)(void);
typedef void (*ctp_on_front_disconnected)(int reason);
typedef void (*ctp_on_heartbeat_warning)(int timelapse);
typedef void (*ctp_on_user_login)(struct CThostFtdcRspUserLoginField *userlogin,
	struct CThostFtdcRspInfoField *rspinfo, int rid, int islast);
typedef void (*ctp_on_user_logout)(struct CThostFtdcUserLogoutField *userlogout,
	struct CThostFtdcRspInfoField *rspinfo, int rid, int islast);
typedef void (*ctp_on_error)(struct CThostFtdcRspInfoField *rspinfo, int rid, int islast);
typedef void (*ctp_on_subscribe_market_data)(struct CThostFtdcSpecificInstrumentField *instrument,
	struct CThostFtdcRspInfoField *rspinfo, int rid, int islast);
typedef void (*ctp_on_unsubscribe_market_data)(struct CThostFtdcSpecificInstrumentField *instrument,
	struct CThostFtdcRspInfoField *rspinfo, int rid, int islast);
typedef void (*ctp_on_subscribe_quote_response)(struct CThostFtdcSpecificInstrumentField *instrument,
	struct CThostFtdcRspInfoField *rspinfo, int rid, int islast);
typedef void (*ctp_on_unsubscribe_quote_response)(struct CThostFtdcSpecificInstrumentField *instrument,
	struct CThostFtdcRspInfoField *rspinfo, int rid, int islast);
typedef void (*ctp_on_deep_market_data)(struct CThostFtdcDepthMarketDataField *deepmd);
typedef void (*ctp_on_quote_response)(struct CThostFtdcForQuoteRspField *quotersp);

/* exported functions */
extern ctp_mdapi_t *ctp_mdapi_create(const char *flowpath, int isudp, int ismulticast);
extern void         ctp_mdapi_destroy(ctp_mdapi_t *mdapi);
extern void         ctp_mdapi_init(ctp_mdapi_t *mdapi);
extern int          ctp_mdapi_join(ctp_mdapi_t *mdapi);
extern const char  *ctp_mdapi_get_today(ctp_mdapi_t *mdapi);
extern void         ctp_mdapi_register_front(ctp_mdapi_t *mdapi, char *frontaddr);
extern void         ctp_mdapi_register_name_server(ctp_mdapi_t *mdapi, char *nsaddr);
/* FIXME */
extern void         ctp_mdapi_register_fens_user(ctp_mdapi_t *mdapi,
			struct CThostFtdcFensUserInfoField *fensuser);
extern void         ctp_mdapi_register_spi(ctp_mdapi_t *mdapi, ctp_mdspi_t *mdspi);
extern int          ctp_mdapi_subscribe_market_data(ctp_mdapi_t *mdapi, char **instruments, int count);
extern int          ctp_mdapi_unsubscribe_market_data(ctp_mdapi_t *mdapi, char **instruments, int count);
extern int          ctp_mdapi_subscribe_quote_response(ctp_mdapi_t *mdapi, char **instruments, int count);
extern int          ctp_mdapi_unsubscribe_quote_response(ctp_mdapi_t *mdapi, char **instruments, int count);
extern int          ctp_mdapi_login_user(ctp_mdapi_t *mdapi,
			struct CThostFtdcReqUserLoginField *userlogin, int rid);
extern int          ctp_mdapi_logout_user(ctp_mdapi_t *mdapi,
			struct CThostFtdcUserLogoutField *userlogout, int rid);
extern ctp_mdspi_t *ctp_mdspi_create(void);
extern void         ctp_mdspi_destroy(ctp_mdspi_t *mdspi);
extern void         ctp_mdspi_on_front_connected(ctp_mdspi_t *mdspi, ctp_on_front_connected func);
extern void         ctp_mdspi_on_front_disconnected(ctp_mdspi_t *mdspi, ctp_on_front_disconnected func);
extern void         ctp_mdspi_on_heartbeat_warning(ctp_mdspi_t *mdspi, ctp_on_heartbeat_warning func);
extern void         ctp_mdspi_on_user_login(ctp_mdspi_t *mdspi, ctp_on_user_login func);
extern void         ctp_mdspi_on_user_logout(ctp_mdspi_t *mdspi, ctp_on_user_logout func);
extern void         ctp_mdspi_on_error(ctp_mdspi_t *mdspi, ctp_on_error func);
extern void         ctp_mdspi_on_subscribe_market_data(ctp_mdspi_t *mdspi, ctp_on_subscribe_market_data func);
extern void         ctp_mdspi_on_unsubscribe_market_data(ctp_mdspi_t *mdspi,
			ctp_on_unsubscribe_market_data func);
extern void         ctp_mdspi_on_subscribe_quote_response(ctp_mdspi_t *mdspi,
			ctp_on_subscribe_quote_response func);
extern void         ctp_mdspi_on_unsubscribe_quote_response(ctp_mdspi_t *mdspi,
			ctp_on_unsubscribe_quote_response func);
extern void         ctp_mdspi_on_deep_market_data(ctp_mdspi_t *mdspi, ctp_on_deep_market_data func);
extern void         ctp_mdspi_on_quote_response(ctp_mdspi_t *mdspi, ctp_on_quote_response func);

#ifdef __cplusplus
}
#endif

#endif /* CTP_INCLUDED */


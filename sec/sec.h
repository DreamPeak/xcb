/*
 * Copyright (c) 2013-2015, Dalian Futures Information Technology Co., Ltd.
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

#ifndef SEC_INCLUDED
#define SEC_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include "DFITCSECApiDataType.h"
#include "DFITCSECApiStruct.h"

/* FIXME: exported types */
typedef struct sec_mdapi_t sec_mdapi_t;
typedef struct sec_mdspi_t sec_mdspi_t;
typedef void (*sec_on_front_connected)(void);
typedef void (*sec_on_front_disconnected)(int reason);
typedef void (*sec_on_error)(struct DFITCSECRspInfoField *rspinfo);
typedef void (*sec_on_stk_user_login)(struct DFITCSECRspUserLoginField *userlogin,
		struct DFITCSECRspInfoField *rspinfo);
typedef void (*sec_on_stk_user_logout)(struct DFITCSECRspUserLogoutField *userlogout,
		struct DFITCSECRspInfoField *rspinfo);
typedef void (*sec_on_stk_subscribe_market_data)(struct DFITCSECSpecificInstrumentField *instrument,
		struct DFITCSECRspInfoField *rspinfo);
typedef void (*sec_on_stk_unsubscribe_market_data)(struct DFITCSECSpecificInstrumentField *instrument,
		struct DFITCSECRspInfoField *rspinfo);
typedef void (*sec_on_stk_deep_market_data)(struct DFITCStockDepthMarketDataField *deepmd);
typedef void (*sec_on_sop_user_login)(struct DFITCSECRspUserLoginField *userlogin,
		struct DFITCSECRspInfoField *rspinfo);
typedef void (*sec_on_sop_user_logout)(struct DFITCSECRspUserLogoutField *userlogout,
		struct DFITCSECRspInfoField *rspinfo);
typedef void (*sec_on_sop_subscribe_market_data)(struct DFITCSECSpecificInstrumentField *instrument,
		struct DFITCSECRspInfoField *rspinfo);
typedef void (*sec_on_sop_unsubscribe_market_data)(struct DFITCSECSpecificInstrumentField *instrument,
		struct DFITCSECRspInfoField *rspinfo);
typedef void (*sec_on_sop_deep_market_data)(struct DFITCSOPDepthMarketDataField *deepmd);

/* FIXME: exported functions */
extern sec_mdapi_t *sec_mdapi_create(void);
extern void         sec_mdapi_destroy(sec_mdapi_t *mdapi);
extern int          sec_mdapi_init(sec_mdapi_t *mdapi, char *svraddr, sec_mdspi_t *mdspi);
extern int          sec_mdapi_stk_user_login(sec_mdapi_t *mdapi,
			struct DFITCSECReqUserLoginField *userlogin);
extern int          sec_mdapi_stk_user_logout(sec_mdapi_t *mdapi,
			struct DFITCSECReqUserLogoutField *userlogout);
extern int          sec_mdapi_stk_subscribe_market_data(sec_mdapi_t *mdapi,
			char **instruments, int count, int requsets);
extern int          sec_mdapi_stk_unsubscribe_market_data(sec_mdapi_t *mdapi,
			char **instruments, int count, int requsets);
extern int          sec_mdapi_sop_user_login(sec_mdapi_t *mdapi,
			struct DFITCSECReqUserLoginField *userlogin);
extern int          sec_mdapi_sop_user_logout(sec_mdapi_t *mdapi,
			struct DFITCSECReqUserLogoutField *userlogout);
extern int          sec_mdapi_sop_subscribe_market_data(sec_mdapi_t *mdapi,
			char **instruments, int count, int requsets);
extern int          sec_mdapi_sop_unsubscribe_market_data(sec_mdapi_t *mdapi,
			char **instruments, int count, int requsets);
extern sec_mdspi_t *sec_mdspi_create(void);
extern void         sec_mdspi_destroy(sec_mdspi_t *mdspi);
extern void         sec_mdspi_on_front_connected(sec_mdspi_t *mdspi, sec_on_front_connected func);
extern void         sec_mdspi_on_front_disconnected(sec_mdspi_t *mdspi, sec_on_front_disconnected func);
extern void         sec_mdspi_on_error(sec_mdspi_t *mdspi, sec_on_error func);
extern void         sec_mdspi_on_stk_user_login(sec_mdspi_t *mdspi, sec_on_stk_user_login func);
extern void         sec_mdspi_on_stk_user_logout(sec_mdspi_t *mdspi, sec_on_stk_user_logout func);
extern void         sec_mdspi_on_stk_subscribe_market_data(sec_mdspi_t *mdspi,
			sec_on_stk_subscribe_market_data func);
extern void         sec_mdspi_on_stk_unsubscribe_market_data(sec_mdspi_t *mdspi,
			sec_on_stk_unsubscribe_market_data func);
extern void         sec_mdspi_on_stk_deep_market_data(sec_mdspi_t *mdspi,
			sec_on_stk_deep_market_data func);
extern void         sec_mdspi_on_sop_user_login(sec_mdspi_t *mdspi, sec_on_sop_user_login func);
extern void         sec_mdspi_on_sop_user_logout(sec_mdspi_t *mdspi, sec_on_sop_user_logout func);
extern void         sec_mdspi_on_sop_subscribe_market_data(sec_mdspi_t *mdspi,
			sec_on_sop_subscribe_market_data func);
extern void         sec_mdspi_on_sop_unsubscribe_market_data(sec_mdspi_t *mdspi,
			sec_on_sop_unsubscribe_market_data func);
extern void         sec_mdspi_on_sop_deep_market_data(sec_mdspi_t *mdspi,
			sec_on_sop_deep_market_data func);

#ifdef __cplusplus
}
#endif

#endif /* SEC_INCLUDED */


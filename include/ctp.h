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

#ifndef CTP_INCLUDED
#define CTP_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include "ThostFtdcUserApiDataType.h"
#include "ThostFtdcUserApiStruct.h"

/* FIXME: exported types */
typedef struct ctp_tdapi_t ctp_tdapi_t;
typedef struct ctp_tdspi_t ctp_tdspi_t;
typedef void (*ctp_on_front_connected)(void);
typedef void (*ctp_on_front_disconnected)(int reason);
typedef void (*ctp_on_heartbeat_warning)(int timelapse);
typedef void (*ctp_on_error)(struct CThostFtdcRspInfoField *rspinfo, int rid, int islast);
typedef void (*ctp_on_authenticate)(struct CThostFtdcRspAuthenticateField *auth,
		struct CThostFtdcRspInfoField *rspinfo, int rid, int islast);
typedef void (*ctp_on_user_login)(struct CThostFtdcRspUserLoginField *userlogin,
		struct CThostFtdcRspInfoField *rspinfo, int rid, int islast);
typedef void (*ctp_on_user_logout)(struct CThostFtdcUserLogoutField *userlogout,
		struct CThostFtdcRspInfoField *rspinfo, int rid, int islast);
typedef void (*ctp_on_update_user_password)(struct CThostFtdcUserPasswordUpdateField *password,
		struct CThostFtdcRspInfoField *rspinfo, int rid, int islast);
typedef void (*ctp_on_update_account_password)(struct CThostFtdcTradingAccountPasswordUpdateField *password,
		struct CThostFtdcRspInfoField *rspinfo, int rid, int islast);
typedef void (*ctp_on_confirm_settlement)(struct CThostFtdcSettlementInfoConfirmField *stmtconfirm,
		struct CThostFtdcRspInfoField *rspinfo, int rid, int islast);
typedef void (*ctp_on_insert_order)(struct CThostFtdcInputOrderField *order,
		struct CThostFtdcRspInfoField *rspinfo, int rid, int islast);
typedef void (*ctp_on_order_action)(struct CThostFtdcInputOrderActionField *orderaction,
		struct CThostFtdcRspInfoField *rspinfo, int rid, int islast);
typedef void (*ctp_on_err_insert_order)(struct CThostFtdcInputOrderField *order,
		struct CThostFtdcRspInfoField *rspinfo);
typedef void (*ctp_on_err_order_action)(struct CThostFtdcOrderActionField *orderation,
		struct CThostFtdcRspInfoField *rspinfo);
typedef void (*ctp_on_order)(struct CThostFtdcOrderField *order);
typedef void (*ctp_on_trade)(struct CThostFtdcTradeField *trade);
typedef void (*ctp_on_insert_parked_order)(struct CThostFtdcParkedOrderField *order,
		struct CThostFtdcRspInfoField *rspinfo, int rid, int islast);
typedef void (*ctp_on_parked_order_action)(struct CThostFtdcParkedOrderActionField *orderaction,
		struct CThostFtdcRspInfoField *rspinfo, int rid, int islast);
typedef void (*ctp_on_remove_parked_order)(struct CThostFtdcRemoveParkedOrderField *order,
		struct CThostFtdcRspInfoField *rspinfo, int rid, int islast);
typedef void (*ctp_on_remove_parked_order_action)(struct CThostFtdcRemoveParkedOrderActionField *orderaction,
		struct CThostFtdcRspInfoField *rspinfo, int rid, int islast);
typedef void (*ctp_on_instrument_status)(struct CThostFtdcInstrumentStatusField *status);
typedef void (*ctp_on_trading_notice)(struct CThostFtdcTradingNoticeInfoField *notice);
typedef void (*ctp_on_query_max_order_volume)(struct CThostFtdcQueryMaxOrderVolumeField *volume,
		struct CThostFtdcRspInfoField *rspinfo, int rid, int islast);
typedef void (*ctp_on_query_order)(struct CThostFtdcOrderField *order,
		struct CThostFtdcRspInfoField *rspinfo, int rid, int islast);
typedef void (*ctp_on_query_trade)(struct CThostFtdcTradeField *trader,
		struct CThostFtdcRspInfoField *rspinfo, int rid, int islast);
typedef void (*ctp_on_query_position)(struct CThostFtdcInvestorPositionField *pstn,
		struct CThostFtdcRspInfoField *rspinfo, int rid, int islast);
typedef void (*ctp_on_query_account)(struct CThostFtdcTradingAccountField *account,
		struct CThostFtdcRspInfoField *rspinfo, int rid, int islast);

/* FIXME: exported functions */
extern ctp_tdapi_t *ctp_tdapi_create(const char *flowpath);
extern void         ctp_tdapi_destroy(ctp_tdapi_t *tdapi);
extern void         ctp_tdapi_register_front(ctp_tdapi_t *tdapi, char *frontaddr);
extern void         ctp_tdapi_register_name_server(ctp_tdapi_t *tdapi, char *nsaddr);
extern void         ctp_tdapi_register_fens_user(ctp_tdapi_t *tdapi,
			struct CThostFtdcFensUserInfoField *fensuser);
extern void         ctp_tdapi_register_spi(ctp_tdapi_t *tdapi, ctp_tdspi_t *tdspi);
extern void         ctp_tdapi_subscribe_private(ctp_tdapi_t *tdapi, THOST_TE_RESUME_TYPE resumetype);
extern void         ctp_tdapi_subscribe_public(ctp_tdapi_t *tdapi, THOST_TE_RESUME_TYPE resumetype);
extern void         ctp_tdapi_init(ctp_tdapi_t *tdapi);
extern int          ctp_tdapi_join(ctp_tdapi_t *tdapi);
extern int          ctp_tdapi_authenticate(ctp_tdapi_t *tdapi,
			struct CThostFtdcReqAuthenticateField *auth, int rid);
extern int          ctp_tdapi_login_user(ctp_tdapi_t *tdapi,
			struct CThostFtdcReqUserLoginField *userlogin, int rid);
extern int          ctp_tdapi_logout_user(ctp_tdapi_t *tdapi,
			struct CThostFtdcUserLogoutField *userlogout, int rid);
extern const char  *ctp_tdapi_get_version(ctp_tdapi_t *tdapi);
extern const char  *ctp_tdapi_get_tradingday(ctp_tdapi_t *tdapi);
extern int          ctp_tdapi_update_user_password(ctp_tdapi_t *tdapi,
			struct CThostFtdcUserPasswordUpdateField *password, int rid);
extern int          ctp_tdapi_update_account_password(ctp_tdapi_t *tdapi,
			struct CThostFtdcTradingAccountPasswordUpdateField *password, int rid);
extern int          ctp_tdapi_confirm_settlement(ctp_tdapi_t *tdapi,
			struct CThostFtdcSettlementInfoConfirmField *stmtconfirm, int rid);
extern int          ctp_tdapi_insert_order(ctp_tdapi_t *tdapi,
			struct CThostFtdcInputOrderField *order, int rid);
extern int          ctp_tdapi_order_action(ctp_tdapi_t *tdapi,
			struct CThostFtdcInputOrderActionField *orderaction, int rid);
extern int          ctp_tdapi_insert_parked_order(ctp_tdapi_t *tdapi,
			struct CThostFtdcParkedOrderField *order, int rid);
extern int          ctp_tdapi_parked_order_action(ctp_tdapi_t *tdapi,
			struct CThostFtdcParkedOrderActionField *orderaction, int rid);
extern int          ctp_tdapi_remove_parked_order(ctp_tdapi_t *tdapi,
			struct CThostFtdcRemoveParkedOrderField *order, int rid);
extern int          ctp_tdapi_remove_parked_order_action(ctp_tdapi_t *tdapi,
			struct CThostFtdcRemoveParkedOrderActionField *orderaction, int rid);
extern int          ctp_tdapi_query_max_order_volume(ctp_tdapi_t *tdapi,
			struct CThostFtdcQueryMaxOrderVolumeField *volume, int rid);
extern int          ctp_tdapi_query_order(ctp_tdapi_t *tdapi, struct CThostFtdcQryOrderField *order, int rid);
extern int          ctp_tdapi_query_trade(ctp_tdapi_t *tdapi, struct CThostFtdcQryTradeField *trade, int rid);
extern int          ctp_tdapi_query_position(ctp_tdapi_t *tdapi,
			struct CThostFtdcQryInvestorPositionField *pstn, int rid);
extern int          ctp_tdapi_query_account(ctp_tdapi_t *tdapi,
			struct CThostFtdcQryTradingAccountField *account, int rid);
extern ctp_tdspi_t *ctp_tdspi_create(void);
extern void         ctp_tdspi_destroy(ctp_tdspi_t *tdspi);
extern void         ctp_tdspi_on_front_connected(ctp_tdspi_t *tdspi, ctp_on_front_connected func);
extern void         ctp_tdspi_on_front_disconnected(ctp_tdspi_t *tdspi, ctp_on_front_disconnected func);
extern void         ctp_tdspi_on_heartbeat_warning(ctp_tdspi_t *tdspi, ctp_on_heartbeat_warning func);
extern void         ctp_tdspi_on_authenticate(ctp_tdspi_t *tdspi, ctp_on_authenticate func);
extern void         ctp_tdspi_on_user_login(ctp_tdspi_t *tdspi, ctp_on_user_login func);
extern void         ctp_tdspi_on_user_logout(ctp_tdspi_t *tdspi, ctp_on_user_logout func);
extern void         ctp_tdspi_on_update_user_password(ctp_tdspi_t *tdspi,
			ctp_on_update_user_password func);
extern void         ctp_tdspi_on_update_account_password(ctp_tdspi_t *tdspi,
			ctp_on_update_account_password func);
extern void         ctp_tdspi_on_confirm_settlement(ctp_tdspi_t *tdspi, ctp_on_confirm_settlement func);
extern void         ctp_tdspi_on_insert_order(ctp_tdspi_t *tdspi, ctp_on_insert_order func);
extern void         ctp_tdspi_on_order_action(ctp_tdspi_t *tdspi, ctp_on_order_action func);
extern void         ctp_tdspi_on_err_insert_order(ctp_tdspi_t *tdspi, ctp_on_err_insert_order func);
extern void         ctp_tdspi_on_err_order_action(ctp_tdspi_t *tdspi, ctp_on_err_order_action func);
extern void         ctp_tdspi_on_order(ctp_tdspi_t *tdspi, ctp_on_order func);
extern void         ctp_tdspi_on_trade(ctp_tdspi_t *tdspi, ctp_on_trade func);
extern void         ctp_tdspi_on_insert_parked_order(ctp_tdspi_t *tdspi, ctp_on_insert_parked_order func);
extern void         ctp_tdspi_on_parked_order_action(ctp_tdspi_t *tdspi, ctp_on_parked_order_action func);
extern void         ctp_tdspi_on_remove_parked_order(ctp_tdspi_t *tdspi, ctp_on_remove_parked_order func);
extern void         ctp_tdspi_on_remove_parked_order_action(ctp_tdspi_t *tdspi,
			ctp_on_remove_parked_order_action func);
extern void         ctp_tdspi_on_instrument_status(ctp_tdspi_t *tdspi, ctp_on_instrument_status func);
extern void         ctp_tdspi_on_trading_notice(ctp_tdspi_t *tdspi, ctp_on_trading_notice func);
extern void         ctp_tdspi_on_query_max_order_volume(ctp_tdspi_t *tdspi,
			ctp_on_query_max_order_volume func);
extern void         ctp_tdspi_on_query_order(ctp_tdspi_t *tdspi, ctp_on_query_order func);
extern void         ctp_tdspi_on_query_trade(ctp_tdspi_t *tdspi, ctp_on_query_trade func);
extern void         ctp_tdspi_on_query_position(ctp_tdspi_t *tdspi, ctp_on_query_position func);
extern void         ctp_tdspi_on_query_account(ctp_tdspi_t *tdspi, ctp_on_query_account func);

#ifdef __cplusplus
}
#endif

#endif /* CTP_INCLUDED */


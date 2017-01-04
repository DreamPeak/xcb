/*
 * Copyright (c) 2013-2017, Dalian Futures Information Technology Co., Ltd.
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
typedef void (*ctp_on_query_max_order_volume)(struct CThostFtdcQueryMaxOrderVolumeField *volume,
		struct CThostFtdcRspInfoField *rspinfo, int rid, int islast);
typedef void (*ctp_on_confirm_settlement)(struct CThostFtdcSettlementInfoConfirmField *stmtconfirm,
		struct CThostFtdcRspInfoField *rspinfo, int rid, int islast);
typedef void (*ctp_on_insert_order)(struct CThostFtdcInputOrderField *order,
		struct CThostFtdcRspInfoField *rspinfo, int rid, int islast);
typedef void (*ctp_on_order_action)(struct CThostFtdcInputOrderActionField *orderaction,
		struct CThostFtdcRspInfoField *rspinfo, int rid, int islast);
typedef void (*ctp_on_order)(struct CThostFtdcOrderField *order);
typedef void (*ctp_on_trade)(struct CThostFtdcTradeField *trade);
typedef void (*ctp_on_err_insert_order)(struct CThostFtdcInputOrderField *order,
		struct CThostFtdcRspInfoField *rspinfo);
typedef void (*ctp_on_err_order_action)(struct CThostFtdcOrderActionField *orderation,
		struct CThostFtdcRspInfoField *rspinfo);
typedef void (*ctp_on_err_conditional_order)(struct CThostFtdcErrorConditionalOrderField *order);
typedef void (*ctp_on_insert_parked_order)(struct CThostFtdcParkedOrderField *order,
		struct CThostFtdcRspInfoField *rspinfo, int rid, int islast);
typedef void (*ctp_on_parked_order_action)(struct CThostFtdcParkedOrderActionField *orderaction,
		struct CThostFtdcRspInfoField *rspinfo, int rid, int islast);
typedef void (*ctp_on_remove_parked_order)(struct CThostFtdcRemoveParkedOrderField *order,
		struct CThostFtdcRspInfoField *rspinfo, int rid, int islast);
typedef void (*ctp_on_remove_parked_order_action)(struct CThostFtdcRemoveParkedOrderActionField *orderaction,
		struct CThostFtdcRspInfoField *rspinfo, int rid, int islast);
typedef void (*ctp_on_bank_to_future)(struct CThostFtdcReqTransferField *transfer,
		struct CThostFtdcRspInfoField *rspinfo, int rid, int islast);
typedef void (*ctp_on_future_to_bank)(struct CThostFtdcReqTransferField *transfer,
		struct CThostFtdcRspInfoField *rspinfo, int rid, int islast);
typedef void (*ctp_on_bank_to_future_by_bank)(struct CThostFtdcRspTransferField *transfer);
typedef void (*ctp_on_future_to_bank_by_bank)(struct CThostFtdcRspTransferField *transfer);
typedef void (*ctp_on_repeal_bank_to_future_by_bank)(struct CThostFtdcRspRepealField *repeal);
typedef void (*ctp_on_repeal_future_to_bank_by_bank)(struct CThostFtdcRspRepealField *repeal);
typedef void (*ctp_on_bank_to_future_by_future)(struct CThostFtdcRspTransferField *transfer);
typedef void (*ctp_on_future_to_bank_by_future)(struct CThostFtdcRspTransferField *transfer);
typedef void (*ctp_on_err_bank_to_future_by_future)(struct CThostFtdcReqTransferField *transfer,
		struct CThostFtdcRspInfoField *rspinfo);
typedef void (*ctp_on_err_future_to_bank_by_future)(struct CThostFtdcReqTransferField *transfer,
		struct CThostFtdcRspInfoField *rspinfo);
typedef void (*ctp_on_repeal_bank_to_future_by_future)(struct CThostFtdcRspRepealField *repeal);
typedef void (*ctp_on_repeal_future_to_bank_by_future)(struct CThostFtdcRspRepealField *repeal);
typedef void (*ctp_on_repeal_bank_to_future_by_future_manual)(struct CThostFtdcRspRepealField *repeal);
typedef void (*ctp_on_repeal_future_to_bank_by_future_manual)(struct CThostFtdcRspRepealField *repeal);
typedef void (*ctp_on_err_repeal_bank_to_future_by_future_manual)(struct CThostFtdcReqRepealField *repeal,
		struct CThostFtdcRspInfoField *rspinfo);
typedef void (*ctp_on_err_repeal_future_to_bank_by_future_manual)(struct CThostFtdcReqRepealField *repeal,
		struct CThostFtdcRspInfoField *rspinfo);
typedef void (*ctp_on_query_bank_account)(struct CThostFtdcReqQueryAccountField *account,
		struct CThostFtdcRspInfoField *rspinfo, int rid, int islast);
typedef void (*ctp_on_query_bank_account_by_future)(struct CThostFtdcNotifyQueryAccountField *account);
typedef void (*ctp_on_err_query_bank_account_by_future)(struct CThostFtdcReqQueryAccountField *account,
		struct CThostFtdcRspInfoField *rspinfo);
typedef void (*ctp_on_open_account_by_bank)(struct CThostFtdcOpenAccountField *account);
typedef void (*ctp_on_cancel_account_by_bank)(struct CThostFtdcCancelAccountField *account);
typedef void (*ctp_on_change_account_by_bank)(struct CThostFtdcChangeAccountField *account);
typedef void (*ctp_on_query_settlement)(struct CThostFtdcSettlementInfoField *stmt,
		struct CThostFtdcRspInfoField *rspinfo, int rid, int islast);
typedef void (*ctp_on_query_settlement_confirm)(struct CThostFtdcSettlementInfoConfirmField *stmtconfirm,
		struct CThostFtdcRspInfoField *rspinfo, int rid, int islast);
typedef void (*ctp_on_query_cfmmc_account_key)(struct CThostFtdcCFMMCTradingAccountKeyField *accountkey,
		struct CThostFtdcRspInfoField *rspinfo, int rid, int islast);
typedef void (*ctp_on_query_order)(struct CThostFtdcOrderField *order,
		struct CThostFtdcRspInfoField *rspinfo, int rid, int islast);
typedef void (*ctp_on_query_trade)(struct CThostFtdcTradeField *trader,
		struct CThostFtdcRspInfoField *rspinfo, int rid, int islast);
typedef void (*ctp_on_query_parked_order)(struct CThostFtdcParkedOrderField *order,
		struct CThostFtdcRspInfoField *rspinfo, int rid, int islast);
typedef void (*ctp_on_query_parked_order_action)(struct CThostFtdcParkedOrderActionField *orderaction,
		struct CThostFtdcRspInfoField *rspinfo, int rid, int islast);
typedef void (*ctp_on_query_investor)(struct CThostFtdcInvestorField *investor,
		struct CThostFtdcRspInfoField *rspinfo, int rid, int islast);
typedef void (*ctp_on_query_position)(struct CThostFtdcInvestorPositionField *pstn,
		struct CThostFtdcRspInfoField *rspinfo, int rid, int islast);
typedef void (*ctp_on_query_position_detail)(struct CThostFtdcInvestorPositionDetailField *pstndetail,
		struct CThostFtdcRspInfoField *rspinfo, int rid, int islast);
typedef void (*ctp_on_query_position_combine)(struct CThostFtdcInvestorPositionCombineDetailField *pstncombine,
		struct CThostFtdcRspInfoField *rspinfo, int rid, int islast);
typedef void (*ctp_on_query_account)(struct CThostFtdcTradingAccountField *account,
		struct CThostFtdcRspInfoField *rspinfo, int rid, int islast);
typedef void (*ctp_on_query_code)(struct CThostFtdcTradingCodeField *code,
		struct CThostFtdcRspInfoField *rspinfo, int rid, int islast);
typedef void (*ctp_on_query_margin_rate)(struct CThostFtdcInstrumentMarginRateField *rate,
		struct CThostFtdcRspInfoField *rspinfo, int rid, int islast);
typedef void (*ctp_on_query_commission_rate)(struct CThostFtdcInstrumentCommissionRateField *rate,
		struct CThostFtdcRspInfoField *rspinfo, int rid, int islast);
typedef void (*ctp_on_query_ewarrant_offset)(struct CThostFtdcEWarrantOffsetField *offset,
		struct CThostFtdcRspInfoField *rspinfo, int rid, int islast);
typedef void (*ctp_on_query_broker_params)(struct CThostFtdcBrokerTradingParamsField *params,
		struct CThostFtdcRspInfoField *rspinfo, int rid, int islast);
typedef void (*ctp_on_query_broker_algos)(struct CThostFtdcBrokerTradingAlgosField *algos,
		struct CThostFtdcRspInfoField *rspinfo, int rid, int islast);
typedef void (*ctp_on_query_exchange)(struct CThostFtdcExchangeField *exchange,
		struct CThostFtdcRspInfoField *rspinfo, int rid, int islast);
typedef void (*ctp_on_query_instrument)(struct CThostFtdcInstrumentField *instrument,
		struct CThostFtdcRspInfoField *rspinfo, int rid, int islast);
typedef void (*ctp_on_query_deep_market_data)(struct CThostFtdcDepthMarketDataField *deepmd,
		struct CThostFtdcRspInfoField *rspinfo, int rid, int islast);
typedef void (*ctp_on_query_notice)(struct CThostFtdcNoticeField *notice,
		struct CThostFtdcRspInfoField *rspinfo, int rid, int islast);
typedef void (*ctp_on_query_trading_notice)(struct CThostFtdcTradingNoticeField *notice,
		struct CThostFtdcRspInfoField *rspinfo, int rid, int islast);
typedef void (*ctp_on_trading_notice)(struct CThostFtdcTradingNoticeInfoField *notice);
typedef void (*ctp_on_instrument_status)(struct CThostFtdcInstrumentStatusField *status);
typedef void (*ctp_on_query_account_register)(struct CThostFtdcAccountregisterField *accountregister,
		struct CThostFtdcRspInfoField *rspinfo, int rid, int islast);
typedef void (*ctp_on_query_transfer_bank)(struct CThostFtdcTransferBankField *bank,
		struct CThostFtdcRspInfoField *rspinfo, int rid, int islast);
typedef void (*ctp_on_query_contract_bank)(struct CThostFtdcContractBankField *bank,
		struct CThostFtdcRspInfoField *rspinfo, int rid, int islast);
typedef void (*ctp_on_query_transfer_serial)(struct CThostFtdcTransferSerialField *serial,
		struct CThostFtdcRspInfoField *rspinfo, int rid, int islast);

/* FIXME: exported functions */
extern ctp_tdapi_t *ctp_tdapi_create(const char *flowpath);
extern void         ctp_tdapi_destroy(ctp_tdapi_t *tdapi);
extern void         ctp_tdapi_register_front(ctp_tdapi_t *tdapi, char *frontaddr);
extern void         ctp_tdapi_register_name_server(ctp_tdapi_t *tdapi, char *nsaddr);
extern void         ctp_tdapi_register_fens_user(ctp_tdapi_t *tdapi,
			struct CThostFtdcFensUserInfoField *fensuser);
extern void         ctp_tdapi_register_spi(ctp_tdapi_t *tdapi, ctp_tdspi_t *tdspi);
extern void         ctp_tdapi_subscribe_private(ctp_tdapi_t *tdapi, enum THOST_TE_RESUME_TYPE resumetype);
extern void         ctp_tdapi_subscribe_public(ctp_tdapi_t *tdapi, enum THOST_TE_RESUME_TYPE resumetype);
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
extern int          ctp_tdapi_query_max_order_volume(ctp_tdapi_t *tdapi,
			struct CThostFtdcQueryMaxOrderVolumeField *volume, int rid);
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
extern int          ctp_tdapi_bank_to_future(ctp_tdapi_t *tdapi,
			struct CThostFtdcReqTransferField *transfer, int rid);
extern int          ctp_tdapi_future_to_bank(ctp_tdapi_t *tdapi,
			struct CThostFtdcReqTransferField *transfer, int rid);
extern int          ctp_tdapi_query_bank_account(ctp_tdapi_t *tdapi,
			struct CThostFtdcReqQueryAccountField *account, int rid);
extern int          ctp_tdapi_query_settlement(ctp_tdapi_t *tdapi,
			struct CThostFtdcQrySettlementInfoField *stmt, int rid);
extern int          ctp_tdapi_query_settlement_confirm(ctp_tdapi_t *tdapi,
			struct CThostFtdcQrySettlementInfoConfirmField *stmtconfirm, int rid);
extern int          ctp_tdapi_query_cfmmc_account_key(ctp_tdapi_t *tdapi,
			struct CThostFtdcQryCFMMCTradingAccountKeyField *accountkey, int rid);
extern int          ctp_tdapi_query_order(ctp_tdapi_t *tdapi, struct CThostFtdcQryOrderField *order, int rid);
extern int          ctp_tdapi_query_trade(ctp_tdapi_t *tdapi, struct CThostFtdcQryTradeField *trade, int rid);
extern int          ctp_tdapi_query_parked_order(ctp_tdapi_t *tdapi,
			struct CThostFtdcQryParkedOrderField *order, int rid);
extern int          ctp_tdapi_query_parked_order_action(ctp_tdapi_t *tdapi,
			struct CThostFtdcQryParkedOrderActionField *orderaction, int rid);
extern int          ctp_tdapi_query_investor(ctp_tdapi_t *tdapi,
			struct CThostFtdcQryInvestorField *investor, int rid);
extern int          ctp_tdapi_query_position(ctp_tdapi_t *tdapi,
			struct CThostFtdcQryInvestorPositionField *pstn, int rid);
extern int          ctp_tdapi_query_position_detail(ctp_tdapi_t *tdapi,
			struct CThostFtdcQryInvestorPositionDetailField *pstndetail, int rid);
extern int          ctp_tdapi_query_position_combine(ctp_tdapi_t *tdapi,
			struct CThostFtdcQryInvestorPositionCombineDetailField *pstncombine, int rid);
extern int          ctp_tdapi_query_account(ctp_tdapi_t *tdapi,
			struct CThostFtdcQryTradingAccountField *account, int rid);
extern int          ctp_tdapi_query_code(ctp_tdapi_t *tdapi,
			struct CThostFtdcQryTradingCodeField *code, int rid);
extern int          ctp_tdapi_query_margin_rate(ctp_tdapi_t *tdapi,
			struct CThostFtdcQryInstrumentMarginRateField *rate, int rid);
extern int          ctp_tdapi_query_commission_rate(ctp_tdapi_t *tdapi,
			struct CThostFtdcQryInstrumentCommissionRateField *rate, int rid);
extern int          ctp_tdapi_query_ewarrant_offset(ctp_tdapi_t *tdapi,
			struct CThostFtdcQryEWarrantOffsetField *offset, int rid);
extern int          ctp_tdapi_query_broker_params(ctp_tdapi_t *tdapi,
			struct CThostFtdcQryBrokerTradingParamsField *params, int rid);
extern int          ctp_tdapi_query_broker_algos(ctp_tdapi_t *tdapi,
			struct CThostFtdcQryBrokerTradingAlgosField *algos, int rid);
extern int          ctp_tdapi_query_exchange(ctp_tdapi_t *tdapi,
			struct CThostFtdcQryExchangeField *exchange, int rid);
extern int          ctp_tdapi_query_instrument(ctp_tdapi_t *tdapi,
			struct CThostFtdcQryInstrumentField *instrument, int rid);
extern int          ctp_tdapi_query_deep_market_data(ctp_tdapi_t *tdapi,
			struct CThostFtdcQryDepthMarketDataField *deepmd, int rid);
extern int          ctp_tdapi_query_notice(ctp_tdapi_t *tdapi,
			struct CThostFtdcQryNoticeField *notice, int rid);
extern int          ctp_tdapi_query_trading_notice(ctp_tdapi_t *tdapi,
			struct CThostFtdcQryTradingNoticeField *notice, int rid);
extern int          ctp_tdapi_query_account_register(ctp_tdapi_t *tdapi,
			struct CThostFtdcQryAccountregisterField *accountregister, int rid);
extern int          ctp_tdapi_query_transfer_bank(ctp_tdapi_t *tdapi,
			struct CThostFtdcQryTransferBankField *bank, int rid);
extern int          ctp_tdapi_query_contract_bank(ctp_tdapi_t *tdapi,
			struct CThostFtdcQryContractBankField *bank, int rid);
extern int          ctp_tdapi_query_transfer_serial(ctp_tdapi_t *tdapi,
			struct CThostFtdcQryTransferSerialField *serial, int rid);
extern ctp_tdspi_t *ctp_tdspi_create(void);
extern void         ctp_tdspi_destroy(ctp_tdspi_t *tdspi);
extern void         ctp_tdspi_on_front_connected(ctp_tdspi_t *tdspi, ctp_on_front_connected func);
extern void         ctp_tdspi_on_front_disconnected(ctp_tdspi_t *tdspi, ctp_on_front_disconnected func);
extern void         ctp_tdspi_on_heartbeat_warning(ctp_tdspi_t *tdspi, ctp_on_heartbeat_warning func);
extern void         ctp_tdspi_on_error(ctp_tdspi_t *tdspi, ctp_on_error func);
extern void         ctp_tdspi_on_authenticate(ctp_tdspi_t *tdspi, ctp_on_authenticate func);
extern void         ctp_tdspi_on_user_login(ctp_tdspi_t *tdspi, ctp_on_user_login func);
extern void         ctp_tdspi_on_user_logout(ctp_tdspi_t *tdspi, ctp_on_user_logout func);
extern void         ctp_tdspi_on_update_user_password(ctp_tdspi_t *tdspi,
			ctp_on_update_user_password func);
extern void         ctp_tdspi_on_update_account_password(ctp_tdspi_t *tdspi,
			ctp_on_update_account_password func);
extern void         ctp_tdspi_on_query_max_order_volume(ctp_tdspi_t *tdspi,
			ctp_on_query_max_order_volume func);
extern void         ctp_tdspi_on_confirm_settlement(ctp_tdspi_t *tdspi, ctp_on_confirm_settlement func);
extern void         ctp_tdspi_on_insert_order(ctp_tdspi_t *tdspi, ctp_on_insert_order func);
extern void         ctp_tdspi_on_order_action(ctp_tdspi_t *tdspi, ctp_on_order_action func);
extern void         ctp_tdspi_on_order(ctp_tdspi_t *tdspi, ctp_on_order func);
extern void         ctp_tdspi_on_trade(ctp_tdspi_t *tdspi, ctp_on_trade func);
extern void         ctp_tdspi_on_err_insert_order(ctp_tdspi_t *tdspi, ctp_on_err_insert_order func);
extern void         ctp_tdspi_on_err_order_action(ctp_tdspi_t *tdspi, ctp_on_err_order_action func);
extern void         ctp_tdspi_on_err_conditional_order(ctp_tdspi_t *tdspi, ctp_on_err_conditional_order func);
extern void         ctp_tdspi_on_insert_parked_order(ctp_tdspi_t *tdspi, ctp_on_insert_parked_order func);
extern void         ctp_tdspi_on_parked_order_action(ctp_tdspi_t *tdspi, ctp_on_parked_order_action func);
extern void         ctp_tdspi_on_remove_parked_order(ctp_tdspi_t *tdspi, ctp_on_remove_parked_order func);
extern void         ctp_tdspi_on_remove_parked_order_action(ctp_tdspi_t *tdspi,
			ctp_on_remove_parked_order_action func);
extern void         ctp_tdspi_on_bank_to_future(ctp_tdspi_t *tdspi, ctp_on_bank_to_future func);
extern void         ctp_tdspi_on_future_to_bank(ctp_tdspi_t *tdspi, ctp_on_future_to_bank func);
extern void         ctp_tdspi_on_bank_to_future_by_bank(ctp_tdspi_t *tdspi,
			ctp_on_bank_to_future_by_bank func);
extern void         ctp_tdspi_on_future_to_bank_by_bank(ctp_tdspi_t *tdspi,
			ctp_on_future_to_bank_by_bank func);
extern void         ctp_tdspi_on_repeal_bank_to_future_by_bank(ctp_tdspi_t *tdspi,
			ctp_on_repeal_bank_to_future_by_bank func);
extern void         ctp_tdspi_on_repeal_future_to_bank_by_bank(ctp_tdspi_t *tdspi,
			ctp_on_repeal_future_to_bank_by_bank func);
extern void         ctp_tdspi_on_bank_to_future_by_future(ctp_tdspi_t *tdspi,
			ctp_on_bank_to_future_by_future func);
extern void         ctp_tdspi_on_future_to_bank_by_future(ctp_tdspi_t *tdspi,
			ctp_on_future_to_bank_by_future func);
extern void         ctp_tdspi_on_err_bank_to_future_by_future(ctp_tdspi_t *tdspi,
			ctp_on_err_bank_to_future_by_future func);
extern void         ctp_tdspi_on_err_future_to_bank_by_future(ctp_tdspi_t *tdspi,
			ctp_on_err_future_to_bank_by_future func);
extern void         ctp_tdspi_on_repeal_bank_to_future_by_future(ctp_tdspi_t *tdspi,
			ctp_on_repeal_bank_to_future_by_future func);
extern void         ctp_tdspi_on_repeal_future_to_bank_by_future(ctp_tdspi_t *tdspi,
			ctp_on_repeal_future_to_bank_by_future func);
extern void         ctp_tdspi_on_repeal_bank_to_future_by_future_manual(ctp_tdspi_t *tdspi,
			ctp_on_repeal_bank_to_future_by_future_manual func);
extern void         ctp_tdspi_on_repeal_future_to_bank_by_future_manual(ctp_tdspi_t *tdspi,
			ctp_on_repeal_future_to_bank_by_future_manual func);
extern void         ctp_tdspi_on_err_repeal_bank_to_future_by_future_manual(ctp_tdspi_t *tdspi,
			ctp_on_err_repeal_bank_to_future_by_future_manual func);
extern void         ctp_tdspi_on_err_repeal_future_to_bank_by_future_manual(ctp_tdspi_t *tdspi,
			ctp_on_err_repeal_future_to_bank_by_future_manual func);
extern void         ctp_tdspi_on_query_bank_account(ctp_tdspi_t *tdspi, ctp_on_query_bank_account func);
extern void         ctp_tdspi_on_query_bank_account_by_future(ctp_tdspi_t *tdspi,
			ctp_on_query_bank_account_by_future func);
extern void         ctp_tdspi_on_err_query_bank_account_by_future(ctp_tdspi_t *tdspi,
			ctp_on_err_query_bank_account_by_future func);
extern void         ctp_tdspi_on_open_account_by_bank(ctp_tdspi_t *tdspi, ctp_on_open_account_by_bank func);
extern void         ctp_tdspi_on_cancel_account_by_bank(ctp_tdspi_t *tdspi,
			ctp_on_cancel_account_by_bank func);
extern void         ctp_tdspi_on_change_account_by_bank(ctp_tdspi_t *tdspi,
			ctp_on_change_account_by_bank func);
extern void         ctp_tdspi_on_query_settlement(ctp_tdspi_t *tdspi, ctp_on_query_settlement func);
extern void         ctp_tdspi_on_query_settlement_confirm(ctp_tdspi_t *tdspi,
			ctp_on_query_settlement_confirm func);
extern void         ctp_tdspi_on_query_cfmmc_account_key(ctp_tdspi_t *tdspi,
			ctp_on_query_cfmmc_account_key func);
extern void         ctp_tdspi_on_query_order(ctp_tdspi_t *tdspi, ctp_on_query_order func);
extern void         ctp_tdspi_on_query_trade(ctp_tdspi_t *tdspi, ctp_on_query_trade func);
extern void         ctp_tdspi_on_query_parked_order(ctp_tdspi_t *tdspi, ctp_on_query_parked_order func);
extern void         ctp_tdspi_on_query_parked_order_action(ctp_tdspi_t *tdspi,
			ctp_on_query_parked_order_action func);
extern void         ctp_tdspi_on_query_investor(ctp_tdspi_t *tdspi, ctp_on_query_investor func);
extern void         ctp_tdspi_on_query_position(ctp_tdspi_t *tdspi, ctp_on_query_position func);
extern void         ctp_tdspi_on_query_position_detail(ctp_tdspi_t *tdspi, ctp_on_query_position_detail func);
extern void         ctp_tdspi_on_query_position_combine(ctp_tdspi_t *tdspi,
			ctp_on_query_position_combine func);
extern void         ctp_tdspi_on_query_account(ctp_tdspi_t *tdspi, ctp_on_query_account func);
extern void         ctp_tdspi_on_query_code(ctp_tdspi_t *tdspi, ctp_on_query_code func);
extern void         ctp_tdspi_on_query_margin_rate(ctp_tdspi_t *tdspi, ctp_on_query_margin_rate func);
extern void         ctp_tdspi_on_query_commission_rate(ctp_tdspi_t *tdspi, ctp_on_query_commission_rate func);
extern void         ctp_tdspi_on_query_ewarrant_offset(ctp_tdspi_t *tdspi, ctp_on_query_ewarrant_offset func);
extern void         ctp_tdspi_on_query_broker_params(ctp_tdspi_t *tdspi, ctp_on_query_broker_params func);
extern void         ctp_tdspi_on_query_broker_algos(ctp_tdspi_t *tdspi, ctp_on_query_broker_algos func);
extern void         ctp_tdspi_on_query_exchange(ctp_tdspi_t *tdspi, ctp_on_query_exchange func);
extern void         ctp_tdspi_on_query_instrument(ctp_tdspi_t *tdspi, ctp_on_query_instrument func);
extern void         ctp_tdspi_on_query_deep_market_data(ctp_tdspi_t *tdspi,
			ctp_on_query_deep_market_data func);
extern void         ctp_tdspi_on_query_notice(ctp_tdspi_t *tdspi, ctp_on_query_notice func);
extern void         ctp_tdspi_on_query_trading_notice(ctp_tdspi_t *tdspi, ctp_on_query_trading_notice func);
extern void         ctp_tdspi_on_trading_notice(ctp_tdspi_t *tdspi, ctp_on_trading_notice func);
extern void         ctp_tdspi_on_instrument_status(ctp_tdspi_t *tdspi, ctp_on_instrument_status func);
extern void         ctp_tdspi_on_query_account_register(ctp_tdspi_t *tdspi,
			ctp_on_query_account_register func);
extern void         ctp_tdspi_on_query_transfer_bank(ctp_tdspi_t *tdspi, ctp_on_query_transfer_bank func);
extern void         ctp_tdspi_on_query_contract_bank(ctp_tdspi_t *tdspi, ctp_on_query_contract_bank func);
extern void         ctp_tdspi_on_query_transfer_serial(ctp_tdspi_t *tdspi, ctp_on_query_transfer_serial func);

#ifdef __cplusplus
}
#endif

#endif /* CTP_INCLUDED */


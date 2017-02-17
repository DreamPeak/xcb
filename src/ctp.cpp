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

#include <stddef.h>
#include "ThostFtdcTraderApi.h"
#include "ctp.h"

extern "C" {

/* FIXME */
struct ctp_tdapi_t {
	CThostFtdcTraderApi					*rep;
};
struct ctp_tdspi_t : public CThostFtdcTraderSpi {
	ctp_on_front_connected					on_front_connected_;
	ctp_on_front_disconnected				on_front_disconnected_;
	ctp_on_heartbeat_warning				on_heartbeat_warning_;
	ctp_on_error						on_error_;
	ctp_on_authenticate					on_authenticate_;
	ctp_on_user_login					on_user_login_;
	ctp_on_user_logout					on_user_logout_;
	ctp_on_update_user_password				on_update_user_password_;
	ctp_on_update_account_password				on_update_account_password_;
	ctp_on_query_max_order_volume				on_query_max_order_volume_;
	ctp_on_confirm_settlement				on_confirm_settlement_;
	ctp_on_insert_order					on_insert_order_;
	ctp_on_order_action					on_order_action_;
	ctp_on_order						on_order_;
	ctp_on_trade						on_trade_;
	ctp_on_err_insert_order					on_err_insert_order_;
	ctp_on_err_order_action					on_err_order_action_;
	ctp_on_err_conditional_order				on_err_conditional_order_;
	ctp_on_insert_parked_order				on_insert_parked_order_;
	ctp_on_parked_order_action				on_parked_order_action_;
	ctp_on_remove_parked_order				on_remove_parked_order_;
	ctp_on_remove_parked_order_action			on_remove_parked_order_action_;
	ctp_on_bank_to_future					on_bank_to_future_;
	ctp_on_future_to_bank					on_future_to_bank_;
	ctp_on_bank_to_future_by_bank				on_bank_to_future_by_bank_;
	ctp_on_future_to_bank_by_bank				on_future_to_bank_by_bank_;
	ctp_on_repeal_bank_to_future_by_bank			on_repeal_bank_to_future_by_bank_;
	ctp_on_repeal_future_to_bank_by_bank			on_repeal_future_to_bank_by_bank_;
	ctp_on_bank_to_future_by_future				on_bank_to_future_by_future_;
	ctp_on_future_to_bank_by_future				on_future_to_bank_by_future_;
	ctp_on_err_bank_to_future_by_future			on_err_bank_to_future_by_future_;
	ctp_on_err_future_to_bank_by_future			on_err_future_to_bank_by_future_;
	ctp_on_repeal_bank_to_future_by_future			on_repeal_bank_to_future_by_future_;
	ctp_on_repeal_future_to_bank_by_future			on_repeal_future_to_bank_by_future_;
	ctp_on_repeal_bank_to_future_by_future_manual		on_repeal_bank_to_future_by_future_manual_;
	ctp_on_repeal_future_to_bank_by_future_manual		on_repeal_future_to_bank_by_future_manual_;
	ctp_on_err_repeal_bank_to_future_by_future_manual	on_err_repeal_bank_to_future_by_future_manual_;
	ctp_on_err_repeal_future_to_bank_by_future_manual	on_err_repeal_future_to_bank_by_future_manual_;
	ctp_on_query_bank_account				on_query_bank_account_;
	ctp_on_query_bank_account_by_future			on_query_bank_account_by_future_;
	ctp_on_err_query_bank_account_by_future			on_err_query_bank_account_by_future_;
	ctp_on_open_account_by_bank				on_open_account_by_bank_;
	ctp_on_cancel_account_by_bank				on_cancel_account_by_bank_;
	ctp_on_change_account_by_bank				on_change_account_by_bank_;
	ctp_on_query_settlement					on_query_settlement_;
	ctp_on_query_settlement_confirm				on_query_settlement_confirm_;
	ctp_on_query_cfmmc_account_key				on_query_cfmmc_account_key_;
	ctp_on_query_order					on_query_order_;
	ctp_on_query_trade					on_query_trade_;
	ctp_on_query_parked_order				on_query_parked_order_;
	ctp_on_query_parked_order_action			on_query_parked_order_action_;
	ctp_on_query_investor					on_query_investor_;
	ctp_on_query_position					on_query_position_;
	ctp_on_query_position_detail				on_query_position_detail_;
	ctp_on_query_position_combine				on_query_position_combine_;
	ctp_on_query_account					on_query_account_;
	ctp_on_query_code					on_query_code_;
	ctp_on_query_margin_rate				on_query_margin_rate_;
	ctp_on_query_commission_rate				on_query_commission_rate_;
	ctp_on_query_ewarrant_offset				on_query_ewarrant_offset_;
	ctp_on_query_broker_params				on_query_broker_params_;
	ctp_on_query_broker_algos				on_query_broker_algos_;
	ctp_on_query_exchange					on_query_exchange_;
	ctp_on_query_instrument					on_query_instrument_;
	ctp_on_query_deep_market_data				on_query_deep_market_data_;
	ctp_on_query_notice					on_query_notice_;
	ctp_on_query_trading_notice				on_query_trading_notice_;
	ctp_on_trading_notice					on_trading_notice_;
	ctp_on_instrument_status				on_instrument_status_;
	ctp_on_query_account_register				on_query_account_register_;
	ctp_on_query_transfer_bank				on_query_transfer_bank_;
	ctp_on_query_contract_bank				on_query_contract_bank_;
	ctp_on_query_transfer_serial				on_query_transfer_serial_;
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
	void OnRspQueryMaxOrderVolume(CThostFtdcQueryMaxOrderVolumeField *pQueryMaxOrderVolume,
		CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
		if (on_query_max_order_volume_)
			(*on_query_max_order_volume_)(pQueryMaxOrderVolume,
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
	void OnErrRtnOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo) {
		if (on_err_insert_order_)
			(*on_err_insert_order_)(pInputOrder, pRspInfo);
	}
	void OnErrRtnOrderAction(CThostFtdcOrderActionField *pOrderAction, CThostFtdcRspInfoField *pRspInfo) {
		if (on_err_order_action_)
			(*on_err_order_action_)(pOrderAction, pRspInfo);
	}
	void OnRtnErrorConditionalOrder(CThostFtdcErrorConditionalOrderField *pErrorConditionalOrder) {
		if (on_err_conditional_order_)
			(*on_err_conditional_order_)(pErrorConditionalOrder);
	}
	void OnRspParkedOrderInsert(CThostFtdcParkedOrderField *pParkedOrder,
		CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
		if (on_insert_parked_order_)
			(*on_insert_parked_order_)(pParkedOrder, pRspInfo, nRequestID, bIsLast ? 1 : 0);
	}
	void OnRspParkedOrderAction(CThostFtdcParkedOrderActionField *pParkedOrderAction,
		CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
		if (on_parked_order_action_)
			(*on_parked_order_action_)(pParkedOrderAction, pRspInfo, nRequestID, bIsLast ? 1 : 0);
	}
	void OnRspRemoveParkedOrder(CThostFtdcRemoveParkedOrderField *pRemoveParkedOrder,
		CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
		if (on_remove_parked_order_)
			(*on_remove_parked_order_)(pRemoveParkedOrder, pRspInfo, nRequestID, bIsLast ? 1 : 0);
	}
	void OnRspRemoveParkedOrderAction(CThostFtdcRemoveParkedOrderActionField *pRemoveParkedOrderAction,
		CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
		if (on_remove_parked_order_action_)
			(*on_remove_parked_order_action_)(pRemoveParkedOrderAction,
				pRspInfo, nRequestID, bIsLast ? 1 : 0);
	}
	void OnRspFromBankToFutureByFuture(CThostFtdcReqTransferField *pReqTransfer,
		CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
		if (on_bank_to_future_)
			(*on_bank_to_future_)(pReqTransfer, pRspInfo, nRequestID, bIsLast ? 1 : 0);
	}
	void OnRspFromFutureToBankByFuture(CThostFtdcReqTransferField *pReqTransfer,
		CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
		if (on_future_to_bank_)
			(*on_future_to_bank_)(pReqTransfer, pRspInfo, nRequestID, bIsLast ? 1 : 0);
	}
	void OnRtnFromBankToFutureByBank(CThostFtdcRspTransferField *pRspTransfer) {
		if (on_bank_to_future_by_bank_)
			(*on_bank_to_future_by_bank_)(pRspTransfer);
	}
	void OnRtnFromFutureToBankByBank(CThostFtdcRspTransferField *pRspTransfer) {
		if (on_future_to_bank_by_bank_)
			(*on_future_to_bank_by_bank_)(pRspTransfer);
	}
	void OnRtnRepealFromBankToFutureByBank(CThostFtdcRspRepealField *pRspRepeal) {
		if (on_repeal_bank_to_future_by_bank_)
			(*on_repeal_bank_to_future_by_bank_)(pRspRepeal);
	}
	void OnRtnRepealFromFutureToBankByBank(CThostFtdcRspRepealField *pRspRepeal) {
		if (on_repeal_future_to_bank_by_bank_)
			(*on_repeal_future_to_bank_by_bank_)(pRspRepeal);
	}
	void OnRtnFromBankToFutureByFuture(CThostFtdcRspTransferField *pRspTransfer) {
		if (on_bank_to_future_by_future_)
			(*on_bank_to_future_by_future_)(pRspTransfer);
	}
	void OnRtnFromFutureToBankByFuture(CThostFtdcRspTransferField *pRspTransfer) {
		if (on_future_to_bank_by_future_)
			(*on_future_to_bank_by_future_)(pRspTransfer);
	}
	void OnErrRtnBankToFutureByFuture(CThostFtdcReqTransferField *pReqTransfer,
		CThostFtdcRspInfoField *pRspInfo) {
		if (on_err_bank_to_future_by_future_)
			(*on_err_bank_to_future_by_future_)(pReqTransfer, pRspInfo);
	}
	void OnErrRtnFutureToBankByFuture(CThostFtdcReqTransferField *pReqTransfer,
		CThostFtdcRspInfoField *pRspInfo) {
		if (on_err_future_to_bank_by_future_)
			(*on_err_future_to_bank_by_future_)(pReqTransfer, pRspInfo);
	}
	void OnRtnRepealFromBankToFutureByFuture(CThostFtdcRspRepealField *pRspRepeal) {
		if (on_repeal_bank_to_future_by_future_)
			(*on_repeal_bank_to_future_by_future_)(pRspRepeal);
	}
	void OnRtnRepealFromFutureToBankByFuture(CThostFtdcRspRepealField *pRspRepeal) {
		if (on_repeal_future_to_bank_by_future_)
			(*on_repeal_future_to_bank_by_future_)(pRspRepeal);
	}
	void OnRtnRepealFromBankToFutureByFutureManual(CThostFtdcRspRepealField *pRspRepeal) {
		if (on_repeal_bank_to_future_by_future_manual_)
			(*on_repeal_bank_to_future_by_future_manual_)(pRspRepeal);
	}
	void OnRtnRepealFromFutureToBankByFutureManual(CThostFtdcRspRepealField *pRspRepeal) {
		if (on_repeal_future_to_bank_by_future_manual_)
			(*on_repeal_future_to_bank_by_future_manual_)(pRspRepeal);
	}
	void OnErrRtnRepealBankToFutureByFutureManual(CThostFtdcReqRepealField *pReqRepeal,
		CThostFtdcRspInfoField *pRspInfo) {
		if (on_err_repeal_bank_to_future_by_future_manual_)
			(*on_err_repeal_bank_to_future_by_future_manual_)(pReqRepeal, pRspInfo);
	}
	void OnErrRtnRepealFutureToBankByFutureManual(CThostFtdcReqRepealField *pReqRepeal,
		CThostFtdcRspInfoField *pRspInfo) {
		if (on_err_repeal_future_to_bank_by_future_manual_)
			(*on_err_repeal_future_to_bank_by_future_manual_)(pReqRepeal, pRspInfo);
	}
	void OnRspQueryBankAccountMoneyByFuture(CThostFtdcReqQueryAccountField *pReqQueryAccount,
		CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
		if (on_query_bank_account_)
			(*on_query_bank_account_)(pReqQueryAccount, pRspInfo, nRequestID, bIsLast ? 1 : 0);
	}
	void OnRtnQueryBankBalanceByFuture(CThostFtdcNotifyQueryAccountField *pNotifyQueryAccount) {
		if (on_query_bank_account_by_future_)
			(*on_query_bank_account_by_future_)(pNotifyQueryAccount);
	}
	void OnErrRtnQueryBankBalanceByFuture(CThostFtdcReqQueryAccountField *pReqQueryAccount,
		CThostFtdcRspInfoField *pRspInfo) {
		if (on_err_query_bank_account_by_future_)
			(*on_err_query_bank_account_by_future_)(pReqQueryAccount, pRspInfo);
	}
	void OnRtnOpenAccountByBank(CThostFtdcOpenAccountField *pOpenAccount) {
		if (on_open_account_by_bank_)
			(*on_open_account_by_bank_)(pOpenAccount);
	}
	void OnRtnCancelAccountByBank(CThostFtdcCancelAccountField *pCancelAccount) {
		if (on_cancel_account_by_bank_)
			(*on_cancel_account_by_bank_)(pCancelAccount);
	}
	void OnRtnChangeAccountByBank(CThostFtdcChangeAccountField *pChangeAccount) {
		if (on_change_account_by_bank_)
			(*on_change_account_by_bank_)(pChangeAccount);
	}
	void OnRspQrySettlementInfo(CThostFtdcSettlementInfoField *pSettlementInfo,
		CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
		if (on_query_settlement_)
			(*on_query_settlement_)(pSettlementInfo, pRspInfo, nRequestID, bIsLast ? 1 : 0);
	}
	void OnRspQrySettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm,
		CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
		if (on_query_settlement_confirm_)
			(*on_query_settlement_confirm_)(pSettlementInfoConfirm,
				pRspInfo, nRequestID, bIsLast ? 1 : 0);
	}
	void OnRspQryCFMMCTradingAccountKey(CThostFtdcCFMMCTradingAccountKeyField *pCFMMCTradingAccountKey,
		CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
		if (on_query_cfmmc_account_key_)
			(*on_query_cfmmc_account_key_)(pCFMMCTradingAccountKey,
				pRspInfo, nRequestID, bIsLast ? 1 : 0);
	}
	void OnRspQryOrder(CThostFtdcOrderField *pOrder,
		CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
		if (on_query_order_)
			(*on_query_order_)(pOrder, pRspInfo, nRequestID, bIsLast ? 1 : 0);
	}
	void OnRspQryTrade(CThostFtdcTradeField *pTrade,
		CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
		if (on_query_trade_)
			(*on_query_trade_)(pTrade, pRspInfo, nRequestID, bIsLast ? 1 : 0);
	}
	void OnRspQryParkedOrder(CThostFtdcParkedOrderField *pParkedOrder,
		CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
		if (on_query_parked_order_)
			(*on_query_parked_order_)(pParkedOrder, pRspInfo, nRequestID, bIsLast ? 1 : 0);
	}
	void OnRspQryParkedOrderAction(CThostFtdcParkedOrderActionField *pParkedOrderAction,
		CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
		if (on_query_parked_order_action_)
			(*on_query_parked_order_action_)(pParkedOrderAction,
				pRspInfo, nRequestID, bIsLast ? 1 : 0);
	}
	void OnRspQryInvestor(CThostFtdcInvestorField *pInvestor,
		CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
		if (on_query_investor_)
			(*on_query_investor_)(pInvestor, pRspInfo, nRequestID, bIsLast ? 1 : 0);
	}
	void OnRspQryInvestorPosition(CThostFtdcInvestorPositionField *pInvestorPosition,
		CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
		if (on_query_position_)
			(*on_query_position_)(pInvestorPosition, pRspInfo, nRequestID, bIsLast ? 1 : 0);
	}
	void OnRspQryInvestorPositionDetail(CThostFtdcInvestorPositionDetailField *pInvestorPositionDetail,
		CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
		if (on_query_position_detail_)
			(*on_query_position_detail_)(pInvestorPositionDetail,
				pRspInfo, nRequestID, bIsLast ? 1 : 0);
	}
	void OnRspQryInvestorPositionCombineDetail(
		CThostFtdcInvestorPositionCombineDetailField *pInvestorPositionCombineDetail,
		CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
		if (on_query_position_combine_)
			(*on_query_position_combine_)(pInvestorPositionCombineDetail,
				pRspInfo, nRequestID, bIsLast ? 1 : 0);
	}
	void OnRspQryTradingAccount(CThostFtdcTradingAccountField *pTradingAccount,
		CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
		if (on_query_account_)
			(*on_query_account_)(pTradingAccount, pRspInfo, nRequestID, bIsLast ? 1 : 0);
	}
	void OnRspQryTradingCode(CThostFtdcTradingCodeField *pTradingCode,
		CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
		if (on_query_code_)
			(*on_query_code_)(pTradingCode, pRspInfo, nRequestID, bIsLast ? 1 : 0);
	}
	void OnRspQryInstrumentMarginRate(CThostFtdcInstrumentMarginRateField *pInstrumentMarginRate,
		CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
		if (on_query_margin_rate_)
			(*on_query_margin_rate_)(pInstrumentMarginRate,
				pRspInfo, nRequestID, bIsLast ? 1 : 0);
	}
	void OnRspQryInstrumentCommissionRate(
		CThostFtdcInstrumentCommissionRateField *pInstrumentCommissionRate,
		CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
		if (on_query_commission_rate_)
			(*on_query_commission_rate_)(pInstrumentCommissionRate,
				pRspInfo, nRequestID, bIsLast ? 1 : 0);
	}
	void OnRspQryEWarrantOffset(CThostFtdcEWarrantOffsetField *pEWarrantOffset,
		CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
		if (on_query_ewarrant_offset_)
			(*on_query_ewarrant_offset_)(pEWarrantOffset, pRspInfo, nRequestID, bIsLast ? 1 : 0);
	}
	void OnRspQryBrokerTradingParams(CThostFtdcBrokerTradingParamsField *pBrokerTradingParams,
		CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
		if (on_query_broker_params_)
			(*on_query_broker_params_)(pBrokerTradingParams,
				pRspInfo, nRequestID, bIsLast ? 1 : 0);
	}
	void OnRspQryBrokerTradingAlgos(CThostFtdcBrokerTradingAlgosField *pBrokerTradingAlgos,
		CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
		if (on_query_broker_algos_)
			(*on_query_broker_algos_)(pBrokerTradingAlgos, pRspInfo, nRequestID, bIsLast ? 1 : 0);
	}
	void OnRspQryExchange(CThostFtdcExchangeField *pExchange,
		CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
		if (on_query_exchange_)
			(*on_query_exchange_)(pExchange, pRspInfo, nRequestID, bIsLast ? 1 : 0);
	}
	void OnRspQryInstrument(CThostFtdcInstrumentField *pInstrument,
		CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
		if (on_query_instrument_)
			(*on_query_instrument_)(pInstrument, pRspInfo, nRequestID, bIsLast ? 1 : 0);
	}
	void OnRspQryDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData,
		CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
		if (on_query_deep_market_data_)
			(*on_query_deep_market_data_)(pDepthMarketData,
				pRspInfo, nRequestID, bIsLast ? 1 : 0);
	}
	void OnRspQryNotice(CThostFtdcNoticeField *pNotice,
		CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
		if (on_query_notice_)
			(*on_query_notice_)(pNotice, pRspInfo, nRequestID, bIsLast ? 1 : 0);
	}
	 void OnRspQryTradingNotice(CThostFtdcTradingNoticeField *pTradingNotice,
		CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
		if (on_query_trading_notice_)
			(*on_query_trading_notice_)(pTradingNotice, pRspInfo, nRequestID, bIsLast ? 1 : 0);
	}
	void OnRtnTradingNotice(CThostFtdcTradingNoticeInfoField *pTradingNoticeInfo) {
		if (on_trading_notice_)
			(*on_trading_notice_)(pTradingNoticeInfo);
	}
	void OnRtnInstrumentStatus(CThostFtdcInstrumentStatusField *pInstrumentStatus) {
		if (on_instrument_status_)
			(*on_instrument_status_)(pInstrumentStatus);
	}
	void OnRspQryAccountregister(CThostFtdcAccountregisterField *pAccountregister,
		CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
		if (on_query_account_register_)
			(*on_query_account_register_)(pAccountregister,
				pRspInfo, nRequestID, bIsLast ? 1 : 0);
	}
	void OnRspQryTransferBank(CThostFtdcTransferBankField *pTransferBank,
		CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
		if (on_query_transfer_bank_)
			(*on_query_transfer_bank_)(pTransferBank, pRspInfo, nRequestID, bIsLast ? 1 : 0);
	}
	void OnRspQryContractBank(CThostFtdcContractBankField *pContractBank,
		CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
		if (on_query_contract_bank_)
			(*on_query_contract_bank_)(pContractBank, pRspInfo, nRequestID, bIsLast ? 1 : 0);
	}
	void OnRspQryTransferSerial(CThostFtdcTransferSerialField *pTransferSerial,
		CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
		if (on_query_transfer_serial_)
			(*on_query_transfer_serial_)(pTransferSerial, pRspInfo, nRequestID, bIsLast ? 1 : 0);
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
int ctp_tdapi_query_max_order_volume(ctp_tdapi_t *tdapi,
	CThostFtdcQueryMaxOrderVolumeField *volume, int rid) {
	if (tdapi)
		return tdapi->rep->ReqQueryMaxOrderVolume(volume, rid);
	return 0;
}

/* FIXME */
int ctp_tdapi_confirm_settlement(ctp_tdapi_t *tdapi,
	struct CThostFtdcSettlementInfoConfirmField *stmtconfirm, int rid) {
	if (tdapi)
		return tdapi->rep->ReqSettlementInfoConfirm(stmtconfirm, rid);
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

/* FIXME */
int ctp_tdapi_insert_parked_order(ctp_tdapi_t *tdapi,
	struct CThostFtdcParkedOrderField *order, int rid) {
	if (tdapi)
		return tdapi->rep->ReqParkedOrderInsert(order, rid);
	return 0;
}

/* FIXME */
int ctp_tdapi_parked_order_action(ctp_tdapi_t *tdapi,
	struct CThostFtdcParkedOrderActionField *orderaction, int rid) {
	if (tdapi)
		return tdapi->rep->ReqParkedOrderAction(orderaction, rid);
	return 0;
}

/* FIXME */
int ctp_tdapi_remove_parked_order(ctp_tdapi_t *tdapi,
	struct CThostFtdcRemoveParkedOrderField *order, int rid) {
	if (tdapi)
		return tdapi->rep->ReqRemoveParkedOrder(order, rid);
	return 0;
}

/* FIXME */
int ctp_tdapi_remove_parked_order_action(ctp_tdapi_t *tdapi,
	struct CThostFtdcRemoveParkedOrderActionField *orderaction, int rid) {
	if (tdapi)
		return tdapi->rep->ReqRemoveParkedOrderAction(orderaction, rid);
	return 0;
}

/* FIXME */
int ctp_tdapi_bank_to_future(ctp_tdapi_t *tdapi, struct CThostFtdcReqTransferField *transfer, int rid) {
	if (tdapi)
		return tdapi->rep->ReqFromBankToFutureByFuture(transfer, rid);
	return 0;
}

/* FIXME */
int ctp_tdapi_future_to_bank(ctp_tdapi_t *tdapi, struct CThostFtdcReqTransferField *transfer, int rid) {
	if (tdapi)
		return tdapi->rep->ReqFromFutureToBankByFuture(transfer, rid);
	return 0;
}

/* FIXME */
int ctp_tdapi_query_bank_account(ctp_tdapi_t *tdapi,
	struct CThostFtdcReqQueryAccountField *account, int rid) {
	if (tdapi)
		return tdapi->rep->ReqQueryBankAccountMoneyByFuture(account, rid);
	return 0;
}

/* FIXME */
int ctp_tdapi_query_settlement(ctp_tdapi_t *tdapi, struct CThostFtdcQrySettlementInfoField *stmt, int rid) {
	if (tdapi)
		return tdapi->rep->ReqQrySettlementInfo(stmt, rid);
	return 0;
}

/* FIXME */
int ctp_tdapi_query_settlement_confirm(ctp_tdapi_t *tdapi,
	struct CThostFtdcQrySettlementInfoConfirmField *stmtconfirm, int rid) {
	if (tdapi)
		return tdapi->rep->ReqQrySettlementInfoConfirm(stmtconfirm, rid);
	return 0;
}

/* FIXME */
int ctp_tdapi_query_cfmmc_account_key(ctp_tdapi_t *tdapi,
	struct CThostFtdcQryCFMMCTradingAccountKeyField *accountkey, int rid) {
	if (tdapi)
		return tdapi->rep->ReqQryCFMMCTradingAccountKey(accountkey, rid);
	return 0;
}

/* FIXME */
int ctp_tdapi_query_order(ctp_tdapi_t *tdapi, struct CThostFtdcQryOrderField *order, int rid) {
	if (tdapi)
		return tdapi->rep->ReqQryOrder(order, rid);
	return 0;
}

/* FIXME */
int ctp_tdapi_query_trade(ctp_tdapi_t *tdapi, struct CThostFtdcQryTradeField *trade, int rid) {
	if (tdapi)
		return tdapi->rep->ReqQryTrade(trade, rid);
	return 0;
}

/* FIXME */
int ctp_tdapi_query_parked_order(ctp_tdapi_t *tdapi, struct CThostFtdcQryParkedOrderField *order, int rid) {
	if (tdapi)
		return tdapi->rep->ReqQryParkedOrder(order, rid);
	return 0;
}

/* FIXME */
int ctp_tdapi_query_parked_order_action(ctp_tdapi_t *tdapi,
	struct CThostFtdcQryParkedOrderActionField *orderaction, int rid) {
	if (tdapi)
		return tdapi->rep->ReqQryParkedOrderAction(orderaction, rid);
	return 0;
}

/* FIXME */
int ctp_tdapi_query_investor(ctp_tdapi_t *tdapi, struct CThostFtdcQryInvestorField *investor, int rid) {
	if (tdapi)
		return tdapi->rep->ReqQryInvestor(investor, rid);
	return 0;
}

/* FIXME */
int ctp_tdapi_query_position(ctp_tdapi_t *tdapi, struct CThostFtdcQryInvestorPositionField *pstn, int rid) {
	if (tdapi)
		return tdapi->rep->ReqQryInvestorPosition(pstn, rid);
	return 0;
}

/* FIXME */
int ctp_tdapi_query_position_detail(ctp_tdapi_t *tdapi,
	struct CThostFtdcQryInvestorPositionDetailField *pstndetail, int rid) {
	if (tdapi)
		return tdapi->rep->ReqQryInvestorPositionDetail(pstndetail, rid);
	return 0;
}

/* FIXME */
int ctp_tdapi_query_position_combine(ctp_tdapi_t *tdapi,
	struct CThostFtdcQryInvestorPositionCombineDetailField *pstncombine, int rid) {
	if (tdapi)
		return tdapi->rep->ReqQryInvestorPositionCombineDetail(pstncombine, rid);
	return 0;
}

/* FIXME */
int ctp_tdapi_query_account(ctp_tdapi_t *tdapi, struct CThostFtdcQryTradingAccountField *account, int rid) {
	if (tdapi)
		return tdapi->rep->ReqQryTradingAccount(account, rid);
	return 0;
}

/* FIXME */
int ctp_tdapi_query_code(ctp_tdapi_t *tdapi, struct CThostFtdcQryTradingCodeField *code, int rid) {
	if (tdapi)
		return tdapi->rep->ReqQryTradingCode(code, rid);
	return 0;
}

/* FIXME */
int ctp_tdapi_query_margin_rate(ctp_tdapi_t *tdapi,
	struct CThostFtdcQryInstrumentMarginRateField *rate, int rid) {
	if (tdapi)
		return tdapi->rep->ReqQryInstrumentMarginRate(rate, rid);
	return 0;
}

/* FIXME */
int ctp_tdapi_query_commission_rate(ctp_tdapi_t *tdapi,
	struct CThostFtdcQryInstrumentCommissionRateField *rate, int rid) {
	if (tdapi)
		return tdapi->rep->ReqQryInstrumentCommissionRate(rate, rid);
	return 0;
}

/* FIXME */
int ctp_tdapi_query_ewarrant_offset(ctp_tdapi_t *tdapi,
	struct CThostFtdcQryEWarrantOffsetField *offset, int rid) {
	if (tdapi)
		return tdapi->rep->ReqQryEWarrantOffset(offset, rid);
	return 0;
}

/* FIXME */
int ctp_tdapi_query_broker_params(ctp_tdapi_t *tdapi,
	struct CThostFtdcQryBrokerTradingParamsField *params, int rid) {
	if (tdapi)
		return tdapi->rep->ReqQryBrokerTradingParams(params, rid);
	return 0;
}

/* FIXME */
int ctp_tdapi_query_broker_algos(ctp_tdapi_t *tdapi,
	struct CThostFtdcQryBrokerTradingAlgosField *algos, int rid) {
	if (tdapi)
		return tdapi->rep->ReqQryBrokerTradingAlgos(algos, rid);
	return 0;
}

/* FIXME */
int ctp_tdapi_query_exchange(ctp_tdapi_t *tdapi, struct CThostFtdcQryExchangeField *exchange, int rid) {
	if (tdapi)
		return tdapi->rep->ReqQryExchange(exchange, rid);
	return 0;
}

/* FIXME */
int ctp_tdapi_query_instrument(ctp_tdapi_t *tdapi, struct CThostFtdcQryInstrumentField *instrument, int rid) {
	if (tdapi)
		return tdapi->rep->ReqQryInstrument(instrument, rid);
	return 0;
}

/* FIXME */
int ctp_tdapi_query_deep_market_data(ctp_tdapi_t *tdapi,
	struct CThostFtdcQryDepthMarketDataField *deepmd, int rid) {
	if (tdapi)
		return tdapi->rep->ReqQryDepthMarketData(deepmd, rid);
	return 0;
}

/* FIXME */
int ctp_tdapi_query_notice(ctp_tdapi_t *tdapi, struct CThostFtdcQryNoticeField *notice, int rid) {
	if (tdapi)
		return tdapi->rep->ReqQryNotice(notice, rid);
	return 0;
}

/* FIXME */
int ctp_tdapi_query_trading_notice(ctp_tdapi_t *tdapi,
	struct CThostFtdcQryTradingNoticeField *notice, int rid) {
	if (tdapi)
		return tdapi->rep->ReqQryTradingNotice(notice, rid);
	return 0;
}

/* FIXME */
int ctp_tdapi_query_account_register(ctp_tdapi_t *tdapi,
	struct CThostFtdcQryAccountregisterField *accountregister, int rid) {
	if (tdapi)
		return tdapi->rep->ReqQryAccountregister(accountregister, rid);
	return 0;
}

/* FIXME */
int ctp_tdapi_query_transfer_bank(ctp_tdapi_t *tdapi, struct CThostFtdcQryTransferBankField *bank, int rid) {
	if (tdapi)
		return tdapi->rep->ReqQryTransferBank(bank, rid);
	return 0;
}

/* FIXME */
int ctp_tdapi_query_contract_bank(ctp_tdapi_t *tdapi, struct CThostFtdcQryContractBankField *bank, int rid) {
	if (tdapi)
		return tdapi->rep->ReqQryContractBank(bank, rid);
	return 0;
}

/* FIXME */
int ctp_tdapi_query_transfer_serial(ctp_tdapi_t *tdapi,
	struct CThostFtdcQryTransferSerialField *serial, int rid) {
	if (tdapi)
		return tdapi->rep->ReqQryTransferSerial(serial, rid);
	return 0;
}

ctp_tdspi_t *ctp_tdspi_create() {
	ctp_tdspi_t *tdspi = new ctp_tdspi_t;

	tdspi->on_front_connected_                            = NULL;
	tdspi->on_front_disconnected_                         = NULL;
	tdspi->on_heartbeat_warning_                          = NULL;
	tdspi->on_error_                                      = NULL;
	tdspi->on_authenticate_                               = NULL;
	tdspi->on_user_login_                                 = NULL;
	tdspi->on_user_logout_                                = NULL;
	tdspi->on_update_user_password_                       = NULL;
	tdspi->on_update_account_password_                    = NULL;
	tdspi->on_query_max_order_volume_                     = NULL;
	tdspi->on_confirm_settlement_                         = NULL;
	tdspi->on_insert_order_                               = NULL;
	tdspi->on_order_action_                               = NULL;
	tdspi->on_order_                                      = NULL;
	tdspi->on_trade_                                      = NULL;
	tdspi->on_err_insert_order_                           = NULL;
	tdspi->on_err_order_action_                           = NULL;
	tdspi->on_err_conditional_order_                      = NULL;
	tdspi->on_insert_parked_order_                        = NULL;
	tdspi->on_parked_order_action_                        = NULL;
	tdspi->on_remove_parked_order_                        = NULL;
	tdspi->on_remove_parked_order_action_                 = NULL;
	tdspi->on_bank_to_future_                             = NULL;
	tdspi->on_future_to_bank_                             = NULL;
	tdspi->on_bank_to_future_by_bank_                     = NULL;
	tdspi->on_future_to_bank_by_bank_                     = NULL;
	tdspi->on_repeal_bank_to_future_by_bank_              = NULL;
	tdspi->on_repeal_future_to_bank_by_bank_              = NULL;
	tdspi->on_bank_to_future_by_future_                   = NULL;
	tdspi->on_future_to_bank_by_future_                   = NULL;
	tdspi->on_err_bank_to_future_by_future_               = NULL;
	tdspi->on_err_future_to_bank_by_future_               = NULL;
	tdspi->on_repeal_bank_to_future_by_future_            = NULL;
	tdspi->on_repeal_future_to_bank_by_future_            = NULL;
	tdspi->on_repeal_bank_to_future_by_future_manual_     = NULL;
	tdspi->on_repeal_future_to_bank_by_future_manual_     = NULL;
	tdspi->on_err_repeal_bank_to_future_by_future_manual_ = NULL;
	tdspi->on_err_repeal_future_to_bank_by_future_manual_ = NULL;
	tdspi->on_query_bank_account_                         = NULL;
	tdspi->on_query_bank_account_by_future_               = NULL;
	tdspi->on_err_query_bank_account_by_future_           = NULL;
	tdspi->on_open_account_by_bank_                       = NULL;
	tdspi->on_cancel_account_by_bank_                     = NULL;
	tdspi->on_change_account_by_bank_                     = NULL;
	tdspi->on_query_settlement_                           = NULL;
	tdspi->on_query_settlement_confirm_                   = NULL;
	tdspi->on_query_cfmmc_account_key_                    = NULL;
	tdspi->on_query_order_                                = NULL;
	tdspi->on_query_trade_                                = NULL;
	tdspi->on_query_parked_order_                         = NULL;
	tdspi->on_query_parked_order_action_                  = NULL;
	tdspi->on_query_investor_                             = NULL;
	tdspi->on_query_position_                             = NULL;
	tdspi->on_query_position_detail_                      = NULL;
	tdspi->on_query_position_combine_                     = NULL;
	tdspi->on_query_account_                              = NULL;
	tdspi->on_query_code_                                 = NULL;
	tdspi->on_query_margin_rate_                          = NULL;
	tdspi->on_query_commission_rate_                      = NULL;
	tdspi->on_query_ewarrant_offset_                      = NULL;
	tdspi->on_query_broker_params_                        = NULL;
	tdspi->on_query_broker_algos_                         = NULL;
	tdspi->on_query_exchange_                             = NULL;
	tdspi->on_query_instrument_                           = NULL;
	tdspi->on_query_deep_market_data_                     = NULL;
	tdspi->on_query_notice_                               = NULL;
	tdspi->on_query_trading_notice_                       = NULL;
	tdspi->on_trading_notice_                             = NULL;
	tdspi->on_instrument_status_                          = NULL;
	tdspi->on_query_account_register_                     = NULL;
	tdspi->on_query_transfer_bank_                        = NULL;
	tdspi->on_query_contract_bank_                        = NULL;
	tdspi->on_query_transfer_serial_                      = NULL;
	return tdspi;
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

void ctp_tdspi_on_error(ctp_tdspi_t *tdspi, ctp_on_error func) {
	if (tdspi && func)
		tdspi->on_error_ = func;
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

void ctp_tdspi_on_query_max_order_volume(ctp_tdspi_t *tdspi, ctp_on_query_max_order_volume func) {
	if (tdspi && func)
		tdspi->on_query_max_order_volume_ = func;
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

void ctp_tdspi_on_err_insert_order(ctp_tdspi_t *tdspi, ctp_on_err_insert_order func) {
	if (tdspi && func)
		tdspi->on_err_insert_order_ = func;
}

void ctp_tdspi_on_err_order_action(ctp_tdspi_t *tdspi, ctp_on_err_order_action func) {
	if (tdspi && func)
		tdspi->on_err_order_action_ = func;
}

void ctp_tdspi_on_err_conditional_order(ctp_tdspi_t *tdspi, ctp_on_err_conditional_order func) {
	if (tdspi && func)
		tdspi->on_err_conditional_order_ = func;
}

void ctp_tdspi_on_insert_parked_order(ctp_tdspi_t *tdspi, ctp_on_insert_parked_order func) {
	if (tdspi && func)
		tdspi->on_insert_parked_order_ = func;
}

void ctp_tdspi_on_parked_order_action(ctp_tdspi_t *tdspi, ctp_on_parked_order_action func) {
	if (tdspi && func)
		tdspi->on_parked_order_action_ = func;
}

void ctp_tdspi_on_remove_parked_order(ctp_tdspi_t *tdspi, ctp_on_remove_parked_order func) {
	if (tdspi && func)
		tdspi->on_remove_parked_order_ = func;
}

void ctp_tdspi_on_remove_parked_order_action(ctp_tdspi_t *tdspi, ctp_on_remove_parked_order_action func) {
	if (tdspi && func)
		tdspi->on_remove_parked_order_action_ = func;
}

void ctp_tdspi_on_bank_to_future(ctp_tdspi_t *tdspi, ctp_on_bank_to_future func) {
	if (tdspi && func)
		tdspi->on_bank_to_future_ = func;
}

void ctp_tdspi_on_future_to_bank(ctp_tdspi_t *tdspi, ctp_on_future_to_bank func) {
	if (tdspi && func)
		tdspi->on_future_to_bank_ = func;
}

void ctp_tdspi_on_bank_to_future_by_bank(ctp_tdspi_t *tdspi, ctp_on_bank_to_future_by_bank func) {
	if (tdspi && func)
		tdspi->on_bank_to_future_by_bank_ = func;
}

void ctp_tdspi_on_future_to_bank_by_bank(ctp_tdspi_t *tdspi, ctp_on_future_to_bank_by_bank func) {
	if (tdspi && func)
		tdspi->on_future_to_bank_by_bank_ = func;
}

void ctp_tdspi_on_repeal_bank_to_future_by_bank(ctp_tdspi_t *tdspi,
	ctp_on_repeal_bank_to_future_by_bank func) {
	if (tdspi && func)
		tdspi->on_repeal_bank_to_future_by_bank_ = func;
}

void ctp_tdspi_on_repeal_future_to_bank_by_bank(ctp_tdspi_t *tdspi,
	ctp_on_repeal_future_to_bank_by_bank func) {
	if (tdspi && func)
		tdspi->on_repeal_future_to_bank_by_bank_ = func;
}

void ctp_tdspi_on_bank_to_future_by_future(ctp_tdspi_t *tdspi, ctp_on_bank_to_future_by_future func) {
	if (tdspi && func)
		tdspi->on_bank_to_future_by_future_ = func;
}

void ctp_tdspi_on_future_to_bank_by_future(ctp_tdspi_t *tdspi, ctp_on_future_to_bank_by_future func) {
	if (tdspi && func)
		tdspi->on_future_to_bank_by_future_ = func;
}

void ctp_tdspi_on_err_bank_to_future_by_future(ctp_tdspi_t *tdspi, ctp_on_err_bank_to_future_by_future func) {
	if (tdspi && func)
		tdspi->on_err_bank_to_future_by_future_ = func;
}

void ctp_tdspi_on_err_future_to_bank_by_future(ctp_tdspi_t *tdspi, ctp_on_err_future_to_bank_by_future func) {
	if (tdspi && func)
		tdspi->on_err_future_to_bank_by_future_ = func;
}

void ctp_tdspi_on_repeal_bank_to_future_by_future(ctp_tdspi_t *tdspi,
	ctp_on_repeal_bank_to_future_by_future func) {
	if (tdspi && func)
		tdspi->on_repeal_bank_to_future_by_future_ = func;
}

void ctp_tdspi_on_repeal_future_to_bank_by_future(ctp_tdspi_t *tdspi,
	ctp_on_repeal_future_to_bank_by_future func) {
	if (tdspi && func)
		tdspi->on_repeal_future_to_bank_by_future_ = func;
}

void ctp_tdspi_on_repeal_bank_to_future_by_future_manual(ctp_tdspi_t *tdspi,
	ctp_on_repeal_bank_to_future_by_future_manual func) {
	if (tdspi && func)
		tdspi->on_repeal_bank_to_future_by_future_manual_ = func;
}

void ctp_tdspi_on_repeal_future_to_bank_by_future_manual(ctp_tdspi_t *tdspi,
	ctp_on_repeal_future_to_bank_by_future_manual func) {
	if (tdspi && func)
		tdspi->on_repeal_future_to_bank_by_future_manual_ = func;
}

void ctp_tdspi_on_err_repeal_bank_to_future_by_future_manual(ctp_tdspi_t *tdspi,
	ctp_on_err_repeal_bank_to_future_by_future_manual func) {
	if (tdspi && func)
		tdspi->on_err_repeal_bank_to_future_by_future_manual_ = func;
}

void ctp_tdspi_on_err_repeal_future_to_bank_by_future_manual(ctp_tdspi_t *tdspi,
	ctp_on_err_repeal_future_to_bank_by_future_manual func) {
	if (tdspi && func)
		tdspi->on_err_repeal_future_to_bank_by_future_manual_ = func;
}

void ctp_tdspi_on_query_bank_account(ctp_tdspi_t *tdspi, ctp_on_query_bank_account func) {
	if (tdspi && func)
		tdspi->on_query_bank_account_ = func;
}

void ctp_tdspi_on_query_bank_account_by_future(ctp_tdspi_t *tdspi, ctp_on_query_bank_account_by_future func) {
	if (tdspi && func)
		tdspi->on_query_bank_account_by_future_ = func;
}

void ctp_tdspi_on_err_query_bank_account_by_future(ctp_tdspi_t *tdspi,
	ctp_on_err_query_bank_account_by_future func) {
	if (tdspi && func)
		tdspi->on_err_query_bank_account_by_future_ = func;
}

void ctp_tdspi_on_open_account_by_bank(ctp_tdspi_t *tdspi, ctp_on_open_account_by_bank func) {
	if (tdspi && func)
		tdspi->on_open_account_by_bank_ = func;
}

void ctp_tdspi_on_cancel_account_by_bank(ctp_tdspi_t *tdspi, ctp_on_cancel_account_by_bank func) {
	if (tdspi && func)
		tdspi->on_cancel_account_by_bank_ = func;
}

void ctp_tdspi_on_change_account_by_bank(ctp_tdspi_t *tdspi, ctp_on_change_account_by_bank func) {
	if (tdspi && func)
		tdspi->on_change_account_by_bank_ = func;
}

void ctp_tdspi_on_query_settlement(ctp_tdspi_t *tdspi, ctp_on_query_settlement func) {
	if (tdspi && func)
		tdspi->on_query_settlement_ = func;
}

void ctp_tdspi_on_query_settlement_confirm(ctp_tdspi_t *tdspi, ctp_on_query_settlement_confirm func) {
	if (tdspi && func)
		tdspi->on_query_settlement_confirm_ = func;
}

void ctp_tdspi_on_query_cfmmc_account_key(ctp_tdspi_t *tdspi, ctp_on_query_cfmmc_account_key func) {
	if (tdspi && func)
		tdspi->on_query_cfmmc_account_key_ = func;
}

void ctp_tdspi_on_query_order(ctp_tdspi_t *tdspi, ctp_on_query_order func) {
	if (tdspi && func)
		tdspi->on_query_order_ = func;
}

void ctp_tdspi_on_query_trade(ctp_tdspi_t *tdspi, ctp_on_query_trade func) {
	if (tdspi && func)
		tdspi->on_query_trade_ = func;
}

void ctp_tdspi_on_query_parked_order(ctp_tdspi_t *tdspi, ctp_on_query_parked_order func) {
	if (tdspi && func)
		tdspi->on_query_parked_order_ = func;
}

void ctp_tdspi_on_query_parked_order_action(ctp_tdspi_t *tdspi, ctp_on_query_parked_order_action func) {
	if (tdspi && func)
		tdspi->on_query_parked_order_action_ = func;
}

void ctp_tdspi_on_query_investor(ctp_tdspi_t *tdspi, ctp_on_query_investor func) {
	if (tdspi && func)
		tdspi->on_query_investor_ = func;
}

void ctp_tdspi_on_query_position(ctp_tdspi_t *tdspi, ctp_on_query_position func) {
	if (tdspi && func)
		tdspi->on_query_position_ = func;
}

void ctp_tdspi_on_query_position_detail(ctp_tdspi_t *tdspi, ctp_on_query_position_detail func) {
	if (tdspi && func)
		tdspi->on_query_position_detail_ = func;
}

void ctp_tdspi_on_query_position_combine(ctp_tdspi_t *tdspi, ctp_on_query_position_combine func) {
	if (tdspi && func)
		tdspi->on_query_position_combine_ = func;
}

void ctp_tdspi_on_query_account(ctp_tdspi_t *tdspi, ctp_on_query_account func) {
	if (tdspi && func)
		tdspi->on_query_account_ = func;
}

void ctp_tdspi_on_query_code(ctp_tdspi_t *tdspi, ctp_on_query_code func) {
	if (tdspi && func)
		tdspi->on_query_code_ = func;
}

void ctp_tdspi_on_query_margin_rate(ctp_tdspi_t *tdspi, ctp_on_query_margin_rate func) {
	if (tdspi && func)
		tdspi->on_query_margin_rate_ = func;
}

void ctp_tdspi_on_query_commission_rate(ctp_tdspi_t *tdspi, ctp_on_query_commission_rate func) {
	if (tdspi && func)
		tdspi->on_query_commission_rate_ = func;
}

void ctp_tdspi_on_query_ewarrant_offset(ctp_tdspi_t *tdspi, ctp_on_query_ewarrant_offset func) {
	if (tdspi && func)
		tdspi->on_query_ewarrant_offset_ = func;
}

void ctp_tdspi_on_query_broker_params(ctp_tdspi_t *tdspi, ctp_on_query_broker_params func) {
	if (tdspi && func)
		tdspi->on_query_broker_params_ = func;
}

void ctp_tdspi_on_query_broker_algos(ctp_tdspi_t *tdspi, ctp_on_query_broker_algos func) {
	if (tdspi && func)
		tdspi->on_query_broker_algos_ = func;
}

void ctp_tdspi_on_query_exchange(ctp_tdspi_t *tdspi, ctp_on_query_exchange func) {
	if (tdspi && func)
		tdspi->on_query_exchange_ = func;
}

void ctp_tdspi_on_query_instrument(ctp_tdspi_t *tdspi, ctp_on_query_instrument func) {
	if (tdspi && func)
		tdspi->on_query_instrument_ = func;
}

void ctp_tdspi_on_query_deep_market_data(ctp_tdspi_t *tdspi, ctp_on_query_deep_market_data func) {
	if (tdspi && func)
		tdspi->on_query_deep_market_data_ = func;
}

void ctp_tdspi_on_query_notice(ctp_tdspi_t *tdspi, ctp_on_query_notice func) {
	if (tdspi && func)
		tdspi->on_query_notice_ = func;
}

void ctp_tdspi_on_query_trading_notice(ctp_tdspi_t *tdspi, ctp_on_query_trading_notice func) {
	if (tdspi && func)
		tdspi->on_query_trading_notice_ = func;
}

void ctp_tdspi_on_trading_notice(ctp_tdspi_t *tdspi, ctp_on_trading_notice func) {
	if (tdspi && func)
		tdspi->on_trading_notice_ = func;
}

void ctp_tdspi_on_instrument_status(ctp_tdspi_t *tdspi, ctp_on_instrument_status func) {
	if (tdspi && func)
		tdspi->on_instrument_status_ = func;
}

void ctp_tdspi_on_query_account_register(ctp_tdspi_t *tdspi, ctp_on_query_account_register func) {
	if (tdspi && func)
		tdspi->on_query_account_register_ = func;
}

void ctp_tdspi_on_query_transfer_bank(ctp_tdspi_t *tdspi, ctp_on_query_transfer_bank func) {
	if (tdspi && func)
		tdspi->on_query_transfer_bank_ = func;
}

void ctp_tdspi_on_query_contract_bank(ctp_tdspi_t *tdspi, ctp_on_query_contract_bank func) {
	if (tdspi && func)
		tdspi->on_query_contract_bank_ = func;
}

void ctp_tdspi_on_query_transfer_serial(ctp_tdspi_t *tdspi, ctp_on_query_transfer_serial func) {
	if (tdspi && func)
		tdspi->on_query_transfer_serial_ = func;
}

}


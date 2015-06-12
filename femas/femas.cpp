/*
 * Copyright (c) 2013-2015, Dalian Futures Information Technology Co., Ltd.
 *
 * Bo Wang <futurewb at dce dot com dot cn>
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
#include "USTPFtdcMduserApi.h"
#include "femas.h"

extern "C" {

/* FIXME */
struct femas_mdapi_t {
	CUstpFtdcMduserApi		*rep;
};
struct femas_mdspi_t : public CUstpFtdcMduserSpi {
	femas_on_front_connected		on_front_connected_;
	femas_on_front_disconnected		on_front_disconnected_;
	femas_on_heartbeat_warning		on_heartbeat_warning_;
	femas_on_package_start			on_package_start_;
	femas_on_package_end			on_package_end_;
	femas_on_error				on_error_;
	femas_on_user_login			on_user_login_;
	femas_on_user_logout			on_user_logout_;
	femas_on_subscribe_topic		on_subscribe_topic_;
	femas_on_query_topic			on_query_topic_;
	femas_on_subscribe_market_data		on_subscribe_market_data_;
	femas_on_unsubscribe_market_data	on_unsubscribe_market_data_;
	femas_on_deep_market_data		on_deep_market_data_;
	/* make gcc happy */
	virtual ~femas_mdspi_t() {};
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
	void OnPackageStart(int nTopicID, int nSequenceNo) {
		if (on_package_start_)
			(*on_package_start_)(nTopicID, nSequenceNo);
	}
	void OnPackageEnd(int nTopicID, int nSequenceNo) {
		if (on_package_end_)
			(*on_package_end_)(nTopicID, nSequenceNo);
	}
	void OnRspError(CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
		if (on_error_)
			(*on_error_)(pRspInfo, nRequestID, bIsLast ? 1 : 0);
	}
	void OnRspUserLogin(CUstpFtdcRspUserLoginField *pRspUserLogin,
		CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
		if (on_user_login_)
			(*on_user_login_)(pRspUserLogin,
				pRspInfo, nRequestID, bIsLast ? 1 : 0);
	}
	void OnRspUserLogout(CUstpFtdcRspUserLogoutField *pRspUserLogout,
		CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
		if (on_user_logout_)
			(*on_user_logout_)(pRspUserLogout,
				pRspInfo, nRequestID, bIsLast ? 1 : 0);
	}
	void OnRspSubscribeTopic(CUstpFtdcDisseminationField *pDissemination,
		CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
		if (on_subscribe_topic_)
			(*on_subscribe_topic_)(pDissemination,
				pRspInfo, nRequestID, bIsLast ? 1 : 0);
	}
	void OnRspQryTopic(CUstpFtdcDisseminationField *pDissemination,
		CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
		if (on_query_topic_)
			(*on_query_topic_)(pDissemination, pRspInfo, nRequestID, bIsLast ? 1 : 0);
	}
	void OnRspSubMarketData(CUstpFtdcSpecificInstrumentField *pSpecificInstrument,
		CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
		if (on_subscribe_market_data_)
			(*on_subscribe_market_data_)(pSpecificInstrument,
				pRspInfo, nRequestID, bIsLast ? 1 : 0);
	}
	void OnRspUnSubMarketData(CUstpFtdcSpecificInstrumentField *pSpecificInstrument,
		CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
		if (on_unsubscribe_market_data_)
			(*on_unsubscribe_market_data_)(pSpecificInstrument,
				pRspInfo, nRequestID, bIsLast ? 1 : 0);
	}
	void OnRtnDepthMarketData(CUstpFtdcDepthMarketDataField *pDepthMarketData) {
		if (on_deep_market_data_)
			(*on_deep_market_data_)(pDepthMarketData);
	}
};

femas_mdapi_t *femas_mdapi_create(const char *flowpath) {
	femas_mdapi_t *mdapi = new femas_mdapi_t;
	mdapi->rep = CUstpFtdcMduserApi::CreateFtdcMduserApi(flowpath);
	return mdapi;
}

void femas_mdapi_destroy(femas_mdapi_t *mdapi) {
	if (mdapi) {
		mdapi->rep->Release();
		delete mdapi;
	}
}

const char *femas_mdapi_getversion(femas_mdapi_t *mdapi, int *majorversion, int *minorversion) {
	if (mdapi) {
		mdapi->rep->GetVersion(*majorversion, *minorversion);
	}
}

void femas_mdapi_init(femas_mdapi_t *mdapi) {
	if (mdapi) {
		mdapi->rep->Init();
	}
}

void femas_mdapi_join(femas_mdapi_t *mdapi) {
	if (mdapi) {
		mdapi->rep->Join();
	}
}

const char *femas_mdapi_get_trading_day(femas_mdapi_t *mdapi) {
	if (mdapi) {
		mdapi->rep->GetTradingDay();
	}
}

void femas_mdapi_register_front(femas_mdapi_t *mdapi, char *frontaddr) {
	if (mdapi) {
		mdapi->rep->RegisterFront(frontaddr);
	}
}

void femas_mdapi_register_name_server(femas_mdapi_t *mdapi, char *nsaddr) {
	if (mdapi) {
		mdapi->rep->RegisterNameServer(nsaddr);
	}
}

void femas_mdapi_register_spi(femas_mdapi_t *mdapi, femas_mdspi_t *mdspi) {
	if (mdapi && mdspi) {
		mdapi->rep->RegisterSpi(mdspi);
	}
}

int femas_mdapi_register_certificate_file(femas_mdapi_t *mdapi, const char *certfilename,
	const char *keyfilename, const char *cafilename, const char *keyfilepassword) {
	if (mdapi) {
		mdapi->rep->RegisterCertificateFile(certfilename, keyfilename, cafilename, keyfilepassword);
	}
}

void femas_mdapi_subscribe_market_data_topic(femas_mdapi_t *mdapi,
	int topicid, enum USTP_TE_RESUME_TYPE resumetype) {
	if (mdapi) {
		mdapi->rep->SubscribeMarketDataTopic(topicid, resumetype);
	}
}

int femas_mdapi_subscribe_market_data(femas_mdapi_t *mdapi, char **instruments, int count) {
	if (mdapi) {
		mdapi->rep->SubMarketData(instruments, count);
	}
}

int femas_mdapi_unsubscribe_market_data(femas_mdapi_t *mdapi, char **instruments, int count) {
	if (mdapi) {
		mdapi->rep->UnSubMarketData(instruments, count);
	}
}

void femas_mdapi_heartbeat_timeout(femas_mdapi_t *mdapi, unsigned int timeout) {
	if (mdapi) {
		mdapi->rep->SetHeartbeatTimeout(timeout);
	}
}

int femas_mdapi_user_login(femas_mdapi_t *mdapi, struct CUstpFtdcReqUserLoginField *userlogin, int rid) {
	if (mdapi) {
		mdapi->rep->ReqUserLogin(userlogin, rid);
	}
}

int femas_mdapi_user_logout(femas_mdapi_t *mdapi, struct CUstpFtdcReqUserLogoutField *userlogout, int rid) {
	if (mdapi) {
		mdapi->rep->ReqUserLogout(userlogout, rid);
	}
}

int femas_mdapi_subscribe_topic(femas_mdapi_t *mdapi,
	struct CUstpFtdcDisseminationField *dissemination, int rid) {
	if (mdapi) {
		mdapi->rep->ReqSubscribeTopic(dissemination, rid);
	}
}

int femas_mdapi_query_topic(femas_mdapi_t *mdapi,
	struct CUstpFtdcDisseminationField *dissemination, int rid) {
	if (mdapi) {
		mdapi->rep->ReqQryTopic(dissemination, rid);
	}
}

int femas_mdapi_request_subscribe_market_data(femas_mdapi_t *mdapi,
	struct CUstpFtdcSpecificInstrumentField *instrument, int rid) {
	if (mdapi) {
		mdapi->rep->ReqSubMarketData(instrument, rid);
	}
}

int femas_mdapi_request_unsubscribe_market_data(femas_mdapi_t *mdapi,
	struct CUstpFtdcSpecificInstrumentField *instrument, int rid) {
	if (mdapi) {
		mdapi->rep->ReqUnSubMarketData(instrument, rid);
	}
}

femas_mdspi_t *femas_mdspi_create() {
	return new femas_mdspi_t;
}

void femas_mdspi_destroy(femas_mdspi_t *mdspi) {
	if (mdspi)
		delete mdspi;
}

void femas_mdspi_on_front_connected(femas_mdspi_t *mdspi, femas_on_front_connected func) {
	if (mdspi && func)
		mdspi->on_front_connected_ = func;
}

void femas_mdspi_on_front_disconnected(femas_mdspi_t *mdspi, femas_on_front_disconnected func) {
	if (mdspi && func)
		mdspi->on_front_disconnected_ = func;
}

void femas_mdspi_on_heartbeat_warning(femas_mdspi_t *mdspi, femas_on_heartbeat_warning func) {
	if (mdspi && func)
		mdspi->on_heartbeat_warning_ = func;
}

void femas_mdspi_on_package_start(femas_mdspi_t *mdspi, femas_on_package_start func) {
	if (mdspi && func)
		mdspi->on_package_start_ = func;
}

void femas_mdspi_on_package_end(femas_mdspi_t *mdspi, femas_on_package_end func) {
	if (mdspi && func)
		mdspi->on_package_end_ = func;
}

void femas_mdspi_on_error(femas_mdspi_t *mdspi, femas_on_error func) {
	if (mdspi && func)
		mdspi->on_error_ = func;
}

void femas_mdspi_on_user_login(femas_mdspi_t *mdspi, femas_on_user_login func) {
	if (mdspi && func)
		mdspi->on_user_login_ = func;
}

void femas_mdspi_on_user_logout(femas_mdspi_t *mdspi, femas_on_user_logout func) {
	if (mdspi && func)
		mdspi->on_user_logout_ = func;
}

void femas_mdspi_on_subscribe_topic(femas_mdspi_t *mdspi, femas_on_subscribe_topic func) {
	if (mdspi && func)
		mdspi->on_subscribe_topic_ = func;
}

void femas_mdspi_on_query_topic(femas_mdspi_t *mdspi, femas_on_query_topic func) {
	if (mdspi && func)
		mdspi->on_query_topic_ = func;
}

void femas_mdspi_on_subscribe_market_data(femas_mdspi_t *mdspi, femas_on_subscribe_market_data func) {
	if (mdspi && func)
		mdspi->on_subscribe_market_data_ = func;
}

void femas_mdspi_on_unsubscribe_market_data(femas_mdspi_t *mdspi, femas_on_unsubscribe_market_data func) {
	if (mdspi && func)
		mdspi->on_unsubscribe_market_data_ = func;
}

void femas_mdspi_on_deep_market_data(femas_mdspi_t *mdspi, femas_on_deep_market_data func) {
	if (mdspi && func)
		mdspi->on_deep_market_data_ = func;
}

}


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

#ifndef FEMAS_INCLUDED
#define FEMAS_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include "USTPFtdcUserApiDataType.h"
#include "USTPFtdcUserApiStruct.h"

/* FIXME: exported types */
typedef struct femas_mdapi_t femas_mdapi_t;
typedef struct femas_mdspi_t femas_mdspi_t;
typedef void (*femas_on_front_connected)(void);
typedef void (*femas_on_front_disconnected)(int reason);
typedef void (*femas_on_heartbeat_warning)(int timelapse);
typedef void (*femas_on_package_start)(int topicid, int sequenceno);
typedef void (*femas_on_package_end)(int topicid, int sequenceno);
typedef void (*femas_on_error)(struct CUstpFtdcRspInfoField *rspinfo, int rid, int islast);
typedef void (*femas_on_user_login)(struct CUstpFtdcRspUserLoginField *userlogin,
	struct CUstpFtdcRspInfoField *rspinfo, int rid, int islast);
typedef void (*femas_on_user_logout)(struct CUstpFtdcRspUserLogoutField *userlogout,
	struct CUstpFtdcRspInfoField *rspinfo, int rid, int islast);
typedef void (*femas_on_subscribe_topic)(struct CUstpFtdcDisseminationField *dissemination,
	struct CUstpFtdcRspInfoField *rspinfo, int rid, int islast);
typedef void (*femas_on_query_topic)(struct CUstpFtdcDisseminationField *dissemination,
	struct CUstpFtdcRspInfoField *rspinfo, int rid, int islast);
typedef void (*femas_on_subscribe_market_data)(struct CUstpFtdcSpecificInstrumentField *instrument,
	struct CUstpFtdcRspInfoField *rspinfo, int rid, int islast);
typedef void (*femas_on_unsubscribe_market_data)(struct CUstpFtdcSpecificInstrumentField *instrument,
	struct CUstpFtdcRspInfoField *rspinfo, int rid, int islast);
typedef void (*femas_on_deep_market_data)(struct CUstpFtdcDepthMarketDataField *deepmd);

/* exported functions */
extern femas_mdapi_t *femas_mdapi_create(const char *flowpath);
extern void           femas_mdapi_destroy(femas_mdapi_t *mdapi);
extern const char    *femas_mdapi_getversion(femas_mdapi_t *mdapi, int *majorversion, int *minorversion);
extern void           femas_mdapi_init(femas_mdapi_t *mdapi);
extern void           femas_mdapi_join(femas_mdapi_t *mdapi);
extern const char    *femas_mdapi_get_trading_day(femas_mdapi_t *mdapi);
extern void           femas_mdapi_register_front(femas_mdapi_t *mdapi, char *frontaddr);
extern void           femas_mdapi_register_name_server(femas_mdapi_t *mdapi, char *nsaddr);
extern void           femas_mdapi_register_spi(femas_mdapi_t *mdapi, femas_mdspi_t *mdspi);
extern int            femas_mdapi_register_certificate_file(femas_mdapi_t *mdapi, const char *certfilename,
			const char *keyfilename, const char *cafilename, const char *keyfilepassword);
extern void           femas_mdapi_subscribe_market_data_topic(femas_mdapi_t *mdapi,
			int topicid, enum USTP_TE_RESUME_TYPE resumetype);
extern int            femas_mdapi_subscribe_market_data(femas_mdapi_t *mdapi,
			char **instruments, int count);
extern int            femas_mdapi_unsubscribe_market_data(femas_mdapi_t *mdapi,
			char **instruments, int count);
extern void           femas_mdapi_heartbeat_timeout(femas_mdapi_t *mdapi, unsigned int timeout);
extern int            femas_mdapi_user_login(femas_mdapi_t *mdapi,
			struct CUstpFtdcReqUserLoginField *userlogin, int rid);
extern int            femas_mdapi_user_logout(femas_mdapi_t *mdapi,
			struct CUstpFtdcReqUserLogoutField *userlogout, int rid);
extern int            femas_mdapi_subscribe_topic(femas_mdapi_t *mdapi,
			struct CUstpFtdcDisseminationField *dissemination, int rid);
extern int            femas_mdapi_query_topic(femas_mdapi_t *mdapi,
			struct CUstpFtdcDisseminationField *dissemination, int rid);
extern int            femas_mdapi_request_subscribe_market_data(femas_mdapi_t *mdapi,
			struct CUstpFtdcSpecificInstrumentField *instrument, int rid);
extern int            femas_mdapi_request_unsubscribe_market_data(femas_mdapi_t *mdapi,
			struct CUstpFtdcSpecificInstrumentField *instrument, int rid);
extern femas_mdspi_t *femas_mdspi_create(void);
extern void           femas_mdspi_destroy(femas_mdspi_t *mdspi);
extern void           femas_mdspi_on_front_connected(femas_mdspi_t *mdspi, femas_on_front_connected func);
extern void           femas_mdspi_on_front_disconnected(femas_mdspi_t *mdspi,
			femas_on_front_disconnected func);
extern void           femas_mdspi_on_heartbeat_warning(femas_mdspi_t *mdspi,
			femas_on_heartbeat_warning func);
extern void           femas_mdspi_on_package_start(femas_mdspi_t *mdspi, femas_on_package_start func);
extern void           femas_mdspi_on_package_end(femas_mdspi_t *mdspi, femas_on_package_end func);
extern void           femas_mdspi_on_error(femas_mdspi_t *mdspi, femas_on_error func);
extern void           femas_mdspi_on_user_login(femas_mdspi_t *mdspi, femas_on_user_login func);
extern void           femas_mdspi_on_user_logout(femas_mdspi_t *mdspi, femas_on_user_logout func);
extern void           femas_mdspi_on_subscribe_topic(femas_mdspi_t *mdspi, femas_on_subscribe_topic func);
extern void           femas_mdspi_on_query_topic(femas_mdspi_t *mdspi, femas_on_query_topic func);
extern void           femas_mdspi_on_subscribe_market_data(femas_mdspi_t *mdspi,
			femas_on_subscribe_market_data func);
extern void           femas_mdspi_on_unsubscribe_market_data(femas_mdspi_t *mdspi,
			femas_on_unsubscribe_market_data func);
extern void           femas_mdspi_on_deep_market_data(femas_mdspi_t *mdspi,
			femas_on_deep_market_data func);

#ifdef __cplusplus
}
#endif

#endif /* FEMAS_INCLUDED */


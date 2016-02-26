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
typedef void (*ctp_on_authenticate)(struct CThostFtdcRspAuthenticateField *auth,
		struct CThostFtdcRspInfoField *rspinfo, int rid, int islast);
typedef void (*ctp_on_user_login)(struct CThostFtdcRspUserLoginField *userlogin,
		struct CThostFtdcRspInfoField *rspinfo, int rid, int islast);
typedef void (*ctp_on_user_logout)(struct CThostFtdcUserLogoutField *userlogout,
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
extern ctp_tdspi_t *ctp_tdspi_create(void);
extern void         ctp_tdspi_destroy(ctp_tdspi_t *tdspi);
extern void         ctp_tdspi_on_front_connected(ctp_tdspi_t *tdspi, ctp_on_front_connected func);
extern void         ctp_tdspi_on_front_disconnected(ctp_tdspi_t *tdspi, ctp_on_front_disconnected func);
extern void         ctp_tdspi_on_heartbeat_warning(ctp_tdspi_t *tdspi, ctp_on_heartbeat_warning func);
extern void         ctp_tdspi_on_authenticate(ctp_tdspi_t *tdspi, ctp_on_authenticate func);
extern void         ctp_tdspi_on_user_login(ctp_tdspi_t *tdspi, ctp_on_user_login func);
extern void         ctp_tdspi_on_user_logout(ctp_tdspi_t *tdspi, ctp_on_user_logout func);

#ifdef __cplusplus
}
#endif

#endif /* CTP_INCLUDED */


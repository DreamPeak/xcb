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

#ifndef SEC2_INCLUDED
#define SEC2_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include "SECL2ApiDataType.h"

/* FIXME: exported types */
typedef struct sec2_mdapi_t sec2_mdapi_t;
typedef struct sec2_mdspi_t sec2_mdspi_t;
typedef void (*sec2_on_connected)(void);
typedef void (*sec2_on_disconnected)(int reason);
typedef void (*sec2_on_user_login)(struct ErrorRtnField *erf);
typedef void (*sec2_on_user_logout)(struct ErrorRtnField *erf);
typedef void (*sec2_on_subscribe_market_data)(struct ErrorRtnField *erf);
typedef void (*sec2_on_unsubscribe_market_data)(struct ErrorRtnField *erf);
typedef void (*sec2_on_subscribe_all)(struct ErrorRtnField *erf);
typedef void (*sec2_on_unsubscribe_all)(struct ErrorRtnField *erf);
typedef void (*sec2_on_modify_password)(struct ErrorRtnField *erf);
typedef void (*sec2_on_heartbeat_lost)(struct ErrorRtnField *erf);
typedef void (*sec2_on_sh3113)(struct SHFAST30_UA3113_t *quote);
typedef void (*sec2_on_sh3115)(struct SHFAST30_UA3115_t *quote);
typedef void (*sec2_on_sh3201)(struct SHFAST30_UA3201_t *quote);
typedef void (*sec2_on_sh3202)(struct SHFAST30_UA3202_t *quote);

/* FIXME: exported functions */
extern sec2_mdapi_t *sec2_mdapi_create(void);
extern void          sec2_mdapi_destroy(sec2_mdapi_t *mdapi);
extern int           sec2_mdapi_connect(sec2_mdapi_t *mdapi, char *addr, sec2_mdspi_t *mdspi, unsigned isudp);
extern int           sec2_mdapi_login_user(sec2_mdapi_t *mdapi, struct SECUserLoginField *userlogin);
extern int           sec2_mdapi_logout_user(sec2_mdapi_t *mdapi, struct SECUserLogoutField *userlogout);
extern int           sec2_mdapi_subscribe_market_data(sec2_mdapi_t *mdapi, char **instruments, int count);
extern int           sec2_mdapi_unsubscribe_market_data(sec2_mdapi_t *mdapi, char **instruments, int count);
extern int           sec2_mdapi_subscribe_all(sec2_mdapi_t *mdapi);
extern int           sec2_mdapi_unsubscribe_all(sec2_mdapi_t *mdapi);
extern int           sec2_mdapi_modify_password(sec2_mdapi_t *mdapi, struct SECPasswdChangeField *pcf);
extern sec2_mdspi_t *sec2_mdspi_create(void);
extern void          sec2_mdspi_destroy(sec2_mdspi_t *mdspi);
extern void          sec2_mdspi_on_connected(sec2_mdspi_t *mdspi, sec2_on_connected func);
extern void          sec2_mdspi_on_disconnected(sec2_mdspi_t *mdspi, sec2_on_disconnected func);
extern void          sec2_mdspi_on_user_login(sec2_mdspi_t *mdspi, sec2_on_user_login func);
extern void          sec2_mdspi_on_user_logout(sec2_mdspi_t *mdspi, sec2_on_user_logout func);
extern void          sec2_mdspi_on_subscribe_market_data(sec2_mdspi_t *mdspi,
			sec2_on_subscribe_market_data func);
extern void          sec2_mdspi_on_unsubscribe_market_data(sec2_mdspi_t *mdspi,
			sec2_on_unsubscribe_market_data func);
extern void          sec2_mdspi_on_subscribe_all(sec2_mdspi_t *mdspi, sec2_on_subscribe_all func);
extern void          sec2_mdspi_on_unsubscribe_all(sec2_mdspi_t *mdspi, sec2_on_unsubscribe_all func);
extern void          sec2_mdspi_on_modify_password(sec2_mdspi_t *mdspi, sec2_on_modify_password func);
extern void          sec2_mdspi_on_heartbeat_lost(sec2_mdspi_t *mdspi, sec2_on_heartbeat_lost func);
extern void          sec2_mdspi_on_sh3113(sec2_mdspi_t *mdspi, sec2_on_sh3113 func);
extern void          sec2_mdspi_on_sh3115(sec2_mdspi_t *mdspi, sec2_on_sh3115 func);
extern void          sec2_mdspi_on_sh3201(sec2_mdspi_t *mdspi, sec2_on_sh3201 func);
extern void          sec2_mdspi_on_sh3202(sec2_mdspi_t *mdspi, sec2_on_sh3202 func);

#ifdef __cplusplus
}
#endif

#endif /* SEC2_INCLUDED */


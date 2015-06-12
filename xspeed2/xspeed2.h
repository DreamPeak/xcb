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

#ifndef XSPEED2_INCLUDED
#define XSPEED2_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include "DFITCL2ApiDataType.h"

/* FIXME: exported types */
typedef struct xspeed_l2api_t xspeed_l2api_t;
typedef struct xspeed_l2spi_t xspeed_l2spi_t;
typedef void (*xspeed_on_front_connected)(void);
typedef void (*xspeed_on_front_disconnected)(int reason);
typedef void (*xspeed_on_user_login)(struct ErrorRtnField *rspinfo);
typedef void (*xspeed_on_user_logout)(struct ErrorRtnField *rspinfo);
typedef void (*xspeed_on_subscribe_market_data)(struct ErrorRtnField *rspinfo);
typedef void (*xspeed_on_unsubscribe_market_data)(struct ErrorRtnField *rspinfo);
typedef void (*xspeed_on_subscribe_all)(struct ErrorRtnField *rspinfo);
typedef void (*xspeed_on_unsubscribe_all)(struct ErrorRtnField *rspinfo);
typedef void (*xspeed_on_best_and_deep)(struct MDBestAndDeep *deepmd);

/* FIXME: exported functions */
extern xspeed_l2api_t *xspeed_l2api_create(void);
extern void            xspeed_l2api_destory(xspeed_l2api_t *l2api);
extern int             xspeed_l2api_user_login(xspeed_l2api_t *l2api,
			struct DFITCUserLoginField *userlogin);
extern int             xspeed_l2api_user_logout(xspeed_l2api_t *l2api,
			struct DFITCUserLogoutField *userlogout);
extern int             xspeed_l2api_connect(xspeed_l2api_t *l2api, char *svraddr,
			xspeed_l2spi_t *l2spi, unsigned int quote_type);
extern int             xspeed_l2api_subscribe_market_data(xspeed_l2api_t *l2api,
			char **instruments, int count);
extern int             xspeed_l2api_unsubscribe_market_data(xspeed_l2api_t *l2api,
			char **instruments, int count);
extern int             xspeed_l2api_subscribe_all(xspeed_l2api_t *l2api);
extern int             xspeed_l2api_unsubscribe_all(xspeed_l2api_t *l2api);
extern xspeed_l2spi_t *xspeed_l2spi_create(void);
extern void            xspeed_l2spi_destroy(xspeed_l2spi_t *l2spi);
extern void            xspeed_l2spi_on_front_connected(xspeed_l2spi_t *l2spi,
			xspeed_on_front_connected func);
extern void            xspeed_l2spi_on_front_disconnected(xspeed_l2spi_t *l2spi,
			xspeed_on_front_disconnected func);
extern void            xspeed_l2spi_on_user_login(xspeed_l2spi_t *l2spi, xspeed_on_user_login func);
extern void            xspeed_l2spi_on_user_logout(xspeed_l2spi_t *l2spi, xspeed_on_user_logout func);
extern void            xspeed_l2spi_on_subscribe_market_data(xspeed_l2spi_t *l2spi,
			xspeed_on_subscribe_market_data func);
extern void            xspeed_l2spi_on_unsubscribe_market_data(xspeed_l2spi_t *l2spi,
			xspeed_on_unsubscribe_market_data func);
extern void            xspeed_l2spi_on_subscribe_all(xspeed_l2spi_t *l2spi, xspeed_on_subscribe_all func);
extern void            xspeed_l2spi_on_unsubscribe_all(xspeed_l2spi_t *l2spi,
			xspeed_on_unsubscribe_all func);
extern void            xspeed_l2spi_on_best_and_deep(xspeed_l2spi_t *l2spi, xspeed_on_best_and_deep func);

#ifdef __cplusplus
}
#endif

#endif /* XSPEED2_INCLUDED */


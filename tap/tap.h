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

#ifndef TAP_INCLUDED
#define TAP_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include "TapAPIError.h"
#include "TapQuoteAPIDataType.h"

/* FIXME: exported types */
typedef struct tap_mdapi_t tap_mdapi_t;
typedef struct tap_mdspi_t tap_mdspi_t;
typedef void (*tap_on_login)(TAPIINT32 error, const struct TapAPIQuotLoginRspInfo *info);
typedef void (*tap_on_api_ready)(void);
typedef void (*tap_on_disconnect)(TAPIINT32 reason);
typedef void (*tap_on_change_passwd)(TAPIUINT32 sid, TAPIINT32 error);
typedef void (*tap_on_query_exchange)(TAPIUINT32 sid, TAPIINT32 error, TAPIYNFLAG islast,
		const struct TapAPIExchangeInfo *info);
typedef void (*tap_on_query_commodity)(TAPIUINT32 sid, TAPIINT32 error, TAPIYNFLAG isLast,
		const struct TapAPIQuoteCommodityInfo *info);
typedef void (*tap_on_query_time_bucket)(TAPIUINT32 sid, TAPIINT32 error, TAPIYNFLAG islast,
		const struct TapAPITimeBucketOfCommodityInfo *info);
typedef void (*tap_on_time_bucket)(const struct TapAPITimeBucketOfCommodityInfo *info);
typedef void (*tap_on_query_contract)(TAPIUINT32 sid, TAPIINT32 error, TAPIYNFLAG islast,
		const struct TapAPIQuoteContractInfo *info);
typedef void (*tap_on_contract)(const struct TapAPIQuoteContractInfo *info);
typedef void (*tap_on_subscribe_quote)(TAPIUINT32 sid, TAPIINT32 error, TAPIYNFLAG islast,
		const struct TapAPIQuoteWhole *info);
typedef void (*tap_on_unsubscribe_quote)(TAPIUINT32 sid, TAPIINT32 error, TAPIYNFLAG islast,
		const struct TapAPIContract *info);
typedef void (*tap_on_quote)(const struct TapAPIQuoteWhole *info);
typedef void (*tap_on_query_hisquote)(TAPIUINT32 sid, TAPIINT32 error, TAPIYNFLAG islast,
		const struct TapAPIHisQuoteQryRsp *info);

/* FIXME: exported functions */
extern tap_mdapi_t                   *tap_mdapi_create(const struct TapAPIApplicationInfo *appinfo,
					TAPIINT32 *iresult);
extern void                           tap_mdapi_destroy(tap_mdapi_t *mdapi);
extern TAPIINT32                      tap_mdapi_set_log_path(const TAPICHAR *path);
extern TAPIINT32                      tap_mdapi_set_log_level(TAPILOGLEVEL level);
extern TAPIINT32                      tap_mdapi_set_spi(tap_mdapi_t *mdapi, tap_mdspi_t *mdspi);
extern TAPIINT32                      tap_mdapi_set_host_addr(tap_mdapi_t *mdapi,
					const TAPICHAR *ip, TAPIUINT16 port);
extern TAPIINT32                      tap_mdapi_login(tap_mdapi_t *mdapi,
					const struct TapAPIQuoteLoginAuth *loginauth);
extern TAPIINT32                      tap_mdapi_disconnect(tap_mdapi_t *mdapi);
extern const TAPICHAR                *tap_mdapi_get_version(void);
extern TAPIINT32                      tap_mdapi_get_servtime(tap_mdapi_t *mdapi, TAPIDATETIME *datetime);
extern TAPIINT32                      tap_mdapi_change_passwd(tap_mdapi_t *mdapi, TAPIUINT32 *sid,
					const struct TapAPIChangePasswordReq *req);
extern TAPIINT32                      tap_mdapi_query_exchange(tap_mdapi_t *mdapi, TAPIUINT32 *sid);
extern TAPIINT32                      tap_mdapi_query_commodity(tap_mdapi_t *mdapi, TAPIUINT32 *sid);
extern TAPIINT32                      tap_mdapi_query_time_bucket(tap_mdapi_t *mdapi, TAPIUINT32 *sid,
					const struct TapAPICommodity *req);
extern TAPIINT32                      tap_mdapi_query_contract(tap_mdapi_t *mdapi, TAPIUINT32 *sid,
					const struct TapAPICommodity *req);
extern TAPIINT32                      tap_mdapi_subscribe_quote(tap_mdapi_t *mdapi, TAPIUINT32 *sid,
					const struct TapAPIContract *contract);
extern TAPIINT32                      tap_mdapi_unsubscribe_quote(tap_mdapi_t *mdapi, TAPIUINT32 *sid,
					const struct TapAPIContract *contract);
extern TAPIINT32                      tap_mdapi_query_hisquote(tap_mdapi_t *mdapi, TAPIUINT32 *sid,
					const struct TapAPIHisQuoteQryReq *req);
extern const struct TapAPIQuoteWhole *tap_mdapi_get_fullquote(tap_mdapi_t *mdapi,
					const struct TapAPIContract *contract);
extern tap_mdspi_t                   *tap_mdspi_create(void);
extern void                           tap_mdspi_destroy(tap_mdspi_t *mdspi);
extern void                           tap_mdspi_on_login(tap_mdspi_t *mdspi, tap_on_login func);
extern void                           tap_mdspi_on_api_ready(tap_mdspi_t *mdspi, tap_on_api_ready func);
extern void                           tap_mdspi_on_disconnect(tap_mdspi_t *mdspi, tap_on_disconnect func);
extern void                           tap_mdspi_on_change_passwd(tap_mdspi_t *mdspi,
					tap_on_change_passwd func);
extern void                           tap_mdspi_on_query_exchange(tap_mdspi_t *mdspi,
					tap_on_query_exchange func);
extern void                           tap_mdspi_on_query_commodity(tap_mdspi_t *mdspi,
					tap_on_query_commodity func);
extern void                           tap_mdspi_on_query_time_bucket(tap_mdspi_t *mdspi,
					tap_on_query_time_bucket func);
extern void                           tap_mdspi_on_time_bucket(tap_mdspi_t *mdspi, tap_on_time_bucket func);
extern void                           tap_mdspi_on_query_contract(tap_mdspi_t *mdspi,
					tap_on_query_contract func);
extern void                           tap_mdspi_on_contract(tap_mdspi_t *mdspi, tap_on_contract func);
extern void                           tap_mdspi_on_subscribe_quote(tap_mdspi_t *mdspi,
					tap_on_subscribe_quote func);
extern void                           tap_mdspi_on_unsubscribe_quote(tap_mdspi_t *mdspi,
					tap_on_unsubscribe_quote func);
extern void                           tap_mdspi_on_quote(tap_mdspi_t *mdspi, tap_on_quote func);
extern void                           tap_mdspi_on_query_hisquote(tap_mdspi_t *mdspi,
					tap_on_query_hisquote func);

#ifdef __cplusplus
}
#endif

#endif /* TAP_INCLUDED */


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

#include <errno.h>
#include <poll.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <algorithm>
#include <map>
#include <string>
#include "utils.h"
#include "logger.h"
#include "config.h"
#include "ThostFtdcTraderApi.h"

/* FIXME */
static struct config *cfg;
static std::map<std::string, std::string> expiries;
static int terminate_pipe[2];
static char path[256];
static char front[1024];

class CMySpi : public CThostFtdcTraderSpi {
public:
	CMySpi(CThostFtdcTraderApi *pApi) : m_pApi(pApi) {}

private:
	virtual void OnFrontConnected() {
		struct CThostFtdcReqUserLoginField req;

		memset(&req, '\0', sizeof req);
		strncat(req.BrokerID, variable_retrieve(cfg, "general", "brokerid"), sizeof req.BrokerID - 1);
		strncat(req.UserID,   variable_retrieve(cfg, "general", "userid"),   sizeof req.UserID - 1);
		strncat(req.Password, variable_retrieve(cfg, "general", "passwd"),   sizeof req.Password - 1);
		m_pApi->ReqUserLogin(&req, 0);
	}
	virtual void OnFrontDisconnected(int nReason) {
		close_logger();
		exit(1);
	}
	virtual void OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {}
	virtual void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,
		CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
		if (pRspInfo && pRspInfo->ErrorID != 0) {
			xcb_log(XCB_LOG_ERROR, "Error logging in: %d,%s",
				pRspInfo->ErrorID, pRspInfo->ErrorMsg);
			close_logger();
			exit(1);
		} else {
			CThostFtdcQryInstrumentField req;

			memset(&req, 0, sizeof(req));
			m_pApi->ReqQryInstrument(&req, 1);
		}
	}
	virtual void OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout,
		CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {}
	virtual void OnRspQryInstrument(CThostFtdcInstrumentField *pInstrument,
		CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
		if (pRspInfo && pRspInfo->ErrorID != 0) {
			xcb_log(XCB_LOG_ERROR, "Error querying instrument: %d,%s",
				pRspInfo->ErrorID, pRspInfo->ErrorMsg);
			close_logger();
			exit(1);
		} else {
			if (pInstrument->ProductClass == THOST_FTDC_PC_Options &&
				strlen(pInstrument->InstrumentID) > 0 && strlen(pInstrument->ExpireDate) > 0)
				expiries.insert(std::make_pair(pInstrument->InstrumentID,
					pInstrument->ExpireDate));
			if (bIsLast) {
				const char one = '1';

				if (write(terminate_pipe[1], &one, sizeof(one)) == -1) {
					xcb_log(XCB_LOG_ERROR, "Error writing: %s", strerror(errno));
					close_logger();
					exit(1);
				}
			}
		}
	}

private:
	CThostFtdcTraderApi *m_pApi;
};

static void usage(void) {
	fprintf(stderr, "Usage: ./xcb-ctp path/to/xcb-ctp.conf\n");
	fprintf(stderr, "       ./xcb-ctp -h or --help\n");
	exit(1);
}

int main(int argc, char **argv) {
	const char *tmp;
	CThostFtdcTraderApi *pApi;
	CMySpi *pSpi;
	struct pollfd rfd[1];
	std::map<std::string, std::string>::iterator iter;

	if (argc != 2)
		usage();
	else if (!strcmp(argv[1], "-h") || !strcmp(argv[1], "--help"))
		usage();
	/* FIXME */
	if (init_logger("/var/log/xcb/xcb-ctp.log", __LOG_DEBUG) == -1) {
		fprintf(stderr, "Error initializing logger\n");
		exit(1);
	}
	if ((cfg = config_load(argv[1])) == NULL)
		exit(1);
	if ((tmp = variable_retrieve(cfg, "general", "log_level"))) {
		if (!strcasecmp(tmp, "info"))
			set_logger_level(__LOG_INFO);
		else if (!strcasecmp(tmp, "notice"))
			set_logger_level(__LOG_NOTICE);
		else if (!strcasecmp(tmp, "warning"))
			set_logger_level(__LOG_WARNING);
	}
	if (pipe(terminate_pipe) != 0) {
		xcb_log(XCB_LOG_ERROR, "Error creating pipe: %s", strerror(errno));
		goto end;
	}
	snprintf(path, sizeof path, "/var/log/xcb/%s/", variable_retrieve(cfg, "general", "userid"));
	makedir(path, 0777);
	pApi = CThostFtdcTraderApi::CreateFtdcTraderApi(path);
	pSpi = new CMySpi(pApi);
	snprintf(front, sizeof front, "tcp://%s:%s",
		variable_retrieve(cfg, "general", "front_ip"),
		variable_retrieve(cfg, "general", "front_port"));
	pApi->RegisterFront(front);
	pApi->RegisterSpi(pSpi);
	pApi->Init();
	rfd[0].fd     = terminate_pipe[0];
	rfd[0].events = POLLIN;
	if (poll(rfd, 1, -1) == -1) {
		xcb_log(XCB_LOG_ERROR, "Error polling: %s", strerror(errno));
		goto end;
	}
	/* FIXME */
	xcb_log(XCB_LOG_NOTICE, "Outputting expiries ...");
	fprintf(stdout, "[expiries]\n");
	for (iter = expiries.begin(); iter != expiries.end(); ++iter) {
		iter->second.erase(std::remove(iter->second.begin(), iter->second.end(), '.'),
			iter->second.end());
		fprintf(stdout, "%s=%s\n", iter->first.c_str(), iter->second.c_str());
	}
	fprintf(stdout, "\n");
	close(terminate_pipe[0]);
	close(terminate_pipe[1]);
	close_logger();
	return 0;

end:
	close_logger();
	exit(1);
}


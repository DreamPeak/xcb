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

#include <errno.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <algorithm>
#include <map>
#include <sstream>
#include <string>
#include "logger.h"
#include "config.h"
#include "DFITCSECTraderApi.h"

/* FIXME */
static struct config *cfg;
static std::map<std::string, std::string> expiries;
static int terminate_pipe[2];
static char front[1024];
static int count = 0;

class CMySpi : public DFITCSECTraderSpi {
public:
	virtual ~CMySpi() {}
	CMySpi(DFITCSECTraderApi *pApi) : m_pApi(pApi) {}

private:
	virtual void OnFrontConnected() {
		struct DFITCSECReqUserLoginField lData;

		memset(&lData, '\0', sizeof lData);
		/* FIXME */
		lData.requestID = 0;
		strcpy(lData.accountID, variable_retrieve(cfg, "general", "account"));
		strcpy(lData.password, variable_retrieve(cfg, "general", "passwd"));
		m_pApi->ReqSOPUserLogin(&lData);
	}
	virtual void OnFrontDisconnected(int nReason) {}
	virtual void OnRspError(struct DFITCSECRspInfoField *pRspInfo) {}
	virtual void OnRspSOPUserLogin(struct DFITCSECRspUserLoginField *pRspUserLogin,
		struct DFITCSECRspInfoField *pRspInfo) {
		if (pRspInfo) {
			xcb_log(XCB_LOG_ERROR, "Error logging in: %d,%s",
				pRspInfo->errorID, pRspInfo->errorMsg);
			close_logger();
			exit(1);
		} else {
			struct DFITCSOPReqQryContactField iData;

			memset(&iData, '\0', sizeof iData);
			/* FIXME */
			iData.requestID = 1;
			strcpy(iData.accountID, variable_retrieve(cfg, "general", "account"));
			/* iData.contractObjectType = DFITCSEC_COT_ETF; */
			m_pApi->ReqSOPQryContactInfo(&iData);
		}
	}
	virtual void OnRspSOPUserLogout(struct DFITCSECRspUserLogoutField *pRspUsrLogout,
		struct DFITCSECRspInfoField *pRspInfo) {}
	virtual void OnRspSOPQryContactInfo(struct DFITCSOPRspQryContactField *pData,
		struct DFITCSECRspInfoField *pRspInfo, bool bIsLast) {
		if (pRspInfo) {
			xcb_log(XCB_LOG_ERROR, "Error querying instrument: %d,%s",
				pRspInfo->errorID, pRspInfo->errorMsg);
			close_logger();
			exit(1);
		} else {
			if (strlen(pData->contractID) > 0) {
				std::ostringstream oss;

				oss << pData->endTradingDay;
				expiries.insert(std::make_pair(pData->contractID, oss.str()));
			}
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

protected:
	DFITCSECTraderApi *m_pApi;
};

static void usage(void) {
	fprintf(stderr, "Usage: ./xcb-exp2 path/to/xcb-exp2.conf\n");
	fprintf(stderr, "       ./xcb-exp2 -h or --help\n");
	exit(1);
}

int main(int argc, char **argv) {
	const char *tmp;
	DFITCSECTraderApi *pApi = DFITCSECTraderApi::CreateDFITCSECTraderApi(NULL);
	CMySpi spi(pApi);
	struct pollfd rfd[1];
	std::map<std::string, std::string>::iterator iter;

	if (argc != 2)
		usage();
	else if (!strcmp(argv[1], "-h") || !strcmp(argv[1], "--help"))
		usage();
	/* FIXME */
	if (init_logger("/var/log/xcb/xcb-exp2.log", __LOG_DEBUG) == -1) {
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
	snprintf(front, sizeof front, "tcp://%s:%s",
		variable_retrieve(cfg, "general", "front_ip"),
		variable_retrieve(cfg, "general", "front_port"));
	if (pApi->Init(front, &spi) != 0) {
		xcb_log(XCB_LOG_ERROR, "Error initializing Api");
		goto end;
	}
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
	pApi->Release();
	close(terminate_pipe[0]);
	close(terminate_pipe[1]);
	close_logger();
	return 0;

end:
	close_logger();
	exit(1);
}


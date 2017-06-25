/////////////////////////////////////////////////////////////////////////////
//
// rtsp.c
//
// MiniWeb - mini webserver implementation
//
/////////////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "httppil.h"
#include "httpapi.h"
#include "httpint.h"

extern FILE *fpLog;

#define LOG_INFO fpLog

#define RTP_VIDEO_PORT 1234
#define RTP_AUDIO_PORT 1236

int uhRTSP(UrlHandlerParam* param)
{
	if (param->hs->flags & FLAG_REQUEST_OPTIONS) {
		param->dataBytes = _snprintf(param->pucBuffer, param->dataBytes,
		"RTSP/1.0 200 OK\r\nCseq: %d\r\nPublic: DESCRIBE, SETUP, TEARDOWN, PLAY\r\n\r\n",
		param->hs->request.iCSeq
		);
		SYSLOG(LOG_INFO, "OPTIONS command answered\n");
	} else if (param->hs->flags & (FLAG_REQUEST_DESCRIBE | FLAG_REQUEST_GET)) {
		// DESCRIBE
		// also allowed for HTTP request
		char sdp[256];
		int sdplen = _snprintf(sdp, sizeof(sdp),
			"v=0\r\n"
			"o=- 0 0 IN IP4 127.0.0.1\r\n"
			"s=No Name\r\n"
			"c=IN IP4 %d.%d.%d.%d\r\n"
			"t=0 0\r\n"
			"a=tool:miniweb\r\n"
#if 0
			"m=audio 1232 RTP/AVP 0\r\n"
			"a=control:rtsp://sdp/audio\r\n"
#endif
			"m=video 1234 RTP/AVP 96\r\n"
			"a=control:%s/video\r\n"
			"b=AS:0\r\n"
			"a=rtpmap:96 H264/90000\r\n"
			"a=fmtp:96 profile-level-id=1\r\n",
			param->hs->ipAddr.caddr[3],
			param->hs->ipAddr.caddr[2],
			param->hs->ipAddr.caddr[1],
			param->hs->ipAddr.caddr[0],
			param->hs->request.pucPath,param->hs->request.pucPath
		);
		if (param->hs->flags & FLAG_REQUEST_GET) {
			param->dataBytes = _snprintf(param->pucBuffer, param->dataBytes, "%s", sdp);
			param->fileType = HTTPFILETYPE_SDP;
			return FLAG_DATA_RAW;
		}
		param->dataBytes = _snprintf(param->pucBuffer, param->dataBytes,
			"%s/1.0 200 OK\r\n"
			"CSeq: %d\r\n"
			"Content-Type: application/sdp\r\n"
			"Content-Length: %d\r\n\r\n%s",
			param->hs->flags & FLAG_REQUEST_DESCRIBE ? "RTSP" : "HTTP",
			param->hs->request.iCSeq, sdplen, sdp);
		SYSLOG(LOG_INFO, "DESCRIBE command answered\n");
	} else if (param->hs->flags & FLAG_REQUEST_SETUP) {
		// SETUP
		int port;
		if (!strcmp(param->pucRequest, "/audio"))
			port = RTP_AUDIO_PORT;
		else if (!strcmp(param->pucRequest, "/video"))
			port = RTP_VIDEO_PORT;
		else
			return 0;	// invalid request

		param->dataBytes = _snprintf(param->pucBuffer, param->dataBytes,
			"RTSP/1.0 200 OK\r\n"
			"CSeq: %d\r\n"
            "Transport: RTP/AVP;unicast;client_port=%d-%d;server_port=0\r\n"
            "Session: 12345678\r\n\r\n",
			param->hs->request.iCSeq, port, port + 1);
		SYSLOG(LOG_INFO, "SETUP command answered\n");
	} else if (param->hs->flags & FLAG_REQUEST_PLAY) {
		// PLAY
		param->dataBytes = _snprintf(param->pucBuffer, param->dataBytes,
			"RTSP/1.0 200 OK\r\n"
			"CSeq: %d\r\n"
			"Session: 12345678\r\n"
			"RTP-Info: url=%s/video\r\n\r\n",
			param->hs->request.iCSeq, param->hs->request.pucPath);
		SYSLOG(LOG_INFO, "PLAY command answered\n");
	} else if (param->hs->flags & FLAG_REQUEST_TEARDOWN) {
		// TEARDOWN
		param->dataBytes = _snprintf(param->pucBuffer, param->dataBytes,
			"RTSP/1.0 200 OK\r\n"
			"CSeq: %d\r\n\r\n",
			param->hs->request.iCSeq);
		SYSLOG(LOG_INFO, "TEARDOWN command answered\n");
	} else {
		DBG("Unhanded RTSP request\r\n%s\r\n", param->pucHeader);
		return 0;
	}
	DBG(param->pucBuffer);
	return FLAG_DATA_RAW | FLAG_CUSTOM_HEADER;
}

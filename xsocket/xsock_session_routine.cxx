#include <xsocket/sock_session_routine.hxx>

#include <cstring>
#include <posix/func/usleep.h>

#if defined(XSOCKET_LOGLEVEL)
#	include <x_logfile.hxx>
#endif

#include <xsocket/sock_core.hxx>
#include <xsocket/on_session.hxx>

xsocket::core::internal::SockSessionRoutine::SockSessionRoutine (int cli_fd,
	const xsocket::NetProtocol& client,
	int svr_fd, const xsocket::NetProtocol& server,
	xsocket::OnSession * on_session)
{
	++instance_num;

#if defined(XSOCKET_LOGLEVEL) && (XSOCKET_LOGLEVEL >= 0x20)
	xlog::AppendV2(__func__, __FILE__, __LINE__, instance_num, XLOG_LEVEL_T);
#endif

	this->cli_fd = cli_fd;
	this->svr_fd = svr_fd;
	memcpy(&(this->client), &client, sizeof(xsocket::NetProtocol));
	memcpy(&(this->server), &server, sizeof(xsocket::NetProtocol));
	this->on_session = on_session;
	this->gc = new xsocket::core::internal::SockSessionRoutineGC();
}

xsocket::core::internal::SockSessionRoutine::~SockSessionRoutine ()
{
	--instance_num;

#if defined(XSOCKET_LOGLEVEL) && (XSOCKET_LOGLEVEL >= 0x20)
	xlog::AppendV2(__func__, __FILE__, __LINE__, instance_num, XLOG_LEVEL_T);
#endif
}

void * xsocket::core::internal::SockSessionRoutine::run (void * /* nil */)
{
#if	defined(XSOCKET_LOGLEVEL) && (XSOCKET_LOGLEVEL >= 0x20)
	xlog::AppendV2(__func__, __FILE__, __LINE__, 0, XLOG_LEVEL_T);
#endif

	xsocket::OnSession * os = this->on_session;
	this->on_session = NULL;

	if (NULL == os) {
#if		defined(XSOCKET_LOGLEVEL) && (XSOCKET_LOGLEVEL >= 0x0)
		xlog::AppendV2("FATAL: NULL on session. now exit", __FILE__,
			__LINE__, 0, XLOG_LEVEL_F);
#endif
		return (void *)(long int)-EINVAL;
	}

	int cli_fd = this->cli_fd;
	int svr_fd = this->svr_fd;

#if	defined(XSOCKET_LOGLEVEL) && (XSOCKET_LOGLEVEL >= 0x30)
	int i;
#endif
	int ret, rcved_bytes;
	bool prev_ok;
	struct timeval timeout_readable;
	struct timeval timeout_rcv;
	int nfds;
	fd_set readfds;
	char msg[128];

	xsocket::SockDidFinish didfi;
	xsocket::SockRecved rcved;
	rcved.data = new uint8_t[4096];
	rcved.count = 0;
	rcved.fd = cli_fd;
	rcved.fd_peer = svr_fd;
	memcpy(&(rcved.info), &(this->client), sizeof(xsocket::NetProtocol));
	memcpy(&(rcved.peer), &(this->server), sizeof(xsocket::NetProtocol));

	didfi.code = 0;
	didfi.fd = cli_fd;
	didfi.fd_peer = svr_fd;
	memcpy(&(didfi.info), &(this->client), sizeof(xsocket::NetProtocol));
	memcpy(&(didfi.peer), &(this->server), sizeof(xsocket::NetProtocol));

	bool tmi = os->shouldTeminate(cli_fd);
	if (! tmi) {
		tmi = os->should_exit();
	}

	if (tmi) {
#if		defined(XSOCKET_LOGLEVEL) && (XSOCKET_LOGLEVEL >= 0x18)
		xlog::AppendV2("user teminate", __FILE__, __LINE__, 0, XLOG_LEVEL_I);
#endif
		didfi.code = 0;
		goto end;
	}

	while (! tmi) {
		/* set select timeout for readable */
		timeout_readable.tv_sec = XSOCKET_WAIT_READABLE_TIMEOUT;
		timeout_readable.tv_usec = 0;

		/* set recv fd */
		FD_ZERO(&readfds);
		FD_SET(cli_fd, &readfds);
		nfds = (cli_fd) + 1;

		/* selecting */
		ret = select(nfds, &readfds, NULL, NULL, &timeout_readable);

		prev_ok = false;

		if (-1 == ret) {
			ret = errno;
#if			defined(XSOCKET_LOGLEVEL) && (XSOCKET_LOGLEVEL >= 0x8)
			snprintf(msg, 127, "FAIL: select: %s. and will teminate",
				strerror(ret));
			msg[127] = '\0';
			xlog::AppendV2(msg, __FILE__, __LINE__, ret, XLOG_LEVEL_E);
#endif /* defined(XSOCKET_LOGLEVEL) ... */
			didfi.code = -ret;
			goto end;
		} else if (0 == ret) {
			/*
			 * no ready and timetout
			 */
#if			defined(XSOCKET_LOGLEVEL) && (XSOCKET_LOGLEVEL >= 0x30)
			xlog::AppendV2("wait readable timeout", __FILE__, __LINE__, 0,
				XLOG_LEVEL_XV);
#endif
		} else if (ret > 0) { /* XXX-FIXED: add > 0 */
#if			defined(XSOCKET_LOGLEVEL) && (XSOCKET_LOGLEVEL >= 0x28)
			xlog::AppendV2("now readable", __FILE__, __LINE__, 0,
				XLOG_LEVEL_V);
#endif
			prev_ok = true;
		}

		/* check and set timeout */
		if (prev_ok) {
			prev_ok = false;

			/*
			 * check if ready to read(recv)
			 */
			if (FD_ISSET(cli_fd, &readfds)) {
				/*
				 * ready to receive
				 * XXX-NOTE: current connected socket
				 */
				(void)memset(rcved.data, 0, 4096);

				/*
				 * set recv
				 */
				timeout_rcv.tv_sec = XSOCKET_READ_TIMEOUT;
				timeout_rcv.tv_usec = 0;

#				if !defined(WIN32)
				ret = setsockopt(cli_fd, SOL_SOCKET, SO_RCVTIMEO, &timeout_rcv,
					(socklen_t)sizeof(struct timeval));
#				else
				ret = setsockopt(cli_fd, SOL_SOCKET, SO_RCVTIMEO,
					reinterpret_cast<char *>(&timeout_rcv),
					(int)sizeof(struct timeval));
#endif

				if (ret < 0) {
					ret = errno;
#if					defined(XSOCKET_LOGLEVEL) && (XSOCKET_LOGLEVEL >= 0x8)
					snprintf(msg, 127,
						"FAIL: setsockopt: %s. and will teminate",
						strerror(ret));
					msg[127] = '\0';
					xlog::AppendV2(msg, __FILE__, __LINE__, ret, XLOG_LEVEL_F);
#endif
					didfi.code = -ret;
					goto end;
				} else {
					prev_ok = true;
				}
			}
		} /* if prev_ok */

		/* recv */
		rcved_bytes = -1;
		if (prev_ok) {
#if			defined(XSOCKET_LOGLEVEL) && (XSOCKET_LOGLEVEL >= 0x28)
			xlog::AppendV2("will recv", __FILE__, __LINE__, 0, XLOG_LEVEL_V);
#endif
			/*
			 * to recv and wait till timeout
			 *
			 * to recv till timeout
			 * MSG_WAITALL: wait till data is available
			 * (SOCK_STREAM only).
			 */
			errno = 0;
			/* ret = recv(cli_fd, rcved.data, 4096, MSG_WAITALL); */
#			if !defined(WIN32)
			ret = recv(cli_fd, rcved.data, 4096, MSG_DONTWAIT);
#			else
			ret = recv(cli_fd, reinterpret_cast<char *>(rcved.data), 4096,
				0);
#endif

			if ((ret < 0) && (EAGAIN == errno)) {
#if				defined(XSOCKET_LOGLEVEL) && (XSOCKET_LOGLEVEL >= 0x10)
				xlog::AppendV2("recv timeout", __FILE__, __LINE__, 0,
					XLOG_LEVEL_W);
#endif
			} else if ((ret < 0) && (EWOULDBLOCK == errno)) {
#if				defined(XSOCKET_LOGLEVEL) && (XSOCKET_LOGLEVEL >= 0x8)
				xlog::AppendV2("EWOULDBLOCK: recv and will teminate",
					__FILE__, __LINE__, 0, XLOG_LEVEL_W);
#endif
				didfi.code = -EWOULDBLOCK;
				goto end;
			} else if (ret <0) {
				ret = errno;
#if				defined(XSOCKET_LOGLEVEL) && (XSOCKET_LOGLEVEL >= 0x0)
				snprintf(msg, 127,
					"FAIL: recv: %s. and will teminate",
					strerror(ret));
				msg[127] = '\0';
				xlog::AppendV2(msg, __FILE__, __LINE__, ret, XLOG_LEVEL_F);
#endif
				didfi.code = -ret;
				goto end;
			} else {
				/* success */
				rcved_bytes = ret;
#if				defined(XSOCKET_LOGLEVEL) && (XSOCKET_LOGLEVEL >= 0x28)
				xlog::AppendV2("recv success", __FILE__, __LINE__,
				rcved_bytes, XLOG_LEVEL_V);
#endif
				prev_ok = true;
			}
		}

		/*
		 * connection lost
		 */
		if (prev_ok && (0 == rcved_bytes)) {
#if			defined(XSOCKET_LOGLEVEL) && (XSOCKET_LOGLEVEL >= 0x10)
				xlog::AppendV2("WARN: connection lost and will teminate",
				__FILE__, __LINE__, 0, XLOG_LEVEL_W);
#endif
			didfi.code = 1;
			goto end;
		}

		if (prev_ok && (rcved_bytes > 0)) {
			/* Show when need */
#if			defined(XSOCKET_LOGLEVEL) && (XSOCKET_LOGLEVEL >= 0x30)
			for (i = 0; i < rcved_bytes; ++i) {
				fprintf(stdout, "0x%02x ", rcved.data[i]);
				if (0 == ((i + 1) % 16)) {
					fprintf(stdout, "\n");
				}
			}

			if (0 != (i % 16)) {
				fprintf(stdout, "\n");
			}
#endif
			/*
			 * tell/callback external
			 */
			rcved.count = rcved_bytes;
			os->onReceived(rcved);
		}

		tmi = os->shouldTeminate(cli_fd);

		if (! tmi) {
			tmi = os->should_exit();
		}
		if (tmi) {
#if			defined(XSOCKET_LOGLEVEL) && (XSOCKET_LOGLEVEL >= 0x18)
			xlog::AppendV2("user teminate", __FILE__, __LINE__, 0,
				XLOG_LEVEL_I);
#endif
			didfi.code = 0;
			goto end;
		}
	} /* while */

end:
	if (NULL != rcved.data) {
		delete rcved.data;
		rcved.data = NULL;
	}
	xsocket::core::ShutdownSocket(cli_fd, xsocket::ShutdownHow::RDWR);
	xsocket::core::CloseSocket(cli_fd);
	os->didFinish(didfi);
	os->markExited(cli_fd);
	os = NULL;

#if	defined(XSOCKET_LOGLEVEL) && (XSOCKET_LOGLEVEL >= 0x18)
	xlog::AppendV2(
		"i'll exit(delete self) and NOT delete on session(will by server)",
		__FILE__, __LINE__, 0);
#endif
	if (NULL != this->gc) {
		this->gc->gc(this);
	}

	return NULL;
}

int xsocket::core::internal::SockSessionRoutine::instance_num = 0;

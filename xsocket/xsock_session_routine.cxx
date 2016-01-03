#include <xsocket/sock_session_routine.hxx>

#include <math.h>
#include <cstring>

#include <posix/func/usleep.h>
#include <posix/simp_sync_mutx.h>
#include <x_logfile.hxx>

#include <xsocket/sock_core.hxx>
#include <xsocket/on_session.hxx>

QList<xsocket::core::internal::SockSessionRoutine::XsocketReceiverPoor>
	xsocket::core::internal::SockSessionRoutine::wait_poor;
QList<xsocket::core::internal::SockSessionRoutine::XsocketReceiverPoor>
	xsocket::core::internal::SockSessionRoutine::work_poor;
bool xsocket::core::internal::SockSessionRoutine::inited = false;
uint8_t xsocket::core::internal::SockSessionRoutine::unlocked = 1;

void xsocket::core::internal::SockSessionRoutine::deinit (void)
{
	xlog::AppendV2("!!!!!!! MASTER FINAL deinit", __FILE__, __LINE__,
		0, XLOG_LEVEL_0);

	int tot = 0;
	CAPTURE_LOCK(xsocket::core::internal::SockSessionRoutine::unlocked);
	int wn = xsocket::core::internal::SockSessionRoutine::wait_poor
		.size();
	if (wn > 0) {
		tot += wn;
		for (int i = 0; i < wn; ++i) {
			xsocket::core::internal::SockSessionRoutine::XsocketReceiverPoor rp
				= xsocket::core::internal::SockSessionRoutine::wait_poor.at(i);
			rp.receiver->should_deinit = true;
			if (rp.receiver->cli_fd > 0) {
				xsocket::core::ShutdownSocket(rp.receiver->cli_fd,
					xsocket::ShutdownHow::RDWR);
				xsocket::core::CloseSocket(rp.receiver->cli_fd);
			}
			xlog::AppendV2("notified", __FILE__, __LINE__, (int)rp.receiver,
				XLOG_LEVEL_0);
		}
	}
	int en = xsocket::core::internal::SockSessionRoutine::work_poor
		.size();
	if (en > 0) {
		tot += en;
		for (int i = 0; i < en; ++i) {
			xsocket::core::internal::SockSessionRoutine::XsocketReceiverPoor rp
				= xsocket::core::internal::SockSessionRoutine::work_poor.front();
			xlog::AppendV2("notified", __FILE__, __LINE__, (int)rp.receiver,
				XLOG_LEVEL_0);
			rp.receiver->should_deinit = true;
			if (rp.receiver->cli_fd > 0) {
				xsocket::core::ShutdownSocket(rp.receiver->cli_fd,
					xsocket::ShutdownHow::RDWR);
				xsocket::core::CloseSocket(rp.receiver->cli_fd);
			}
		}
	}
	RELEASE_LOCK(xsocket::core::internal::SockSessionRoutine::unlocked);
	int to = 0;
	while (xsocket::core::internal::SockSessionRoutine::instance_num > 0) {
		usleep(100 * 1e3);
		++to;
		if (to > (tot * 10)) {
			break;
		}
	}
	usleep(500 * 1e3);
}

int xsocket::core::internal::SockSessionRoutine::push (
	int cli_fd, const xsocket::NetProtocol& client,
	int svr_fd, const xsocket::NetProtocol& server,
	xsocket::OnSession * on_session)
{
	CAPTURE_LOCK(xsocket::core::internal::SockSessionRoutine::unlocked);
	if (! xsocket::core::internal::SockSessionRoutine::inited) {
		xsocket::core::internal::SockSessionRoutine::wait_poor.clear();
		xsocket::core::internal::SockSessionRoutine::work_poor.clear();
		xsocket::core::internal::SockSessionRoutine::XsocketReceiverPoor rp;
		for (int i = 0; i < XSOCKET_POOR_CHANGE; ++i) {
			rp.receiver = new xsocket::core::internal::SockSessionRoutine();
			rp.receiver->start();
			xsocket::core::internal::SockSessionRoutine::wait_poor.append(rp);
		}
		xsocket::core::internal::SockSessionRoutine::inited = true;
	}
	const int wn = xsocket::core::internal::SockSessionRoutine::wait_poor
		.size();
	if (wn <= 0) {
		const int en
			= xsocket::core::internal::SockSessionRoutine::work_poor.size();
		xlog::AppendV2("too less wait to new some (en)", __FILE__, __LINE__,
			en, XLOG_LEVEL_0);
		xsocket::core::internal::SockSessionRoutine::XsocketReceiverPoor rp;
		for (int i = 0; i < XSOCKET_POOR_CHANGE; ++i) {
			rp.receiver = new xsocket::core::internal::SockSessionRoutine();
			rp.receiver->start();
			xsocket::core::internal::SockSessionRoutine::wait_poor.append(rp);
		}
	}

	xsocket::core::internal::SockSessionRoutine::XsocketReceiverPoor rp
		= xsocket::core::internal::SockSessionRoutine::wait_poor.front();
	xsocket::core::internal::SockSessionRoutine::wait_poor.pop_front();

	rp.receiver->cli_fd = cli_fd;
	rp.receiver->client = client;
	rp.receiver->svr_fd = svr_fd;
	rp.receiver->server = server;
	rp.receiver->on_session = on_session;

	xsocket::core::internal::SockSessionRoutine::work_poor.append(rp);
	rp.receiver->activted = true;
	RELEASE_LOCK(xsocket::core::internal::SockSessionRoutine::unlocked);
	return 0;
}

void xsocket::core::internal::SockSessionRoutine::markHasNotActivited (
	const xsocket::core::internal::SockSessionRoutine * s)
{
	xlog::AppendV2(__func__, __FILE__, __LINE__, (int)s, XLOG_LEVEL_0);
	static int shx = 0;
	CAPTURE_LOCK(xsocket::core::internal::SockSessionRoutine::unlocked);
	bool found = false;
	xsocket::core::internal::SockSessionRoutine::XsocketReceiverPoor rp;
	const int en
		= xsocket::core::internal::SockSessionRoutine::work_poor.size();
	for (int i = 0; i < en; ++i) {
		rp = xsocket::core::internal::SockSessionRoutine::work_poor.at(i);
		if ((const long unsigned int)s == (const long unsigned int)
			(rp.receiver)) {
			xsocket::core::internal::SockSessionRoutine::work_poor.removeAt(i);
			found = true;
			break;
		}
	}
	if (found && (NULL != s)) {
		rp.receiver->activted = false;
		xsocket::core::internal::SockSessionRoutine::wait_poor.append(rp);
	}
	const int wn = xsocket::core::internal::SockSessionRoutine::wait_poor
		.size();
	if (wn > XSOCKET_POOR_CHANGE) {
		++shx;
	} else {
		shx = 0;
	}
	if (shx >= XSOCKET_POOR_CHANGE) {
		shx = 0;
		const int x = wn - XSOCKET_POOR_CHANGE;
		xlog::AppendV2("too more wait to deinit some", __FILE__, __LINE__,
			x, XLOG_LEVEL_0);
		for (int i = 0; i < x; ++i) {
			xsocket::core::internal::SockSessionRoutine::XsocketReceiverPoor rp
				= xsocket::core::internal::SockSessionRoutine::wait_poor.front();
			xsocket::core::internal::SockSessionRoutine::wait_poor.pop_front();
			rp.receiver->should_deinit = true;
		}
	}
	RELEASE_LOCK(xsocket::core::internal::SockSessionRoutine::unlocked);
}

void xsocket::core::internal::SockSessionRoutine::markIsRemoved (
	const xsocket::core::internal::SockSessionRoutine * s)
{
	xlog::AppendV2(__func__, __FILE__, __LINE__, (int)s, XLOG_LEVEL_0);
	CAPTURE_LOCK(xsocket::core::internal::SockSessionRoutine::unlocked);
	xsocket::core::internal::SockSessionRoutine::XsocketReceiverPoor rp;
	const int en
		= xsocket::core::internal::SockSessionRoutine::work_poor.size();
	for (int i = 0; i < en; ++i) {
		rp = xsocket::core::internal::SockSessionRoutine::work_poor.at(i);
		if ((const long unsigned int)s == (const long unsigned int)
			(rp.receiver)) {
			xsocket::core::internal::SockSessionRoutine::work_poor.removeAt(i);
			break;
		}
	}
	const int wn
		= xsocket::core::internal::SockSessionRoutine::wait_poor.size();
	for (int i = 0; i < wn; ++i) {
		rp = xsocket::core::internal::SockSessionRoutine::wait_poor.at(i);
		if ((const long unsigned int)s == (const long unsigned int)
			(rp.receiver)) {
			xsocket::core::internal::SockSessionRoutine::wait_poor.removeAt(i);
			break;
		}
	}
	RELEASE_LOCK(xsocket::core::internal::SockSessionRoutine::unlocked);
}

xsocket::core::internal::SockSessionRoutine::SockSessionRoutine (void)
{
	this->activted = false;
	this->should_deinit = false;
	this->deinited = false;

	this->cli_fd = -1;
	this->svr_fd = -1;
	this->on_session = NULL;

	this->gc = new xsocket::core::internal::SockSessionRoutineGC();

	++instance_num;
	xlog::AppendV2("did instance_num", __FILE__, __LINE__, instance_num,
		XLOG_LEVEL_0);
}

/*
xsocket::core::internal::SockSessionRoutine::SockSessionRoutine (int cli_fd,
	const xsocket::NetProtocol& client,
	int svr_fd, const xsocket::NetProtocol& server,
	xsocket::OnSession * on_session)
{
	++instance_num;
	xlog::AppendV2(__func__, __FILE__, __LINE__, instance_num, XLOG_LEVEL_T);

	this->cli_fd = cli_fd;
	this->svr_fd = svr_fd;
	memcpy(&(this->client), &client, sizeof(xsocket::NetProtocol));
	memcpy(&(this->server), &server, sizeof(xsocket::NetProtocol));
	this->on_session = on_session;
	this->gc = new xsocket::core::internal::SockSessionRoutineGC();
}
*/

xsocket::core::internal::SockSessionRoutine::~SockSessionRoutine ()
{
	--instance_num;
	xlog::AppendV2("will instance_num", __FILE__, __LINE__, instance_num,
		XLOG_LEVEL_0);
	this->deinited = true;
}

void xsocket::core::internal::SockSessionRoutine::tillRunable (void)
{
	while ((! this->should_deinit) && (! this->activted)) {
		usleep(10 * 1e3);
	}
}

/*
static uint8_t __recv_unlocked = 1;
*/
#if 1
void * xsocket::core::internal::SockSessionRoutine::run (void * /* nil */)
{
	xlog::AppendV2(__func__, __FILE__, __LINE__, (int)this, XLOG_LEVEL_0);

	xsocket::OnSession * os = NULL;
	bool tmi = false;
	int cli_fd = -1;
	int svr_fd = -1;
	int i, ret, rcved_bytes;
	bool prev_ok;
	struct timeval timeout_readable;
	struct timeval timeout_rcv;
	int nfds;
	fd_set readfds;
	char msg[128];
	xsocket::SockDidFinish didfi;
	xsocket::SockRecved rcved;
	rcved.data = NULL;

	do {
		this->tillRunable();
		if (this->should_deinit) {
			goto final;
		}

		os = this->on_session;
		this->on_session = NULL;

		if (NULL == os) {
			xlog::AppendV2(
				"FATAL: NULL on session",
				__FILE__, __LINE__, 0, XLOG_LEVEL_F);
			goto cc;
		}

		cli_fd = this->cli_fd;
		svr_fd = this->svr_fd;

		if (-1 == cli_fd) {
			xlog::AppendV2("-1 cli fd", __FILE__, __LINE__, 0, XLOG_LEVEL_0);
			goto cc;
		}

		if (NULL == rcved.data) {
			rcved.data = new uint8_t[4096];
		}
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

		tmi = os->shouldTeminate(cli_fd);
		if (! tmi) {
			tmi = os->should_exit();
		}

		if (tmi) {
			xlog::AppendV2("user teminate", __FILE__, __LINE__, 0,
				XLOG_LEVEL_0);
			didfi.code = 0;
			goto cc;
		}

		while (! tmi) {
			/*
			xlog::AppendV2("this run", __FILE__, __LINE__, cli_fd,
				XLOG_LEVEL_V);
			*/

			/* set select timeout for readable */
			timeout_readable.tv_sec = XSOCKET_WAIT_READABLE_TIMEOUT;
			timeout_readable.tv_usec = 0;

			/* set recv fd */
			FD_ZERO(&readfds);
			FD_SET(cli_fd, &readfds);
			nfds = (cli_fd) + 1;

			/*
			xlog::AppendV2("before select", __FILE__, __LINE__, cli_fd,
				XLOG_LEVEL_V);
			*/

			/* selecting */
			ret = select(nfds, &readfds, NULL, NULL, &timeout_readable);

			/*
			xlog::AppendV2("after select", __FILE__, __LINE__, cli_fd,
				XLOG_LEVEL_V);
			*/

			prev_ok = false;

			if (-1 == ret) {
				ret = errno;
				snprintf(msg, 127, "FAIL: select: %s. and will teminate",
					strerror(ret));
				msg[127] = '\0';
				xlog::AppendV2(msg, __FILE__, __LINE__, ret, XLOG_LEVEL_E);
				didfi.code = -ret;
				goto cc;
			} else if (0 == ret) {
				/*
				 * no ready and timetout
				xlog::AppendV2("wait readable timeout", __FILE__, __LINE__,
					cli_fd, XLOG_LEVEL_XV);
				 */
			} else if (ret > 0) { /* XXX-FIXED: add > 0 */
				/*
				xlog::AppendV2("now readable", __FILE__, __LINE__, cli_fd,
					XLOG_LEVEL_V);
				*/
				prev_ok = true;
			}

			/* check and set timeout */
			if (prev_ok) {
				/*
				xlog::AppendV2("prev ok", __FILE__, __LINE__,
					cli_fd, XLOG_LEVEL_XV);
				*/
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

#					if !defined(WIN32)
					ret = setsockopt(cli_fd, SOL_SOCKET, SO_RCVTIMEO,
						&timeout_rcv,
						(socklen_t)sizeof(struct timeval));
#					else
					ret = setsockopt(cli_fd, SOL_SOCKET, SO_RCVTIMEO,
						reinterpret_cast<char *>(&timeout_rcv),
						(int)sizeof(struct timeval));
#					endif

					if (ret < 0) {
						ret = errno;
						snprintf(msg, 127,
							"FAIL: setsockopt: %s. and will teminate",
							strerror(ret));
						msg[127] = '\0';
						xlog::AppendV2(msg, __FILE__, __LINE__, ret,
							XLOG_LEVEL_F);
						didfi.code = -ret;
						goto cc;
					} else {
						prev_ok = true;
					}
				}
			} /* if prev_ok */ else {
				/*
				xlog::AppendV2("prev not ok", __FILE__, __LINE__,
					cli_fd, XLOG_LEVEL_XV);
				*/
			}

			/* recv */
			rcved_bytes = -1;
			if (prev_ok) {
				/*
				xlog::AppendV2("will recv", __FILE__, __LINE__, 0,
					XLOG_LEVEL_V);
				 * to recv and wait till timeout
				 *
				 * to recv till timeout
				 * MSG_WAITALL: wait till data is available
				 * (SOCK_STREAM only).
				 */
				errno = 0;
				/* ret = recv(cli_fd, rcved.data, 4096, MSG_WAITALL); */
				/* CAPTURE_LOCK(__recv_unlocked); */
#				if !defined(WIN32)
				ret = recv(cli_fd, rcved.data, 4096, MSG_DONTWAIT);
#				else
				ret = recv(cli_fd, reinterpret_cast<char *>(rcved.data), 4096,
					0);
#				endif
				/* RELEASE_LOCK(__recv_unlocked); */

				if ((ret < 0) && (EAGAIN == errno)) {
					/*
					xlog::AppendV2("recv timeout", __FILE__, __LINE__, 0,
						XLOG_LEVEL_W);
					*/
				} else if ((ret < 0) && (EWOULDBLOCK == errno)) {
					xlog::AppendV2("EWOULDBLOCK: recv and will teminate",
						__FILE__, __LINE__, 0, XLOG_LEVEL_W);
					didfi.code = -EWOULDBLOCK;
					goto cc;
				} else if (ret <0) {
					ret = errno;
					snprintf(msg, 127,
						"FAIL: recv: %s. and will teminate",
						strerror(ret));
					msg[127] = '\0';
					xlog::AppendV2(msg, __FILE__, __LINE__, ret, XLOG_LEVEL_F);
					didfi.code = -ret;
					goto cc;
				} else {
					/* success */
					rcved_bytes = ret;
					/*
					xlog::AppendV2("recv success", __FILE__, __LINE__,
						rcved_bytes, XLOG_LEVEL_V);
					*/
					prev_ok = true;
				}
			}

			/*
			 * connection lost
			 */
			if (prev_ok && (0 == rcved_bytes)) {
				xlog::AppendV2("WARN: connection lost and will teminate",
					__FILE__, __LINE__, cli_fd, XLOG_LEVEL_W);
				didfi.code = 1;
				goto cc;
			}

			if (prev_ok && (rcved_bytes > 0)) {
				/*
				xlog::AppendV2("show", __FILE__, __LINE__, cli_fd,
					XLOG_LEVEL_V);
				*/
				/* Show when need */
				// for (i = 0; i < rcved_bytes; ++i) {
				// 	fprintf(stdout, "0x%02x ", rcved.data[i]);
				// 	if (0 == ((i + 1) % 16)) {
				// 		fprintf(stdout, "\n");
				// 	}
				// }

				// if (0 != (i % 16)) {
				// 	fprintf(stdout, "\n");
				// }
				/*
				 * tell/callback external
				 */
				if (rcved_bytes > 4095) {
					rcved_bytes = 4095;
				}
				rcved.count = rcved_bytes;
				rcved.data[4095] = '\0';
				xlog::AppendV2((char *)rcved.data, __FILE__, __LINE__,
					cli_fd, XLOG_LEVEL_V);
				os->onReceived(rcved);
			}
			/*
			xlog::AppendV2("show done", __FILE__, __LINE__, cli_fd,
				XLOG_LEVEL_V);
			*/

			tmi = os->shouldTeminate(cli_fd);

			if (! tmi) {
				tmi = os->should_exit();
			}
			if (tmi) {
				xlog::AppendV2("user teminate", __FILE__, __LINE__, 0,
					XLOG_LEVEL_I);
				didfi.code = 0;
				goto cc;
			}

			/*
			xlog::AppendV2("next run", __FILE__, __LINE__, cli_fd,
				XLOG_LEVEL_V);
			*/
		} /* while */

cc:
		if (this->should_deinit) {
			goto final;
		}
		this->activted = false;
		xsocket::core::internal::SockSessionRoutine::markHasNotActivited(this);
		if (-1 != cli_fd) {
			xsocket::core::ShutdownSocket(cli_fd, xsocket::ShutdownHow::RDWR);
			xsocket::core::CloseSocket(cli_fd);
		}
		if (NULL != os) {
			os->didFinish(didfi);
			if (-1 != cli_fd) {
				os->markExited(cli_fd);
			}
			os = NULL;
		}
		cli_fd = -1;

		xlog::AppendV2("exception cc wait 0.5 secs", __FILE__, __LINE__, 0,
			XLOG_LEVEL_0);
		usleep(500 * 1e3);
	} while (true);

final:
	this->activted = false;
	/*
	xsocket::core::internal::SockSessionRoutine::markHasNotActivited(this);
	*/
	xsocket::core::internal::SockSessionRoutine::markIsRemoved(this);
	if (NULL != rcved.data) {
		delete rcved.data;
		rcved.data = NULL;
	}
	if (-1 != cli_fd) {
		xsocket::core::ShutdownSocket(cli_fd, xsocket::ShutdownHow::RDWR);
		xsocket::core::CloseSocket(cli_fd);
	}
	if (NULL != os) {
		os->didFinish(didfi);
		if (-1 != cli_fd) {
			os->markExited(cli_fd);
			cli_fd = -1;
		}
		os = NULL;
	}

	xlog::AppendV2(
		"FIANL !!!! i'll exit(delete self) and NOT delete on session(will by server)",
		__FILE__, __LINE__, 0, XLOG_LEVEL_0);
	if (NULL != this->gc) {
		this->gc->gc(this);
	}

	return NULL;
}
#endif

int xsocket::core::internal::SockSessionRoutine::instance_num = 0;

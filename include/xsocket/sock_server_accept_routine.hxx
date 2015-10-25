#if !defined(XSOCKET_SOCK_SERVER_ACCEPT_ROUTINE_HXX__)
#define XSOCKET_SOCK_SERVER_ACCEPT_ROUTINE_HXX__ (1)

#include <posix/thread/simple_thread.hxx> /* pthreadx::SimpleThread */

namespace xsocket { class SockServerHelper; }

namespace xsocket { namespace core { namespace internal {

class SockServerAcceptRoutine: public pthreadx::SimpleThread {

friend class xsocket::SockServerHelper;

public: SockServerAcceptRoutine (void * arg) : SimpleThread (arg) { }


protected: ~SockServerAcceptRoutine () { this->nowFinish(); }

protected: virtual void * run(void * server);

};

} } }


#endif /* XSOCKET_SOCK_SERVER_ACCEPT_ROUTINE_HXX__ */

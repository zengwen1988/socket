#if !defined(SOCKSERVERACCEPTROUTINE_HXX__)
#define SOCKSERVERACCEPTROUTINE_HXX__ (1)

#include <posix/thread/XSimpleThread.hxx> /* PosixXSimpleThread */

class XSockServerHelper;

class XSockServerAcceptRoutine: public PosixXSimpleThread
{

friend class XSockServerHelper;

public: XSockServerAcceptRoutine (void * arg)
	: PosixXSimpleThread (arg) {
}


protected: ~XSockServerAcceptRoutine () {
	this->nowFinish();
}

protected: virtual void * run(void * server);

};

#endif /* SOCKSERVERACCEPTROUTINE_HXX__ */

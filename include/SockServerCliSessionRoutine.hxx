#if !defined(SOCKSERVERCLISESSIONROUTINE_HXX__)
#define SOCKSERVERCLISESSIONROUTINE_HXX__ (1)

#include <posix/thread/XSimpleThread.hxx> /* PosixXSimpleThread */
#include <SockServerAcceptRoutine.hxx>

/*
 * For server
 * manage each connected client
 * main recv
 */
class XSockServerCliSessionRoutine: public PosixXSimpleThread
{

/* can call delete */
friend class XSockServerAcceptRoutine;

public: XSockServerCliSessionRoutine (void * arg)
	: PosixXSimpleThread (arg) {
}


protected: ~XSockServerCliSessionRoutine () {
	this->nowFinish();
}

protected: virtual void * run(void * server);

};

#endif /* SOCKSERVERCLISESSIONROUTINE_HXX__ */

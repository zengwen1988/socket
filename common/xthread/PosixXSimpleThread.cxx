#include <posix/thread/XSimpleThread.hxx>

#include <posix/func/usleep.h>

#if defined(WIN32)
#	include <windows.h>
#endif

PosixXSimpleThread::PosixXSimpleThread (void * arg)
{
	this->running = false;
	this->arg = arg;
	this->exitCode = NULL;
}


int PosixXSimpleThread::start (void * arg)
{
	int ret;

	if (NULL != arg) {
		this->setArg(arg);/* store user data */
	}

#if defined (WIN32)
	this->tid = CreateThread(NULL, 0,
		reinterpret_cast<LPTHREAD_START_ROUTINE>(
			(PosixXSimpleThread::entryPoint)),
		reinterpret_cast<LPVOID>(this), 0, (LPDWORD)&(this->id));

	if (NULL == this->tid) {
		ret = -1;/* create fail */
	} else {
		this->setRunning(true);
		ret = 0;/* success */
	}
#else
	this->setRunning(true);
	ret = pthread_create(&(this->tid), NULL, PosixXSimpleThread::entryPoint,
		this);
#endif

	if (0 != ret) {
		this->setRunning(false);
		return -ret;/* create fail */
	} else {
		return 0;
	}

}


void * PosixXSimpleThread::entryPoint (void * self)
{

	PosixXSimpleThread * thread = (PosixXSimpleThread *)self;

	if (NULL == self) {
		return (void *)-1;
	} else {
		thread->exitCode =  thread->run(thread->getArg());
		thread->setRunning(false);
		return thread->exitCode;
	}
}


int PosixXSimpleThread::waitFinish (void ** retval)
{

	int ret = 0;

#if !defined(WIN32)
	ret = pthread_join(this->tid, retval);
#else
	while (this->running) {
		usleep(10 * 1e3);
	}

	if (NULL != retval) {
		*retval = this->exitCode;
	}

	ret = CloseHandle(this->tid) ? 0 : 1;
#endif

	return -ret;

}


int PosixXSimpleThread::nowFinish (void ** retval)
{

	int ret = 0;

	if (this->isRunning()) {
		if (NULL != retval) {
			*retval = this->exitCode;
		}

		return 0;
	}

#if !defined(WIN32)
	ret = pthread_cancel(this->tid);
#else
	ret = CloseHandle(this->tid) ? 0 : 1;
#endif

	this->setRunning(false);

	if (NULL != retval) {
		*retval = this->exitCode;
	}

	return -ret;

}

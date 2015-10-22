#if !defined(POSIX_THREAD_XSIMPLETHREAD_H__)
#define POSIX_THREAD_XSIMPLETHREAD_H__ (1)

#if defined (_WIN32)
# include <windows.h>
#else
# include <pthread.h>
#endif


/*
 * Extremely simple thread class.
 * uses either windows threads (win32)
 * pthreads (unix)
 */
class PosixXSimpleThread
{

public: int start(void * arg = NULL);
/* TODO: add wait timeout */
public: int waitFinish(void ** retval = NULL);
public: int nowFinish(void ** retval = NULL);

public: bool isRunning (void) const { return this->running; }
public: void * getArg (void) const { return this->arg; }
public: ~PosixXSimpleThread() {}

protected: virtual void * run (void *) = 0;

protected: PosixXSimpleThread(void * arg = NULL);

protected: static void * entryPoint(void * self);
protected: void setRunning (bool running) { this->running = running; }
protected: void setArg (void * arg) { this->arg = arg; }

private: void * arg;
private: void * exitCode;

#if defined (_WIN32)
private: void * tid; /* thread handle */
private: unsigned long id;
#else
private: pthread_t tid;
#endif

private: bool running;

};

#endif  /* POSIX_THREAD_XSIMPLETHREAD_H__ */

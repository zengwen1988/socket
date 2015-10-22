#if !defined(POSIX_TYPE_SSIZE_T_H__)
#define POSIX_TYPE_SSIZE_T_H__ (1)

#if !defined(_WIN32)
#	include <sys/types.h>
#elif !defined(HAS_SSIZE_T)
	typedef int ssize_t;
#endif

#endif /* POSIX_TYPE_SSIZE_T_H__ */

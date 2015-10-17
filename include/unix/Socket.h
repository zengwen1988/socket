#if !defined (UNIXSOCKET_H__)
#define UNIXSOCKET_H__ (1)

#include <unix/SockParams.h>


typedef void * (connect_routine_t)(ConnectRoutineParams *);

typedef void * (receive_routine_t)(RecveiveParams *);


class UNIXSocket {

public: static int startConnectByProtocol(uint32_t ip, uint16_t port);

public: static connect_routine_t * getS2ConnectRoutine();

public: static receive_routine_t * getReceiveRoutine();

};

#endif /* UNIXSOCKET_H__ */

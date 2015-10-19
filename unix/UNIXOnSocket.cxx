#include <unix/OnSocket.h>

/* override this */
int UNIXOnSocket::onConnect (sock_on_conn_t) { return 0; }
/* override this */
int UNIXOnSocket::willFinish(sock_will_finish_t) { return 0; }
/* override this */
int UNIXOnSocket::onReceived (const uint8_t *, size_t) { return 0; }
/* override this */
bool UNIXOnSocket::shouldTeminateRecv(int /* sockfd */) { return false; }

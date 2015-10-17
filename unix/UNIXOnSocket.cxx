#include <unix/OnSocket.h>

/* override this */
int UNIXOnSocket::onConnect (unix_sock_on_conn_params_t) { return 0; }
/* override this */
int UNIXOnSocket::onDisconnected(unix_sock_on_conn_params_t) { return 0; }
/* override this */
int UNIXOnSocket::onReceived (const uint8_t * data, size_t sz) { return 0; }
/* override this */
int UNIXOnSocket::onSent (const uint8_t * data, size_t sz) { return 0; }

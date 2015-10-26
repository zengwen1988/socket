TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

# target
TARGET = cli2

# -I
INCLUDEPATH += ../../../include

# LDFLAGS
LIBS += -lpthread

# -Dxxx
DEFINES += "ENABLE_SOCK_DEBUG=1"

# source
# common
SOURCES += ../../../common/c_logfile.c
SOURCES += ../../../common/time/timestamp.c
SOURCES += ../../../common/time/timezone_util.c
SOURCES += ../../../common/net/is_ipv4_str.c
SOURCES += ../../../common/net/simple_net_conv.c
#
SOURCES += ../../../common/simple_posix_thread.cxx
#
SOURCES += ../../../xsocket/xsock_core.cxx
# client only
SOURCES += ../../../xsocket/xsock_client_core.cxx
SOURCES += ../../../xsocket/xsock_client_params.cxx
SOURCES += ../../../xsocket/xsock_client_helper.cxx

SOURCES += ../../../test/2/random.c
SOURCES += ../../../test/2/timer/SimpleTimer.cxx

# target
SOURCES += ../../../test/2/MyOnSocket.cxx
SOURCES += ../../../test/2/GSSockHelper.cxx
SOURCES += ../../../test/2/main.cxx

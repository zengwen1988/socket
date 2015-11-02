TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

# target
TARGET = server

# -I
INCLUDEPATH += ../../../include

# LDFLAGS
LIBS += -lpthread

# -Dxxx
DEFINES += "XSOCK_LOGLEVEL=0x100"

# source
# common
SOURCES += ../../../common/time/timestamp.c
SOURCES += ../../../common/time/timezone_util.c
SOURCES += ../../../common/net/is_ipv4_str.c
SOURCES += ../../../common/net/simple_net_conv.c
#
SOURCES += ../../../common/x_logfile.cxx
SOURCES += ../../../common/simple_posix_thread.cxx
#
SOURCES += ../../../xsocket/xsock_core.cxx
# client only
SOURCES += ../../../xsocket/xsock_server_core.cxx
SOURCES += ../../../xsocket/xon_server_socket.cxx
SOURCES += ../../../xsocket/xsock_server_accept_routine.cxx
SOURCES += ../../../xsocket/xsock_server_helper.cxx

SOURCES += ../../../test/2/random.c
SOURCES += ../../../test/2/timer/xsimple_timer.cxx

# target
SOURCES += ../../../test/xwin/main.cxx

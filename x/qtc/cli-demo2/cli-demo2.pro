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
DEFINES += "UNIX_SOCK_DEBUG=1"

# source
# common
SOURCES += ../../../sock/sock.c
SOURCES += ../../../sock/sock_util.c
# client only
SOURCES += ../../../xsock/XSockClientUtil.cxx
SOURCES += ../../../xsock/XOnClientSocket.cxx
SOURCES += ../../../xsock/XSockParams.cxx
SOURCES += ../../../xsock/XSockClientHelper.cxx

SOURCES += ../../../test/2/random.c
SOURCES += ../../../test/2/timer/SimpleTimer.cxx

# target
SOURCES += ../../../test/2/MyOnSocket.cxx
SOURCES += ../../../test/2/GSSockHelper.cxx
SOURCES += ../../../test/2/main.cxx

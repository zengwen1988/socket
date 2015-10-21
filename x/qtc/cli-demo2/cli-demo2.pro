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
SOURCES += ../../../unix/unix_sock.c
SOURCES += ../../../unix/unix_sock_util.c
SOURCES += ../../../unix/UNIXOnSocket.cxx
SOURCES += ../../../unix/UNIXSocket.cxx
SOURCES += ../../../unix/UNIXSockParams.cxx
SOURCES += ../../../unix/UNIXSocketClientHelper.cxx

SOURCES += ../../../test/2/random.c
SOURCES += ../../../test/2/timer/SimpleTimer.cxx

# target
SOURCES += ../../../test/2/MyOnSocket.cxx
SOURCES += ../../../test/2/GSSockHelper.cxx
SOURCES += ../../../test/2/main.cxx

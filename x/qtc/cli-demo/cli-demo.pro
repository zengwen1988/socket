TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

# target
TARGET = cli-demo

# -I
INCLUDEPATH += ../../../include

# LDFLAGS
LIBS += -lpthread

# -Dxxx
DEFINES += "UNIX_SOCK_DEBUG=1"

# source
# common
SOURCES += ../../../unix/UNIXOnSocket.cxx
SOURCES += ../../../unix/UNIXSocket.cxx
SOURCES += ../../../unix/UNIXSockParams.cxx
SOURCES += ../../../unix/UNIXSocketClientUtil.cxx
SOURCES += ../../../unix/UNIXSocketClientHelper.cxx


# target
SOURCES += ../../../../test/main.cxx


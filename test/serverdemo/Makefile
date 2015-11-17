# CC
CC := $(CROSS_COMPILER)gcc
# CXX
CXX := $(CROSS_COMPILER)g++

DEFINES := -DXSOCKET_LOGLEVEL=0x100
# DEFINES += -DXLOG_HASSTDOUT=1
INCPATH = -I../../include

CFLAGS += -pipe -Wall -W $(DEFINES)
CXXFLAGS += -pipe -Wall -W $(DEFINES)
CFLAGS += $(INCPATH)
CXXFLAGS += $(INCPATH)
CFLAGS += -Wp,-MD,$@.d
CXXFLAGS += -Wp,-MD,$@.d
ifneq ($(MINGW32), )
  CFLAGS += -DMINGW32=1
  CXXFLAGS += -DMINGW32=1
endif
CXFLAGS += -Wall -Wno-unused-parameter
CXXFLAGS += -Wall -Wno-unused-parameter

LINKER := $(CXX)
LDFLAGS += -Wl,-O1
LDFLAGS += -lpthread
ifneq ($(MINGW32), )
  LDFLAGS += -lws2_32 --static
endif
STRIP := $(CROSS_COMPILER)strip
RM = rm -f

VPATH += ../../common
VPATH += ../../common/net
VPATH += ../../common/time
VPATH += ../../xsocket
VPATH += ../../test/2

OUT ?= .

OBJS += $(OUT)/timestamp.o
OBJS += $(OUT)/timezone_util.o
OBJS += $(OUT)/is_ipv4_str.o
OBJS += $(OUT)/simple_net_conv.o
OBJS += $(OUT)/x_logfile.o
OBJS += $(OUT)/simple_posix_thread.o
OBJS += $(OUT)/xsock_core.o
OBJS += $(OUT)/xsock_server_core.o
OBJS += $(OUT)/xon_server_socket.o
OBJS += $(OUT)/on_session.o
OBJS += $(OUT)/xsock_session_routine.o
OBJS += $(OUT)/xsock_server_accept_routine.o
OBJS += $(OUT)/xsock_server_helper.o
OBJS += $(OUT)/random.o
OBJS += $(OUT)/xsimple_timer.o
OBJS += $(OUT)/main.o

PORT ?= 12345
CXXFLAGS += -DPORT=$(PORT)
TARGET ?= $(OUT)/ncsr$(PORT)$(SUFFIX)

all: .prepare $(TARGET)
	@echo "All done"

.prepare:
	@mkdir -p $(OUT)

$(TARGET): $(OBJS)
	$(LINKER) -o $@ $^ $(LDFLAGS)
	$(STRIP) $@

$(OUT)/%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)
$(OUT)/%.o: %.cxx
	$(CXX) -c -o $@ $< $(CXXFLAGS)

eac_files := $(foreach f,$(OBJS),$(OUT)/.$(f).d) # each
dep_files := $(wildcard $(eac_files)) # depend
ifneq ($(dep_files),)
  include $(dep_files)
endif

clean:
	@rm -fv $(OBJS) $(TARGET) $(OUT)/*.o.d

# @rm -fv $(OUT)/*.o.d
# ifneq ($(OUT), .)
# 	@rmdir $(OUT)/
# endif
distclean: clean

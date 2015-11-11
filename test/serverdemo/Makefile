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
CXFLAGS += -Wall -Wno-unused-parameter
CXXFLAGS += -Wall -Wno-unused-parameter

LINKER := $(CXX)
LDFLAGS += -Wl,-O1
LDFLAGS += -lpthread
STRIP := $(CROSS_COMPILER)strip
RM = rm -f

VPATH += ../../common
VPATH += ../../common/net
VPATH += ../../common/time
VPATH += ../../xsocket
VPATH += ../../test/2

OBJS +=
OBJS += timestamp.o
OBJS += timezone_util.o
OBJS += is_ipv4_str.o
OBJS += simple_net_conv.o
OBJS += x_logfile.o
OBJS += simple_posix_thread.o
OBJS += xsock_core.o
OBJS += xsock_server_core.o
OBJS += xon_server_socket.o
OBJS += on_session.o
OBJS += xsock_session_routine.o
OBJS += xsock_server_accept_routine.o
OBJS += xsock_server_helper.o
OBJS += random.o
OBJS += xsimple_timer.o
OBJS += main.o

OUT ?= .

PORT ?= 12345
CXXFLAGS += -DPORT=$(PORT)
TARGET ?= $(OUT)/ncsr$(PORT)

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
	@rm -fv $(OBJS) $(TARGET)

distclean: clean
	@rm -fv $(OUT)/*.o.d
ifneq ($(OUT), .)
	@rmdir $(OUT)/
endif

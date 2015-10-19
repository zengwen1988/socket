#############################################################################
## compiler and related, set or default

# CC
CC := $(CROSS_COMPILER)gcc

# CXX
CXX := $(CROSS_COMPILER)g++

# # AR
AR := $(CROSS_COMPILER)ar

# # RANLIB
RANLIB := $(CROSS_COMPILER)ranlib

# LINKER
LINKER := $(CXX)

# STRIP
STRIP := $(CROSS_COMPILER)strip


#############################################################################
## CFLAGS, CXXFLAGS and LDFLAGS
#
PATH2INC ?= ../include
CFLAGS += -I$(PATH2INC)
CFLAGS += -Wall -Wno-unused-parameter
CFLAGS += -DUNIX_SOCK_DEBUG=1
# deps
CFLAGS += -Wp,-MD,$@.d

CXXFLAGS += -I$(PATH2INC)
CXXFLAGS += -Wall -Wno-unused-parameter
CXXFLAGS += -DUNIX_SOCK_DEBUG=1
# deps
CXXFLAGS += -Wp,-MD,$@.d


# ##
# ARFLAGS = crv


## _DBG_LEVEL_


## LDFLAGS
LDFLAGS += -lpthread


#############################################################################
## VPATH
VPATH += ../common
VPATH += ../unix


#############################################################################
## out
OUT ?= .

# out
VPATH += $(OUT)


## TARGET
#
TARGET1 ?= $(OUT)/cli

#
TARGET += $(TARGET1) $(TARGET2)
all: .ready $(TARGET)
	@echo "All done"


#############################################################################
## OBJS
#
SUFFIX_OBJ ?= .o

#
OBJS_COMMON += $(OUT)/UNIXOnSocket$(SUFFIX_OBJ)
OBJS_COMMON += $(OUT)/unix_sock$(SUFFIX_OBJ)
OBJS_COMMON += $(OUT)/UNIXSocketClientHelper$(SUFFIX_OBJ)
OBJS_COMMON += $(OUT)/UNIXSocket$(SUFFIX_OBJ)
OBJS_COMMON += $(OUT)/UNIXSockParams$(SUFFIX_OBJ)
OBJS_COMMON += $(OUT)/unix_sock_util$(SUFFIX_OBJ)

#
OBJS1 += $(OBJS_COMMON)
OBJS1 += $(OUT)/main$(SUFFIX_OBJ)


#############################################################################
## build
#
.ready:
	@mkdir -p $(OUT)

#
$(TARGET1): $(OBJS1)
	$(LINKER) -o $@ $^ $(LDFLAGS)
	$(STRIP) $@
$(TARGET2): $(OBJS2)
	$(LINKER) -o $@ $^ $(LDFLAGS)
	$(STRIP) $@

$(OUT)/%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)
$(OUT)/%.o: %.cxx
	$(CXX) -c -o $@ $< $(CXXFLAGS)
$(OUT)/%.o: %.cpp
	$(CXX) -c -o $@ $< $(CXXFLAGS)

# UPDATE_COMMON
# UPDATE_COMMON:
# 	cd $(PWD)/common; ./update.gitrepo.sh $(PWD)/common


#############################################################################
## deps
eac_files := $(foreach f,$(OBJS),$(OUT)/.$(f).d) # each
dep_files := $(wildcard $(eac_files)) # depend
ifneq ($(dep_files),)
  include $(dep_files)
endif

# common
SUFFIX_C ?= .c
SUFFIX_H ?= .h
# TODO


#############################################################################
## clean
clean:
	@rm -fv $(OBJS1) $(TARGET)

distclean: clean
	@rm -fv $(OUT)/*.o.d
ifneq ($(OUT), .)
	@rmdir $(OUT)/
endif


## install
install: .ready $(TARGET)
ifeq ($(PREFIX),)
	$(error "install: should provide a NOT-NULL PREFIX")
endif
	install -d $(PREFIX)/bin
	install -m 775 $(TARGET1) $(PREFIX)/bin

##
.PHONY: all install clean distclean .ready

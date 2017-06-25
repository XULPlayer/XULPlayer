# common.mk
ifdef DEBUG
BUILD  := debug
else
BUILD  := release
endif

ifeq ($(SUBDIRNAME),)
ROOTDIR := $(PWD)
else
ifneq ($(findstring $(SUBDIRNAME),$(PWD)),)
ROOTDIR := $(PWD)/..
else
ROOTDIR := $(PWD)
endif
endif

PREFIX      =$(ROOTDIR)/$(BUILD)
BINDIR      =$(PREFIX)/bin
PLUGINDIR   =$(PREFIX)/plugins
LIBDIR      =$(PREFIX)/lib
INCLUDEDIR  =$(PREFIX)/include
EXTRAINCLUDE =$(ROOTDIR)/include

CC      := gcc
CPP     := c++
AR      := ar
RANLIB  := ranlib
STRIP   := strip
RM      := rm -f
MV      := mv -f
INSTALL := install
LN      := ln -sf

ifdef DEBUG
DEFINES += _DEBUG
LDFLAGS += -g
else
LDFLAGS += -s
endif

ifdef COMSPEC
DEFINES += WIN32
OS      = WIN32
EXE     =.exe
else
CFLAGS  += -fPIC
EXE     =
OS      = LINUX
X86_64  = $(shell uname -m | grep 64)
ifneq ($(strip $(X86_64)),)
DEFINES += X86_64
endif
endif

INCLUDES += $(INCLUDEDIR) $(EXTRAINCLUDE)

CFLAGS  += -I. -O2 -Wall
CFLAGS  += $(addprefix -I,$(INCLUDES))
CFLAGS  += $(addprefix -D,$(DEFINES))
CXXFLAGS = $(CFLAGS)

DEPEND  =depend
all: default

OBJS    := $(SRCS:%.cpp=%.o)
OBJS    := $(OBJS:%.c=%.o)

APPOBJS := $(APPSRCS:%.cpp=%.o)
APPOBJS := $(APPOBJS:%.c=%.o)

.PHONY: all default $(DEPEND) clean install tags help

$(LIBNAME): .depend $(OBJS)
	$(AR) rc $(LIBNAME) $(OBJS)
	$(RANLIB) $(LIBNAME)

$(SONAME): .depend $(OBJS) $(SODEPS)
	$(CC) -shared -o $@ $(OBJS) $(SODEPS) $(SOFLAGS) $(LDFLAGS)

$(APPNAME): .depend $(APPOBJS) $(APPDEPS)
	$(CC) -o $@ $(APPOBJS) $(APPDEPS) $(LDFLAGS)

.depend:
	$(RM) .depend
	$(foreach SRC, $(filter %.cpp,$(SRCS) $(APPSRCS)), $(CPP) $(CXXFLAGS) $(ALTIVECFLAGS) $(SRC) -MT $(SRC:%.cpp=%.o) -MM -g0 1>> .depend;)
	$(foreach SRC, $(filter %.c,$(SRCS) $(APPSRCS)), $(CC) $(CFLAGS) $(ALTIVECFLAGS) $(SRC) -MT $(SRC:%.c=%.o) -MM -g0 1>> .depend;)

$(DEPEND): .depend
ifneq ($(wildcard .depend),)
include .depend
endif

clean:
	$(RM) $(OBJS) $(LIBNAME) $(SONAME) *.a $(APPNAME) $(APPOBJS) .depend TAGS

distclean: clean
	$(RM) cscope.* tags

install: all
	$(INSTALL) -d -m 755 $(BINDIR)
	$(INSTALL) -d -m 755 $(LIBDIR)
ifneq ($(wildcard $(APPNAME)),)
	$(INSTALL) -c -m 755 $(APPNAME) $(BINDIR)
endif
ifneq ($(wildcard $(SONAME)),)
	$(INSTALL) -c -m 755 $(SONAME) $(LIBDIR)
endif
ifneq ($(wildcard $(PLUGIN)),)
	$(INSTALL) -d -m 755 $(PLUGINDIR)
	$(INSTALL) -c -m 755 $(PLUGIN) $(PLUGINDIR)
endif

etags: TAGS

TAGS:
	etags $(SRCS)

tags:
	ctags -R
	cscope -Rbq

help:
	@echo "make	<target>"
	@echo "	tags"
	@echo "	install (prefix=$(PREFIX))"
	@echo "	(dist)clean"


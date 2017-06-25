# Makefile
include ./config.mak

SUBDIRNAME  =plugin

INCLUDES    = $(ROOTDIR)/../webencoder ..
INCLUDES    += $(MOZPLUG_INCLUDES)

DEFINES     = STRPRO_STATIC UTILS_STATIC PLUGINCOMMON_STATIC
DEFINES     += $(MOZPLUG_DEFINES)
ifeq ($(ENABLE_AUTH),yes)
DEFINES     += ENABLE_AUTH
endif

ALTIVECFLAGS=
LDFLAGS= -lm -lpthread -liconv
#LDFLAGS= -lm -lpthread

SONAME = libxulplayer.so
PLUGIN = $(SONAME)
SRCS_COMMON = methods.cpp npn_gate.cpp properties.cpp utils.cpp np_entry.cpp npp_gate.cpp plugin.cpp Xulplayer.cpp scriptInvoke.cpp
SRCS_LINUX_UI = ui_linux.cpp
SRCS_DEVICE = device.cpp

SRCS =

SRCS += $(SRCS_COMMON)
SRCS += $(SRCS_LINUX_UI)

ifeq ($(ENABLE_DEVICE),yes)
SRCS += $(SRCS_DEVICE)
endif

SODEPS = $(STRPRO_LIB) $(PLUGINCOM_LIB)
#SODEPS = $(PLUGINCOM_LIB)
ifeq ($(ENABLE_AUTH),yes)
SODEPS += $(AUTH_LIB)
endif

include ../rules.mk

default: $(DEPEND) $(SONAME)

# Makefile
include ../config.mk
THREAD			= yes

##
SUBDIRNAME      :=httpd

#ifdef THREAD
DEFINES		= HAVE_PTHREAD
#endif

ifneq ($(DEBUG),yes)
  DEFINES     += NOCONSOLE 
else
  DEFINES     += HTTPD_DEBUG
endif

ALTIVECFLAGS    =

#ifdef THREAD
LDFLAGS	= -lpthread
#else
LDFLAGS	=
#endif

LIBNAME =libhttpd.a
SRCS = httppil.c http.c httpxml.c

ifdef APPMINI
APPNAME = httpd
APPSRCS = httpmin.c
else
APPNAME = miniweb$(EXE)
APPSRCS = miniweb.c
endif
APPDEPS = $(LIBNAME)

ifndef NOPOST
SRCS    += httppost.c
DEFINES += HTTPPOST
endif

ifdef AUTH
SRCS    += httpauth.c
DEFINES += HTTPAUTH
endif

ifndef THREAD
DEFINES += NOTHREAD
endif

ifdef MPD
DEFINES += _MPD
SRCS    += mpd.c procpil.c
endif

ifdef VOD
DEFINES += _VOD
SRCS    += httpvod.c crc32.c
endif

ifdef COMSPEC
LDFLAGS += -lws2_32
else
ifdef THREAD
LDFLAGS += -lpthread
endif
endif


include ../rules.mk

default: $(DEPEND) $(LIBNAME)

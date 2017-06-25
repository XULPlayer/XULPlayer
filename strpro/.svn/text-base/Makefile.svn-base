# Makefile
include ../config.mk

SUBDIRNAME  :=strpro
INCLUDES    = tinyXml $(INCLUDEDIR)/libxml2
DEFINES     = STRPRO_STATIC
ALTIVECFLAGS=
LDFLAGS     = -lm -lpthread -lxml2

LIBNAME =libstrpro.a
SRCS = tinyXml/tinystr.cpp  tinyXml/tinyxml.cpp  tinyXml/tinyxmlerror.cpp  tinyXml/tinyxmlparser.cpp
SRCS += charset.cpp  StrHelper.cpp  StrPro.cpp  xmlaccessor.cpp  xmlaccessor2.cpp xmlaccessorTiny.cpp

include ../rules.mk

default: $(DEPEND) $(LIBNAME)

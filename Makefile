#Makefile

##config
ENABLE_AUTH     = yes

LIB_STRPRO      := strpro
LIB_PLUGINCOM   := ../webencoder/PluginCommon
LIB_AUTH        := ../webencoder/auth

LIBS := $(LIB_STRPRO) $(LIB_PLUGINCOM)

ifeq ($(ENABLE_AUTH),yes)
LIBS += $(LIB_AUTH)
endif

SO_PLUGIN       := plugin

.PHONY: all install preinstall clean $(SO_PLUGIN) $(LIBS)

$(SO_PLUGIN) $(LIBS):
ifeq ($(ENABLE_AUTH),yes)
	$(MAKE) -C $@ ENABLE_AUTH=yes
else
	$(MAKE) -C $@
endif

all: $(SO_PLUGIN)

preinstall:
	if test -f preinstall.sh; then sh preinstall.sh; fi

install: $(SO_PLUGIN) $(LIBS) preinstall

clean install:
	@for d in $(LIBS) $(SO_PLUGIN); \
	do  \
	    $(MAKE) -C $$d $@; \
	done
    
$(SO_PLUGIN): $(LIBS)

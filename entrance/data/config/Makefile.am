DB = entrance_config.cfg
SCRIPT = build_config.sh

CLEANFILES = $(DB)
MAINTAINERCLEANFILES = Makefile.in

sysconf_DATA = $(DB)

EXTRA_DIST = $(SCRIPT)

all: $(DB)

$(DB): $(SCRIPT)
		sh $< || :

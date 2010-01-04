DB = entrance_config.cfg
SCRIPT = build_config.sh

CLEANFILES = $(DB)
MAINTAINERCLEANFILES = Makefile.in

sysconf_DATA = $(DB)

EXTRA_DIST = $(SCRIPT) entrance_edit

all: $(DB)

$(DB): $(SCRIPT)
		sh ${SCRIPT} || :

editdir = $(bindir)
edit_SCRIPTS = entrance_edit

DB = entrance_config.db
SCRIPT = build_config.sh

CLEANFILES = $(DB)
EXTRA_DIST = $(SCRIPT)

configdbdir = $(sysconfdir)
configdb_DATA = $(DB)

all: $(DB)

$(DB): $(SCRIPT)
		sh $<

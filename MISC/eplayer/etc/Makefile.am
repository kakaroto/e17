# $Id$

DB = eplayer.db
CLEANFILES = $(DB)

config_DATA = $(DB)
configdir = $(sysconfdir)

all: $(DB)

$(DB):
	rm -f $(DB)

	edb_ed $(DB) add /eplayer/evas_engine str "software"
	edb_ed $(DB) add /eplayer/output_plugin str "OSS"
	edb_ed $(DB) add /eplayer/theme str "default"
	edb_ed $(DB) add /eplayer/time_display_show_left int 0

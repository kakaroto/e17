## $Id$

DB = euphoria.db
CLEANFILES = $(DB)

config_DATA = $(DB)
configdir = $(sysconfdir)

all: $(DB)

$(DB):
	rm -f $(DB)

	edb_ed $(DB) add /euphoria/evas_engine str "software"
	edb_ed $(DB) add /euphoria/theme str "default"
	edb_ed $(DB) add /euphoria/time_display_show_left int 0

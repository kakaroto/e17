/*
 * $Id$
 * vim:noexpandtab:sw=4:sts=4:ts=4
 */

#include <config.h>
#include <Edje.h>
#include <Edb.h>
#include <ltdl.h>
#include <xmms/xmmsclient.h>
#include <xmms/xmmsclient-ecore.h>
#include <xmms/signal_xmms.h>
#include <string.h>
#include <assert.h>
#include <getopt.h>
#include "euphoria.h"
#include "interface.h"
#include "callbacks.h"
#include "utils.h"

static void config_init(Config *cfg) {
	snprintf(cfg->evas_engine, sizeof(cfg->evas_engine), "%s",
	         "software");
	snprintf(cfg->theme, sizeof(cfg->theme), "%s", "default");
}

static bool config_load(Config *cfg, const char *file) {
	E_DB_File *edb;
	char *str;
	int val = 0;

	if (!cfg || !file || !*file)
		return false;

	if (!(edb = e_db_open_read((char *) file)))
		return false;

	if (e_db_int_get(edb, "/euphoria/time_display_show_left", &val))
		cfg->time_display = !!val;

	if ((str = e_db_str_get(edb, "/euphoria/evas_engine"))) {
		snprintf(cfg->evas_engine, sizeof(cfg->evas_engine), "%s", str);
		free(str);
	}

	if ((str = e_db_str_get(edb, "/euphoria/theme"))) {
		snprintf(cfg->theme, sizeof(cfg->theme), "%s", str);
		free(str);
	}

	e_db_close(edb);

	return true;
}

static void euphoria_free(Euphoria *e) {
	if (!e)
		return;

	xmmsc_playback_stop(e->xmms);

	playlist_free(e->playlist);

	ui_shutdown(e);

	xmmsc_deinit(e->xmms);

	free(e);
}

static bool setup_xmms(Euphoria *e) {
	char path[PATH_MAX + 1];

	if (!(e->xmms = xmmsc_init()))
		return false;

	snprintf(path, sizeof(path), "unix:path=/tmp/xmms-dbus-%s", get_login());

	xmmsc_connect(e->xmms, path);
	xmmsc_setup_with_ecore(e->xmms);

	xmmsc_playback_status(e->xmms);
	xmmsc_playlist_list(e->xmms);
	xmmsc_playback_current_id(e->xmms);

	xmmsc_set_callback(e->xmms, XMMS_SIGNAL_PLAYBACK_STATUS,
	                   (XmmsCb) on_xmms_playback_status, e);
	xmmsc_set_callback(e->xmms, XMMS_SIGNAL_PLAYBACK_PLAYTIME,
	                   (XmmsCb) on_xmms_playback_playtime, e);
	xmmsc_set_callback(e->xmms, XMMS_SIGNAL_PLAYBACK_CURRENTID,
	                   (XmmsCb) on_xmms_playback_currentid, e);
	xmmsc_set_callback(e->xmms, XMMS_SIGNAL_PLAYLIST_MEDIAINFO,
	                   (XmmsCb) on_xmms_playlist_mediainfo, e);
	xmmsc_set_callback(e->xmms, XMMS_SIGNAL_PLAYLIST_MEDIAINFO_ID,
	                   (XmmsCb) on_xmms_playlist_mediainfo_id, e);
	xmmsc_set_callback(e->xmms, XMMS_SIGNAL_PLAYLIST_LIST,
	                   (XmmsCb) on_xmms_playlist_list, e);
	xmmsc_set_callback(e->xmms, XMMS_SIGNAL_PLAYLIST_ADD,
	                   (XmmsCb) on_xmms_playlist_add, e);
	xmmsc_set_callback(e->xmms, XMMS_SIGNAL_PLAYLIST_REMOVE,
	                   (XmmsCb) on_xmms_playlist_remove, e);
	xmmsc_set_callback(e->xmms, XMMS_SIGNAL_PLAYLIST_CLEAR,
	                   (XmmsCb) on_xmms_playlist_clear, e);

	return true;
}

static Euphoria *euphoria_new() {
	Euphoria *e;
	char path[PATH_MAX + 1];

	if (!(e = calloc(1, sizeof(Euphoria))))
		return NULL;

	if (!setup_xmms(e)) {
		free (e);
		return NULL;
	}

	/* load config */
	config_init(&e->cfg);

	snprintf(path, sizeof(path),
	         "%s/.e/apps/" PACKAGE "/" PACKAGE ".db",
	         getenv("HOME"));

	if (!config_load(&e->cfg, path)) {
		snprintf(path, sizeof(path), "%s",
		         SYSCONF_DIR "/" PACKAGE ".db");

		if (!config_load(&e->cfg, path))
			debug(DEBUG_LEVEL_WARNING, "Cannot load config, "
			      "falling back to default settings!\n");
	}

	return e;
}

static void handle_args(Euphoria *e, int argc, const char **argv) {
	int o;
	struct option opts[] = {{"help", no_argument, 0, 'h'},
	                        {"version", no_argument, 0, 'v'},
	                        {"output", required_argument, 0, 'o'},
	                        {"engine", required_argument, 0, 'e'},
	                        {"theme", required_argument, 0, 't'},
	                        {NULL, 0, NULL, 0}};

	while ((o = getopt_long(argc, (char **) argv, "hvo:e:t:", opts,
	                        NULL)) != -1) {
		switch (o) {
			case 'h':
				printf("Usage: euphoria"
				       " [playlist.m3u] [file.ogg] [some/dir] ...\n\n");
				exit(1);
				break;
			case 'v':
				printf("Euphoria " VERSION "\n\n"
				       "Copyright (C) 2003-2004 Euphoria project\n\n");
				exit(1);
				break;
			case 'e':
				snprintf(e->cfg.evas_engine, sizeof(e->cfg.evas_engine),
				         "%s", optarg);
				break;
			case 't':
				snprintf(e->cfg.theme, sizeof(e->cfg.theme),
				         "%s", optarg);
				break;
			default:
				break;
		}
	}

	for (o = optind; o < argc; o++) {
		char buf[PATH_MAX];

		if (*argv[o] == '/')
			snprintf(buf, PATH_MAX, "file://%s", (char *)argv[o]);
		else
			snprintf(buf, PATH_MAX, "file://%s/%s", getenv("PWD"),
					(char *)argv[o]);
		xmmsc_playlist_add(e->xmms, buf);
	}

	e->args = argv;
	e->opt_start = optind;
}

int main(int argc, const char **argv) {
	Euphoria *e;

	ecore_init();
	lt_dlinit();

	if (!(e = euphoria_new()))
		return 1;

	handle_args(e, argc, argv);

	if (!ui_init(e)) {
		euphoria_free(e);
		return 1;
	}

	ui_refresh_time(e, 0);
	ui_refresh_seeker(e, 0);
	ui_refresh_volume(e);
	ecore_timer_add(1.5, ui_refresh_volume, e);

	debug(DEBUG_LEVEL_INFO, "Starting main loop\n");
	ecore_main_loop_begin();
	debug(DEBUG_LEVEL_INFO, "Shutting down\n");

	euphoria_free(e);

	lt_dlexit();
	ecore_shutdown();

	return 0;
}


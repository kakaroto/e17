/* Eplayer OggVorbis Player - Phase 3 - Started 5/6/03 */
/* Edje Overhaul startnig phase 4 - Started 7/30/03 */

#include <config.h>
#include <Edje.h>
#include <Edb.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include "eplayer.h"
#include "interface.h"
#include "track.h"
#include "utils.h"

static Evas_List *load_input_plugins() {
	Evas_List *files, *l, *plugins = NULL;
	InputPlugin *ip;
	char name[128];

	if (!(files = dir_get_files(PLUGIN_DIR "/input")))
		return NULL;

	for (l = files; l; l = l->next) {
		/* get the plugin name from the filename */
		sscanf((char *) l->data, "lib%127[^.].so", name);
			
		if ((ip = plugin_new(name, PLUGIN_TYPE_INPUT)))
			plugins = evas_list_prepend(plugins, ip);
	}

	while (files) {
		free(files->data);
		files = evas_list_remove(files, files->data);
	}

	return plugins;
}

static void config_init(Config *cfg) {
	snprintf(cfg->evas_engine, sizeof(cfg->evas_engine),
	         "software");
	snprintf(cfg->output_plugin, sizeof(cfg->output_plugin),
	         "OSS");
	snprintf(cfg->theme, sizeof(cfg->theme), "default");
}

static int config_load(Config *cfg, const char *file) {
	E_DB_File *edb;
	char *str;
	int val = 0;
	
	if (!cfg || !file || !*file)
		return 0;

	if (!(edb = e_db_open_read((char *) file)))
		return 0;

	if (e_db_int_get(edb, "/eplayer/time_display_show_left", &val))
		cfg->time_display = !!val;
	
	if ((str = e_db_str_get(edb, "/eplayer/evas_engine"))) {
		snprintf(cfg->evas_engine, sizeof(cfg->evas_engine), str);
		free(str);
	}
	
	if ((str = e_db_str_get(edb, "/eplayer/output_plugin"))) {
		snprintf(cfg->output_plugin, sizeof(cfg->output_plugin), str);
		free(str);
	}
	
	if ((str = e_db_str_get(edb, "/eplayer/theme"))) {
		snprintf(cfg->theme, sizeof(cfg->theme), str);
		free(str);
	}

	return 1;
}

static void eplayer_free(ePlayer *player) {
	Evas_List *l;
	
	if (!player)
		return;

	eplayer_playback_stop(player);
	track_close(player);

	playlist_free(player->playlist);

	/* unload plugins */
	if (player->output)
		plugin_free(player->output);

	for (l = player->input_plugins; l; l = l->next)
		plugin_free(l->data);
	
	pthread_mutex_destroy(&player->playback_stop_mutex);
	pthread_mutex_destroy(&player->playback_next_mutex);
	
	free(player);
}

static ePlayer *eplayer_new(const char **args) {
	ePlayer *player;
	char cfg_file[PATH_MAX + 1];

	if (!(player = malloc(sizeof(ePlayer))))
		return NULL;

	memset(player, 0, sizeof(ePlayer));

	player->args = args;

	/* load config */
	config_init(&player->cfg);
	
	snprintf(cfg_file, sizeof(cfg_file),
	         "%s/.e/apps/" PACKAGE "/" PACKAGE ".db",
	         getenv("HOME"));
	
	if (!config_load(&player->cfg, cfg_file)) {
		snprintf(cfg_file, sizeof(cfg_file),
		         SYSCONF_DIR "/" PACKAGE ".db");

		if (!config_load(&player->cfg, cfg_file))
			debug(DEBUG_LEVEL_WARNING, "Cannot load config, "
			      "falling back to default settings!\n");
	}

	player->input_plugins = load_input_plugins();

	player->playlist = playlist_new(player->input_plugins);

	/* load the output plugin */
	player->output = plugin_new(player->cfg.output_plugin,
	                            PLUGIN_TYPE_OUTPUT);

	if (!player->output) {
		debug(DEBUG_LEVEL_CRITICAL, "Cannot load %s output plugin!\n",
		      player->cfg.output_plugin);

		eplayer_free(player);
		return NULL;
	}

	pthread_mutex_init(&player->playback_next_mutex, NULL);
	pthread_mutex_init(&player->playback_stop_mutex, NULL);
	player->playback_stop = 1;

	return player;
}

/**
 * Stops playback.
 *
 * @param player
 */
void eplayer_playback_stop(ePlayer *player) {
	if (!player)
		return;

	/* stop the timer that updates the time part */
	if (player->time_timer) {
		ecore_timer_del(player->time_timer);
		player->time_timer = NULL;
	}

	if (player->playback_stop)
		return;

	pthread_mutex_lock(&player->playback_stop_mutex);
	player->playback_stop = 1;
	pthread_mutex_unlock(&player->playback_stop_mutex);

	pthread_join(player->playback_thread, NULL);
}

static int check_playback_next(void *udata) {
	ePlayer *player = udata;

	if (!player->playback_next)
		return 1;

	pthread_join(player->playback_thread, NULL);
	player->playback_next = 0;

	edje_object_signal_emit(player->gui.edje,
	                        "PLAY_NEXT", "next_button");

	return 0; /* stop timer */
}

/**
 * Starts playback.
 *
 * @param player
 * @param rewind_track
 */
void eplayer_playback_start(ePlayer *player, int rewind_track) {
	if (rewind_track)
		track_rewind(player);

	ecore_timer_add(0.5, check_playback_next, player);
	player->time_timer = ecore_timer_add(0.5, track_update_time,
	                                     player);
	
	pthread_mutex_lock(&player->playback_stop_mutex);
	player->playback_stop = 0;
	pthread_mutex_unlock(&player->playback_stop_mutex);

	pthread_create(&player->playback_thread, NULL,
	               (void *) &track_play_chunk, player);
}

/**
 * Add files/directories/m3u's to the playlist
 *
 * @param player
 */
static int load_playlist(void *data) {
	ePlayer *player = data;
	int i;

	for (i = 1; player->args[i]; i++)
		playlist_load_any(player->playlist, player->args[i], i > 1);

	debug(DEBUG_LEVEL_INFO, "Got %i playlist entries\n",
	      player->playlist->num);

	/* update the ui */
	ui_fill_playlist(player);

	if (player->playlist->num)
		track_open(player);
	
	ui_refresh_time(player, 0);

	return 0; /* stop timer */
}

int main(int argc, const char **argv) {
	ePlayer *player;

	if (argc == 1) {
		printf("%s v%s  - Usage: %s playlist.m3u [file.ogg] [some/dir] ...\n\n",
		       PACKAGE, VERSION, argv[0]);
		return 1;
	}
	
	if (!(player = eplayer_new(argv)))
		return 1;
	
	if (!ui_init(player)) {
		eplayer_free(player);
		return 1;
	}	
	
	ecore_timer_add(1, load_playlist, player);

	ui_refresh_volume(player);
	ecore_timer_add(1.5, ui_refresh_volume, player);

	debug(DEBUG_LEVEL_INFO, "Starting main loop\n");

	ecore_main_loop_begin();

	eplayer_free(player);

	ecore_evas_shutdown();
	ecore_shutdown();
	
	return 0;
}


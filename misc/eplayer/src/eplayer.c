/* Eplayer OggVorbis Player - Phase 3 - Started 5/6/03 */
/* Edje Overhaul startnig phase 4 - Started 7/30/03 */

#include <config.h>
#include <Ecore_Evas.h>
#include <Edb.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include "eplayer.h"
#include "interface.h"
#include "track.h"

int is_dir(const char *dir) {
	struct stat st;

	if (stat(dir, &st))
		return 0;

	return (S_ISDIR(st.st_mode));
}

static Evas_List *load_input_plugins() {
	Evas_List *list = NULL;
	InputPlugin *ip;
	DIR *dir;
	struct dirent *entry;
	char name[128];

	if (!(dir = opendir(PLUGIN_DIR "/input")))
		return NULL;

	/* ignore "." and ".." */
	while ((entry = readdir(dir))
	       && (!strcmp(entry->d_name, ".")
	       || !strcmp(entry->d_name, "..")));

	if (!entry)
		return NULL;
	
	/* real entries */
	do {
		if (!is_dir(entry->d_name)) {
			/* get the plugin name from the filename */
			sscanf(entry->d_name, "lib%[^.].so", name);
			
			if ((ip = plugin_new(name, PLUGIN_TYPE_INPUT)))
				list = evas_list_prepend(list, ip);
		}
	} while ((entry = readdir(dir)));

	closedir(dir);

	return list;
}

static void config_init(Config *cfg) {
	snprintf(cfg->evas_engine, sizeof(cfg->evas_engine),
	         "software");
	snprintf(cfg->output_plugin, sizeof(cfg->output_plugin),
	         "OSS");
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

	return 1;
}

static void eplayer_free(ePlayer *player) {
	Evas_List *l;
	
	if (!player)
		return;

	playlist_free(player->playlist);

	/* unload plugins */
	if (player->output)
		plugin_free(player->output);

	for (l = player->input_plugins; l; l = l->next)
		plugin_free(l->data);
	
	free(player);
}

static ePlayer *eplayer_new() {
	ePlayer *player;
	char cfg_file[PATH_MAX + 1];

	if (!(player = malloc(sizeof(ePlayer))))
		return NULL;

	memset(player, 0, sizeof(ePlayer));

	/* load config */
	config_init(&player->cfg);
	
	snprintf(cfg_file, sizeof(cfg_file), "%s/." PACKAGE ".db",
	         getenv("HOME"));
	
	if (!config_load(&player->cfg, cfg_file)) {
		snprintf(cfg_file, sizeof(cfg_file),
		         SYSCONF_DIR "/" PACKAGE ".db");

		if (!config_load(&player->cfg, cfg_file)) {
#ifdef DEBUG
			printf("Cannot load config, "
			       "falling back to default settings!\n");
#endif
		}
	}

	player->input_plugins = load_input_plugins();

	player->playlist = playlist_new(player->input_plugins);

	/* load the output plugin */
	player->output = plugin_new(player->cfg.output_plugin,
	                            PLUGIN_TYPE_OUTPUT);

	if (!player->output) {
		fprintf(stderr, "Cannot load %s output plugin!\n",
		        player->cfg.output_plugin);

		eplayer_free(player);
		return NULL;
	}

	return player;
}

/**
 * Stops playback.
 *
 * @param player
 * @param rewind_track
 */
void eplayer_playback_stop(ePlayer *player, int rewind_track) {
	if (!player)
		return;

	/* stop the playloop */
	if (player->play_idler) {
		ecore_idler_del(player->play_idler);
		player->play_idler = NULL;
	}

	/* stop the timer that updates the time part */
	if (player->time_timer) {
		ecore_timer_del(player->time_timer);
		player->time_timer = NULL;
	}

	if (rewind_track) {
		track_close(player);
		track_open(player);
	}
}

/**
 * Starts playback.
 *
 * @param player
 * @param rewind_track
 */
void eplayer_playback_start(ePlayer *player, int rewind_track) {
	if (rewind_track) {
		track_close(player);
		track_open(player);
	}

	/* start the playloop */
	player->play_idler = ecore_idler_add(track_play_chunk, player);
	player->time_timer = ecore_timer_add(0.5, track_update_time,
	                                     player);
}

int main(int argc, const char **argv) {
	ePlayer *player;
	int i;

	if (!(player = eplayer_new()))
		return 1;

	if (argc == 1) {
		printf("%s v%s  - Usage: %s playlist.m3u [file.ogg] [some/dir] ...\n\n",
		       PACKAGE, VERSION, argv[0]);
		return 1;
	}
	
	/* Parse Args */
	for (i = 1; i < argc; i++) {
#ifdef DEBUG
		printf("Adding file to playlist: %s\n", argv[i]);
#endif
		
		playlist_load_any(player->playlist, argv[i], i > 1);
	}
	
	if (!player->playlist->num) {
		fprintf(stderr, "No files loaded!\n");
		eplayer_free(player);
		return 1;
	}
	
	if (!setup_gui(player)) {
		eplayer_free(player);
		return 1;
	}
		
	show_playlist(player);

	refresh_volume(player);
	ecore_timer_add(1.5, refresh_volume, player);

	track_open(player);
	refresh_time(player, 0);

#ifdef DEBUG
	printf("DEBUG: Starting main loop\n");
#endif

	ecore_main_loop_begin();

	eplayer_free(player);

	ecore_evas_shutdown();
	ecore_shutdown();
	
	return 0;
}


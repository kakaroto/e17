/* vim: set sw=4 ts=4 sts=4 expandtab: */
#include <stdio.h>
#include <string.h>
#include <Eet.h>
#include <Evas.h>
#include <Ecore_File.h>
#include "eli_statistics.h"

/* avoid warings */
typedef void * (*list_next)    (void *);
typedef void * (*list_append)  (void *, void *);
typedef void * (*list_data)    (void *);
typedef void * (*list_free)    (void *);
typedef void   (*hash_foreach) (void *,
                                int (*func) (void *, const char *, void *, void *), 
                                void *);
typedef void * (*hash_add)     (void *, const char *, void *);
typedef void   (*hash_free)    (void *);

/* globals */
static Eet_Data_Descriptor * edd_statistics;
static char * eet_file_name;

int _eli_statistics_write(const char * game, Eli_Statistics *stats);

/* extarnals */
void eli_statistics_init(const char * app)
{
    char buffer[1024];
    const char * home;

    edd_statistics = eet_data_descriptor_new("Eli_Statistics",
                                             sizeof(Eli_Statistics),
                                             (list_next) evas_list_next,
                                             (list_append) evas_list_append,
                                             (list_data) evas_list_data,
                                             (list_free) evas_list_free,
                                             (hash_foreach) evas_hash_foreach,
                                             (hash_add) evas_hash_add,
                                             (hash_free) evas_hash_free);

    EET_DATA_DESCRIPTOR_ADD_BASIC(edd_statistics, Eli_Statistics,
                                  "won_games", won_games, EET_T_INT);
    EET_DATA_DESCRIPTOR_ADD_BASIC(edd_statistics, Eli_Statistics,
                                  "lost_games", lost_games, EET_T_INT);

    home = getenv("HOME");
    if (!home)
	    home = "/tmp";

    snprintf(buffer, sizeof(buffer), 
		    "%s/.e/apps/%s/statistics.eet", home, app);

    eet_file_name = strdup(buffer);
}

void eli_statistics_shutdown(void)
{
    eet_data_descriptor_free(edd_statistics);
    free(eet_file_name);
}

void eli_statistics_lost_add(const char * game)
{
    Eli_Statistics stat;

    stat = eli_statistics_get(game);
    stat.lost_games++;

    _eli_statistics_write(game, &stat);
}

void eli_statistics_win_add(const char * game)
{
    Eli_Statistics stat;

    stat = eli_statistics_get(game);
    stat.won_games++;

    _eli_statistics_write(game, &stat);
}

Eli_Statistics eli_statistics_get(const char * game)
{
    Eet_File * file = NULL;
    Eli_Statistics stats;
    Eli_Statistics * pstats;

    /* if it is the first time */
    stats.won_games = 0;
    stats.lost_games = 0;

    if (!ecore_file_exists(eet_file_name)) return stats;
        
    file = eet_open(eet_file_name, EET_FILE_MODE_READ);
    if (!file) return stats;

    pstats = (Eli_Statistics *) eet_data_read(file, edd_statistics, game);
    if (pstats) {
        stats = *pstats;
        free(pstats);
    }
    eet_close(file);

    return stats;
}

void eli_statistics_del(const char * game)
{
    Eli_Statistics stat;

    stat.lost_games = 0;
    stat.won_games = 0;

    _eli_statistics_write(game, &stat);
}

int _eli_statistics_write(const char * game, Eli_Statistics *stats)
{
    Eet_File * file;

    if (ecore_file_exists(eet_file_name)) {
        file = eet_open(eet_file_name, EET_FILE_MODE_READ_WRITE);
    }
    else {
        file = eet_open(eet_file_name, EET_FILE_MODE_WRITE);
        if (!file) {
            fprintf(stderr, "Could not create file %s\n!", eet_file_name);
	    return 0;
	}
    }

    eet_data_write(file, edd_statistics, game, stats, 1);
    eet_close(file);

    return 1;
}


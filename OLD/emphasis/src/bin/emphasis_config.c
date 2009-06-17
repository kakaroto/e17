#include "emphasis.h"
#include "emphasis_config.h"

/* TODO: add emphasis state */
void
set_defaults(void)
{
  ecore_config_string_default(MPD_HOSTNAME_KEY, "localhost");
  ecore_config_int_default(MPD_PORT_KEY, 6600);
  ecore_config_string_default(MPD_PASSWORD_KEY, NULL);
  ecore_config_int_default(MPD_CROSSFADE_KEY, 0);

  ecore_config_int_default(EMP_GEOMETRY_X_KEY, 0);
  ecore_config_int_default(EMP_GEOMETRY_Y_KEY, 0);
  ecore_config_int_default(EMP_GEOMETRY_W_KEY, 700);
  ecore_config_int_default(EMP_GEOMETRY_H_KEY, 600);

  ecore_config_int_default(EMP_GEOMETRY_MEDIA_W_KEY, 600);
  ecore_config_int_default(EMP_GEOMETRY_MEDIA_H_KEY, 500);

  ecore_config_int_default(EMP_COLWIDTH_TITLE_KEY,  220);
  ecore_config_int_default(EMP_COLWIDTH_TIME_KEY,   50);
  ecore_config_int_default(EMP_COLWIDTH_ARTIST_KEY, 100);
  ecore_config_int_default(EMP_COLWIDTH_ALBUM_KEY,  100);

  ecore_config_int_default(EMP_MEDIA_PANED_KEY, 50);

  ecore_config_int_default(EMP_MODE_KEY,           EMPHASIS_FULL);
  ecore_config_int_default(EMP_PLS_SHOW_KEY,       ETK_FALSE);
  ecore_config_int_default(EMP_STOP_ON_EXIT_KEY,   ETK_FALSE);
  ecore_config_int_default(EMP_COVER_KEEP_ASPECT,  ETK_TRUE);
}

Emphasis_Config *
config_load(void)
{
  Emphasis_Config *config;

  config = malloc(sizeof(Emphasis_Config));
  if(!config) return NULL;
  set_defaults();
  ecore_config_load();

  config->hostname = ecore_config_string_get(MPD_HOSTNAME_KEY);
  config->port = ecore_config_int_get(MPD_PORT_KEY);
  config->password = ecore_config_string_get(MPD_PASSWORD_KEY);
  config->crossfade = ecore_config_int_get(MPD_CROSSFADE_KEY);

  config->geometry.x = ecore_config_int_get(EMP_GEOMETRY_X_KEY);
  config->geometry.y = ecore_config_int_get(EMP_GEOMETRY_Y_KEY);
  config->geometry.w = ecore_config_int_get(EMP_GEOMETRY_W_KEY);
  config->geometry.h = ecore_config_int_get(EMP_GEOMETRY_H_KEY);

  config->geometry.media_w = ecore_config_int_get(EMP_GEOMETRY_MEDIA_W_KEY);
  config->geometry.media_h = ecore_config_int_get(EMP_GEOMETRY_MEDIA_H_KEY);

  config->mode = ecore_config_int_get(EMP_MODE_KEY);

  config->colwidth.title  = ecore_config_int_get(EMP_COLWIDTH_TITLE_KEY);
  config->colwidth.time   = ecore_config_int_get(EMP_COLWIDTH_TIME_KEY);
  config->colwidth.artist = ecore_config_int_get(EMP_COLWIDTH_ARTIST_KEY);
  config->colwidth.album  = ecore_config_int_get(EMP_COLWIDTH_ALBUM_KEY);

  config->media_paned = ecore_config_int_get(EMP_MEDIA_PANED_KEY);

  config->mode         = ecore_config_int_get(EMP_MODE_KEY);
  config->pls_show     = ecore_config_int_get(EMP_PLS_SHOW_KEY);
  config->stop_on_exit = ecore_config_int_get(EMP_STOP_ON_EXIT_KEY);
  config->keep_aspect  = ecore_config_int_get(EMP_COVER_KEEP_ASPECT);

  return config;
}

void
config_save(Emphasis_Config *config)
{
  ecore_config_string_set(MPD_HOSTNAME_KEY, config->hostname);
  ecore_config_int_set(MPD_PORT_KEY, config->port);
  ecore_config_string_set(MPD_PASSWORD_KEY, config->password);
  ecore_config_int_set(MPD_CROSSFADE_KEY, config->crossfade);

  ecore_config_int_set(EMP_GEOMETRY_X_KEY, config->geometry.x);
  ecore_config_int_set(EMP_GEOMETRY_Y_KEY, config->geometry.y);
  ecore_config_int_set(EMP_GEOMETRY_W_KEY, config->geometry.w);
  ecore_config_int_set(EMP_GEOMETRY_H_KEY, config->geometry.h);

  ecore_config_int_set(EMP_MODE_KEY, config->mode);

  ecore_config_int_set(EMP_COLWIDTH_TITLE_KEY, config->colwidth.title);
  ecore_config_int_set(EMP_COLWIDTH_TIME_KEY, config->colwidth.time);
  ecore_config_int_set(EMP_COLWIDTH_ARTIST_KEY, config->colwidth.artist);
  ecore_config_int_set(EMP_COLWIDTH_ALBUM_KEY, config->colwidth.album);

  ecore_config_int_set(EMP_MEDIA_PANED_KEY, config->media_paned);

  ecore_config_int_set(EMP_MODE_KEY,           config->mode);
  ecore_config_int_set(EMP_PLS_SHOW_KEY,       config->pls_show);
  ecore_config_int_set(EMP_STOP_ON_EXIT_KEY,   config->stop_on_exit);
  ecore_config_int_set(EMP_COVER_KEEP_ASPECT,  config->keep_aspect);

  ecore_config_save();
}

void
config_free(Emphasis_Config *config)
{
  if (config->hostname)
    {
      free(config->hostname);
    }
  if (config->password)
    {
      free(config->password);
    }
  free(config);
}

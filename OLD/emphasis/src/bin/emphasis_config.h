#ifndef EMPHASIS_CONFIG_H_
#define EMPHASIS_CONFIG_H_
/**
 * @defgroup config
 * @{
 */

#include <Ecore_Config.h>

#define MPD_HOSTNAME_KEY         "/mpd/hostname"
#define MPD_PORT_KEY             "/mpd/port"
#define MPD_PASSWORD_KEY         "/mpd/password"
#define MPD_CROSSFADE_KEY        "/mpd/crossfade"

#define EMP_GEOMETRY_X_KEY       "/geometry/x"
#define EMP_GEOMETRY_Y_KEY       "/geometry/y"
#define EMP_GEOMETRY_W_KEY       "/geometry/w"
#define EMP_GEOMETRY_H_KEY       "/geometry/h"
#define EMP_GEOMETRY_MEDIA_W_KEY "/geometry/media/w"
#define EMP_GEOMETRY_MEDIA_H_KEY "/geometry/media/h"

#define EMP_MODE_KEY             "/emphasis/mode"
#define EMP_PLS_SHOW_KEY         "/emphasis/plsshow"

#define EMP_COLWIDTH_TITLE_KEY   "/emphasis/colwidth/title"
#define EMP_COLWIDTH_TIME_KEY    "/emphasis/colwidth/time"
#define EMP_COLWIDTH_ARTIST_KEY  "/emphasis/colwidth/artist"
#define EMP_COLWIDTH_ALBUM_KEY   "/emphasis/colwidth/album"

#define EMP_MEDIA_PANED_KEY      "/emphasis/media/paned"

#define EMP_STOP_ON_EXIT_KEY     "/emphasis/stop"
#define EMP_COVER_KEEP_ASPECT    "/emphasis/cover_skeep_aspect"

typedef struct _Emphasis_Config Emphasis_Config;
struct _Emphasis_Config
{
  char *hostname;
  int port;
  char *password;
  int crossfade;

  struct
  {
    int x;
    int y;
    int w;
    int h;
    int media_w;
    int media_h;
  } geometry;

  struct
  {
    int title;
    int time;
    int artist;
    int album;
  } colwidth;

  int media_paned;

  Emphasis_Mode mode;
  int pls_show;
  int stop_on_exit;
  int keep_aspect;
};

void config_save(Emphasis_Config *config);
Emphasis_Config *config_load(void);
void config_save(Emphasis_Config *config);
void config_free(Emphasis_Config *config);

 /** @} */
#endif /* EMPHASIS_CONFIG_H_ */

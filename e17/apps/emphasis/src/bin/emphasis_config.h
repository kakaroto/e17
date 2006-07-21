#ifndef EMPHASIS_CONFIG_H_
#define EMPHASIS_CONFIG_H_
/**
 * @defgroup config
 * @{
 */

#include <Ecore_Config.h>

#define MPD_HOSTNAME_KEY  "/mpd/hostname"
#define MPD_PORT_KEY      "/mpd/port"
#define MPD_PASSWORD_KEY  "/mpd/password"
#define MPD_CROSSFADE_KEY "/mpd/crossfade"  /*TODO handle the crossfade */

#define EMP_GEOMETRY_X_KEY  "/geometry/x"
#define EMP_GEOMETRY_Y_KEY  "/geometry/y"
#define EMP_GEOMETRY_W_KEY  "/geometry/w"
#define EMP_GEOMETRY_H_KEY  "/geometry/h"
#define EMP_MODE_KEY        "/emphasis/mode"

#define EMP_COLWIDTH_TITLE_KEY      "/emphasis/colwidth/title"
#define EMP_COLWIDTH_TIME_KEY       "/emphasis/colwidth/time"
#define EMP_COLWIDTH_ARTIST_KEY     "/emphasis/colwidth/artist"
#define EMP_COLWIDTH_ALBUM_KEY      "/emphasis/colwidth/album"

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
  } geometry;

  struct
  {
    int title;
    int time;
    int artist;
    int album;
  } colwidth;

  Emphasis_Mode mode;
};

void config_save(Emphasis_Config *config);
Emphasis_Config *config_load(void);
void config_save(Emphasis_Config *config);
void config_free(Emphasis_Config *config);

 /** @} */
#endif /* EMPHASIS_CONFIG_H_ */

#ifndef _EPHYSICS_GAME_H
#define _EPHYSICS_GAME_H

#include <Edje.h>
#include <Evas.h>
#include <Eina.h>
#include <Elementary.h>
#include <EPhysics.h>
#include <Etrophy.h>

#include "game_world.h"

#ifdef GAME_LOG_COLOR
#undef GAME_LOG_COLOR
#endif
#define GAME_LOG_COLOR EINA_COLOR_LIGHTCYAN

#ifdef ERR
# undef ERR
#endif
#define ERR(...) EINA_LOG_DOM_ERR(_efbb_log_dom, __VA_ARGS__)

#ifdef DBG
# undef DBG
#endif
#define DBG(...) EINA_LOG_DOM_DBG(_efbb_log_dom, __VA_ARGS__)

#ifdef INF
# undef INF
#endif
#define INF(...) EINA_LOG_DOM_INFO(_efbb_log_dom, __VA_ARGS__)

#ifdef WRN
# undef WRN
#endif
#define WRN(...) EINA_LOG_DOM_WARN(_efbb_log_dom, __VA_ARGS__)

#ifdef CRIT
# undef CRIT
#endif
#define CRIT(...) EINA_LOG_DOM_CRIT(_efbb_log_dom, __VA_ARGS__)

#define GAME_THEME "efbb"

#define MENU_MUSIC "main.wav"
#define MENU_MUSIC_ID 1
#define GAME_MUSIC "battle.wav"
#define GAME_MUSIC_ID 2

#define WIDTH (1280)
#define HEIGHT (720)

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _Game Game;

extern int _efbb_log_dom;

typedef enum
{
    LAYER_BG,
    LAYER_OBJ,
    LAYER_BLOCKS,
    LAYER_OBJ_ABOVE,
    LAYER_BG_ABOVE,
    LAYER_EVENT,
    LAYER_UI,
} Game_Layer;

void game_main_to_level_select(Game *game);
void game_level_select_to_main(Game *game);
void game_level_select_to_level(Game *game, Level *level);

Eina_Bool islevel(EPhysics_World *world);

#ifdef __cplusplus
}
#endif

#endif

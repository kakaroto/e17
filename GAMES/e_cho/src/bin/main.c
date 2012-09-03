#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <Elementary.h>

#ifndef ELM_LIB_QUICKLAUNCH

#include <Eina.h>
#include <Evas.h>
#include <Etrophy.h>
#include <stdlib.h>

#include "gettext.h"
#include "log.h"
#include "sound.h"

#define LEVEL_DEFAULT 3
#define LEVEL_MAX     100
#define SPEED_DEFAULT 0.8
#define SPEED_MIN     0.3
#define SPEED_MAX     3.0

#define DF_THEME "pocket"

typedef enum _Mode
{
    OFF,
    PLAYING,
    REPEATING
} Mode;

typedef struct _Game
{
    Ecore_Timer *play_timer, *turn_timer;
    Evas_Object *layout;
    double speed;
    int step, level, score;
    int seq[LEVEL_MAX];
    int next;
    Mode mode;
    Etrophy_Gamescore *gamescore;
} Game;

int _log_domain = -1;

static void
_create_seq(Game *game)
{
    int i;

    for (i = 0; i < game->level; i++)
    {
        game->seq[i] = random() % 4;
    }
}

static void
_score_inc(Game *game, int score_inc)
{
    char buf[32];
    game->score += score_inc;
    snprintf(buf, sizeof(buf), "%i", game->score);
    elm_object_part_text_set(game->layout, "score", buf);
}

static void
_hi_score_set(Game *game)
{
    char buf[32];
    int score;

    score = etrophy_gamescore_level_hi_score_get(game->gamescore, "E_CHO");
    if (score < 0) score = 0;
    snprintf(buf, sizeof(buf), "%i", score);
    elm_object_part_text_set(game->layout, "hi-score", buf);
}

static void
_player_fail(Game *game)
{
    const char *name = "Player";

    if (getenv("USER"))
        name = getenv("USER");

    etrophy_gamescore_level_score_add(game->gamescore, "E_CHO", name,
            game->score, 0);
    _hi_score_set(game);
    elm_object_signal_emit(game->layout, "fail", "");

    game->score = 0;
    game->mode = OFF;
}

static void
_player_turn(Game *game)
{
    game->next = 0;
    game->mode = REPEATING;
    elm_object_signal_emit(game->layout, "repeating", "");
}

static Eina_Bool
_play_next(void *data)
{
    Game *game = data;

    switch (game->seq[game->next])
    {
        case 0:
            elm_object_signal_emit(game->layout, "hi", "up");
            break;
        case 1:
            elm_object_signal_emit(game->layout, "hi", "right");
            break;
        case 2:
            elm_object_signal_emit(game->layout, "hi", "down");
            break;
        case 3:
            elm_object_signal_emit(game->layout, "hi", "left");
            break;
        default:
            ERR("Wrong number on sequence");
            game->play_timer = NULL;
            return ECORE_CALLBACK_CANCEL;
    }

    (game->next)++;

    if (game->next == game->step)
    {
        _player_turn(game);
        game->play_timer = NULL;
        return ECORE_CALLBACK_CANCEL;
    }

    return ECORE_CALLBACK_RENEW;
}

static void
_cpu_turn(Game *game)
{
    elm_object_signal_emit(game->layout, "playing", "");
    game->next = 0;
    game->mode = PLAYING;
    game->step++;
    game->play_timer = ecore_timer_add(game->speed, _play_next, game);
}

static Eina_Bool
_cpu_turn_cb(void *data)
{
    Game *game = data;
    _cpu_turn(game);
    game->turn_timer = NULL;
    return ECORE_CALLBACK_CANCEL;
}

static void
_next_level(Game *game, int level)
{
    char buf[32];

    if (level > LEVEL_MAX)
    {
        elm_object_signal_emit(game->layout, "player_won", "");
        game->mode = OFF;
        return;
    }

    _score_inc(game, 100 * (level - LEVEL_DEFAULT));
    snprintf(buf, sizeof(buf), "%i", level - LEVEL_DEFAULT + 1);
    elm_object_part_text_set(game->layout, "level", buf);
    if (level != LEVEL_DEFAULT)
        elm_object_signal_emit(game->layout, "next_level", "");
    game->mode = PLAYING;
    game->level = level;
    _create_seq(game);
    game->step = 0;

    if (level == LEVEL_DEFAULT)
        _cpu_turn(game);
    else
        game->turn_timer = ecore_timer_add(1.0, _cpu_turn_cb, game);
}

static void
_start_new_game(Game *game)
{
    elm_object_signal_emit(game->layout, "new_game", "");
    _next_level(game, LEVEL_DEFAULT);
}

static void
_start_new_game_cb(void *data, Evas_Object *o __UNUSED__, const char *sig __UNUSED__, const char *src __UNUSED__)
{
    Game *game = data;
    game->mode = OFF;
    if (game->play_timer)
        ecore_timer_del(game->play_timer);
    game->play_timer = NULL;
    if (game->turn_timer)
        ecore_timer_del(game->turn_timer);
    game->turn_timer = NULL;
    game->score = 0;
    _start_new_game(game);
}

static void
_bt_clicked(void *data, Evas_Object *o __UNUSED__, const char *sig __UNUSED__, const char *src)
{
    Game *game = data;
    int clicked;

    if (game->mode != REPEATING)
        return;

    if (!strcmp(src, "up"))
        clicked = 0;
    else if (!strcmp(src, "right"))
        clicked = 1;
    else if (!strcmp(src, "down"))
        clicked = 2;
    else if (!strcmp(src, "left"))
        clicked = 3;
    else
    {
        ERR("Wrong source of button clicked");
        return;
    }

    if (clicked == game->seq[game->next])
    {
        (game->next)++;
        _score_inc(game, (SPEED_MAX - game->speed) * 10 + 30);

        if (game->next == game->step)
        {
            if (game->step == game->level)
            {
                _next_level(game, game->level + 1);
                return;
            }
            _cpu_turn(game);
            return;
        }
        return;
    }
    _player_fail(game);
}

static void
_win_delete_request(void *data __UNUSED__, Evas_Object *obj, void *event_info __UNUSED__)
{
    evas_object_del(obj);
    elm_exit();
}

static Eina_Bool
_win_new(Game *game)
{
    Evas_Object *win, *layout;

    win = elm_win_add(NULL, PACKAGE_NAME, ELM_WIN_BASIC);
    if (!win) return EINA_FALSE;
    evas_object_smart_callback_add(win, "delete,request",
            _win_delete_request, NULL);

    layout = elm_layout_add(win);
    if (!layout) return EINA_FALSE;
    evas_object_size_hint_weight_set
        (layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    evas_object_size_hint_align_set(layout, EVAS_HINT_FILL, EVAS_HINT_FILL);
    elm_win_resize_object_add(win, layout);

    if (!elm_layout_theme_set(layout, "e_cho", "win", DF_THEME))
    {
        ERR("Couldn't find theme for 'win' at %s.edj",
                PACKAGE_DATA_DIR "/" DF_THEME);
        evas_object_del(win);
        return EINA_FALSE;
    }

    elm_object_signal_callback_add(layout, "*", "sound", sound_play_cb, NULL);
    elm_object_signal_callback_add(layout, "gamebt,clicked", "*",
            _bt_clicked, game);
    elm_object_signal_callback_add(layout, "clicked", "bt_start",
            _start_new_game_cb, game);
    game->layout = layout;
    evas_object_show(layout);

    elm_win_title_set(win, "Echo");
    evas_object_show(win);

    return EINA_TRUE;
}

EAPI int
elm_main(int argc __UNUSED__, char **argv __UNUSED__)
{
    Game game;
    int r = 0;
#if ENABLE_NLS
    setlocale(LC_ALL, "");
    bindtextdomain(GETTEXT_PACKAGE, LOCALEDIR);
    bind_textdomain_codeset(GETTEXT_PACKAGE, "UTF-8");
    textdomain(GETTEXT_PACKAGE);
#endif

    _log_domain = eina_log_domain_register("e_cho", EINA_COLOR_GREEN);
    if (_log_domain < 0) {
        EINA_LOG_CRIT("Could not register log domain: e_cho");
        return -1;
    }

    if (!sound_init())
        return -1;

    if (!etrophy_init())
    {
        r = -1;
        goto err_etrophy;
    }

    game.gamescore = etrophy_gamescore_load("e_cho");
    if (!game.gamescore)
        game.gamescore = etrophy_gamescore_new("e_cho");
    if (!game.gamescore)
    {
        r = -1;
        goto end;
    }

    elm_theme_overlay_add(NULL, PACKAGE_DATA_DIR "/" DF_THEME ".edj");

    game.speed = SPEED_DEFAULT;
    game.level = LEVEL_DEFAULT;
    game.mode = OFF;
    game.turn_timer = NULL;
    game.play_timer = NULL;
    game.score = 0;

    if (!_win_new(&game))
    {
        r = -1;
        goto end;
    }
    _hi_score_set(&game);

    DBG("Game initialized");
    elm_run();
    DBG("Game shutdown");

    etrophy_gamescore_save(game.gamescore, NULL);

end:
    etrophy_shutdown();
err_etrophy:
    sound_shutdown();
    eina_log_domain_unregister(_log_domain);
    elm_shutdown();
    return r;
}

#endif
ELM_MAIN()

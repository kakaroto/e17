#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <Elementary.h>
#include <Edje.h>
#include <Evas.h>
#ifndef ELM_LIB_QUICKLAUNCH

#include <stdlib.h>
#include "gettext.h"
#include "sound.h"

#define LEVEL_DEFAULT 3
#define LEVEL_MAX     100
#define SPEED_DEFAULT 0.8
#define SPEED_MIN     0.3
#define SPEED_MAX     3.0

#define DF_THEME "/pocket.edj"

typedef enum _Mode
{
    OFF,
    PLAYING,
    REPEATING
} Mode;

typedef struct _Game
{
    Ecore_Timer *play_timer, *turn_timer;
    Evas_Object *base;
    double speed;
    int step, level, score, hi_score;
    int seq[LEVEL_MAX];
    int next;
    Mode mode;
} Game;

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
    edje_object_part_text_set(game->base, "score", buf);
}

static void
_player_fail(Game *game)
{
    edje_object_signal_emit(game->base, "fail", "");
    if (game->score > game->hi_score)
    {
        char buf[32];
        game->hi_score = game->score;
        snprintf(buf, sizeof(buf), "%i", game->hi_score);
        edje_object_part_text_set(game->base, "hi-score", buf);
    }
    game->score = 0;
    game->mode = OFF;
}

static void
_player_turn(Game *game)
{
    game->next = 0;
    game->mode = REPEATING;
    edje_object_signal_emit(game->base, "repeating", "");
}

static Eina_Bool
_play_next(void *data)
{
    Game *game = data;

    switch (game->seq[game->next])
    {
        case 0:
            edje_object_signal_emit(game->base, "hi", "up");
            break;
        case 1:
            edje_object_signal_emit(game->base, "hi", "right");
            break;
        case 2:
            edje_object_signal_emit(game->base, "hi", "down");
            break;
        case 3:
            edje_object_signal_emit(game->base, "hi", "left");
            break;
        default:
            fprintf(stderr, "Wrong number on sequence\n");
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
    edje_object_signal_emit(game->base, "playing", "");
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
        edje_object_signal_emit(game->base, "player_won", "");
        game->mode = OFF;
        return;
    }

    _score_inc(game, 100 * (level - LEVEL_DEFAULT));
    snprintf(buf, sizeof(buf), "%i", level - LEVEL_DEFAULT + 1);
    edje_object_part_text_set(game->base, "level", buf);
    if (level != LEVEL_DEFAULT)
        edje_object_signal_emit(game->base, "next_level", "");
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
    edje_object_signal_emit(game->base, "new_game", "");
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
        fprintf(stderr, "Wrong source of button clicked\n");
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
    Evas_Object *win, *layout, *edje;
    Evas_Coord min_w, min_h;

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

    if (!elm_layout_file_set(layout, PACKAGE_DATA_DIR DF_THEME, "win"))
    {
        fprintf(stderr, "Couldn't find theme for 'win' at %s\n",
                PACKAGE_DATA_DIR DF_THEME);
        evas_object_del(win);
        return EINA_FALSE;
    }

    edje = elm_layout_edje_get(layout);
    edje_object_size_min_get(edje, &min_w, &min_h);
    edje_object_size_min_restricted_calc(edje, &min_w, &min_h, min_w, min_h);
    edje_object_signal_callback_add(edje, "*", "sound", sound_play_cb, NULL);
    edje_object_signal_callback_add(edje, "gamebt,clicked", "*",
            _bt_clicked, game);
    edje_object_signal_callback_add(edje, "clicked", "bt_start",
            _start_new_game_cb, game);
    game->base = edje;
    evas_object_show(layout);

    evas_object_resize(win, min_w, min_h);
    evas_object_size_hint_min_set(win, min_w, min_h);
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

    sound_disabled = EINA_FALSE;
    if (!sound_init())
        return -1;

    elm_theme_extension_add(NULL, PACKAGE_DATA_DIR DF_THEME);
    elm_theme_overlay_add(NULL, PACKAGE_DATA_DIR DF_THEME);

    game.speed = SPEED_DEFAULT;
    game.level = LEVEL_DEFAULT;
    game.mode = OFF;
    game.turn_timer = NULL;
    game.play_timer = NULL;
    game.score = 0;
    game.hi_score = 0;

    if(!_win_new(&game))
    {
        r = -1;
        goto end;
    }

    elm_run();

end:
    elm_shutdown();
    sound_shutdown();
    return r;
}

#endif
ELM_MAIN()

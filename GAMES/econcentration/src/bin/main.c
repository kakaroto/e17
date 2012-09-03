#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <Elementary.h>
#include <Edje.h>
#include <Eina.h>
#include <Etrophy.h>
#include <Evas.h>
#ifndef ELM_LIB_QUICKLAUNCH

#include <stdlib.h>

#define BOARD_INDEX_DEFAULT 1
#define BOARD_SIZE_DEFAULT (BOARD_INDEX_DEFAULT * 2 + 2)
#define BOARD_SIZE_MAX 64
#define FLIP_HIDE 3

#define HELP_STRING "To play this game in a mobile version use econcentration" \
                    " --mobile"

static const char *BOARD_SIZE[] = {"Small", "Normal", "Big"};

typedef struct _Card
{
    int value;
    Evas_Object *flip;
} Card;

typedef struct _Game
{
    int play_time, attempts, found;
    int board_index, board_size, prev_board_size, total_size;
    Card cards[BOARD_SIZE_MAX];
    Card *first_card, *second_card;
    Evas_Object *time_lb, *best_time_lb, *attempts_lb, *found_lb;
    Evas_Object *win, *table, *sp;
    Ecore_Timer *play_timer, *show_timer, *dl_pop;
    Etrophy_Gamescore *gamescore;
    Eina_Bool running:1;
    /* Are we running in a mobile device? */
    Eina_Bool econcentration_mobile:1;
} Game;

static void
_board_size_cb(void *data, Evas_Object *obj, void *event_info __UNUSED__)
{
    Game *game = data;
    char buf[16];
    int score;
    game->board_index = elm_spinner_value_get(obj);
    game->board_size = game->board_index * 2 + 2;
    game->total_size = game->board_size * game->board_size;
    score = etrophy_gamescore_level_low_score_get(
       game->gamescore, BOARD_SIZE[game->board_index - 1]);
    if (score == -1)
      {
         elm_object_text_set(game->best_time_lb, "");
         return;
      }

    snprintf(buf, sizeof(buf), "%i points", score);
    elm_object_text_set(game->best_time_lb, buf);
}

static Eina_Bool
_play_timer_cb(void *data)
{
    Game *game = data;
    char buf[16];
    game->play_time++;
    snprintf(buf, sizeof(buf), "%i s", game->play_time);
    elm_object_text_set(game->time_lb, buf);
    return ECORE_CALLBACK_RENEW;
}

static Eina_Bool
_hide_cards(void *data)
{
    Game *game = data;

    elm_flip_go(game->first_card->flip, random() % 2);
    elm_flip_go(game->second_card->flip, random() % 2);
    game->first_card = NULL;
    game->second_card = NULL;
    game->show_timer = NULL;

    return ECORE_CALLBACK_CANCEL;
}

static void
_won_ok(void *data, Evas_Object *obj, void *event_info __UNUSED__)
{
    evas_object_del(data);
    evas_object_del(obj);
}

static inline int
_score_get(Game *game)
{
   int score;
   score = 60 * game->board_index - game->play_time +
      2 * (game->total_size - game->attempts);
   if (score < 0) score = 0;
   return score;
}

static void
_player_win(Game *game)
{
    Evas_Object *popup, *bt;
    char buf[320];

    ecore_timer_del(game->play_timer);
    game->play_timer = NULL;

    popup = elm_popup_add(game->win);
    elm_object_part_text_set(popup, "title,text", "You won!");

    bt = elm_button_add(popup);
    elm_object_text_set(bt, "Yay!");
    evas_object_smart_callback_add(bt, "clicked", _won_ok, popup);
    elm_object_part_content_set(popup, "button1", bt);

    if (game->econcentration_mobile)
      {
         if (game->play_time >= 0 && game->play_time <= 60)
           snprintf(buf, sizeof(buf),
                    "Wow ! You're very fast.<br>I liked your ninja style.");
         else if (game->play_time > 60 && game->play_time <= 120)
           snprintf(buf, sizeof(buf),
                    "Nice try, you won but try to be faster!");
         else
           snprintf(buf, sizeof(buf),
                    "ZzzzZz. You won.<br>But you can do better.");
      }
    else
      {
         int score = _score_get(game);
         const char *name = "Player";

         if (getenv("USER"))
           name = getenv("USER");

         etrophy_gamescore_level_score_add(game->gamescore,
                                           BOARD_SIZE[game->board_index - 1],
                                           name, score, 0);
         snprintf(buf, sizeof(buf), "%i points",
                  etrophy_gamescore_level_hi_score_get(
                     game->gamescore,
                     BOARD_SIZE[game->board_index - 1]));
         elm_object_text_set(game->best_time_lb, buf);

         snprintf(buf, sizeof(buf),
                  "Congratulations, you solved it!<br>"
                  "You spent %i seconds and made %i attempts.<br>"
                  "Your score was: %i.",
                  game->play_time, game->attempts, score);
      }

    elm_object_text_set(popup, buf);
    evas_object_show(popup);
}

static void
_fl_selected(void *data, Evas_Object *obj, void *event_info __UNUSED__)
{
    Game *game = data;
    char buf[16];
    Card *card;
    if (!game->running) return;
    /* If show timer is set it means 2 cards are flipped */
    if (game->show_timer) return;

    card = evas_object_data_get(obj, "card");
    elm_flip_go(card->flip, random() % 2);

    if (!game->first_card)
    {
        game->first_card = card;
        return;
    }

    if (game->first_card->value != card->value)
    {
        game->attempts++;
        snprintf(buf, sizeof(buf), "%i", game->attempts);
        elm_object_text_set(game->attempts_lb, buf);
        game->second_card = card;
        game->show_timer = ecore_timer_add(FLIP_HIDE, _hide_cards, game);
        return;
    }

    /* first and second cards match */
    game->found++;

    if (!game->econcentration_mobile)
      {
         snprintf(buf, sizeof(buf), "%i", game->found);
         elm_object_text_set(game->found_lb, buf);
      }

    if (game->found == (game->total_size / 2))
        _player_win(game);
    game->first_card = NULL;
}

/* modulo bias - discardable here, but as we c&p a lot, I will leave it */
static int
_random(int n)
{
    int limit = RAND_MAX - RAND_MAX % n;
    int rnd;

    while ((rnd = random()) >= limit) {}
    return rnd % n;
}

/* Fisher–Yates shuffle implementation, hopefully
 * http://en.wikipedia.org/wiki/Fisher-Yates_shuffle
 */
static void
_shuffle(int *array, int n)
{
    int i, j, tmp;

    for (i = n - 1; i > 0; i--)
    {
        j = _random(i + 1);
        tmp = array[j];
        array[j] = array[i];
        array[i] = tmp;
    }
}

static Eina_Bool
_populate_table(void *data)
{
    Game *game = data;
    int values[] = {0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8,
        9, 9, 10, 10, 11, 11, 12, 12, 13, 13, 14, 14, 15, 15, 16, 16, 17, 17,
        18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 23, 24, 24, 25, 25, 26, 26,
        27, 27, 28, 28, 29, 29, 30, 30, 31, 31};
    int i;
    char buf[1024];
    Evas_Object *ic;

    srandom(time(NULL));
    _shuffle(values, game->total_size);

    for (i = 0; i < game->total_size; i++)
    {
        if (!elm_flip_front_visible_get(game->cards[i].flip))
            elm_flip_go(game->cards[i].flip, random() % 2);
        game->cards[i].value = values[i];

        ic = elm_icon_add(game->cards[i].flip);
        elm_object_scale_set(ic, 1);

        if (game->econcentration_mobile)
          snprintf(buf, sizeof(buf), "%s/images/mobile_icon_%02i.png",
                   PACKAGE_DATA_DIR, values[i]);
        else
          snprintf(buf, sizeof(buf), "%s/images/icon_%02i.png",
                   PACKAGE_DATA_DIR, values[i]);

        elm_image_file_set(ic, buf, NULL);
        evas_object_size_hint_weight_set(ic, EVAS_HINT_EXPAND,
                                         EVAS_HINT_EXPAND);
        evas_object_size_hint_align_set(ic, 0.5, 0.5);
        elm_object_part_content_set(game->cards[i].flip, "back", ic);
        evas_object_show(ic);
    }
    game->play_timer = ecore_timer_add(1, _play_timer_cb, game);
    game->running = EINA_TRUE;
    game->dl_pop = NULL;
    return ECORE_CALLBACK_CANCEL;
}

static void
_unflip_table(Game *game)
{
    int i;
    for (i = 0; i < game->total_size; i++)
    {
        if (!elm_flip_front_visible_get(game->cards[i].flip))
            elm_flip_go(game->cards[i].flip, random() % 2);
    }
    if (game->dl_pop)
        ecore_timer_del(game->dl_pop);
    game->dl_pop = ecore_timer_add(1, _populate_table, game);
}

static void
_prepopulate_table(Game *game)
{
    Evas_Object *fl, *bt, *table;
    int i;
    char buf[1024];
    Evas_Object *ic;

    table = game->table;

    if (game->econcentration_mobile)
      snprintf(buf, sizeof(buf), "%s/images/cards.png",
               PACKAGE_DATA_DIR);

    for (i = 0; i < game->total_size; i++)
    {
        fl = elm_flip_add(table);
        evas_object_size_hint_align_set(fl, EVAS_HINT_FILL, EVAS_HINT_FILL);
        evas_object_size_hint_weight_set(fl, EVAS_HINT_EXPAND,
                                         EVAS_HINT_EXPAND);
        elm_table_pack(table, fl, i % game->board_size, i / game->board_size,
            1, 1);
        evas_object_show(fl);
        game->cards[i].flip = fl;

        bt = elm_button_add(table);
        evas_object_size_hint_align_set(bt, EVAS_HINT_FILL, EVAS_HINT_FILL);
        evas_object_size_hint_weight_set(bt, EVAS_HINT_EXPAND,
                                         EVAS_HINT_EXPAND);
        elm_object_part_content_set(fl, "front", bt);
        evas_object_show(bt);

        if (game->econcentration_mobile)
          {
             ic = elm_icon_add(bt);
             elm_object_scale_set(ic, 1);
             elm_image_file_set(ic, buf, NULL);
             evas_object_size_hint_weight_set(ic, EVAS_HINT_EXPAND,
                                              EVAS_HINT_EXPAND);
             evas_object_size_hint_align_set(ic, EVAS_HINT_FILL,
                                             EVAS_HINT_FILL);
             elm_object_part_content_set(bt, NULL, ic);
             evas_object_show(ic);
          }

        evas_object_data_set(bt, "card", &(game->cards[i]));
        evas_object_smart_callback_add(bt, "clicked", _fl_selected, game);
    }
}

static void
_popup_del_cb(void *data, Evas *e __UNUSED__, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   Evas_Object *eobj = data;
   evas_object_del(eobj);
}

static void
_scores_hide_cb(void *data, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   Evas_Object *popup = data;
   evas_object_del(popup);
}

static void
_scores_show_cb(void *data, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
    Evas_Object *popup, *bt, *leaderboard;
    Game *game = data;

    popup = elm_popup_add(game->win);
    elm_object_part_text_set(popup, "title,text", "Leaderboard");

    bt = elm_button_add(popup);
    elm_object_text_set(bt, "OK");
    elm_object_part_content_set(popup, "button1", bt);

    leaderboard = etrophy_score_layout_add(popup, game->gamescore);
    elm_object_content_set(popup, leaderboard);
    evas_object_smart_callback_add(bt, "clicked", _scores_hide_cb, popup);

    evas_object_event_callback_add(popup, EVAS_CALLBACK_DEL, _popup_del_cb, bt);
    evas_object_event_callback_add(popup, EVAS_CALLBACK_DEL, _popup_del_cb,
                                   leaderboard);
    evas_object_show(popup);
}

static void
_start_game_cb(void *data, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
    Game *game = data;

    game->running = EINA_FALSE;
    game->play_time = 0;
    game->attempts = 0;
    game->found = 0;
    game->first_card = NULL;
    game->second_card = NULL;

    elm_object_text_set(game->time_lb, "0 s");
    elm_object_text_set(game->attempts_lb, "0");
    if (!game->econcentration_mobile)
      elm_object_text_set(game->found_lb, "0");

    if (game->play_timer)
        ecore_timer_del(game->play_timer);
    game->play_timer = NULL;
    if (game->show_timer)
        ecore_timer_del(game->show_timer);
    game->show_timer = NULL;

    if (game->prev_board_size != game->board_size)
    {
        elm_table_clear(game->table, EINA_TRUE);
        _prepopulate_table(game);
    }

    game->prev_board_size = game->board_size;
    _unflip_table(game);
}

static void
_win_delete_request(void *data, Evas_Object *obj, void *event_info __UNUSED__)
{
    Game *game = data;
    if (game->play_timer)
        ecore_timer_del(game->play_timer);
    if (game->show_timer)
        ecore_timer_del(game->show_timer);
    if (game->dl_pop)
        ecore_timer_del(game->dl_pop);
    evas_object_del(obj);
    elm_exit();
}

static Eina_Bool
_create_window_mobile(Game *game)
{
    Evas_Object *win, *bg, *bx, *bxctl, *table, *bt, *lb, *boxLabels;

    game->win = win = elm_win_add(NULL, PACKAGE_NAME, ELM_WIN_BASIC);
    if (!win) return EINA_FALSE;
    evas_object_smart_callback_add(win, "delete,request",
                                   _win_delete_request, game);

    elm_win_fullscreen_set(win, EINA_TRUE);
    evas_object_move(win, 0, 0);

    bg = elm_bg_add(win);
    elm_win_resize_object_add(win, bg);
    evas_object_size_hint_weight_set(bg, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    evas_object_show(bg);

    bx = elm_box_add(win);
    evas_object_size_hint_weight_set(bx, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    elm_win_resize_object_add(win, bx);
    elm_box_padding_set(bx, 10, 10);
    evas_object_show(bx);

    game->table = table = elm_table_add(win);
    elm_table_padding_set(table, 2, 2);
    _prepopulate_table(game);
    evas_object_size_hint_weight_set(table, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    evas_object_size_hint_align_set(table, EVAS_HINT_FILL, EVAS_HINT_FILL);
    evas_object_show(table);
    elm_box_pack_end(bx, table);

    bxctl = elm_box_add(win);
    evas_object_size_hint_align_set(bxctl, EVAS_HINT_FILL, 0);
    evas_object_show(bxctl);
    elm_box_pack_start(bx, bxctl);
    elm_box_horizontal_set(bxctl, EINA_TRUE);

    bt = elm_button_add(win);
    elm_object_text_set(bt, "Start");
    evas_object_smart_callback_add(bt, "clicked", _start_game_cb, game);
    evas_object_size_hint_align_set(bt, EVAS_HINT_FILL, 0);
    evas_object_size_hint_min_set(bt, 180, 88);
    evas_object_show(bt);
    elm_box_pack_end(bxctl, bt);

    boxLabels = elm_box_add(win);
    evas_object_size_hint_weight_set(boxLabels,
                                     EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    evas_object_size_hint_align_set(boxLabels, EVAS_HINT_FILL, EVAS_HINT_FILL);
    evas_object_show(boxLabels);
    elm_box_pack_end(bxctl, boxLabels);
    elm_box_horizontal_set(boxLabels, EINA_TRUE);

    game->attempts_lb = lb = elm_label_add(win);
    elm_object_text_set(lb, "0");
    evas_object_size_hint_weight_set(lb, 0.0, 0.0);
    evas_object_size_hint_align_set(lb, 1, 0.5);
    evas_object_color_set(lb, 255, 255, 255, 255);
    elm_box_pack_end(boxLabels, lb);
    evas_object_show(lb);

    lb = elm_label_add(win);
    elm_object_text_set(lb, " attempts in ");
    evas_object_size_hint_weight_set(lb, 0.0, 0.0);
    evas_object_size_hint_align_set(lb, 1, 0.5);
    evas_object_color_set(lb, 102, 102, 102, 255);
    elm_box_pack_end(boxLabels, lb);
    evas_object_show(lb);

    game->time_lb = lb = elm_label_add(win);
    elm_object_text_set(lb, "0 s");
    evas_object_size_hint_weight_set(lb, 0.0, 0.0);
    evas_object_size_hint_align_set(lb, 1, 0.5);
    evas_object_color_set(lb, 255, 255, 255, 255);
    elm_box_pack_end(boxLabels, lb);
    evas_object_show(lb);

    bt = elm_button_add(win);
    elm_object_text_set(bt, "Quit");
    evas_object_smart_callback_add(bt, "clicked", _win_delete_request, game);
    evas_object_size_hint_align_set(bt, EVAS_HINT_FILL, EVAS_HINT_FILL);
    evas_object_size_hint_min_set(bt, 180, 88);
    evas_object_show(bt);
    elm_box_pack_end(bxctl, bt);

    elm_win_title_set(win, "Econcentration");
    evas_object_show(win);

    return EINA_TRUE;
}

static Eina_Bool
_create_window_desktop(Game *game)
{
    Evas_Object *win, *bg, *bx, *bxctl, *table, *bt, *sp, *fr, *lb;
    char buf[16];

    game->win = win = elm_win_add(NULL, PACKAGE_NAME, ELM_WIN_BASIC);
    if (!win) return EINA_FALSE;
    evas_object_smart_callback_add(win, "delete,request",
            _win_delete_request, game);

    bg = elm_bg_add(win);
    elm_win_resize_object_add(win, bg);
    evas_object_size_hint_weight_set(bg, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    evas_object_show(bg);

    bx = elm_box_add(win);
    elm_box_horizontal_set(bx, EINA_TRUE);
    evas_object_size_hint_weight_set(bx, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    elm_win_resize_object_add(win, bx);
    evas_object_show(bx);

    game->table = table = elm_table_add(win);
    elm_table_padding_set(table, 2, 2);
    _prepopulate_table(game);
    evas_object_size_hint_weight_set(table, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    evas_object_size_hint_align_set(table, EVAS_HINT_FILL, EVAS_HINT_FILL);
    evas_object_show(table);
    elm_box_pack_end(bx, table);

    bxctl = elm_box_add(win);
    evas_object_size_hint_align_set(bxctl, EVAS_HINT_FILL, 0);
    evas_object_show(bxctl);
    elm_box_pack_end(bx, bxctl);

    bt = elm_button_add(win);
    elm_object_text_set(bt, "New game");
    evas_object_smart_callback_add(bt, "clicked", _start_game_cb, game);
    evas_object_size_hint_align_set(bt, EVAS_HINT_FILL, 0);
    evas_object_show(bt);
    elm_box_pack_end(bxctl, bt);

    bt = elm_button_add(win);
    elm_object_text_set(bt, "Leaderboard");
    evas_object_smart_callback_add(bt, "clicked", _scores_show_cb, game);
    evas_object_size_hint_align_set(bt, EVAS_HINT_FILL, 0);
    evas_object_show(bt);
    elm_box_pack_end(bxctl, bt);

    bt = elm_button_add(win);
    elm_object_text_set(bt, "Quit");
    evas_object_smart_callback_add(bt, "clicked", _win_delete_request, game);
    evas_object_size_hint_align_set(bt, EVAS_HINT_FILL, 0);
    evas_object_show(bt);
    elm_box_pack_end(bxctl, bt);

    fr = elm_frame_add(win);
    evas_object_size_hint_weight_set(fr, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    evas_object_size_hint_align_set(fr, EVAS_HINT_FILL, EVAS_HINT_FILL);
    elm_object_text_set(fr, "Board size:");
    elm_box_pack_end(bxctl, fr);
    evas_object_show(fr);

    sp = elm_spinner_add(win);
    elm_spinner_min_max_set(sp, 1, 3);
    elm_spinner_step_set(sp, 1.0);
    elm_spinner_editable_set(sp, EINA_FALSE);
    elm_spinner_special_value_add(sp, 1, BOARD_SIZE[0]);
    elm_spinner_special_value_add(sp, 2, BOARD_SIZE[1]);
    elm_spinner_special_value_add(sp, 3, BOARD_SIZE[2]);
    evas_object_smart_callback_add(sp, "changed", _board_size_cb, game);
    evas_object_size_hint_align_set(sp, EVAS_HINT_FILL, 0);
    elm_object_content_set(fr, sp);
    evas_object_show(sp);
    game->sp = sp;

    fr = elm_frame_add(win);
    evas_object_size_hint_weight_set(fr, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    evas_object_size_hint_align_set(fr, EVAS_HINT_FILL, EVAS_HINT_FILL);
    elm_object_text_set(fr, "Time:");
    elm_box_pack_end(bxctl, fr);
    evas_object_show(fr);

    game->time_lb = lb = elm_label_add(win);
    elm_object_text_set(lb, "0 s");
    evas_object_size_hint_weight_set(lb, 0.0, 0.0);
    evas_object_size_hint_align_set(lb, 1, 0.5);
    elm_object_content_set(fr, lb);
    evas_object_show(lb);

    fr = elm_frame_add(win);
    evas_object_size_hint_weight_set(fr, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    evas_object_size_hint_align_set(fr, EVAS_HINT_FILL, EVAS_HINT_FILL);
    elm_object_text_set(fr, "Attempts:");
    elm_box_pack_end(bxctl, fr);
    evas_object_show(fr);

    game->attempts_lb = lb = elm_label_add(win);
    elm_object_text_set(lb, "0");
    evas_object_size_hint_weight_set(lb, 0.0, 0.0);
    evas_object_size_hint_align_set(lb, 1, 0.5);
    elm_object_content_set(fr, lb);
    evas_object_show(lb);

    fr = elm_frame_add(win);
    evas_object_size_hint_weight_set(fr, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    evas_object_size_hint_align_set(fr, EVAS_HINT_FILL, EVAS_HINT_FILL);
    elm_object_text_set(fr, "Found:");
    elm_box_pack_end(bxctl, fr);
    evas_object_show(fr);

    game->found_lb = lb = elm_label_add(win);
    elm_object_text_set(lb, "0");
    evas_object_size_hint_weight_set(lb, 0.0, 0.0);
    evas_object_size_hint_align_set(lb, 1, 0.5);
    elm_object_content_set(fr, lb);
    evas_object_show(lb);

    fr = elm_frame_add(win);
    evas_object_size_hint_weight_set(fr, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    evas_object_size_hint_align_set(fr, EVAS_HINT_FILL, EVAS_HINT_FILL);
    elm_object_text_set(fr, "Hi-Score:");
    elm_box_pack_end(bxctl, fr);
    evas_object_show(fr);

    lb = elm_label_add(win);
    snprintf(buf, sizeof(buf), "%i points",
            etrophy_gamescore_level_hi_score_get(game->gamescore,
                BOARD_SIZE[0]));
    elm_object_text_set(lb, buf);
    evas_object_size_hint_weight_set(lb, 0.0, 0.0);
    evas_object_size_hint_align_set(lb, 1, 0.5);
    elm_object_content_set(fr, lb);
    evas_object_show(lb);
    game->best_time_lb = lb;

    evas_object_resize(win, 480, 320);
    evas_object_size_hint_min_set(win, 480, 320);
    elm_win_title_set(win, "Econcentration");
    evas_object_show(win);

    return EINA_TRUE;
}

static Eina_Bool
_win_new(Game *game)
{
    if (game->econcentration_mobile)
      return _create_window_mobile(game);

    return _create_window_desktop(game);
}

void
checkArguments(char **argv, Game *game)
{
    if (!strcmp(argv[1], "--mobile"))
      {
         game->econcentration_mobile = EINA_TRUE;
         return;
      }

    fprintf(stdout, "%s\n", HELP_STRING);
    exit(0);
}

EAPI int
elm_main(int argc, char **argv)
{
    Game game;
    int r = 0;

    game.board_index = BOARD_INDEX_DEFAULT;
    game.board_size = BOARD_SIZE_DEFAULT;
    game.total_size = BOARD_SIZE_DEFAULT * BOARD_SIZE_DEFAULT;
    game.prev_board_size = BOARD_SIZE_DEFAULT;
    game.running = EINA_FALSE;
    game.econcentration_mobile = EINA_FALSE;
    game.play_timer = NULL;
    game.show_timer = NULL;
    game.dl_pop = NULL;

    if (argc > 1)
      checkArguments(argv, &game);

    etrophy_init();
    game.gamescore = etrophy_gamescore_load("econcentration");
    if (!game.gamescore)
        game.gamescore = etrophy_gamescore_new("econcentration");
    if (!game.gamescore)
    {
        r = -1;
        goto no_score;
    }

    if(!_win_new(&game))
    {
        r = -1;
        goto end;
    }

    elm_run();

    etrophy_gamescore_save(game.gamescore, NULL);

end:
    etrophy_shutdown();
no_score:
    elm_shutdown();
    return r;
}

#endif
ELM_MAIN()

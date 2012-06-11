#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <Elementary.h>
#include <Edje.h>
#include <Evas.h>
#ifndef ELM_LIB_QUICKLAUNCH

#include <stdlib.h>

#define BOARD_SIZE_DEFAULT 4
#define BOARD_SIZE_MAX 64
#define FLIP_HIDE 3

typedef struct _Card
{
    int value;
    Evas_Object *flip;
} Card;

typedef struct _Game
{
    int play_time, attempts, found, board_size, prev_board_size, total_size;
    Card cards[BOARD_SIZE_MAX];
    Card *first_card, *second_card;
    Evas_Object *time_lb, *attempts_lb, *won_lb, *popup, *table;
    Ecore_Timer *play_timer, *show_timer, *dl_pop;
    Eina_Bool running:1;
} Game;

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
_won_ok(void *data, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
    Game *game = data;
    evas_object_hide(game->popup);
}

static void
_player_win(Game *game)
{
    char buf[320];

    ecore_timer_del(game->play_timer);
    game->play_timer = NULL;

    evas_object_show(game->popup);

    if (game->play_time >= 0 && game->play_time <= 60)
      snprintf(buf, sizeof(buf),
               "Wow ! you're very fast.<br> I liked your ninja style.<br>");
    else if (game->play_time > 60 && game->play_time <= 120)
      snprintf(buf, sizeof(buf),
               "Nice try, you won but try to be faster!<br>");
    else
      snprintf(buf, sizeof(buf),
               "ZzzzZz. You won.<br>But you're slow like a turtle.<br>");

    elm_object_text_set(game->won_lb, buf);
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
    int values[] = {1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8};
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
        snprintf(buf, sizeof(buf), "%s/images/icon_%02i.png",
                PACKAGE_DATA_DIR, values[i]);
        elm_icon_file_set(ic, buf, NULL);
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
    char buf[1024];
    Evas_Object *ic;
    int i;

    table = game->table;

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


        ic = elm_icon_add(bt);
        elm_object_scale_set(ic, 1);
        elm_icon_file_set(ic, buf, NULL);
        evas_object_size_hint_weight_set(ic, EVAS_HINT_EXPAND,
                                         EVAS_HINT_EXPAND);
        evas_object_size_hint_align_set(ic, EVAS_HINT_FILL, EVAS_HINT_FILL);
        elm_object_part_content_set(bt, NULL, ic);
        evas_object_show(ic);

        evas_object_data_set(bt, "card", &(game->cards[i]));
        evas_object_smart_callback_add(bt, "clicked", _fl_selected, game);
    }
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
_win_new(Game *game)
{
    Evas_Object *win, *popup, *bg, *bx, *bxctl, *table, *bt, *sp, *fr, *lb,
                *boxLabels;

    win = elm_win_add(NULL, PACKAGE_NAME, ELM_WIN_BASIC);
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
    evas_object_size_hint_weight_set(boxLabels, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
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

    game->popup = popup = elm_popup_add(win);
    elm_object_part_text_set(popup, "title,text", "You won!");

    bx = elm_box_add(popup);
    evas_object_show(bx);
    evas_object_size_hint_align_set(bx, EVAS_HINT_FILL, EVAS_HINT_FILL);

    game->won_lb = lb = elm_label_add(popup);
    evas_object_size_hint_weight_set(lb, 0.0, 0.0);
    evas_object_size_hint_align_set(lb, 0.5, 0.5);
    evas_object_color_set(lb, 255, 255, 255, 255);
    evas_object_show(lb);
    elm_box_pack_end(bx, lb);

    bt = elm_button_add(popup);
    elm_object_text_set(bt, "Yay!");
    evas_object_smart_callback_add(bt, "clicked", _won_ok, game);
    evas_object_size_hint_weight_set(fr, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    evas_object_size_hint_align_set(bt, EVAS_HINT_FILL, EVAS_HINT_FILL);
    evas_object_show(bt);
    elm_box_pack_end(bx, bt);

    elm_object_content_set(popup, bx);

    elm_win_title_set(win, "Econcentration");
    evas_object_show(win);

    return EINA_TRUE;
}

EAPI int
elm_main(int argc __UNUSED__, char **argv __UNUSED__)
{
    Game game;
    int r = 0;

    game.board_size = BOARD_SIZE_DEFAULT;
    game.total_size = BOARD_SIZE_DEFAULT * BOARD_SIZE_DEFAULT;
    game.prev_board_size = BOARD_SIZE_DEFAULT;
    game.running = EINA_FALSE;
    game.play_timer = NULL;
    game.show_timer = NULL;
    game.dl_pop = NULL;

    if(!_win_new(&game))
    {
        r = -1;
        goto end;
    }

    elm_run();

end:
    elm_shutdown();
    return r;
}

#endif
ELM_MAIN()

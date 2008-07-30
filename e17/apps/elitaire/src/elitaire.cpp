/* vim: set sw=4 ts=4 sts=4 expandtab: */
#include "Rules.h"
#include "Elitaire.h"
#include "elitaire_private.h"

/* ***************************************************************************
 * Declarations
 * ***************************************************************************/

/* Callbacks */
static void _elitaire_move_cb           (playingCard * pcard, bool single);
static void _elitaire_flip_cb           (playingCard * pcard);
static void _elitaire_wait_cb           (playingCard * pcard, int t);
static void _elitaire_move_no_ani_cb    (playingCard * pcard, bool single);
static void _elitaire_flip_no_ani_cb    (playingCard * pcard);
static void _elitaire_stack_mouse_up_cb (void * data, Evas * e, Evas_Object * obj,
                                  void * event_info);

/* smart object handlers */
static Evas_Smart * _elitaire_smart_get();

static void _elitaire_object_add        (Evas_Object * o);
static void _elitaire_object_del        (Evas_Object * o);
static void _elitaire_object_move       (Evas_Object * o, Evas_Coord x, Evas_Coord y);
static void _elitaire_object_resize     (Evas_Object * o, Evas_Coord w, Evas_Coord h);
static void _elitaire_object_show       (Evas_Object * o);
static void _elitaire_object_hide       (Evas_Object * o);
static void _elitaire_object_color_set  (Evas_Object * o, int r, int g, int b, int a);
static void _elitaire_object_clip_set   (Evas_Object * o, Evas_Object * clip);
static void _elitaire_object_clip_unset (Evas_Object * o);

/* keep a global copy of this, so it only has to be created once */
static Evas_Smart * smart;

/* what kind of games can we play */
const char * elitaire_available_games[] = {
    "klondike",
    "klondike 3",
    "thoughtful",
    "vegas",
    "freecell",
    "Baker's Dozen",
    "Golf",
    "Spider 1",
    "Spider 2",
    "Spider 4",
    ""                          /* this is the end */
};

/* ***************************************************************************
 * external available functions 
 * ***************************************************************************/
const char ** elitaire_available_games_get()
{
    return elitaire_available_games;
}

Evas_Object * elitaire_object_new(Evas * evas)
{
    Evas_Object * elitaire;
    Elitaire * eli;

    elitaire = evas_object_smart_add(evas, _elitaire_smart_get());

    eli = (Elitaire *) evas_object_smart_data_get(elitaire);
    if (!eli) return NULL;

    /* init variables */
    eli->evas = evas;
    eli->file = NULL;
    eli->velocity = 1000;
    eli->spacing = 0.1;
    eli->margin_h = 0.4;
    eli->margin_v = 0.15;
    eli->x = 0;
    eli->y = 0;
    eli->w = 0;
    eli->h = 0;
    eli->offset.t = 0;
    eli->offset.l = 0;
    eli->offset.r = 0;
    eli->offset.b = 0;
    eli->frame_rate = 80;
    eli->deal = false;
    eli->shadow = false;
    eli->animate = true;
    eli->num_card_jobs = 0;
    eli->elitaire = elitaire;

    /* jobs */
    eli->jobs = elitaire_job_list_new(NULL, NULL);
    return elitaire;
}

Evas_Bool elitaire_object_game_set(Evas_Object * elitaire, const char * game)
{
    Elitaire * eli;
    
    eli = (Elitaire *) evas_object_smart_data_get(elitaire);
    if (!eli || eli->game) return false;

   /* choose game */
    if (!strcmp(game, "klondike")) {
        eli->game = new klondike;
    }
    else if (!strcmp(game, "klondike 3")) {
        eli->game = new klondike3;
    }
    else if (!strcmp(game, "freecell")) {
        eli->game = new freeCell;
    }
    else if (!strcmp(game, "thoughtful")) {
        eli->game = new klondike(true);
    }
    else if (!strcmp(game, "vegas")) {
        eli->game = new vegas;
    }
    else if (!strcmp(game, "Baker's Dozen")) {
        eli->game = new bakersDozen;
    }
    else if (!strcmp(game, "Golf")) {
        eli->game = new golf;
    }
    else if (!strcmp(game, "Spider 1")) {
        eli->game = new spider(1);
    }
    else if (!strcmp(game, "Spider 2")) {
        eli->game = new spider(2);
    }
    else if (!strcmp(game, "Spider 4")) {
        eli->game = new spider(4);
    }
    else {
        fprintf(stderr, _("Elitaire Error: unknown game %s\n"), game);
        return false;
    }

    /* calculate  the longest line */
    eli->max_cols = elitaire_cols_max_calc(eli);
    eli->num_rows = elitaire_num_of_rows_calc(eli);

    /* how many stacks? */
    eli->num_stacks = eli->game->getNumStacks();

    /* add callbacks */
    if (eli->animate) {
        eli->game->addMoveCallback(_elitaire_move_cb);
        eli->game->addFlipCallback(_elitaire_flip_cb);
    }
    else {
        eli->game->addMoveCallback(_elitaire_move_no_ani_cb);
        eli->game->addFlipCallback(_elitaire_flip_no_ani_cb);
    }
    eli->game->addWaitCallback(_elitaire_wait_cb);  //waiting is always the same

    /* init clip */
    eli->clip = evas_object_rectangle_add(eli->evas);

    return true;
}

int elitaire_object_file_set(Evas_Object * elitaire, const char * file)
{
    int i, number;
    Evas_Object * card;
    Elitaire * eli;
    vector<stackLook>::const_iterator iord, iend;
    Evas_Bool change_file;
    card_iterator it, it_end;
    int num_decks;

    eli = (Elitaire *) evas_object_smart_data_get(elitaire);
    if (!eli) return 0;

    /* for the stack images */
    iord = eli->game->getStackLooksBegin();
    iend = eli->game->getStackLooksEnd();

    if (eli->file) change_file = true;
    else change_file = false;

    if (eli->file) {
        free(eli->file);
        eli->file = NULL;
    }

    if (!file) return 0;
    else eli->file = strdup(file);

    /* get the maximal and minimal size of the cards via edje */
    /* I expect that every card in the game has the same size */
    card = edje_object_add(eli->evas);
    if (!edje_object_file_set(card, eli->file, "elitaire/card/00")) return 0;
        
    edje_object_size_max_get(card, &eli->card_max_w, &eli->card_max_h);
    edje_object_size_min_get(card, &eli->card_min_w, &eli->card_min_h);
    evas_object_del(card);

    /*connect the playingCards with elitaire_cards and set the callbacks */
    if (change_file) elitaire_cards_del(eli);

    num_decks = eli->game->getNumDecks();

    for (i = 0; i < num_decks; i++) {
        it = eli->game->getDeckBegin(i);
        it_end = eli->game->getDeckEnd(i);

        while (it != it_end) {
            card = elitaire_card_new(eli, *it);
            evas_object_smart_member_add(card, elitaire);
            evas_object_clip_set(card, eli->clip);
            evas_object_show(card);
            (*it)->data = (void *) card;
            elitaire_card_callback_add(card, EVAS_CALLBACK_MOUSE_DOWN,
                                       elitaire_mouse_down_cb);
            elitaire_card_callback_add(card, EVAS_CALLBACK_MOUSE_UP,
                                       elitaire_mouse_up_cb);
            //evas_object_layer_set(card, 1);
            if (evas_alloc_error() != EVAS_ALLOC_ERROR_NONE) {
                fprintf(stderr,
                        _("Elitaire Error: Callback registering failed!\n"));
                return 0;
            }
            it++;
        }
    }

    /* give the stacks an pic */
    if (change_file)
        elitaire_stacks_del(eli);

    eli->stacks =
        (Evas_Object **) calloc(eli->num_stacks, sizeof(Elitaire*));
    number = 0;

    while (iord != iend) {
        switch (*iord) {
        case STACK:
        case LAST:
            eli->stacks[number] = edje_object_add(eli->evas);
            evas_object_smart_member_add(eli->stacks[number], elitaire);
            edje_object_file_set(eli->stacks[number], eli->file, "elitaire/stack/last");
            evas_object_event_callback_add(eli->stacks[number],
                                           EVAS_CALLBACK_MOUSE_UP,
                                           _elitaire_stack_mouse_up_cb, eli);
            evas_object_show(eli->stacks[number]);
            evas_object_lower(eli->stacks[number]);
            number++;
            break;

        case ALL:
            eli->stacks[number] = edje_object_add(eli->evas);
            evas_object_smart_member_add(eli->stacks[number], elitaire);
            edje_object_file_set(eli->stacks[number], eli->file, "elitaire/stack/all");
            evas_object_event_callback_add(eli->stacks[number],
                                           EVAS_CALLBACK_MOUSE_UP,
                                           _elitaire_stack_mouse_up_cb, eli);
            evas_object_show(eli->stacks[number]);
            evas_object_lower(eli->stacks[number]);
            number++;
            break;
        
        default:
            break;
        }
        iord++;
    }

    if (change_file) {
        elitaire_force_resize(eli);
        elitaire_reinit(eli);
    }
    return 1;
}

void elitaire_object_pause(Evas_Object * elitaire)
{
    int i;
    Evas_Object * card;
    Elitaire * eli;
    card_iterator it, it_end;
    int num_decks;

    eli = (Elitaire *) evas_object_smart_data_get(elitaire);
    if (!eli) return;

    num_decks = eli->game->getNumDecks();

    for (i = 0; i < num_decks; i++) {
        it = eli->game->getDeckBegin(i);
        it_end = eli->game->getDeckEnd(i);

        while (it != it_end) {
            card = (Evas_Object *) (*it)->data;

            elitaire_card_callback_del(card, EVAS_CALLBACK_MOUSE_DOWN,
                                       elitaire_mouse_down_cb);
            elitaire_card_callback_del(card, EVAS_CALLBACK_MOUSE_UP,
                                       elitaire_mouse_up_cb);
            it++;
        }
    }

    for (i = 0; i < eli->num_stacks; i++) {
        evas_object_event_callback_del(eli->stacks[i],
                                       EVAS_CALLBACK_MOUSE_UP,
                                       _elitaire_stack_mouse_up_cb);
    }
}

/*
 * Highlight the cards that are moveable
 */
void elitaire_object_hints_show(Evas_Object * elitaire)
{
    Elitaire * eli;
    card_iterator it, it_end;
    vector<playingCard*> * hints;

    eli = (Elitaire *) evas_object_smart_data_get(elitaire);
    if (!eli) return;

    hints = eli->game->getHints();
    
    it = hints->begin();
    it_end = hints->end();

    while (it != it_end) {
        Evas_Object * card;
        
        card = (Evas_Object *) (*it)->data;
        elitaire_card_hint_highlight_show(card);
        it++;
    }

    eli->hints_on = true;
    delete hints;
}

/*
 * Unhighlight the cards that are moveable
 */
void elitaire_object_hints_hide(Evas_Object * elitaire)
{
    Elitaire * eli;
    
    eli = (Elitaire *) evas_object_smart_data_get(elitaire);
    elitaire_hints_hide(eli);
}

/*
 * return if the highlights are visible
 */
Evas_Bool elitaire_object_hints_visible(Evas_Object * elitaire)
{
    Elitaire * eli;

    eli = (Elitaire *) evas_object_smart_data_get(elitaire);
    if (!eli) return false;
    
    return eli->hints_on;
}

void elitaire_object_size_min_get(Evas_Object * elitaire, Evas_Coord * minw,
                           Evas_Coord * minh)
{
    Elitaire * eli;

    eli = (Elitaire *) evas_object_smart_data_get(elitaire);

    *minw = (Evas_Coord) (eli->card_min_w * (((float) eli->max_cols)
                                               + eli->spacing * ((float) (eli->max_cols - 1.0)) 
                                               + (float) (2.0 * eli->margin_h)))
                          + eli->offset.l + eli->offset.r;
    *minh = (Evas_Coord) ((float) eli->num_rows * eli->card_min_h
                          + (float) (eli->num_rows -
                                     1.0) * eli->card_min_w * eli->spacing +
                          2.0 * eli->margin_h * eli->card_min_h)
                + eli->offset.t + eli->offset.b;

}

void elitaire_object_size_max_get(Evas_Object * elitaire, Evas_Coord * maxw,
                           Evas_Coord * maxh)
{
    Elitaire * eli;

    eli = (Elitaire *) evas_object_smart_data_get(elitaire);

    if (maxw)
        *maxw = (Evas_Coord) (eli->card_max_w * (((float) eli->max_cols)
                                                 +
                                                 eli->spacing *
                                                 ((float)
                                                  (eli->max_cols - 1.0)) +
                                                 (float) (2.0 *
                                                          eli->margin_h)));
    if (maxh)
        *maxh = 2 * (Evas_Coord) ((float) eli->num_rows * eli->card_max_h
                                  + (float) (eli->num_rows -
                                             1.0) * eli->card_max_w *
                                  eli->spacing +
                                  2.0 * eli->margin_h * eli->card_max_h);

}

int elitaire_object_file_get(Evas_Object * elitaire, char ** file)
{
    Elitaire * eli;

    eli = (Elitaire *) evas_object_smart_data_get(elitaire);

    if (eli) {
        file = &eli->file;
        return 1;
    }
    return 0;
}

int elitaire_object_velocity_get(Evas_Object * elitaire)
{
    Elitaire * eli;

    eli = (Elitaire *) evas_object_smart_data_get(elitaire);

    if (eli) return eli->velocity;
    return 0;
}

void elitaire_object_velocity_set(Evas_Object * elitaire, int v)
{
    Elitaire * eli;

    eli = (Elitaire *) evas_object_smart_data_get(elitaire);

    if (eli) {
        eli->velocity = v;
    }

}

void elitaire_object_frame_rate_set(Evas_Object * elitaire, int fr)
{
    Elitaire * eli;

    eli = (Elitaire *) evas_object_smart_data_get(elitaire);

    if (eli) {
        eli->frame_rate = fr;
        ecore_animator_frametime_set(1.0 / fr);
    }

}

int elitaire_object_frame_rate_get(Evas_Object * elitaire)
{
    Elitaire * eli;

    eli = (Elitaire *) evas_object_smart_data_get(elitaire);

    if (eli) return eli->frame_rate;
        
    return 0;
}

void elitaire_object_animations_set(Evas_Object * elitaire, Evas_Bool on)
{
    Elitaire *eli;

    eli = (Elitaire *) evas_object_smart_data_get(elitaire);


    if (eli) {
        if (eli->animate == on) return;
            
        eli->animate = on;
        if (eli->animate) {
            eli->game->addMoveCallback(_elitaire_move_cb);
            eli->game->addFlipCallback(_elitaire_flip_cb);
        }
        else {
            eli->game->addMoveCallback(_elitaire_move_no_ani_cb);
            eli->game->addFlipCallback(_elitaire_flip_no_ani_cb);
        }
    }

}

Evas_Bool elitaire_object_animations_get(Evas_Object * elitaire)
{
    Elitaire * eli;

    eli = (Elitaire *) evas_object_smart_data_get(elitaire);

    if (eli) return eli->animate;
        
    return 0;
}

void elitaire_object_shadows_set(Evas_Object * elitaire, Evas_Bool on)
{
    Elitaire * eli;

    eli = (Elitaire *) evas_object_smart_data_get(elitaire);

    if (eli) {
        eli->shadow = on;
    }
}

Evas_Bool elitaire_object_shadows_get(Evas_Object * elitaire)
{
    Elitaire * eli;

    eli = (Elitaire *) evas_object_smart_data_get(elitaire);

    if (eli) return eli->shadow;
    return 0;
}

void elitaire_object_lazy_mode_set(Evas_Object * elitaire, int lm)
{
    Elitaire * eli;

    eli = (Elitaire *) evas_object_smart_data_get(elitaire);

    if (eli) eli->game->setLazyMode(lm);
}

int elitaire_object_lazy_mode_get(Evas_Object * elitaire)
{
    Elitaire * eli;

    eli = (Elitaire *) evas_object_smart_data_get(elitaire);

    if (eli) return eli->game->getLazyMode();
    return 0;
}

void elitaire_object_deal(Evas_Object * elitaire)
{

    Elitaire * eli;

    eli = (Elitaire *) evas_object_smart_data_get(elitaire);
    if (eli) {
        if (!eli->deal) {
            eli->game->deal();
            eli->deal = true;
        }
    }
}

void elitaire_object_callback_win_add(Evas_Object * elitaire,
                               void (*func) (int, float, pointsType, void *),
                               void *data)
{
    Elitaire * eli;

    eli = (Elitaire *) evas_object_smart_data_get(elitaire);
    if (eli) {
        eli->game->addWinCallback(func, data);
    }
}

void elitaire_object_callback_points_add(Evas_Object * elitaire,
                                  void (*func) (float, pointsType, void *),
                                  void * data)
{
    Elitaire * eli;

    eli = (Elitaire *) evas_object_smart_data_get(elitaire);
    if (eli) {
        eli->game->addPointsCallback(func, data);
    }
}


void elitaire_object_undo(Evas_Object * elitaire)
{
    Elitaire * eli;

    eli = (Elitaire *) evas_object_smart_data_get(elitaire);
    if (eli) {
        elitaire_hints_hide(eli);
        eli->game->undo();
    }
}

void elitaire_object_restart(Evas_Object * elitaire)
{
    Elitaire * eli;

    eli = (Elitaire *) evas_object_smart_data_get(elitaire);
    if (eli) {
        elitaire_hints_hide(eli);
        eli->game->restart();
    }
}

Evas_Bool elitaire_object_giveup(Evas_Object * elitaire)
{
    Elitaire * eli;

    eli = (Elitaire *) evas_object_smart_data_get(elitaire);
    if (eli) {
        elitaire_hints_hide(eli);
        return eli->game->giveUp();
    }
    return false;
}


void elitaire_object_offset_set(Evas_Object * elitaire, Evas_Coord l, Evas_Coord r,
                         Evas_Coord t, Evas_Coord b)
{
    Elitaire * eli;

    eli = (Elitaire *) evas_object_smart_data_get(elitaire);
    if (eli) {
        eli->offset.l = l;
        eli->offset.r = r;
        eli->offset.t = t;
        eli->offset.b = b;
    }
}

void elitaire_object_offset_get(Evas_Object * elitaire, Evas_Coord * l, 
                                Evas_Coord * r, Evas_Coord * t, 
                                Evas_Coord * b)
{
    Elitaire * eli;

    eli = (Elitaire *) evas_object_smart_data_get(elitaire);
    if (eli) {
        if (l) *l = eli->offset.l;
        if (r) *r = eli->offset.r;
        if (t) *t = eli->offset.t;
        if (b) *b = eli->offset.b;
    }
}

Evas_Bool elitaire_object_wait_for_end_of_jobs(Evas_Object * elitaire,
                                        int (*func) (void * data), void * data)
{
    Elitaire * eli;

    eli = (Elitaire *) evas_object_smart_data_get(elitaire);
    if (eli) {
        if (eli->num_card_jobs == 0)
            ecore_timer_add(0.01, func, data);
        else {
            eli->no_job = func;
            eli->no_job_data = data;
            return true;
        }
    }
    return false;
}

/**********
 * callback for the stacks 
 **********/

static void _elitaire_stack_mouse_up_cb(void * data, Evas * e, Evas_Object * obj,
                                 void * event_info)
{

    Evas_Coord x, y;
    int clstack;
    Evas_Event_Mouse_Up * ev = (Evas_Event_Mouse_Up *) event_info;
    Elitaire * eli = (Elitaire *) data;

    if (eli) {
        if (ev->button == 1) {
            evas_pointer_canvas_xy_get(e, &x, &y);

            clstack = elitaire_clicked_stack_get(eli, x, y);

            if (clstack < 0) return;
            eli->game->clickOn(clstack);
        }
    }
}

/*********
 * callbacks for cardGame 
 *********/

static void _elitaire_move_cb(playingCard * pcard, bool single)
{
    Evas_Object * card;

    card = (Evas_Object *) pcard->data;

    if (card) {
        Evas_Coord x, y;

        elitaire_card_xy_get(card, &x, &y);
        
        if (!single) elitaire_card_chain_make(card);

        elitaire_card_slide(card, x, y);
    }
}
static void _elitaire_flip_cb(playingCard * pcard)
{
    Evas_Object * card;

    card = (Evas_Object *) pcard->data;

    if (card) {
        elitaire_card_flip(card);
    }
}
static void _elitaire_wait_cb(playingCard * pcard, int t)
{
    Evas_Object * card;

    card = (Evas_Object *) pcard->data;

    if (card) {
        elitaire_card_wait(card, t);
    }
}
static void _elitaire_move_no_ani_cb(playingCard * pcard, bool single)
{
    Evas_Object * card;

    card = (Evas_Object *) pcard->data;

    if (card) {
        Elitaire * eli;

        eli = elitaire_card_elitaire_get(card);
        elitaire_reinit(eli);
    }
}

static void _elitaire_flip_no_ani_cb(playingCard * pcard)
{
    Evas_Object * card;

    card = (Evas_Object *) pcard->data;

    if (card) {
        elitaire_card_reinit(card);
    }
}


/********
 * smart object handler functions 
 ********/

static Evas_Smart * _elitaire_smart_get()
{
    if (!smart) {
        static const Evas_Smart_Class sc = {
            "Elitaire",
            EVAS_SMART_CLASS_VERSION,
            _elitaire_object_add, 
            _elitaire_object_del,
            _elitaire_object_move,
            _elitaire_object_resize,
            _elitaire_object_show,
            _elitaire_object_hide,
            _elitaire_object_color_set,
            _elitaire_object_clip_set,
            _elitaire_object_clip_unset,
            NULL
        };
        smart = evas_smart_class_new(&sc);
    }

    return smart;
}

static void _elitaire_object_add(Evas_Object * o)
{
    Elitaire * data;

    data = (Elitaire *) calloc(1, sizeof(Elitaire));
    evas_object_smart_data_set(o, data);
}


static void _elitaire_object_del(Evas_Object * o)
{
    Elitaire * eli;

    eli = (Elitaire *) evas_object_smart_data_get(o);
    if (!eli) return;

    elitaire_cards_del(eli);
    evas_object_del(eli->clip);
    elitaire_stacks_del(eli);
    
    if (eli->game) {
        delete eli->game;
    }
    // elitaire_job_list_del(eli->jobs);
    free(eli);
}

static void _elitaire_object_move(Evas_Object * o, Evas_Coord x, Evas_Coord y)
{
    Elitaire * eli;
    Evas_Coord nx, ny;

    eli = (Elitaire *) evas_object_smart_data_get(o);
    if (!eli) return;

    evas_object_move(eli->clip, x, y);

    nx = x + eli->offset.l;
    ny = y + eli->offset.t;

    if (eli->y == ny && eli->x == nx) return;
    eli->x = nx;
    eli->y = ny;

    if (eli->w == 0 || eli->h == 0) return;
    
    if (eli->deal) elitaire_reinit(eli);
    else elitaire_stacks_reinit(eli);
}

static void _elitaire_object_resize(Evas_Object * o, Evas_Coord w, Evas_Coord h)
{
    Elitaire * eli;
    Evas_Coord nw, nh;

    eli = (Elitaire *) evas_object_smart_data_get(o);

    evas_object_resize(eli->clip, w, h);

    nw = w - eli->offset.l - eli->offset.r;
    nh = h - eli->offset.t - eli->offset.b;

    if (eli->w == nw) {
        eli->h = nh;
        return;
    }

    eli->w = nw;
    eli->h = nh;

    elitaire_force_resize(eli);

    if (eli->deal)
        elitaire_reinit(eli);
    else
        elitaire_stacks_reinit(eli);
}

static void _elitaire_object_show(Evas_Object * o)
{
    Elitaire * eli;
    int i;

    eli = (Elitaire *) evas_object_smart_data_get(o);
    evas_object_show(eli->clip);

    if (!eli->deal) {
        Evas_Object * card;
        card_iterator it, it_end;
        int i, num_decks;

        num_decks = eli->game->getNumDecks();

        for (i = 0; i < num_decks; i++) {
            it = eli->game->getDeckBegin(i);
            it_end = eli->game->getDeckEnd(i);

            while (it != it_end) {
                if ((card = (Evas_Object *) (*it++)->data)) {
                    evas_object_move(card, eli->x + eli->w / 2,
                                     eli->y + eli->h + 1);
                }
            }
        }
    }
    for (i = 0; i < eli->num_stacks; i++) {
        evas_object_show(eli->stacks[i]);
    }
}

static void _elitaire_object_hide(Evas_Object * o)
{
    Elitaire * eli;
    int i;

    eli = (Elitaire *) evas_object_smart_data_get(o);

    evas_object_hide(eli->clip);

    for (i = 0; i < eli->num_stacks; i++) {
        evas_object_hide(eli->stacks[i]);
    }
}

static void _elitaire_object_color_set(Evas_Object * o, int r, int g, int b, int a)
{
    Elitaire * data;

    data = (Elitaire *) evas_object_smart_data_get(o);

    evas_object_color_set(data->clip, r, g, b, a);
}

static void _elitaire_object_clip_set(Evas_Object * o, Evas_Object * clip)
{
    Elitaire * data;

    data = (Elitaire *) evas_object_smart_data_get(o);

    evas_object_clip_set(data->clip, clip);
}

static void _elitaire_object_clip_unset(Evas_Object * o)
{
    Elitaire * data;

    data = (Elitaire *) evas_object_smart_data_get(o);

    evas_object_clip_unset(data->clip);
}

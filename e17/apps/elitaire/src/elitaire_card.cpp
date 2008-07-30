/* vim: set sw=4 ts=4 sts=4 expandtab: */
#include "elitaire_card_private.h"
/* ***************************************************************************
 * Declarations								      			            
 * ***************************************************************************/

/* smart object handlers */
static void _elitaire_card_add        (Evas_Object * o);
static void _elitaire_card_del        (Evas_Object * o);
static void _elitaire_card_move       (Evas_Object * o, Evas_Coord x, Evas_Coord y);
static void _elitaire_card_resize     (Evas_Object * o, Evas_Coord w, Evas_Coord h);
static void _elitaire_card_show       (Evas_Object * o);
static void _elitaire_card_hide       (Evas_Object * o);
static void _elitaire_card_color_set  (Evas_Object * o, int r, int g, int b, int a);
static void _elitaire_card_clip_set   (Evas_Object * o, Evas_Object * clip);
static void _elitaire_card_clip_unset (Evas_Object * o);

/* keep a global copy of this, so it only has to be created once */
static Evas_Smart * smart;

/*** smart object handler function ***/
static Evas_Smart * _elitaire_card_smart_get();

/* ****************************************************************************
 * external API 
 * ****************************************************************************/
Evas_Object * elitaire_card_new(Elitaire * eli, playingCard * pcard)
{
    Evas_Object * card;
    Elitaire_Card * data = NULL;
    char buffer[128];

    card = evas_object_smart_add(eli->evas, _elitaire_card_smart_get());
    data = (Elitaire_Card *) evas_object_smart_data_get(card);

    if (data) {
        snprintf(buffer, sizeof(buffer), "elitaire/card/%02d", 
                                                            pcard->getName());
        data->image = edje_object_add(eli->evas);
        evas_object_smart_member_add(data->image, card);
        data->pcard = pcard;
        data->next_card = NULL;
        data->eli = eli;
        data->shadow = NULL;
        data->jobs = elitaire_job_list_new(elitaire_card_job_clean_up, card);
        elitaire_job_del_func_set(data->jobs, elitaire_card_job_del_cb);
        if (data->pcard->isVisible()) {
            if (!edje_object_file_set(data->image, eli->file, buffer))
                fprintf(stderr, _("Elitaire Error: Card %s is missed!\n"),
                        buffer);
        }
        else {
            if (!edje_object_file_set(data->image, eli->file, 
                                                        "elitaire/card/bg"))
                fprintf(stderr, _("Elitaire Error: The bg pic is missed!\n"));
        }
    }

    return card;
}

void elitaire_card_playingCard_set(Evas_Object * card, playingCard * pcard)
{
    Elitaire_Card * data = NULL;

    data = (Elitaire_Card *) evas_object_smart_data_get(card);

    if (data) {
        data->pcard = pcard;
    }
}

playingCard * elitaire_card_playingCard_get(Evas_Object * card)
{
    Elitaire_Card * data = NULL;

    data = (Elitaire_Card *) evas_object_smart_data_get(card);

    if (data) {
        return data->pcard;
    }
    return NULL;
}

Evas_Bool elitaire_card_is_flying(Evas_Object * card)
{
    Elitaire_Card * data = NULL;

    data = (Elitaire_Card *) evas_object_smart_data_get(card);

    if (data) {
        return data->flying || data->waiting;
    }
    return false;
}

void elitaire_card_callback_add(Evas_Object * o, Evas_Callback_Type type,
                                void (*func) (void *data, Evas * e,
                                              Evas_Object * obj,
                                              void *event_info))
{
    Elitaire_Card * data = NULL;

    data = (Elitaire_Card *) evas_object_smart_data_get(o);

    if (data) {
        evas_object_event_callback_add(data->image, type, func, o);
    }
}

void elitaire_card_callback_del(Evas_Object * o, Evas_Callback_Type type,
                                void (*func) (void *data, Evas * e,
                                              Evas_Object * obj,
                                              void *event_info))
{
    Elitaire_Card * data = NULL;

    data = (Elitaire_Card *) evas_object_smart_data_get(o);

    if (data) {
        evas_object_event_callback_del(data->image, type, func);
    }
}

Evas_Bool elitaire_card_moveable(Evas_Object * card)
{
    Elitaire_Card * data = NULL;

    data = (Elitaire_Card *) evas_object_smart_data_get(card);

    if (data) {
        return data->eli->game->moveableCard(data->pcard);
    }
    return 0;
}

void elitaire_card_hint_highlight_show(Evas_Object * card)
{
    Elitaire_Card * data;

    printf("card show hilite");
    data = (Elitaire_Card *) evas_object_smart_data_get(card);
    if (!data) return;
        
    edje_object_signal_emit(data->image, "hint,on", "elitaire");
}

void elitaire_card_hint_highlight_hide(Evas_Object * card)
{
    Elitaire_Card * data;

    data = (Elitaire_Card *) evas_object_smart_data_get(card);
    if (!data) return;
        
    edje_object_signal_emit(data->image, "hint,off", "elitaire");
}

Elitaire * elitaire_card_elitaire_get(Evas_Object * card)
{
    Elitaire_Card * data = NULL;

    data = (Elitaire_Card *) evas_object_smart_data_get(card);

    if (data) return data->eli;
    
    return NULL;
}

void elitaire_card_chain_make(Evas_Object * card)
{
    Elitaire_Card *data = NULL;

    card_iterator it, it_end;
    Evas_Object * next_card = NULL;
    Evas_Object * owner_card;

    data = (Elitaire_Card *) evas_object_smart_data_get(card);

    if (data) {
        it = data->pcard->myStack->begin();
        it_end = data->pcard->myStack->end();

        /* Go to the card we are talking bout */
        while (*it != data->pcard) {
            it++;
        }

        owner_card = card;
        it++;
        
        while (it != it_end) {
            next_card = (Evas_Object *) (*it)->data;
            evas_object_smart_member_add(next_card, card);
            //evas_object_smart_member_add(next_card, owner_card);
            elitaire_card_chain_next_set(owner_card, next_card);
            owner_card = next_card;
            it++;
        }
    }
}

void elitaire_card_reinit(Evas_Object * card)
{
    Elitaire_Card * data = NULL;
    char buffer[128];

    data = (Elitaire_Card *) evas_object_smart_data_get(card);

    if (data) {
        Evas_Coord x, y, w, h;
        Evas_Object *new_image;

        //int l;
        evas_object_geometry_get(data->image, &x, &y, &w, &h);
        //l = evas_object_layer_get(data->image);
        new_image = edje_object_add(data->eli->evas);

        if (data->pcard->isVisible()) {
            snprintf(buffer, sizeof(buffer), "elitaire/card/%02d", 
                                                        data->pcard->getName());
            if (!edje_object_file_set(new_image, data->eli->file, buffer))
                fprintf(stderr, _("Elitaire Error: Card %s is missed!\n"),
                        buffer);
        }
        else {
            if (!edje_object_file_set(new_image, data->eli->file, 
                                                        "elitaire/card/bg"))
                fprintf(stderr, _("Elitaire Error: The bg pic is missed!\n"));
        }

        evas_object_del(data->image);
        data->image = new_image;
        evas_object_smart_member_add(new_image, card);
        evas_object_resize(data->image, w, h);
        evas_object_move(data->image, x, y);
        evas_object_clip_set(data->image, data->eli->clip);
        //evas_object_layer_set(data->image,l);
        evas_object_show(data->image);
        elitaire_card_callback_add(card, EVAS_CALLBACK_MOUSE_DOWN,
                                   elitaire_mouse_down_cb);
        elitaire_card_callback_add(card, EVAS_CALLBACK_MOUSE_UP,
                                   elitaire_mouse_up_cb);
    }
}

void elitaire_card_xy_get(Evas_Object * card, Evas_Coord * x, Evas_Coord * y)
{
    card_iterator itBegin, itEnd;
    Elitaire * eli = elitaire_card_elitaire_get(card);

    vector<stackLook>::const_iterator iord = eli->game->getStackLooksBegin();
    vector<stackLook>::const_iterator iend = eli->game->getStackLooksEnd();

    playingCard * pcard;
    int number = 0, line = 1;
    
    Evas_Coord rel_x = eli->x + (Evas_Coord) (eli->card_w * eli->margin_h);
    Evas_Coord rel_y = eli->y + (Evas_Coord) (eli->card_h * eli->margin_v);
    Evas_Coord rel_y2, rel_x2, offset;
    Evas_Coord spacing;

    offset = elitaire_card_offset_get(eli);

    rel_x2 = rel_x;
    spacing = elitaire_spacing_abs_calc(eli, line);

    pcard = elitaire_card_playingCard_get(card);

    while (iord != iend) {
        switch (*iord) {
        case STACK:
        case LAST:
            itBegin = eli->game->getStackBegin(number);
            itEnd = eli->game->getStackEnd(number);

            while (itBegin != itEnd) {
                if (pcard == *itBegin) {
                    *x = rel_x2;
                    *y = rel_y;
                    return;
                }

                itBegin++;
            }
            rel_x2 += eli->card_w + spacing;
            number++;
            break;

        case SEPERATOR:
            rel_x2 += eli->card_w + spacing;
            break;
        
        case ENDL:
            rel_y += eli->card_h + (Evas_Coord) (eli->spacing * eli->card_w);
            line++;
            spacing = elitaire_spacing_abs_calc(eli, line);
            rel_x2 = rel_x;
            break;
        
        case ALL:
            itBegin = eli->game->getStackBegin(number);
            itEnd = eli->game->getStackEnd(number);
            rel_y2 = rel_y;
            while (itBegin != itEnd) {
                if (pcard == *itBegin) {
                    *x = rel_x2;
                    *y = rel_y2;
                    return;
                }
                rel_y2 += offset;
                itBegin++;

            }
            rel_x2 += eli->card_w + spacing;
            number++;
            break;
            
        case VALL:
        {
            Evas_Coord rem_x;
            
            itBegin = eli->game->getStackBegin(number);
            itEnd = eli->game->getStackEnd(number);
            rel_y2 = rel_y;
            rem_x = rel_x2;
            
            while (itBegin != itEnd) {
                if (pcard == *itBegin) {
                    *x = rel_x2;
                    *y = rel_y2;
                    return;
                }
                rel_x2 += offset/2;
                itBegin++;

            }
            rel_x2 = rem_x = eli->card_w + spacing;
            number++;
        }
            break;
            
        default:
            break;
        }
        iord++;
    }
}

Evas_Object * elitaire_card_next_card_get(Evas_Object * card)
{
    playingCard * pcard;

    card_iterator itBegin, itEnd;

    pcard = elitaire_card_playingCard_get(card);
    itBegin = pcard->myStack->begin();
    itEnd = pcard->myStack->end();

    while (*itBegin != pcard) itBegin++;
        
    itBegin++;

    if (itBegin == itEnd) return NULL;
    
    return (Evas_Object *) (*itBegin)->data;
}

/* ****************************************************************************
 *  smart object handler functions
 * ****************************************************************************/

static Evas_Smart * _elitaire_card_smart_get()
{
    if (!smart) {
        static const Evas_Smart_Class sc = {
            "Elitaire_Card",
            EVAS_SMART_CLASS_VERSION,
            _elitaire_card_add,
            _elitaire_card_del,
            _elitaire_card_move,
            _elitaire_card_resize,
            _elitaire_card_show,
            _elitaire_card_hide,
            _elitaire_card_color_set,
            _elitaire_card_clip_set,
            _elitaire_card_clip_unset,
            NULL
        };
        smart = evas_smart_class_new(&sc);
    }
    return smart;
}

static void _elitaire_card_add(Evas_Object * o)
{
    Elitaire_Card * data;

    data = (Elitaire_Card *) calloc(1, sizeof(Elitaire_Card));
    if (data) {
        data->image = NULL;
        data->next_card = NULL;
        data->pcard = NULL;
        data->eli = NULL;
        data->shadow = NULL;

        evas_object_smart_data_set(o, data);
    }
}


static void _elitaire_card_del(Evas_Object * o)
{
    Elitaire_Card * data = NULL;

    data = (Elitaire_Card *) evas_object_smart_data_get(o);

    if (data) {
        evas_object_del(data->image);
        if (data->shadow) evas_object_del(data->shadow);
        elitaire_job_list_del(data->jobs);
        free(data);
    }
}

static void _elitaire_card_move(Evas_Object * o, Evas_Coord x, Evas_Coord y)
{
    Elitaire_Card * data;
    
    data = (Elitaire_Card *) evas_object_smart_data_get(o);
    
    if (data) {
        Evas_Coord ox, oy;
        Evas_Coord ix, iy;
        
        evas_object_geometry_get(o, &ox, &oy, NULL, NULL);
        evas_object_geometry_get(data->image, &ix, &iy, NULL, NULL);
        
        ox -= x;
        oy -= y;
        
        evas_object_move(data->image, ix - ox, iy - oy);
        
        if (data->next_card) {
            Evas_Coord dy;

            dy = elitaire_card_offset_get(data->eli);
            evas_object_move(data->next_card, ix - ox, iy - oy + dy);
        }
        
        if (data->shadow) {
            evas_object_geometry_get(data->shadow, &ix, &iy, NULL, NULL);
            evas_object_move(data->shadow, ix - ox, iy - oy);
        }
        /*if (data->shadow) {
            Evas_Coord w;

            evas_object_move(data->shadow, x, y);
            evas_object_geometry_get(data->shadow, NULL, NULL, &w, NULL);
            x -= data->eli->card_w / 10;
            y -= data->eli->card_w / 10;
        }
        evas_object_move(data->image, x, y);*/
        
    }
}


static void _elitaire_card_resize(Evas_Object * o, Evas_Coord w, Evas_Coord h)
{
    Elitaire_Card * data = NULL;

    data = (Elitaire_Card *) evas_object_smart_data_get(o);

    if (data) {
        if (data->shadow) {
            Evas_Coord x, y;

            evas_object_resize(data->shadow, w, h);
            evas_object_geometry_get(data->shadow, &x, &y, NULL, NULL);
            x -= data->eli->card_w / 10;
            y -= data->eli->card_w / 10;
            evas_object_move(data->image, x, y);
        }

        evas_object_resize(data->image, w, h);

        if (data->next_card) {
            Evas_Coord x, y;
            Evas_Coord dy;

            evas_object_resize(data->next_card, w, h);
            evas_object_geometry_get(data->image, &x, &y, NULL, NULL);
            dy = elitaire_card_offset_calc(data->eli, h);
            evas_object_move(data->next_card, x, y + dy);
        }
    }
}

static void _elitaire_card_show(Evas_Object * o)
{
    Elitaire_Card * data = NULL;

    data = (Elitaire_Card *) evas_object_smart_data_get(o);

    if (data) {
        evas_object_show(data->image);
        if (data->next_card)
            evas_object_show(data->next_card);
    }
}

static void _elitaire_card_hide(Evas_Object * o)
{
    Elitaire_Card * data = NULL;

    data = (Elitaire_Card *) evas_object_smart_data_get(o);

    if (data) {
        evas_object_hide(data->image);
        
        if (data->next_card) evas_object_hide(data->next_card);
        if (data->shadow) evas_object_hide(data->shadow);
    }
}

static void _elitaire_card_color_set(Evas_Object * o, int r, int g, int b, int a)
{
    Elitaire_Card * data = NULL;

    data = (Elitaire_Card *) evas_object_smart_data_get(o);

    if (data) {
        evas_object_color_set(data->image, r, g, b, a);
        
        if (data->next_card) evas_object_color_set(data->next_card, r, g, b, a);
    }
}

static void _elitaire_card_clip_set(Evas_Object * o, Evas_Object * clip)
{
    Elitaire_Card * data = NULL;

    data = (Elitaire_Card *) evas_object_smart_data_get(o);

    if (data) {
        evas_object_clip_set(data->image, clip);
        
        if (data->next_card) evas_object_clip_set(data->next_card, clip);
        if (data->shadow) evas_object_clip_set(data->shadow, clip);
    }
}

static void _elitaire_card_clip_unset(Evas_Object * o)
{
    Elitaire_Card * data = NULL;

    data = (Elitaire_Card *) evas_object_smart_data_get(o);

    if (data) {
        evas_object_clip_unset(data->image);
        
        if (data->next_card) evas_object_clip_unset(data->next_card);
        if (data->shadow) evas_object_clip_unset(data->shadow);
    }
}

/* ***************************************************************************
 *  chain stuff
 * ***************************************************************************/

void elitaire_card_chain_delete(Evas_Object * card)
{
    Evas_Object * owner_card, * next_card;
    Evas_Object * elitaire;

    elitaire = elitaire_card_elitaire_get(card)->elitaire;

    owner_card = card;
    next_card = elitaire_card_chain_next_get(owner_card);
    elitaire_card_chain_next_set(owner_card, NULL);
    evas_object_smart_member_add(next_card, elitaire);

    while (next_card != NULL) {
        owner_card = next_card;
        evas_object_smart_member_add(next_card, elitaire);
        next_card = elitaire_card_chain_next_get(owner_card);
        elitaire_card_chain_next_set(owner_card, NULL);
    }
}

int elitaire_card_chain_length_calc(Evas_Object * card)
{
    Evas_Object * next_card;
    int length = 0;

    next_card = elitaire_card_chain_next_get(card);

    while (next_card != NULL) {
        next_card = elitaire_card_chain_next_get(next_card);
        length++;
    }

    return length;
}

void elitaire_card_chain_next_set(Evas_Object * o, Evas_Object * ncard)
{
    Elitaire_Card * data = NULL;

    data = (Elitaire_Card *) evas_object_smart_data_get(o);

    if (data) {
        data->next_card = ncard;
    }
}

Evas_Object * elitaire_card_chain_next_get(Evas_Object * o)
{
    Elitaire_Card * data = NULL;

    data = (Elitaire_Card *) evas_object_smart_data_get(o);

    if (data) {
        return data->next_card;
    }
    return NULL;
}


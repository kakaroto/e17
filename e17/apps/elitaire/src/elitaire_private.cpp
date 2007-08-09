/* vim: set sw=4 ts=4 sts=4 expandtab: */
#include "Rules.h"
#include "Elitaire.h"
#include "elitaire_private.h"


/* ***************************************************************************
 * private functions 
 *
 * used by    elitaire.cpp elitaire_card.cpp elitaire_card_jobs.cpp  
 * ***************************************************************************/

/* return the offset value for a card on a stack to the next card */
Evas_Coord elitaire_card_offset_get(Elitaire * eli)
{
    /* FIXME this hardcoded value should be configurable probably by the
     * theme */
    return eli->card_h / 5;
}

Evas_Coord elitaire_card_offset_calc(Elitaire * eli, Evas_Coord h)
{
    return h / 5;
}

int elitaire_clicked_stack_get(Elitaire * eli, Evas_Coord x, Evas_Coord y)
{

    vector<stackLook>::const_iterator iord = eli->game->getStackLooksBegin();
    vector<stackLook>::const_iterator iend = eli->game->getStackLooksEnd();

    int number = 0, line = 1;

    /* rem_x,rem_y are for remembering the coords of the start of a line or 
     * something similar */

    Evas_Coord rel_x = eli->x + (Evas_Coord) (eli->card_w * eli->margin_h);
    Evas_Coord rel_y = eli->y + (Evas_Coord) (eli->card_h * eli->margin_v);
    Evas_Coord rel_y2, rel_x2, offset;
    Evas_Coord spacing;

    offset = elitaire_card_offset_get(eli);
    rel_y2 = rel_y;
    rel_x2 = rel_x;
    spacing = elitaire_spacing_abs_calc(eli, line);


    while (iord != iend) {
        switch (*iord) {
        case STACK:
        case LAST:
            if (0 < (x - rel_x2) && eli->card_w > (x - rel_x2)
                && 0 < (y - rel_y) && eli->card_h > (y - rel_y)) {
                return number;
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
            rel_y2 =
                rel_y + offset * (eli->game->getStackEnd(number) -
                                  eli->game->getStackBegin(number));

            if (0 < (x - rel_x2) && eli->card_w > (x - rel_x2)
                && 0 < (y - rel_y2) && eli->card_h > (y - rel_y2)) {
                return number;
            }

            rel_x2 += eli->card_w + spacing;
            number++;
            break;
        
        case VALL:
        {
            Evas_Coord rem_x;
            
            rem_x = rel_x2;
            rel_x2 += (offset/2) * (eli->game->getStackEnd(number) -
                                  eli->game->getStackBegin(number));
            rel_y2 = rel_y;

            if (0 < (x - rel_x2) && eli->card_w > (x - rel_x2)
                && 0 < (y - rel_y2) && eli->card_h > (y - rel_y2)) {
                return number;
            }

            rel_x2 = rem_x + eli->card_w + spacing;
            number++;
        }
            break;
            
        default:
            break;
        }
        iord++;
    }
    return -1;
}

void elitaire_stack_layer_reinit(Elitaire * eli, playingCard * pcard)
{
    card_iterator it, it_end, it_begin;
    Evas_Object * card;
    Evas_Object * rem_card;

    it_begin = it = pcard->myStack->begin();
    it_end = pcard->myStack->end();
    /* we set the stack as remebered card, so that every card in
     * the stack is above it */
    rem_card = eli->stacks[pcard->getStackNumber()];

    while (it != it_end) {
        if ((card = (Evas_Object *) (*it)->data)) {
            if (!elitaire_card_is_flying(card) || *it == pcard) {
                evas_object_stack_above(card, rem_card);
                rem_card = card;
            }

        }
        it++;
    }
}

/*
 * hide the hints higlights
 * this function is private
 */
void elitaire_hints_hide(Elitaire * eli)
{
    int i;
    Evas_Object * card;
    card_iterator it, it_end;
    int num_decks;

    if (!eli || !eli->hints_on) return;

    eli->hints_on = false;
    num_decks = eli->game->getNumDecks();

    for (i = 0; i < num_decks; i++) {
        it = eli->game->getDeckBegin(i);
        it_end = eli->game->getDeckEnd(i);

        while (it != it_end) {
            card = (Evas_Object *) (*it)->data;
            elitaire_card_hint_highlight_hide(card);
            it++;
        }
    }
}


/* calculate  the longest line */
int elitaire_cols_max_calc(Elitaire * eli)
{                               
    int i, max_i;

    vector<stackLook>::const_iterator it_begin, it_end;
    
    it_begin = eli->game->getStackLooksBegin();
    it_end = eli->game->getStackLooksEnd();
    i = 0;
    max_i = 0;

    while (it_begin != it_end) {
        if (*(it_begin++) == ENDL) {
            max_i = (max_i < i) ? i : max_i;
            i = 0;
        }
        else
            i++;
    }
    return max_i;
}

/* calculate  the muber of rows*/
int elitaire_num_of_rows_calc(Elitaire * eli)
{

    int num;

    vector<stackLook>::const_iterator it_begin, it_end;
    
    it_begin = eli->game->getStackLooksBegin();
    it_end = eli->game->getStackLooksEnd();
    num = 1;

    while (it_begin != it_end) {
        if (*(it_begin++) == ENDL)
            num++;
    }

    return num;
}

/* calculate  the length of an row */
Evas_Coord elitaire_spacing_abs_calc(Elitaire * eli, int row)
{

    int i, irow;

    vector<stackLook>::const_iterator it_begin, it_end;

    it_begin = eli->game->getStackLooksBegin();
    it_end = eli->game->getStackLooksEnd();
    i = 0;
    irow = 1;

    while (it_begin != it_end) {
        if (*(it_begin++) == ENDL) {
            if (irow == row)
                break;
            i = 0;
            irow++;
        }
        else
            i++;
    }
    return (Evas_Coord) (eli->w - 2 * eli->margin_h * eli->card_w -
                         i * eli->card_w) / (i - 1);
}

void elitaire_reinit(Elitaire * eli)
{
    int number, line;
    Evas_Coord rel_x, rel_x2, rel_y, rel_y2, spacing, offset;
    card_iterator itBegin, itEnd;
    vector<stackLook>::const_iterator iord, iend;
    Evas_Object * card;
    Evas_Object * remember_card;
    
    iord = eli->game->getStackLooksBegin();
    iend = eli->game->getStackLooksEnd();
    card = NULL;
    remember_card = NULL;
    number = 0;
    line = 1;
    rel_x = eli->x + (Evas_Coord) (eli->card_w * eli->margin_h);
    rel_y = eli->y + (Evas_Coord) (eli->card_h * eli->margin_v);
    rel_x2 = rel_x;
    rel_y2 = rel_y;
    offset = elitaire_card_offset_get(eli);
    spacing = elitaire_spacing_abs_calc(eli, line);


    while (iord != iend) {
        switch (*iord) {
        case STACK:
        case LAST:
            itBegin = eli->game->getStackBegin(number);
            itEnd = eli->game->getStackEnd(number);
            evas_object_move(eli->stacks[number], rel_x2, rel_y);

            while (itBegin != itEnd) {
                if ((card = (Evas_Object *) (*itBegin)->data)) {
                    if (!elitaire_card_is_flying(card)) {
                        evas_object_move(card, rel_x2, rel_y);
                        if (remember_card)
                            evas_object_stack_above(card, remember_card);
                        else
                            evas_object_stack_above(card,
                                                    eli->stacks[number]);
                        remember_card = card;
                    }
                }
                itBegin++;
            }
            rel_x2 += eli->card_w + spacing;
            number++;
            remember_card = NULL;
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
            evas_object_move(eli->stacks[number], rel_x2, rel_y);

            itBegin = eli->game->getStackBegin(number);
            itEnd = eli->game->getStackEnd(number);
            rel_y2 = rel_y;
            while (itBegin != itEnd) {
                if ((card = (Evas_Object *) ((*itBegin)->data))) {
                    if (!elitaire_card_is_flying(card)) {
                        evas_object_move(card, rel_x2, rel_y2);
                        if (remember_card)
                            evas_object_stack_above(card, remember_card);
                        else
                            evas_object_stack_above(card,
                                                    eli->stacks[number]);
                        remember_card = card;
                    }
                    rel_y2 += offset;
                }
                itBegin++;
            }
            rel_x2 += eli->card_w + spacing;
            remember_card = NULL;
            number++;
            break;
        
        case VALL:
        {
            int rem_x;
            
            evas_object_move(eli->stacks[number], rel_x2, rel_y);

            itBegin = eli->game->getStackBegin(number);
            itEnd = eli->game->getStackEnd(number);
            rem_x = rel_x2;
            rel_y2 = rel_y;
            while (itBegin != itEnd) {
                if ((card = (Evas_Object *) ((*itBegin)->data))) {
                    if (!elitaire_card_is_flying(card)) {
                        evas_object_move(card, rel_x2, rel_y2);
                        if (remember_card)
                            evas_object_stack_above(card, remember_card);
                        else
                            evas_object_stack_above(card,
                                                    eli->stacks[number]);
                        remember_card = card;
                    }
                    rel_x2 += offset/2;
                }
                itBegin++;
            }
            rel_x2 = rem_x + eli->card_w + spacing;
            remember_card = NULL;
            number++;
        }
            break;
            
        default:
            break;
            
        }
        iord++;
    }
}

void elitaire_stacks_reinit(Elitaire * eli)
{
    
    int number, line;
    vector<stackLook>::const_iterator iord, iend;
    Evas_Coord rel_x, rel_x2, rel_y, spacing, offset;

    
    rel_x = eli->x + (Evas_Coord) (eli->card_w * eli->margin_h);
    rel_y = eli->y + (Evas_Coord) (eli->card_h * eli->margin_v);
    rel_x2 = rel_x;
    iord = eli->game->getStackLooksBegin();
    iend = eli->game->getStackLooksEnd();
    number = 0;
    line = 1;
    offset = elitaire_card_offset_get(eli);
    spacing = elitaire_spacing_abs_calc(eli, line);


    while (iord != iend) {
        switch (*iord) {
        case STACK:
        case ALL:
        case VALL:
        case LAST:
            evas_object_move(eli->stacks[number], rel_x2, rel_y);
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
        }
        iord++;
    }
}

void elitaire_cards_del(Elitaire * eli)
{
    int i, num_decks;

    num_decks = eli->game->getNumDecks();

    for (i = 0; i < num_decks; i++) {
        card_iterator it, it_end;
        
        it = eli->game->getDeckBegin(i);
        it_end = eli->game->getDeckEnd(i);

        while (it != it_end) {
            Evas_Object * card;
            if ((card = (Evas_Object *) (*it++)->data)) {
                evas_object_del(card);
            }
        }
    }
}

void elitaire_stacks_del(Elitaire * eli)
{
    int i;

    i = 0;
    while (i < eli->num_stacks) evas_object_del(eli->stacks[i++]);
    if (eli->stacks) free(eli->stacks);
}

void elitaire_force_resize(Elitaire * eli)
{
    int i, num_decks;
    Evas_Object * card;
    card_iterator it, it_end;

    eli->card_w = (Evas_Coord) (eli->w / ((float) eli->max_cols
                                          +
                                          eli->spacing *
                                          ((float) (eli->max_cols - 1.0))
                                          + (float) (2.0 * eli->margin_h)));
    eli->card_h = (Evas_Coord) (((float) eli->card_w)
                                / ((float) eli->card_max_w) *
                                eli->card_max_h);

    /* resizing the cards */
    num_decks = eli->game->getNumDecks();

    for (i = 0; i < num_decks; i++) {
        it = eli->game->getDeckBegin(i);
        it_end = eli->game->getDeckEnd(i);

        while (it != it_end) {
            if ((card = (Evas_Object *) (*it++)->data)) {
                evas_object_resize(card, eli->card_w, eli->card_h);

            }
        }
    }
    /*resizing the stacks */
    for (i = 0; i < eli->num_stacks; i++) {
        evas_object_resize(eli->stacks[i], eli->card_w, eli->card_h);
    }
}

void elitaire_card_job_reg(Elitaire * eli)
{
    eli->num_card_jobs++;
}

void elitaire_card_job_unreg(Elitaire * eli)
{
    eli->num_card_jobs--;
    if (eli->num_card_jobs == 0 && eli->no_job) {
        ecore_timer_add(00.1, eli->no_job, eli->no_job_data);
        eli->no_job = NULL;
        eli->no_job_data = NULL;
    }
}

/* ***************************************************************************
 * Mouse Actions Callbacks
 * ***************************************************************************/

/* cards *********************************************************************/

void elitaire_mouse_down_cb(void * data, Evas * e, Evas_Object * obj,
                             void * event_info)
{
    Evas_Object * card;
    Evas_Event_Mouse_Down * ev;

    card = (Evas_Object *) data;
    ev = (Evas_Event_Mouse_Down *) event_info;
    
    if (card) {
        Elitaire *eli;

        eli = elitaire_card_elitaire_get(card);

        if (ev->button == 1 && elitaire_card_moveable(card)) {
            elitaire_card_chain_make(card);
            elitaire_card_dragged(card);
            elitaire_hints_hide(eli);
        }
        else if (ev->button == 2) {
            Evas_Object * next_card;

            next_card = elitaire_card_next_card_get(card);
            if (next_card) {
                elitaire_card_chain_make(next_card);
                elitaire_card_fade(next_card);
            }
        }
    }
}

void elitaire_mouse_up_cb(void *data, Evas * e, Evas_Object * obj,
                           void *event_info)
{

    Evas_Object * card;
    Evas_Event_Mouse_Up * ev;
    
    card = (Evas_Object *) data;
    ev = (Evas_Event_Mouse_Up *) event_info;
    
    if (card) {
        Elitaire * eli;
        
        eli = elitaire_card_elitaire_get(card);

        switch (ev->button) {
        case 1:
            eli->game->clickOn(elitaire_card_playingCard_get(card));
            elitaire_hints_hide(eli);
            break;
        case 3:
            eli->game->rightClickOn(elitaire_card_playingCard_get(card));
            elitaire_hints_hide(eli);
            break;
        }
    }
}

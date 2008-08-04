/* vim: set sw=4 ts=4 sts=4 expandtab: */
#include "edje_frontend.h"
#include "eli_hiscore.h"
#include "eli_statistics.h"
#include <Esmart/Esmart_Container.h>

/* internal declaration */
static void _win_game_cb(int end, float points, pointsType ptype, void * data);
static void _points_cb(float points, pointsType ptype, void * data);
static int _eli_app_wait_for_end(void * data);

/* 
 *   only used in _win_game_cb and ...
 */
struct _Eli_App_End
{
    Eli_App  * eap;
    int        end;
    pointsType type;
    float      points;
};

/* 
 *   external functions
 */

void _eli_edje_frontend_menu_container_fill(Eli_App * eap, 
		                             Evas_Object * container)
{
    const char ** games;
    char * file;

    file = ecore_config_theme_with_path_from_name_get(eap->theme.gui.current);
    games = elitaire_available_games_get();

    for (int i = 0; games[i][0]; i++) {
        Evas_Object * item;

        item = edje_object_add(evas_object_evas_get(container));

        if (edje_object_file_set(item, file, "elitaire/element")) {
            Evas_Coord w, h;

            edje_object_size_min_get(item, NULL, &h);
            evas_object_geometry_get(container, NULL, NULL, &w, NULL);
            evas_object_resize(item, w, h);

            if (edje_object_part_exists(item, "elitaire_element_value")) {
                edje_object_part_text_set(item, "elitaire_element_value",
                                          games[i]);
                evas_object_show(item);

                edje_object_signal_callback_add(item, "item_selected",
                                                "item_selected",
                                                _eli_edje_frontend_new_game_cb,
						eap);

                esmart_container_element_append(container, item);
            }
            else {
                fprintf(stderr, _("Elitaire Error in %s: "
                                  "missing element_value part!\n"), file);
                evas_object_del(item);
            }
        }
        else {
            fprintf(stderr,
                    _("Elitaire Error in %s: missing element group!\n"),
                    file);
            evas_object_del(item);
        }
    }
}



/*  
 *  internal functions
 */


/* to win not window */
static void _win_game_cb(int end, float points, pointsType ptype, void * data)
{
    Eli_App * eap;

    eap = (Eli_App *) data;
    
    if (eap) {
        _Eli_App_End * eend = NULL;
	Eli_Edje_Frontend * eef;
        char * game;

	eef = eli_app_edje_frontend_get(eap);
        game = eap->current.game;

        elitaire_object_pause(eef->elitaire);
        if (end == 0) {
            if (edje_object_part_exists(eef->gui, "elitaire_win_text")) {
                edje_object_part_text_set(eef->gui, "elitaire_win_text",
                                          _("You win!"));
            }
            eli_statistics_win_add(game);
        }
        else if (end == 1) {
            if (edje_object_part_exists(eef->gui, "elitaire_win_text")) {
                edje_object_part_text_set(eef->gui, "elitaire_win_text",
                                          _("You lose!"));
            }
            eli_statistics_lost_add(game);
	    eli_app_state_set(eap, ELI_STATE_GAME_KILL);
            eli_app_game_end(eap);
            return;
        }
        else {
            if (edje_object_part_exists(eef->gui, "elitaire_win_text")) {
                edje_object_part_text_set(eef->gui, "elitaire_win_text",
                                          _("Game Over"));
            }
            eli_statistics_lost_add(game);
        }
        edje_object_signal_emit(eef->gui, "win,on", "");

        eend = (_Eli_App_End *) malloc(sizeof(_Eli_App_End));
        eend->eap = eap;
        eend->end = end;
        eend->points = points;
        eend->type = ptype;

	eli_app_state_set(eap, ELI_STATE_GAME_END);
        elitaire_object_wait_for_end_of_jobs(eef->elitaire, 
			                       _eli_app_wait_for_end,
                                               eend);
	
    }
}

static int _eli_app_wait_for_end(void * data)
{
    _Eli_App_End * eend;

    eend = (_Eli_App_End *) data;
    if (eli_highscore_accept(eend->eap->current.game, 
			       eend->points, eend->type)) 
    {
        eli_app_highscore_add(eend->eap, eend->points, eend->type);
    }
    else {
        eli_app_state_leave(eend->eap);
    }
    free(eend);

    return 0;
}

static void _points_cb(float points, pointsType ptype, void *data)
{
    Eli_Edje_Frontend * eef;

    eef = (Eli_Edje_Frontend *) data;
    if (eef) {
        if (edje_object_part_exists(eef->gui, "elitaire_points")) {
            const char * pstring;

            pstring = pointsType_point_string_get(points, ptype);
            edje_object_part_text_set(eef->gui, "elitaire_points", pstring);
        }
    }
}

void _eli_edje_frontend_game_end(Eli_App * eap)
{
    Eli_Edje_Frontend * eef;

    eef = eli_app_edje_frontend_get(eap);
    elitaire_object_giveup(eef->elitaire);
}

void _eli_edje_frontend_game_kill(Eli_App * eap)
{
    Eli_Edje_Frontend * eef;

    eef = eli_app_edje_frontend_get(eap);

    if (eef->elitaire) {
        evas_object_del(eef->elitaire);
        eef->elitaire = NULL;   
    } 

    eli_app_state_leave(eap);
}

void _eli_edje_frontend_game_new(Eli_App * eap)
{
    Eli_Edje_Frontend * eef;
    Evas_Coord x, y, w, h;
    Evas_Coord l, r, t, b;
    Evas_Coord min_w, min_h;
    char * card_theme;
    int vel;
    Evas_Bool bo;

    eef = eli_app_edje_frontend_get(eap);
   
    edje_object_signal_emit(eef->gui, "win,off", "");

    /* init elitaire, set the theme, ... */
    if (!(eef->elitaire = elitaire_object_new(eef->evas)))
        return;
    
    elitaire_object_game_set(eef->elitaire, eap->current.game);

    card_theme =
        ecore_config_theme_with_path_from_name_get(eap->theme.cards.current);
    /* get and set the offset */
    eli_edje_frontend_theme_offset_get(eap, &l, &r, &t, &b);
    elitaire_object_offset_set(eef->elitaire, l, r, t, b);

    if (elitaire_object_file_set(eef->elitaire, card_theme)) {
        elitaire_object_size_min_get(eef->elitaire, &w, &h);
    }
    else {
        fprintf(stderr, _("Elitaire Error: cards file %s not found!\n"),
                eap->theme.cards.current);
        return;
    }
    free(card_theme);
    card_theme = NULL;

    /* find and set the minimum size */
    elitaire_object_size_min_get(eef->elitaire, &min_w, &min_h);
    if (!edje_object_part_exists(eef->gui, "elitaire_card_field")) {
        fprintf(stderr,
                _("Elitaire Error in %s:"
		  " elitaire_card_field does not exist!\n"),
                eap->theme.gui.current);
        return;
    }
    edje_object_part_geometry_get(eef->gui, "elitaire_card_field", &x, &y, &w, &h);
    
    if (w < min_w || h < min_h) {
        Evas_Coord win_h, win_w;

        ecore_evas_geometry_get(eef->ee, NULL, NULL, &win_w, &win_h);
        ecore_evas_resize(eef->ee, win_w + min_w - w, win_h + min_h - h);
    }
    /* move and resize */
    evas_object_move(eef->elitaire, x, y);
    evas_object_resize(eef->elitaire, w, h);
    evas_object_show(eef->elitaire);

    /* tell elitaire the ecore_config vlaues */
    vel = ecore_config_int_get("velocity");
    elitaire_object_velocity_set(eef->elitaire, vel);

    vel = ecore_config_int_get("frame_rate");
    elitaire_object_frame_rate_set(eef->elitaire, vel);

    vel = ecore_config_int_get("lazy");
    elitaire_object_lazy_mode_set(eef->elitaire, vel);

    bo = ecore_config_boolean_get("/graphic/animations");
    elitaire_object_animations_set(eef->elitaire, bo);

    bo = ecore_config_boolean_get("/graphic/shadows");
    elitaire_object_shadows_set(eef->elitaire, bo);

    /* swallow the elitaire object */
    edje_object_part_swallow(eef->gui, "elitaire_card_field", eef->elitaire);
    /* Add the callbacks */
    elitaire_object_callback_win_add(eef->elitaire, _win_game_cb, eap);
    elitaire_object_callback_points_add(eef->elitaire, _points_cb, eef);
    
    /* and now deal */
    elitaire_object_deal(eef->elitaire);

    eli_app_state_set(eap, ELI_STATE_PLAYING);
    return;
}

void _eli_edje_frontend_new_game_cb(void * data, Evas_Object * o, const char * emission,
                          const char * source)
{
    Eli_App * eap;
    Eli_Edje_Frontend * eef;

    eap = (Eli_App *) data;
    eef = eli_app_edje_frontend_get(eap);

    if (!strcmp(source, "")) {
        if (eap->current.game) {
            eli_app_game_new(eap, eap->current.game);
        }
        else return;
    }
    else if (strcmp(source, "item_selected")) eli_app_game_new(eap, emission);
    else {
        const char * game;

        game = edje_object_part_text_get(o, "elitaire_element_value");
        if (game) {
            edje_object_signal_emit(eef->gui, "game,selected", "elitaire");
            eli_app_game_new(eap, game);
        }
    }
}

void _eli_edje_frontend_config_changed(Eli_App * eap, const int tag)
{
    Eli_Edje_Frontend * eef;

    eef = eli_app_edje_frontend_get(eap);

    switch (tag) {
    case THEME:
        if (!eef->gui) break;
        else {
            char * theme;

            if ((theme = ecore_config_theme_get("/theme/gui"))) {
                eli_app_theme_change(eap, theme);
                edje_object_signal_emit(eef->gui, "theme,selected", "elitaire");
                free(theme);
            }
        }
        break;
		
    case CARDS:
    {
        char * cards;

        if ((cards = ecore_config_theme_get("/theme/cards"))) {
            if (eap->theme.cards.current) free(eap->theme.cards.current);                   eap->theme.cards.current = cards;
        }

        edje_object_signal_emit(eef->gui, "cards,selected", "elitaire");

        if (eap->theme.cards.current && eef->elitaire
                && (cards = ecore_config_theme_with_path_from_name_get(
                        eap->theme.cards.current))) {
            elitaire_object_file_set(eef->elitaire, cards);
            free(cards);
        }
        break;
    }	
    case VELOCITY:
    {    
	    int v;
	    
        v = ecore_config_int_get("velocity");
			
        if (!eef->elitaire) break;
        else {
            elitaire_object_velocity_set(eef->elitaire, v);
        }
        break;
	}
    case ANIMATIONS:
        if (!eef->elitaire) break;
        else {
            Evas_Bool b;

            b = ecore_config_boolean_get("/graphic/animations");
            elitaire_object_animations_set(eef->elitaire, b);
        }
        break;
		
    case SHADOWS:
        if (!eef->elitaire) break;
        else {
            Evas_Bool b;

            b = ecore_config_boolean_get("/graphic/shadows");
            elitaire_object_shadows_set(eef->elitaire, b);
        }
        break;
		
    case FRAME_RATE:
        if (!eef->elitaire) break;
        else {
            int fr;

            fr = ecore_config_int_get("frame_rate");
            elitaire_object_frame_rate_set(eef->elitaire, fr);
        }
        break;
		
    case LAZY:
        if (!eef->elitaire) break;
        else {
            int lazy;

            lazy = ecore_config_int_get("lazy");
            elitaire_object_lazy_mode_set(eef->elitaire, lazy);
        }
        break;
		
    default:
	    break;
    }
}


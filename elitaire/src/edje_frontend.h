#include <Ewl.h>
#include <Ecore_Evas.h>
#include "Eli_App.h"
#include "Elitaire.h"
#include <Edje.h>
#include <stdio.h>
#include <stdlib.h>

struct Eli_Edje_Frontend {
    Ecore_Evas  * ee;
    Evas_Object * elitaire;
    Evas        * evas;
    Evas_Object * gui;
    struct {
   	Evas_Bool ctrl_down;
	Evas_Bool alt_down;
    } key;
    Ewl_Widget  * conmenu;
};

void _eli_edje_frontend_new_game_cb(void * data, Evas_Object * o,
		const char * emission, const char * source);
/*
 * the virtual functions for the frontend
 */
void _eli_edje_frontend_del(Eli_App * eap);
void _eli_edje_frontend_gui_make(Eli_App * eap);
void _eli_edje_frontend_gui_del(Eli_App * eap);
void _eli_edje_frontend_game_new(Eli_App * eap);
void _eli_edje_frontend_game_end(Eli_App * eap);
void _eli_edje_frontend_game_kill(Eli_App * eap);
void _eli_edje_frontend_state_notify(Eli_App * eap);
void _eli_edje_frontend_config_changed(Eli_App * eap, const int tag);

Ewl_Widget * eli_edje_frontend_conmenu_new(Eli_App * eap);
void eli_edje_frontend_theme_offset_get(Eli_App * eap, 
		                Evas_Coord * l, Evas_Coord * r,
				Evas_Coord * t, Evas_Coord * b);

void _eli_edje_frontend_menu_container_fill(Eli_App * eap, 
		                                Evas_Object * container);

inline Eli_Edje_Frontend * eli_app_edje_frontend_get(Eli_App *eap)
{
    return (Eli_Edje_Frontend *) eap->frontend->data;
}

/* */
void _eli_edje_frontend_new_game_cb(void * data, Evas_Object * o, 
		const char * emission, const char * source);


/* vim: set sw=4 ts=4 sts=4 expandtab: */
#include "Eli_App.h"
#include <Ecore_Config.h>
#include <stdlib.h>
#include <stdio.h>
#include <Ecore.h>
#include <Ecore_File.h>
#include <Ecore_Str.h>
#include <Edje.h>
#include <Eina.h>

static int _eli_app_config_listen_cb(const char * key, 
               const Ecore_Config_Type type,
               const int tag, void *data);
static char * theme_cut_off_suffix(const char * file);

/* allocate memory for eap */
Eli_App * eli_app_new(void)
{
    Eli_App * eap;

    eap = (Eli_App *) calloc(1, sizeof(Eli_App));
	
    return eap;
}

/* init configs and theme stuff*/
int eli_app_init(Eli_App * eap, const char * frontend)
{
    eap->state = ELI_STATE_GAME_NONE;
    
    ecore_config_theme_search_path_append(PACKAGE_DATA_DIR "/themes/");
    
    if (!eli_app_theme_init(eap)) return false;
    if (!eli_app_config_init(eap)) return false;
    if (!eli_app_frontend_init(eap, frontend)) return false;

    return true;
}

void eli_app_del(Eli_App * eap)
{
    if (!eap) return;
    
    eli_app_gui_del(eap);
    
    if (eap->theme.gui.current)
	    ecore_config_theme_set("/theme/gui", eap->theme.gui.current);
    if (eap->theme.gui.current)
	    ecore_config_theme_set("/theme/cards", eap->theme.cards.current);

    ecore_config_save();
}

/*
 * state stuff
 */
void eli_app_state_leave(Eli_App * eap)
{
    if (!eap) return;

    if (eap->state == ELI_STATE_PLAYING) {
       eli_app_state_set(eap, ELI_STATE_GAME_END);
       eli_app_game_end(eap);
    }
    else if (eap->state == ELI_STATE_GAME_END) {
       eli_app_state_set(eap, ELI_STATE_GAME_KILL);
       eli_app_game_end(eap);
    }
    else if (eap->state == ELI_STATE_HIGHSCORE) {
       eli_app_state_set(eap, ELI_STATE_GAME_KILL);
       eli_app_game_end(eap);
    }
    else if (eap->state == ELI_STATE_GAME_KILL) {
       eli_app_state_set(eap, ELI_STATE_GAME_NONE);
       if (eap->current.game) {
	       free(eap->current.game);
	       eap->current.game = NULL;
       }
       eli_app_game_end(eap);
    }
}

void eli_app_state_set(Eli_App * eap, Eli_State state)
{
    if (!eap) return;
    
    eap->state = state;
    
    if (!eap->frontend || !eap->frontend->state_notify) return;

    eap->frontend->state_notify(eap);
}

/*
 * gui stuff
 */
void eli_app_gui_make(Eli_App * eap)
{
    if (!eap || !eap->frontend || !eap->frontend->gui_make) return;

    eap->frontend->gui_make(eap);
}

void eli_app_gui_del(Eli_App * eap)
{
    if (!eap || !eap->frontend || !eap->frontend->gui_del) return;
    
    eap->frontend->gui_del(eap);
}

void eli_app_theme_change(Eli_App * eap, const char * theme)
{
    eli_app_gui_del(eap);
    if (eap->theme.gui.current)
        free(eap->theme.gui.current);

    eap->theme.gui.current = strdup(theme);
    eli_app_gui_make(eap);
}

/*
 * game stuff
 */
inline void eli_app_game_kill(Eli_App * eap)
{
     if (!eap || !eap->frontend || !eap->frontend->game_kill) return;

     eap->frontend->game_kill(eap);
}

inline void eli_app_game_new_force(Eli_App * eap)
{
     if (!eap || !eap->frontend || !eap->frontend->game_new) return;
     
     eap->frontend->game_new(eap);
}

void eli_app_game_end(Eli_App * eap)
{
    if (!eap) return;
    
    if (eap->state == ELI_STATE_PLAYING) {
	if (eap->frontend && eap->frontend->game_end)
	    eap->frontend->game_end(eap);
    }
    if (eap->state == ELI_STATE_GAME_END
                || eap->state == ELI_STATE_HIGHSCORE) return;
    if (eap->state == ELI_STATE_EXIT ||
            (eap->next.game && !strcmp(eap->next.game, "exit"))) {
        eap->frontend->game_kill(eap);
        ecore_main_loop_quit();
        return;
    }
    if (eap->state == ELI_STATE_GAME_KILL) {
        eap->frontend->game_kill(eap);
    }
    if (eap->state == ELI_STATE_GAME_NONE && eap->next.game) {
        if (eap->current.game)
	    free(eap->current.game);
        eap->current.game = eap->next.game;
        eap->next.game = NULL;
    }
    
    if (eap->state == ELI_STATE_GAME_NONE && eap->current.game) {
        eli_app_game_new_force(eap);
    }
}

void eli_app_game_new(Eli_App * eap, const char * game)
{
    if (eap->next.game) return;

    eap->next.game = strdup(game);
    /* end current game */
    eli_app_game_end(eap);
}

/* config listner */
void eli_app_config_changed(Eli_App * eap, const int tag)
{
     if (!eap || !eap->frontend || !eap->frontend->config_changed) return;
    
    eap->frontend->config_changed(eap, tag);
}

/*
 * dialogs
 */
void eli_app_config_open(Eli_App * eap)
{
    if (!eap || !eap->frontend || !eap->frontend->win_config_open) return;
    
    eap->frontend->win_config_open(eap);
}

void eli_app_about_open(Eli_App * eap)
{
    if (!eap || !eap->frontend || !eap->frontend->win_about_open) return;
    
    eap->frontend->win_about_open(eap);
}

void eli_app_highscore_open(Eli_App * eap, const char * game)
{
    if (!eap || !eap->frontend || !eap->frontend->win_hiscore_open) return;
    
    eap->frontend->win_hiscore_open(eap, game);
}

void eli_app_highscore_add(Eli_App * eap, float points, pointsType type)
{
    if (!eap || !eap->frontend || !eap->frontend->win_hiscore_add) return;
   
    eli_app_state_set(eap, ELI_STATE_HIGHSCORE);
    eap->frontend->win_hiscore_add(eap, points, type);
}

/*
 * init functions
 */
int eli_app_frontend_init(Eli_App * eap, const char * frontend)
{
    if (!eap || !frontend) return false;

    eap->frontend = (Eli_Frontend *) calloc(1, sizeof(Eli_Frontend));
    
    if (!strcmp(frontend, "edje")) {
        eli_frontend_edje_init(eap, eap->frontend);
        eli_frontend_ewl_dialogs_init(eap);
    }
    else return false;

    return true;
}

int eli_app_config_init(Eli_App * eap)
{
    char *gui;
    char *cards;

    /* THEME */
    ecore_config_theme_default("/theme/gui", "e17");
    ecore_config_theme_preview_group_set("/theme/gui", "elitaire/main");
    ecore_config_listen("theme", "/theme/gui", _eli_app_config_listen_cb, 
                                     THEME, eap);
    /* CARDS */
    ecore_config_theme_default("/theme/cards", "default_cards");
    ecore_config_theme_preview_group_set("/theme/cards", "elitaire/01");
    ecore_config_listen("cards", "/theme/cards", _eli_app_config_listen_cb,
                                     CARDS, eap);
    /* VELOCITY */
    ecore_config_int_default("velocity", 400);
    ecore_config_listen("velocity", "velocity", _eli_app_config_listen_cb,
                                     VELOCITY, eap);
    /* FRAME RATE */
    ecore_config_int_default("frame_rate", 65);
    ecore_config_listen("frame_rate", "frame_rate", _eli_app_config_listen_cb,
                                     FRAME_RATE, eap);
    /* ANIMATIONS */
    ecore_config_boolean_default("/graphic/animations", 1);
    ecore_config_listen("/graphic/animations", "/graphic/animations",
                                     _eli_app_config_listen_cb,
				     ANIMATIONS, eap);
    /* SHADOWS */
    ecore_config_boolean_default("/graphic/shadows", 0);
    ecore_config_listen("/graphic/shadows", "/graphic/shadows", 
                                     _eli_app_config_listen_cb,
				     SHADOWS, eap);
    /* GL or SOFTWARE */
    ecore_config_boolean_default("/graphic/gl", 0);
    /* LAZY MODE */
    ecore_config_int_default("lazy", 1);
    ecore_config_listen("lazy", "lazy", _eli_app_config_listen_cb,
                                     LAZY, eap);

    /* load*/
    ecore_config_load();

    /* set the themes */
    gui = ecore_config_theme_get("/theme/gui");
    cards = ecore_config_theme_get("/theme/cards");

    if (eli_theme_gui_exists(eap, gui)) eap->theme.gui.current = gui;
    else {
        /* take the first theme if the wanted doesnt exists */
        char * src;

        src = (char *) ecore_list_first_goto(eap->theme.gui.list);
        if (!src) {
	    fprintf(stderr, _("Elitaire Error: No theme file in %s!\n"),
             			  ecore_config_theme_search_path_get());
	    exit(1);
	}
        eap->theme.gui.current = strdup(src);
    }

    if (eli_theme_cards_exists(eap, cards)) 
        eap->theme.cards.current = cards;

    else {
        /* take the first theme if the wanted doesnt exists */
        char *src;

        src = (char *) ecore_list_first_goto(eap->theme.cards.list);
        if (!src)
            fprintf(stderr, _("Elitaire Error: No cards file in %s!\n"),
                    ecore_config_theme_search_path_get());
        else eap->theme.cards.current = strdup(src);
    }
    return true;
}

int _eli_app_config_listen_cb(const char * key, const Ecore_Config_Type type,
               const int tag, void * data)
{
    Eli_App * eap;

    eap = (Eli_App *) data;
    if (!eap) return 0;
    
    eli_app_config_changed(eap, tag);

    return 1;
}



int eli_app_theme_init(Eli_App * eap)
{
    Ecore_List * l;
    Ecore_List * gui, * cards;
    char * name;
    char * file;

    gui = ecore_list_new();
    cards = ecore_list_new();

    l = eli_theme_names_get();

    /* selecting the gui theme */
    ecore_list_first_goto(l);
    while ((name = (char *) ecore_list_next(l))) {
        file = ecore_config_theme_with_path_from_name_get(name);

        /* selecting the gui theme */
        if (edje_file_group_exists(file, "elitaire/main"))
            ecore_list_append(gui, name);
        /* selecting the card theme */
        if (edje_file_group_exists(file, "elitaire/card/01"))
            ecore_list_append(cards, name);

        free(file);
    }
    ecore_list_first_goto(l);

    eap->theme.gui.list = gui;
    eap->theme.cards.list = cards;

    ecore_list_destroy(l);

    return true;
}

/*
 * theme stuff
 */
/* return a list of all files with .edj suffix*/
Ecore_List * eli_theme_names_get()
{
    char ** dir_list;
    int i = 0;
    Eina_List *file_l;
    Ecore_List * out_l;

    dir_list = eli_theme_dir_get();

    if (!dir_list) {
        fprintf(stderr, "Warning: there are no possible theme paths\n"
                        "I'm unsure why this happens :(\n");
        return NULL;
    }

    out_l = ecore_list_new();

    while (dir_list[i]) {
	void *tmp;
        char *dir;

        dir = dir_list[i];
        file_l = ecore_file_ls(dir);
	EINA_LIST_FREE(file_l, tmp)
	  {
	     char *file = (char*) tmp;
                if (ecore_str_has_suffix(file, ".edj"))
                    ecore_list_append(out_l, theme_cut_off_suffix(file));
	     free(file);
        }
        i++;
    }

    free(*dir_list);
    free(dir_list);

    ecore_list_first_goto(out_l);

    return out_l;
}

char ** eli_theme_dir_get()
{
    char * dirs;
    char ** dirs_array;

    dirs = ecore_config_theme_search_path_get();
    dirs_array = ecore_str_split(dirs, "|", -1);
    free(dirs);

    return dirs_array;
}

static char * theme_cut_off_suffix(const char * file)
{
    char * new_file, * pt;
    char * end;

    /*find the begin of the suffix, the new end */
    end = strrchr(file, '.');

    /*copy the rest */
    new_file = (char *) malloc(sizeof(char) * (end - file + 1));
    pt = new_file;

    while (file != end) *pt++ = *file++;
    *pt = '\0';

    return new_file;

}

inline Evas_Bool 
_eli_theme_exists(Ecore_List * l, const char * theme)
{
    char * data;
    
    if (!l) return false;

    ecore_list_first_goto(l);
    while ((data = (char *) ecore_list_next(l))) {
        if (!strcmp(theme, data)) {
            return true;
        }
    }

    ecore_list_first_goto(l);
    return false;
}

Evas_Bool eli_theme_gui_exists(Eli_App * eap, const char * theme)
{
    return _eli_theme_exists(eap->theme.gui.list, theme);
}

Evas_Bool eli_theme_cards_exists(Eli_App * eap, const char * theme)
{
    return _eli_theme_exists(eap->theme.cards.list, theme);
}


#include <Ecore_Data.h>
#include <Ecore_Config.h>
#include <Evas.h>
#include <config.h>
#include "points.h"
#include "intl.h"

enum Eli_State {
    ELI_STATE_GAME_NONE,
    ELI_STATE_PLAYING,
    ELI_STATE_GAME_END,
    ELI_STATE_HIGHSCORE,
    ELI_STATE_GAME_KILL,
    ELI_STATE_EXIT,
    ELI_STATE_NONE
};

struct Eli_App;

struct Eli_Frontend {
    /*
     * the virtual functions
     */
    void (*init)     (Eli_App * eap);
    void (*del)      (Eli_App * eap);
    
    void (*gui_make) (Eli_App * eap);
    void (*gui_del)  (Eli_App * eap);
    
    void (*game_new) (Eli_App * eap);
    void (*game_end) (Eli_App * eap);
    void (*game_kill)(Eli_App * eap);

    void (*state_notify)(Eli_App * eap);
    void (*state_leave)(Eli_App * eap);
    void (*config_changed)(Eli_App * eap, const int tag);
    
    void (*win_config_open)  (Eli_App * eap);
    void (*win_hiscore_open) (Eli_App * eap, const char * game);
    void (*win_hiscore_add)  (Eli_App * eap, float points, pointsType type);
    void (*win_about_open)   (Eli_App * eap);
    
    void * data;
};

struct Eli_App {    
    Eli_State       state;
    
    struct {
        struct{
            char      * current;
            Ecore_List* list;
        } gui, cards; // should gui removed??
    } theme;
    
    struct {
        char          * game;
    } current, next;
    
    Eli_Frontend * frontend;
    void * main_win;
};

/* The Config Types */
enum Config_Types {
    THEME,
    CARDS,
    VELOCITY,
    SHADOWS,
    ANIMATIONS,
    FRAME_RATE,
    LAZY
};

/* eli_app.cpp */
Eli_App    * eli_app_new(void);
int          eli_app_init(Eli_App * eap, const char * frontend);
void         eli_app_del(Eli_App * eap);
void         eli_app_end(Eli_App * eap);

void         eli_app_state_leave(Eli_App * eap);
void         eli_app_state_set(Eli_App * eap, Eli_State state);

void         eli_app_gui_make(Eli_App * eap);
void         eli_app_gui_del(Eli_App * eap);
void         eli_app_theme_change(Eli_App * eap, const char * theme);

void         eli_app_game_new(Eli_App * eap, const char * game);
void         eli_app_game_end(Eli_App * eap);

void         eli_app_config_open(Eli_App * eap);
void         eli_app_about_open(Eli_App * eap);
void         eli_app_highscore_open(Eli_App * eap, const char * game);
void         eli_app_highscore_add(Eli_App * eap, float points, pointsType type);
void         eli_app_highscore_added(Eli_App * eap);

int          eli_app_frontend_init(Eli_App * eap, const char * frontend);
int          eli_app_config_init(Eli_App * eap);
int          eli_app_theme_init(Eli_App * eap);

Ecore_List * eli_theme_names_get();
char      ** eli_theme_dir_get();
Evas_Bool    eli_theme_gui_exists(Eli_App * eap, const char * theme);
Evas_Bool    eli_theme_cards_exists(Eli_App * eap, const char * theme);

/*
 * frontend
 */
void eli_frontend_edje_init(Eli_App * eap, Eli_Frontend * frontend);
void eli_frontend_ewl_dialogs_init(Eli_App * eap);


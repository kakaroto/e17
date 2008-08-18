/* vim: set sw=4 ts=4 sts=4 expandtab: */
#include <Ewl.h>
#include "ewl_context_menu.h"
#include "edje_frontend.h"
#include "ewl_help_reader.h"

#define ELI_ICON(icon) ewl_icon_theme_icon_path_get(EWL_ICON_ ## icon, EWL_ICON_SIZE_SMALL)

static void _item_select_cb(Ewl_Widget * w, void * ev_data, void * user_data);
static void _add_item(const char * text, const char * image, Ewl_Widget * container, void * data);
static void _new_cb(Ewl_Widget * w, void * ev_data, void * user_data);
static Ewl_Widget * _new_menu(Eli_App * eap);

Ewl_Widget * eli_edje_frontend_conmenu_new(Eli_App * eap)
{

    Ewl_Widget * menu;
    Ewl_Widget * w;

    menu = ewl_context_menu_new();

    w = _new_menu(eap);
    ewl_container_child_append(EWL_CONTAINER(menu), w);
    _add_item(sgettext(N_("CONTEXT|Undo")), ELI_ICON(DOCUMENT_REVERT), menu, eap);
    _add_item(sgettext(N_("CONTEXT|Highscore")), NULL, menu, eap);
    _add_item(sgettext(N_("CONTEXT|Preferences")), ELI_ICON(PREFERENCES_OTHER), menu, eap);
    _add_item(sgettext(N_("CONTEXT|About")), ELI_ICON(HELP_ABOUT), menu, eap);
    _add_item(sgettext(N_("CONTEXT|Help")), ELI_ICON(HELP_CONTENTS), menu, eap);
    
    return EWL_WIDGET(menu);
}

static void _add_item(const char * text, const char * icon, Ewl_Widget * container, void * data)
{
    Ewl_Widget * w;

    if (!icon) icon = "";

    w = ewl_menu_item_new();
    ewl_button_label_set(EWL_BUTTON(w), text);
    ewl_button_image_set(EWL_BUTTON(w), icon, NULL);
    ewl_container_child_append(EWL_CONTAINER(container), w);
    ewl_callback_append(w, EWL_CALLBACK_CLICKED, _item_select_cb, data);
    ewl_widget_show(w);
}

static void _item_select_cb(Ewl_Widget * w, void * ev_data, void * user_data)
{
    const char * text;
    Eli_App * eap;
    Eli_Edje_Frontend * eef;

    eap = (Eli_App *) user_data;
    eef = eli_app_edje_frontend_get(eap);

    text = ewl_button_label_get(EWL_BUTTON(w));
    if (!strcmp(text, sgettext(N_("CONTEXT|Undo")))) {
        if (eef->elitaire)
            elitaire_object_undo(eef->elitaire);
    }
    else if (!strcmp(text, sgettext(N_("CONTEXT|Highscore"))))
        eli_app_highscore_open(eap, "");
    else if (!strcmp(text, sgettext(N_("CONTEXT|Preferences"))))
        eli_app_config_open(eap);
    else if (!strcmp(text, sgettext(N_("CONTEXT|About"))))
        eli_app_about_open(eap);
    else if (!strcmp(text, sgettext(N_("CONTEXT|Help"))))
        help_reader_page_set("index");
}

static Ewl_Widget * _new_menu(Eli_App * eap)
{
    const char ** games;
    Ewl_Widget * menu;

    games = elitaire_available_games_get();

    menu = ewl_menu_new();
    ewl_button_label_set(EWL_BUTTON(menu), sgettext(N_("CONTEXT|New")));
    ewl_button_image_set(EWL_BUTTON(menu), ELI_ICON(DOCUMENT_NEW), NULL);

    for (int i = 0; games[i][0]; i++) {
        Ewl_Widget * w;

        w = ewl_menu_item_new();
        ewl_button_label_set(EWL_BUTTON(w), games[i]);
        ewl_container_child_append(EWL_CONTAINER(menu), w);
        ewl_callback_append(w, EWL_CALLBACK_CLICKED, _new_cb, eap);
        ewl_widget_show(w);
    }
    ewl_widget_show(menu);
    return menu;
}

static void _new_cb(Ewl_Widget * w, void * ev_data, void * user_data)
{
    const char * text;
    Eli_App * eap;

    eap = (Eli_App *) user_data;

    text = ewl_button_label_get(EWL_BUTTON(w));

    eli_app_game_new(eap, text);
}


#include <Ecore_File.h>
#include "exg_gui.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

static Ecore_Hash *exg_eapps = NULL;

static char *
exg_gui_theme_path_get()
{
    static char edj_file[PATH_MAX + 1];
    char *theme_name = exg_conf_theme_get();
    
    snprintf(edj_file, sizeof(edj_file), PACKAGE_DATA_DIR"/data/exige/%s.edj",
	     theme_name);
    
    return edj_file;
}

void 
exg_gui_init(Exige *exg)
{    
     exg->ee = ecore_evas_software_x11_new(NULL, 0, 0, 0, 0, 0);
     ecore_evas_title_set(exg->ee, "Exige");
     ecore_evas_name_class_set(exg->ee, "Exige", "Exige");
     
     exg->evas = ecore_evas_get(exg->ee);
     ecore_evas_data_set(exg->ee, "Exige", exg);
     
     exg_conf_init(exg);

     exg->gui=edje_object_add(exg->evas);     
     edje_object_file_set(exg->gui,exg_gui_theme_path_get(),"Exige");
     
     edje_object_signal_callback_add(exg->gui, "button,Cancel,clicked",
				     "*", _exg_quit, NULL);
     edje_object_signal_callback_add(exg->gui, "button,Run,clicked",
				     "*", _run_cb, exg);
     
     exg_text_entry_create(exg);
     
     exg_window_geometry_set(exg);
     
     exg->eapp_edj=NULL;
     evas_object_show(exg->gui);
     evas_object_show(exg->txt);
}


void 
exg_text_entry_create(Exige *exg)
{
    exg->txt=esmart_text_entry_new(exg->evas);
    evas_object_event_callback_add(exg->txt, EVAS_CALLBACK_KEY_DOWN, key_cb, exg);
    esmart_text_entry_edje_part_set(exg->txt,exg->gui,"entry");    
    esmart_text_entry_return_key_callback_set(exg->txt,_enter_cb,exg);
    esmart_text_entry_is_password_set(exg->txt, 0);
    
    evas_object_focus_set(exg->txt, 1);
    evas_object_move(exg->txt,0,0);
    evas_object_layer_set(exg->txt,200);
    evas_object_color_set(exg->txt,0,0,0,255);
}


void
exg_window_geometry_set(Exige *exg)
{
    Evas_Coord w,h;
    int mouse_x = 0, mouse_y = 0;
    Ecore_X_Window win;
 
    edje_object_size_min_get(exg->gui, &w, &h);
    ecore_evas_resize(exg->ee, w, h);
    
    ecore_evas_size_min_set(exg->ee,w,h);
    ecore_evas_size_max_set(exg->ee,w,h);

    if(exg_conf_on_mouse_get())
    {
        /* ge't the coords. of the mouse */
        win = ecore_evas_software_x11_window_get(exg->ee);
        ecore_x_pointer_xy_get(win, &mouse_x, &mouse_y);
        ecore_evas_show(exg->ee);
        ecore_evas_move(exg->ee, mouse_x - (w / 2), mouse_y - (h / 2));
    } else {
        ecore_evas_show(exg->ee);
    }
    
    evas_object_move(exg->gui,0,0);
    evas_object_resize(exg->gui,w,h);
}

int
exg_eapps_init()
{
    char path[PATH_MAX];
    char *home;
    Ecore_List *eapps;
    char *name;

    home = getenv("HOME");
    if (!home)
    {
        fprintf(stderr, "Unable to get HOME from environment.\n");
        return 0;
    }
    snprintf(path, PATH_MAX, "%s/.e/e/applications/all", home);

    if (!ecore_file_exists(path))
    {
        fprintf(stderr, "%s dosen't exist. Where are the eapps?\n", path);
        return 0;
    }

    eapps = ecore_file_ls(path);
    if (!eapps)
    {
        fprintf(stderr, "Didn't get any Eapp files.\n");
        return 0;
    }

    exg_eapps = ecore_hash_new(ecore_str_hash, ecore_str_compare);

    while ((name = ecore_list_next(eapps)))
    {
        char *ret;
        int ret_size;
        Eet_File *ef;
        char e_path[PATH_MAX];
        Exg_Eapp *eapp;

        eapp = calloc(1, sizeof(Exg_Eapp));
        if (!eapp) 
        {
            fprintf(stderr, "Not enough memory to create eapp.\n");
            continue;
        }

        eapp->eapp_name = strdup(name);

        snprintf(e_path, PATH_MAX, "%s/%s", path, name);
        eapp->path = strdup(e_path);
        ef = eet_open(e_path, EET_FILE_MODE_READ);
        if (!ef) 
        {
            fprintf(stderr, "Error reading: %s\n", e_path);
            continue;
        }
        ret = eet_read(ef, "app/info/exe", &ret_size);
        if (ret_size > 0)
        {
            eapp->exe = malloc(sizeof(char) * (ret_size + 1));
            snprintf(eapp->exe, ret_size + 1, "%s", ret);
        }

        ecore_hash_set(exg_eapps, eapp->exe, eapp);
        eet_close(ef);
    }

    return 1;
}

void
eapp_display(Exige *exg)
{
    Exg_Eapp *eapp;
    const char *command;    
    command = esmart_text_entry_text_get(exg->txt);

    if ((eapp = ecore_hash_get(exg_eapps, (void *) command))) {
	if(!exg->eapp_show)
	    {    
		exg->eapp_edj= edje_object_add(exg->evas);
		edje_object_file_set(exg->eapp_edj,eapp->path,"icon");
		edje_object_part_swallow(exg->gui,"eapp_swallow",exg->eapp_edj);
		evas_object_show(exg->eapp_edj);
		exg->eapp_show=1;
	    }
    }
    else {
	if (exg->eapp_show)
	    { 
		evas_object_del(exg->eapp_edj);
		exg->eapp_show=0;
	    }
    }
}

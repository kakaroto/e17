#include "exg_gui.h"

char * exg_gui_theme_path_get()
{
    char edj_file[PATH_MAX+1];
    char *theme_name = exg_conf_theme_get();
    
    snprintf(edj_file, sizeof(edj_file), PACKAGE_DATA_DIR"/data/exige/%s.edj",
	     theme_name);
    
    return edj_file;
}

void 
exg_gui_init(Exige *exg)
{    
     exg->ee = ecore_evas_software_x11_new(NULL,0,0,0,0,0);
     ecore_evas_title_set(exg->ee,"Exige");
     
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
     
     evas_object_show(exg->gui);
     evas_object_show(exg->txt);
}


void 
exg_text_entry_create(Exige *exg)
{
    exg->txt=esmart_text_entry_new(exg->evas);

    esmart_text_entry_edje_part_set(exg->txt,exg->gui,"entry");    
    esmart_text_entry_return_key_callback_set(exg->txt,_enter_cb,NULL);
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

/* vim: set sw=4 ts=4 sts=4 expandtab: */
#include "Eli_App.h"
#include <Ewl.h>
#include "ewl_dialogs.h"
#include <Ecore_Config.h>
#include <stdio.h>

static void destroy_cb          (Ewl_Widget * w, void * event, void * data);
static void conf_win_clicked_cb (Ewl_Widget * w, void * event, void * data);
static void _check_selected     (Ewl_Widget * w, void * event, void * data);
static void _hseeker_cb         (Ewl_Widget * w, void * event, void * data);

static Ewl_Widget * conf_win = NULL;

static struct
{
    Evas_Bool    shadows;
    Evas_Bool    animations;
    Evas_Bool    gl;
    int          lazy;
    Ewl_Widget * vel_label;
    Ewl_Widget * frt_label;
    int          velocity;
    int          frame_rate;
} config;

void ewl_frontend_dialog_config_open(Eli_App * eap)
{
    Ewl_Widget * o;
    Ewl_Widget * border_box;
    Ewl_Widget * hbox;
    Ewl_Widget * vbox;
    Ewl_Widget * main_box;
    Ewl_Widget * radio_b[2];

    /* one open config dialog should be enough */
    if (conf_win) return;
    /* Setup and show the configuration window */
    conf_win = ewl_dialog_new();
    ewl_dialog_action_position_set(EWL_DIALOG(conf_win), EWL_POSITION_BOTTOM);
    ewl_window_title_set(EWL_WINDOW(conf_win), _("Configuration"));
    ewl_window_name_set(EWL_WINDOW(conf_win), "Elitaire");
    ewl_window_class_set(EWL_WINDOW(conf_win), "Elitaire");
    ewl_window_leader_foreign_set(EWL_WINDOW(conf_win),
		    		EWL_EMBED_WINDOW(eap->main_win));
    ewl_callback_append(conf_win, EWL_CALLBACK_DELETE_WINDOW, destroy_cb,
                        NULL);
    ewl_dialog_has_separator_set(EWL_DIALOG(conf_win), 1);
    ewl_object_fill_policy_set(EWL_OBJECT(conf_win), EWL_FLAG_FILL_NONE);
    ewl_widget_show(conf_win);
    
    /* the main_box contain the border_boxes */
    ewl_dialog_active_area_set(EWL_DIALOG(conf_win), EWL_POSITION_TOP);
    main_box = ewl_vbox_new();
    ewl_container_child_append(EWL_CONTAINER(conf_win), main_box);
    ewl_object_fill_policy_set(EWL_OBJECT(main_box), EWL_FLAG_ALIGN_CENTER);
    ewl_widget_show(main_box);

    /* Setup and show the stock icons */
    ewl_dialog_active_area_set(EWL_DIALOG(conf_win), EWL_POSITION_BOTTOM);

    o = ewl_button_new();
    ewl_stock_type_set(EWL_STOCK(o), EWL_STOCK_CANCEL);
    ewl_container_child_append(EWL_CONTAINER(conf_win), o);
    ewl_callback_append(o, EWL_CALLBACK_CLICKED, conf_win_clicked_cb,
                        conf_win);
    ewl_widget_show(o);

    o = ewl_button_new();
    ewl_stock_type_set(EWL_STOCK(o), EWL_STOCK_APPLY);
    ewl_container_child_append(EWL_CONTAINER(conf_win), o);
    ewl_callback_append(o, EWL_CALLBACK_CLICKED, conf_win_clicked_cb,
                        conf_win);
    ewl_widget_show(o);

    o = ewl_button_new();
    ewl_stock_type_set(EWL_STOCK(o), EWL_STOCK_OK);
    ewl_container_child_append(EWL_CONTAINER(conf_win), o);
    ewl_callback_append(o, EWL_CALLBACK_CLICKED, conf_win_clicked_cb,
                        conf_win);
    ewl_widget_show(o);

    /* *** Graphic Box *** */
    /* Setup and show the border box */
    border_box = ewl_border_new();
    ewl_border_label_set(EWL_BORDER(border_box), _("Graphic"));
    ewl_container_child_append(EWL_CONTAINER(main_box), border_box);
    //ewl_object_fill_policy_set(EWL_OBJECT(border_box), EWL_FLAG_FILL_HFILL);
    ewl_widget_show(border_box);

    /* Setup and show the checkbuttons */
    o = ewl_checkbutton_new();
    ewl_button_label_set(EWL_BUTTON(o), _("animated movements"));
    ewl_container_child_append(EWL_CONTAINER(border_box), o);
    ewl_object_alignment_set(EWL_OBJECT(o), EWL_FLAG_ALIGN_LEFT);
    ewl_widget_show(o);
    config.animations = ecore_config_boolean_get("/graphic/animations");
    ewl_togglebutton_checked_set(EWL_TOGGLEBUTTON(o), config.animations);
    ewl_callback_append(o, EWL_CALLBACK_CLICKED, _check_selected, NULL);

    o = ewl_checkbutton_new();
    ewl_button_label_set(EWL_BUTTON(o), _("shadows"));
    ewl_container_child_append(EWL_CONTAINER(border_box), o);
    ewl_object_alignment_set(EWL_OBJECT(o), EWL_FLAG_ALIGN_LEFT);
    ewl_widget_show(o);
    config.shadows = ecore_config_boolean_get("/graphic/shadows");
    ewl_togglebutton_checked_set(EWL_TOGGLEBUTTON(o), config.shadows);
    ewl_callback_append(o, EWL_CALLBACK_CLICKED, _check_selected, NULL);

    /* Setup and show the velocity label and seeker */
    hbox = ewl_grid_new();
    ewl_container_child_append(EWL_CONTAINER(border_box), hbox);
    ewl_object_fill_policy_set(EWL_OBJECT(hbox), EWL_FLAG_FILL_FILL);
    ewl_grid_column_preferred_w_use(EWL_GRID(hbox), 1);
    ewl_widget_show(hbox);

    o = ewl_label_new();
    ewl_label_text_set(EWL_LABEL(o), _("velocity:"));
    ewl_container_child_append(EWL_CONTAINER(hbox), o);
    ewl_object_fill_policy_set(EWL_OBJECT(o), EWL_FLAG_FILL_NONE);
    ewl_object_alignment_set(EWL_OBJECT(o), EWL_FLAG_ALIGN_LEFT);
    ewl_widget_show(o);

    o = ewl_label_new();
    ewl_container_child_append(EWL_CONTAINER(hbox), o);
    ewl_object_fill_policy_set(EWL_OBJECT(o), EWL_FLAG_FILL_NONE);
    ewl_widget_show(o);
    config.vel_label = o;

    o = ewl_hseeker_new();
    config.velocity = ecore_config_int_get("velocity");
    ewl_range_minimum_value_set(EWL_RANGE(o), 200.0);
    ewl_range_maximum_value_set(EWL_RANGE(o), 800.0);
    ewl_range_step_set(EWL_RANGE(o), 60.0);
    ewl_range_value_set(EWL_RANGE(o), (double) config.velocity);
    ewl_widget_name_set(o, "velocity");
    ewl_container_child_append(EWL_CONTAINER(border_box), o);
    ewl_callback_append(o, EWL_CALLBACK_VALUE_CHANGED, _hseeker_cb, NULL);
                          _hseeker_cb(o, NULL, NULL);
    ewl_widget_show(o);

    /* Setup and show the frame_rate label and seeker */
    hbox = ewl_grid_new();
    ewl_container_child_append(EWL_CONTAINER(border_box), hbox);
    ewl_object_fill_policy_set(EWL_OBJECT(hbox), EWL_FLAG_FILL_ALL);
    ewl_grid_column_preferred_w_use(EWL_GRID(hbox), 1);
    ewl_widget_show(hbox);

    o = ewl_label_new();
    ewl_label_text_set(EWL_LABEL(o), _("frame rate:"));
    ewl_container_child_append(EWL_CONTAINER(hbox), o);
    ewl_object_fill_policy_set(EWL_OBJECT(o), EWL_FLAG_FILL_NONE);
    ewl_object_alignment_set(EWL_OBJECT(o), EWL_FLAG_ALIGN_LEFT);
    ewl_widget_show(o);
    
    o = ewl_label_new();
    ewl_container_child_append(EWL_CONTAINER(hbox), o);
    ewl_object_fill_policy_set(EWL_OBJECT(o), EWL_FLAG_FILL_NONE);
    ewl_widget_show(o);
    config.frt_label = o;

    o = ewl_hseeker_new();
    config.frame_rate = ecore_config_int_get("frame_rate");
    ewl_range_minimum_value_set(EWL_RANGE(o), 10.0);
    ewl_range_maximum_value_set(EWL_RANGE(o), 100.0);
    ewl_range_step_set(EWL_RANGE(o), 10.0);
    ewl_range_value_set(EWL_RANGE(o), (double) config.frame_rate);
    ewl_widget_name_set(o, "frame_rate");
    ewl_container_child_append(EWL_CONTAINER(border_box), o);
    ewl_callback_append(o, EWL_CALLBACK_VALUE_CHANGED, _hseeker_cb, NULL);
    _hseeker_cb(o, NULL, NULL);
    ewl_widget_show(o);

    /* *** Lazy Box *** */
    vbox = ewl_vbox_new();
    ewl_container_child_append(EWL_CONTAINER(main_box), vbox);
    ewl_object_fill_policy_set(EWL_OBJECT(vbox), EWL_FLAG_FILL_NORMAL);
    ewl_widget_show(vbox);

    border_box = ewl_border_new();
    ewl_container_child_append(EWL_CONTAINER(vbox), border_box);
    ewl_border_label_set(EWL_BORDER(border_box), _("Laziness"));
    ewl_widget_show(border_box);
    /* the radio buttons */
    config.lazy = ecore_config_int_get("lazy");
    radio_b[0] = ewl_radiobutton_new();
    ewl_button_label_set(EWL_BUTTON(radio_b[0]), _("normal mode"));
    ewl_object_alignment_set(EWL_OBJECT(radio_b[0]), EWL_FLAG_ALIGN_LEFT);
    ewl_container_child_append(EWL_CONTAINER(border_box), radio_b[0]);
    ewl_callback_append(radio_b[0], EWL_CALLBACK_CLICKED, _check_selected,
                        NULL);
    ewl_widget_show(radio_b[0]);

    radio_b[1] = ewl_radiobutton_new();
    ewl_button_label_set(EWL_BUTTON(radio_b[1]), _("lazy mode"));
    ewl_container_child_append(EWL_CONTAINER(border_box), radio_b[1]);
    ewl_object_alignment_set(EWL_OBJECT(radio_b[1]), EWL_FLAG_ALIGN_LEFT);
    ewl_radiobutton_chain_set(EWL_RADIOBUTTON(radio_b[1]),
                              EWL_RADIOBUTTON(radio_b[0]));
    ewl_callback_append(radio_b[1], EWL_CALLBACK_CLICKED, _check_selected,
                        NULL);
    ewl_widget_show(radio_b[1]);
    if (config.lazy)
        ewl_togglebutton_checked_set(EWL_TOGGLEBUTTON(radio_b[1]), 1);
    else
        ewl_togglebutton_checked_set(EWL_TOGGLEBUTTON(radio_b[0]), 1);

    /* *** Render Box *** */
    /* Setup and show the border box */
    border_box = ewl_border_new();
    ewl_border_label_set(EWL_BORDER(border_box), _("Render Engine"));
    ewl_container_child_append(EWL_CONTAINER(vbox), border_box);
    ewl_object_fill_policy_set(EWL_OBJECT(border_box), EWL_FLAG_FILL_FILL);
    ewl_object_alignment_set(EWL_OBJECT(border_box), EWL_FLAG_ALIGN_CENTER);
    ewl_object_alignment_set(EWL_OBJECT(main_box), EWL_FLAG_ALIGN_TOP);
    ewl_widget_show(border_box);

    o = ewl_text_new();
    ewl_text_text_set(EWL_TEXT(o), _("Changes only affect after new start"));
    ewl_container_child_append(EWL_CONTAINER(border_box), o);
    ewl_widget_show(o);

    /* the radio buttons */
    config.gl = ecore_config_boolean_get("/graphic/gl");
    radio_b[0] = ewl_radiobutton_new();
    ewl_button_label_set(EWL_BUTTON(radio_b[0]), _("Software"));
    ewl_container_child_append(EWL_CONTAINER(border_box), radio_b[0]);
    ewl_object_alignment_set(EWL_OBJECT(radio_b[0]), EWL_FLAG_ALIGN_LEFT);
    ewl_callback_append(radio_b[0], EWL_CALLBACK_CLICKED, _check_selected,
                        NULL);
    ewl_widget_show(radio_b[0]);

    radio_b[1] = ewl_radiobutton_new();
    ewl_button_label_set(EWL_BUTTON(radio_b[1]), _("OpenGL (testing)"));
    ewl_container_child_append(EWL_CONTAINER(border_box), radio_b[1]);
    ewl_object_alignment_set(EWL_OBJECT(radio_b[1]), EWL_FLAG_ALIGN_LEFT);
    ewl_radiobutton_chain_set(EWL_RADIOBUTTON(radio_b[1]),
                              EWL_RADIOBUTTON(radio_b[0]));
    ewl_callback_append(radio_b[1], EWL_CALLBACK_CLICKED, _check_selected,
                        NULL);
    ewl_widget_show(radio_b[1]);
    if (config.gl)
        ewl_togglebutton_checked_set(EWL_TOGGLEBUTTON(radio_b[1]), 1);
    else
        ewl_togglebutton_checked_set(EWL_TOGGLEBUTTON(radio_b[0]), 1);

}

static void destroy_cb(Ewl_Widget * w, void * event, void * data)
{
    ewl_widget_destroy(w);
    conf_win = NULL;
}

static void conf_win_clicked_cb(Ewl_Widget * w, void * event, void * data)
{
    Ewl_Stock_Type response;

    response = ewl_stock_type_get(EWL_STOCK(w));

    if (response == EWL_STOCK_OK || response == EWL_STOCK_APPLY) {
        ecore_config_boolean_set("/graphic/animations", config.animations);
        ecore_config_boolean_set("/graphic/shadows", config.shadows);
        ecore_config_boolean_set("/graphic/gl", config.gl);
        ecore_config_int_set("velocity", config.velocity);
        ecore_config_int_set("frame_rate", config.frame_rate);
        ecore_config_int_set("lazy", config.lazy);

        if (ecore_config_save() != ECORE_CONFIG_ERR_SUCC)
            fprintf(stderr,
                    _("Elitaire Error: Could not save configuration.\n"));
    }

    if (response == EWL_STOCK_OK || response == EWL_STOCK_CANCEL) {
        ewl_widget_destroy(EWL_WIDGET(data));
        conf_win = NULL;
    }
}

static void _check_selected(Ewl_Widget * w, void * event, void * data)
{
    const char * label;

    label = ewl_button_label_get(EWL_BUTTON(w));

    if (!strcmp(label, _("animated movements"))) {
        config.animations = ewl_togglebutton_checked_get(EWL_TOGGLEBUTTON(w));
    }
    else if (!strcmp(label, _("shadows"))) {
        config.shadows = ewl_togglebutton_checked_get(EWL_TOGGLEBUTTON(w));
    }
    else if (!strcmp(label, _("Software"))) {
        config.gl = !ewl_togglebutton_checked_get(EWL_TOGGLEBUTTON(w));
    }
    else if (!strcmp(label, _("OpenGL (testing)"))) {
        config.gl = ewl_togglebutton_checked_get(EWL_TOGGLEBUTTON(w));
    }
    else if (!strcmp(label, _("lazy mode"))) {
        config.lazy = ewl_togglebutton_checked_get(EWL_TOGGLEBUTTON(w));
    }
    else if (!strcmp(label, _("normal mode"))) {
        config.lazy = !ewl_togglebutton_checked_get(EWL_TOGGLEBUTTON(w));
    }
}

static void _hseeker_cb(Ewl_Widget * w, void * event, void * data)
{
    double value;
    char buffer[10];
    const char * name;

    value = ewl_range_value_get(EWL_RANGE(w));

    name = ewl_widget_name_get(w);
    if (!strcmp(name, "velocity")) {
        config.velocity = (int) value ;
        snprintf(buffer, 10, "%i", config.velocity);
        ewl_label_text_set(EWL_LABEL(config.vel_label), buffer);
    }
    else if (!strcmp(name, "frame_rate")) {
        config.frame_rate = (int) value ;
        snprintf(buffer, 10, "%i", config.frame_rate);
        ewl_label_text_set(EWL_LABEL(config.frt_label), buffer);
    }
}

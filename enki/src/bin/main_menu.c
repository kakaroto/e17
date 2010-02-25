
#include "main.h"

Evas_Object *main_menu_new(Evas_Object *parent)
{
    Evas_Object *ly, *bt;

    ly = elm_layout_add(parent);
    elm_layout_file_set(ly, PACKAGE_DATA_DIR"/theme.edj", "main_menu");
    evas_object_size_hint_weight_set(ly, -1.0, -1.0);
    evas_object_size_hint_align_set(ly, -1.0, -1.0);
    evas_object_show(ly);

    bt = elm_button_add(parent);
    evas_object_size_hint_weight_set(bt, -1.0, -1.0);
    evas_object_size_hint_align_set(bt, 0.5, 0.0);
    evas_object_show(bt);
    elm_layout_content_set(ly, "library", bt);
    elm_button_label_set(bt, D_("New Library"));

    return ly;
}



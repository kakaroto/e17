#include "main.h"

static Evas_Object *_pager;
static Evas_Object *_area, *_empty, *_image;

static Evas_Object *_bt_area_new, *_bt_areas_init;

static Evas_Object *_area_layout;
static Evas_Object *_image_file_entry, *_image_check_border, *_image_check_shadow, *_image_check_keep_aspect;

Evas_Object *rightpanel_create()
{
    Evas_Object *vbox, *bt, *fr, *bx, *bx2, *tb, *entry, *sc, *lbl, *ck, *hover;

    vbox = elm_box_add(win);
    evas_object_size_hint_weight_set(vbox, -1.0, -1.0);
    evas_object_size_hint_align_set(vbox, -1.0, -1.0);
    evas_object_show(vbox);

    bt = elm_button_add(win);
    _bt_area_new = bt;
    elm_button_label_set(bt, D_("Add a new area"));
    evas_object_size_hint_weight_set(bt, -1.0, -1.0);
    evas_object_size_hint_align_set(bt, -1.0, 0.0);
    evas_object_smart_callback_add(bt, "clicked", utils_edit_area_add, NULL);
    evas_object_show(bt);
    elm_box_pack_end(vbox, bt);

    bt = elm_button_add(win);
    _bt_areas_init = bt;
    elm_button_label_set(bt, D_("Initialise the default areas"));
    evas_object_size_hint_weight_set(bt, -1.0, -1.0);
    evas_object_size_hint_align_set(bt, -1.0, 0.0);
    evas_object_smart_callback_add(bt, "clicked", utils_edit_slide_default_areas_reinit, NULL);
    evas_object_show(bt);
    elm_box_pack_end(vbox, bt);

    //
    _pager = elm_pager_add(win);
    evas_object_size_hint_weight_set(_pager, 1.0, 1.0);
    evas_object_size_hint_align_set(_pager, -1.0, -1.0);
    elm_box_pack_end(vbox, _pager);
    evas_object_show(_pager);
    //

    //Empty
    bx = elm_box_add(win);
    _empty = bx;
    elm_pager_content_push(_pager, bx);

    //Area
    bx = elm_box_add(win);
    evas_object_size_hint_weight_set(bx, -1.0, 1.0);
    evas_object_size_hint_align_set(bx, -1.0, -1.0);
    evas_object_show(bx);

    fr = elm_frame_add(win);
    _area = fr;
    evas_object_size_hint_weight_set(bx, 1.0, 1.0);
    evas_object_size_hint_align_set(bx, -1.0, -1.0);
    elm_frame_label_set(fr, D_("Area"));
    elm_frame_content_set(fr, bx);
    evas_object_show(fr);

    bt = elm_button_add(win);
    elm_button_label_set(bt, D_("UP"));
    evas_object_size_hint_weight_set(bt, 0.0, 0.0);
    evas_object_size_hint_align_set(bt, -1.0, 0.0);
    evas_object_smart_callback_add(bt, "clicked", utils_edit_area_up, NULL);
    evas_object_show(bt);
    elm_box_pack_end(bx, bt);

    bt = elm_button_add(win);
    elm_button_label_set(bt, D_("DOWN"));
    evas_object_size_hint_weight_set(bt, -1.0, -1.0);
    evas_object_size_hint_align_set(bt, -1.0, 0.0);
    evas_object_smart_callback_add(bt, "clicked", utils_edit_area_down, NULL);
    evas_object_show(bt);
    elm_box_pack_end(bx,bt);

    hover = elm_hoversel_add(win);
    _area_layout = hover;
    elm_hoversel_hover_parent_set(hover,win);
    elm_hoversel_label_set(hover, "no label");
    evas_object_size_hint_weight_set(hover, -1.0, -1.0);
    evas_object_size_hint_align_set(hover, -1.0, 0.0);
    elm_hoversel_item_add(hover, D_("Vertical"), NULL, ELM_ICON_NONE, utils_edit_area_layout_vertical_set, NULL);
    elm_hoversel_item_add(hover, D_("Horizontal"), NULL, ELM_ICON_NONE, utils_edit_area_layout_horizontal_set, NULL);
    elm_hoversel_item_add(hover, D_("Vertical homogeneous"), NULL, ELM_ICON_NONE, utils_edit_area_layout_vertical_homogeneous_set, NULL);
    elm_hoversel_item_add(hover, D_("Vertical flow"), NULL, ELM_ICON_NONE, utils_edit_area_layout_vertical_flow_set, NULL);
    elm_hoversel_item_add(hover, D_("Horizontal homogeneous"), NULL, ELM_ICON_NONE, utils_edit_area_layout_horizontal_homogeneous_set, NULL);
    elm_hoversel_item_add(hover, D_("Horizontal flow"), NULL, ELM_ICON_NONE, utils_edit_area_layout_horizontal_flow_set, NULL);
    elm_hoversel_item_add(hover, D_("Stack"), NULL, ELM_ICON_NONE, utils_edit_area_layout_stack_set, NULL);
    evas_object_show(hover);
    elm_box_pack_end(bx,hover);

    bt = elm_button_add(win);
    elm_button_label_set(bt, D_("Add an image"));
    evas_object_size_hint_weight_set(bt, -1.0, -1.0);
    evas_object_size_hint_align_set(bt, -1.0, 0.0);
    evas_object_smart_callback_add(bt, "clicked", utils_edit_area_image_add, NULL);
    evas_object_show(bt);
    elm_box_pack_end(bx,bt);

    bx2 = elm_box_add(win);
    evas_object_size_hint_weight_set(bx2, -1.0, 1.0);
    evas_object_size_hint_align_set(bx2, -1.0, -1.0);
    evas_object_show(bx2);
    elm_box_pack_end(bx, bx2);

    bt = elm_button_add(win);
    elm_button_label_set(bt, D_("Delete the area"));
    evas_object_color_set(bt, 255, 0, 0, 255);
    evas_object_size_hint_weight_set(bt, -1.0, -1.0);
    evas_object_size_hint_align_set(bt, -1.0, 0.0);
    evas_object_smart_callback_add(bt, "clicked", utils_edit_area_delete, NULL);
    evas_object_show(bt);
    elm_box_pack_end(bx, bt);

    elm_pager_content_push(_pager, fr);
    //

    //Image
    bx = elm_box_add(win);
    evas_object_size_hint_weight_set(bx, -1.0, 1.0);
    evas_object_size_hint_align_set(bx, -1.0, -1.0);
    evas_object_show(bx);

    fr = elm_frame_add(win);
    _image = fr;
    evas_object_size_hint_weight_set(bx, 1.0, 1.0);
    evas_object_size_hint_align_set(bx, -1.0, -1.0);
    elm_frame_label_set(fr, D_("Image"));
    elm_frame_content_set(fr, bx);
    evas_object_show(fr);

    tb = elm_table_add(win);
    evas_object_size_hint_weight_set(tb, -1.0, 1.0);
    evas_object_size_hint_align_set(tb, -1.0, -1.0);
    evas_object_show(tb);
    elm_box_pack_end(bx, tb);

    bt = elm_button_add(win);
    elm_button_label_set(bt, D_("UP"));
    evas_object_size_hint_weight_set(bt, 0.0, 0.0);
    evas_object_size_hint_align_set(bt, -1.0, 0.0);
    evas_object_smart_callback_add(bt, "clicked", utils_edit_object_up, NULL);
    evas_object_show(bt);
    elm_table_pack(tb, bt, 0, 0, 2, 1);

    bt = elm_button_add(win);
    elm_button_label_set(bt, D_("DOWN"));
    evas_object_size_hint_weight_set(bt, -1.0, -1.0);
    evas_object_size_hint_align_set(bt, -1.0, 0.0);
    evas_object_smart_callback_add(bt, "clicked", utils_edit_object_down, NULL);
    evas_object_show(bt);
    elm_table_pack(tb, bt, 0, 1, 2, 1);

    bt = elm_button_add(win);
    elm_button_label_set(bt, D_("Image : "));
    evas_object_size_hint_weight_set(bt, -1.0, -1.0);
    evas_object_size_hint_align_set(bt, -1.0, 0.0);
    evas_object_smart_callback_add(bt, "clicked", utils_edit_image_file_change, NULL);
    evas_object_show(bt);
    elm_table_pack(tb, bt, 0, 2, 1, 1);

    entry = elm_entry_add(win);
    _image_file_entry = entry;
    elm_entry_editable_set(entry, 0);
    elm_entry_entry_set(entry, "tesgq");
    evas_object_size_hint_weight_set(entry, -1.0, -1.0);
    evas_object_size_hint_align_set(entry, -1.0, 0.0);
    evas_object_show(entry);

    sc = elm_scroller_add(win);
    elm_table_pack(tb, sc, 1, 2, 1,1);
    evas_object_size_hint_weight_set(sc, 1.0, 0.0);
    evas_object_size_hint_align_set(sc, -1.0, 0.5);
    elm_scroller_content_min_limit(sc, 0, 1);
    elm_scroller_policy_set(sc, ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_OFF);
    elm_scroller_content_set(sc, entry);
    evas_object_show(sc);

    lbl = elm_label_add(win);
    elm_label_label_set(lbl, D_("Border : "));
    evas_object_show(lbl);
    elm_table_pack(tb, lbl, 0, 3, 1, 1);
    ck = elm_check_add(win);
    _image_check_border = ck;
    evas_object_show(ck);
    evas_object_smart_callback_add(ck, "changed", utils_edit_image_border_change, NULL);
    elm_table_pack(tb, ck, 1, 3, 1, 1);

    lbl = elm_label_add(win);
    elm_label_label_set(lbl, D_("Shadow : "));
    evas_object_show(lbl);
    elm_table_pack(tb, lbl, 0, 4, 1, 1);
    ck = elm_check_add(win);
    _image_check_shadow = ck;
    evas_object_show(ck);
    evas_object_smart_callback_add(ck, "changed", utils_edit_image_shadow_change, NULL);
    elm_table_pack(tb, ck, 1, 4, 1, 1);

    lbl = elm_label_add(win);
    elm_label_label_set(lbl, D_("Keep aspect : "));
    evas_object_show(lbl);
    elm_table_pack(tb, lbl, 0, 5, 1, 1);
    ck = elm_check_add(win);
    _image_check_keep_aspect = ck;
    evas_object_show(ck);
    evas_object_smart_callback_add(ck, "changed", utils_edit_image_keep_aspect_change, NULL);
    elm_table_pack(tb, ck, 1, 5, 1, 1);



    bx2 = elm_box_add(win);
    evas_object_size_hint_weight_set(bx2, -1.0, 1.0);
    evas_object_size_hint_align_set(bx2, -1.0, -1.0);
    evas_object_show(bx2);
    elm_box_pack_end(bx, bx2);

    bt = elm_button_add(win);
    elm_button_label_set(bt, D_("Delete the image"));
    evas_object_color_set(bt, 255, 0, 0, 255);
    evas_object_size_hint_weight_set(bt, -1.0, -1.0);
    evas_object_size_hint_align_set(bt, -1.0, 0.0);
    evas_object_smart_callback_add(bt, "clicked", utils_edit_object_delete, NULL);
    evas_object_show(bt);
    elm_box_pack_end(bx, bt);

    elm_pager_content_push(_pager, fr);
    //
    elm_pager_content_promote(_pager, _empty);

    return vbox;
}

void rightpanel_noslide_disabled_set(Eina_Bool disabled)
{
    elm_object_disabled_set(_bt_area_new, disabled);
    elm_object_disabled_set(_bt_areas_init, disabled);
}

void rightpanel_area_show()
{
    elm_pager_content_promote(_pager, _area);
}

void rightpanel_empty_show()
{
    elm_pager_content_promote(_pager, _empty);
}

void rightpanel_image_show()
{
    elm_pager_content_promote(_pager, _image);
}

void rightpanel_area_layout_set(const char *layout)
{
    elm_hoversel_label_set(_area_layout, layout);
}

void rightpanel_image_data_set(const char* file, int border, int shadow, int keep_aspect)
{
    if(file)
        elm_entry_entry_set(_image_file_entry, file);
    if(border > -1)
        elm_check_state_set(_image_check_border, border);
    if(shadow > -1)
        elm_check_state_set(_image_check_shadow, shadow);
    if(keep_aspect > -1)
        elm_check_state_set(_image_check_keep_aspect, keep_aspect);
}


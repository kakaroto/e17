/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "Ewl_Test.h"
#include "ewl_test_private.h"
#include "ewl_border.h"
#include "ewl_label.h"
#include "ewl_radiobutton.h"
#include "ewl_separator.h"

#include <stdio.h>
#include <string.h>

extern Ewl_Unit_Test border_unit_tests[];

static int create_test(Ewl_Container *box);

static Ewl_Widget *button_aleft, *button_acenter;
static Ewl_Widget *button_aright, *button_atop, *button_abottom;
static Ewl_Widget *button_pleft, *button_pright, *button_ptop, *button_pbottom;

static void border_change_alignment(Ewl_Widget *w, void *ev, void *data);
static void border_change_position(Ewl_Widget *w, void *ev, void *data);
static void checkbutton_cb(Ewl_Widget *w, void *ev, void *data);

void
test_info(Ewl_Test *test)
{
        test->name = "Border";
        test->tip = "Defines the Ewl_Border class for displaying\n"
                        "a container with a label.";
        test->filename = __FILE__;
        test->func = create_test;
        test->type = EWL_TEST_TYPE_CONTAINER;
        test->unit_tests = border_unit_tests;
}

static int
create_test(Ewl_Container *box)
{
        Ewl_Widget *border;
        Ewl_Widget *border_box;
        Ewl_Widget *separator;
        Ewl_Widget *alabel, *avbox, *pvbox, *plabel, *cvbox, *clabel;
        Ewl_Widget *button_check;

        border = ewl_border_new();
        ewl_border_label_set(EWL_BORDER(border), "Box Title");
        ewl_container_child_append(EWL_CONTAINER(box), border);
        ewl_widget_show(border);

        border_box = ewl_hbox_new();
        ewl_container_child_append(EWL_CONTAINER(border), border_box);
        ewl_widget_show(border_box);

        avbox = ewl_vbox_new();
        ewl_object_alignment_set(EWL_OBJECT(avbox), EWL_FLAG_ALIGN_CENTER);
        ewl_container_child_append(EWL_CONTAINER(border_box), avbox);
        ewl_widget_show(avbox);

        alabel = ewl_label_new();
        ewl_label_text_set(EWL_LABEL(alabel), "Tabs Alignment");
        ewl_container_child_append(EWL_CONTAINER(avbox), alabel);
        ewl_widget_show(alabel);

        button_atop = ewl_radiobutton_new();
        ewl_button_label_set(EWL_BUTTON(button_atop), "Top");
        ewl_container_child_append(EWL_CONTAINER(avbox), button_atop);
        ewl_callback_append(button_atop, EWL_CALLBACK_VALUE_CHANGED,
                            border_change_alignment, border);
        ewl_widget_show(button_atop);

        button_aleft = ewl_radiobutton_new();
        ewl_button_label_set(EWL_BUTTON(button_aleft), "Left");
        ewl_checkbutton_checked_set(EWL_CHECKBUTTON(button_aleft), TRUE);
        ewl_container_child_append(EWL_CONTAINER(avbox), button_aleft);
        ewl_radiobutton_chain_set(EWL_RADIOBUTTON(button_aleft),
                                  EWL_RADIOBUTTON(button_atop));
        ewl_callback_append(button_aleft, EWL_CALLBACK_VALUE_CHANGED,
                            border_change_alignment, border);
        ewl_widget_show(button_aleft);

        button_acenter = ewl_radiobutton_new();
        ewl_button_label_set(EWL_BUTTON(button_acenter), "Center");
        ewl_radiobutton_chain_set(EWL_RADIOBUTTON(button_acenter),
                                  EWL_RADIOBUTTON(button_aleft));
        ewl_container_child_append(EWL_CONTAINER(avbox), button_acenter);
        ewl_callback_append(button_acenter, EWL_CALLBACK_VALUE_CHANGED,
                            border_change_alignment, border);
        ewl_widget_show(button_acenter);

        button_aright = ewl_radiobutton_new();
        ewl_button_label_set(EWL_BUTTON(button_aright), "Right");
        ewl_radiobutton_chain_set(EWL_RADIOBUTTON(button_aright),
                                  EWL_RADIOBUTTON(button_acenter));
        ewl_container_child_append(EWL_CONTAINER(avbox), button_aright);
        ewl_callback_append(button_aright, EWL_CALLBACK_VALUE_CHANGED,
                            border_change_alignment, border);
        ewl_widget_show(button_aright);

        button_abottom = ewl_radiobutton_new();
        ewl_button_label_set(EWL_BUTTON(button_abottom), "Bottom");
        ewl_radiobutton_chain_set(EWL_RADIOBUTTON(button_abottom),
                                  EWL_RADIOBUTTON(button_aright));
        ewl_container_child_append(EWL_CONTAINER(avbox), button_abottom);
        ewl_callback_append(button_abottom, EWL_CALLBACK_VALUE_CHANGED,
                            border_change_alignment, border);
        ewl_widget_show(button_abottom);

        separator = ewl_vseparator_new();
        ewl_container_child_append(EWL_CONTAINER(border_box), separator);
        ewl_widget_show(separator);

        pvbox = ewl_vbox_new();
        ewl_object_alignment_set(EWL_OBJECT(pvbox), EWL_FLAG_ALIGN_CENTER);
        ewl_container_child_append(EWL_CONTAINER(border_box), pvbox);
        ewl_widget_show(pvbox);

        plabel = ewl_label_new();
        ewl_label_text_set(EWL_LABEL(plabel), "Tabs Position");
        ewl_container_child_append(EWL_CONTAINER(pvbox), plabel);
        ewl_widget_show(plabel);

        button_pleft = ewl_radiobutton_new();
        ewl_button_label_set(EWL_BUTTON(button_pleft), "Left");
        ewl_container_child_append(EWL_CONTAINER(pvbox), button_pleft);
        ewl_callback_append(button_pleft, EWL_CALLBACK_VALUE_CHANGED,
                            border_change_position, border);
        ewl_widget_show(button_pleft);

        button_pright = ewl_radiobutton_new();
        ewl_button_label_set(EWL_BUTTON(button_pright), "Right");
        ewl_radiobutton_chain_set(EWL_RADIOBUTTON(button_pright),
                                  EWL_RADIOBUTTON(button_pleft));
        ewl_container_child_append(EWL_CONTAINER(pvbox), button_pright);
        ewl_callback_append(button_pright, EWL_CALLBACK_VALUE_CHANGED,
                            border_change_position, border);
        ewl_widget_show(button_pright);

        button_ptop = ewl_radiobutton_new();
        ewl_button_label_set(EWL_BUTTON(button_ptop), "Top");
        ewl_checkbutton_checked_set(EWL_CHECKBUTTON(button_ptop), TRUE);
        ewl_radiobutton_chain_set(EWL_RADIOBUTTON(button_ptop),
                                  EWL_RADIOBUTTON(button_pright));
        ewl_container_child_append(EWL_CONTAINER(pvbox), button_ptop);
        ewl_callback_append(button_ptop, EWL_CALLBACK_VALUE_CHANGED,
                            border_change_position, border);
        ewl_widget_show(button_ptop);

        button_pbottom = ewl_radiobutton_new();
        ewl_button_label_set(EWL_BUTTON(button_pbottom), "Bottom");
        ewl_radiobutton_chain_set(EWL_RADIOBUTTON(button_pbottom),
                                  EWL_RADIOBUTTON(button_ptop));
        ewl_container_child_append(EWL_CONTAINER(pvbox), button_pbottom);
        ewl_callback_append(button_pbottom, EWL_CALLBACK_VALUE_CHANGED,
                            border_change_position, border);
        ewl_widget_show(button_pbottom);

        cvbox = ewl_vbox_new();
        ewl_object_alignment_set(EWL_OBJECT(cvbox), EWL_FLAG_ALIGN_CENTER);
        ewl_container_child_append(EWL_CONTAINER(box), cvbox);
        ewl_widget_show(cvbox);

        clabel = ewl_label_new();
        ewl_label_text_set(EWL_LABEL(clabel), "Enable/Disable");
        ewl_container_child_append(EWL_CONTAINER(cvbox), clabel);
        ewl_widget_show(clabel);

        button_check = ewl_checkbutton_new();
        ewl_button_label_set(EWL_BUTTON(button_check), "Disable");
        ewl_container_child_append(EWL_CONTAINER(cvbox), button_check);
        ewl_callback_append(button_check, EWL_CALLBACK_CLICKED,
                            checkbutton_cb, border);
        ewl_widget_show(button_check);

        return 1;
}

static void
border_change_alignment(Ewl_Widget *w, void *ev __UNUSED__,
                                        void *data __UNUSED__)
{
        if (!ewl_checkbutton_is_checked(EWL_CHECKBUTTON(w)))
                return;

        if (w == button_aleft)
                ewl_border_label_alignment_set(data, EWL_FLAG_ALIGN_LEFT);

        else if (w == button_acenter)
                ewl_border_label_alignment_set(data, EWL_FLAG_ALIGN_CENTER);

        else if (w == button_aright)
                ewl_border_label_alignment_set(data, EWL_FLAG_ALIGN_RIGHT);

        else if (w == button_atop)
                ewl_border_label_alignment_set(data, EWL_FLAG_ALIGN_TOP);

        else if (w == button_abottom)
                ewl_border_label_alignment_set(data, EWL_FLAG_ALIGN_BOTTOM);

        return;
}

static void
border_change_position(Ewl_Widget *w, void *ev __UNUSED__,
                                        void *data __UNUSED__)
{
        if (!ewl_checkbutton_is_checked(EWL_CHECKBUTTON(w)))
                return;

        if (w == button_pleft)
                ewl_border_label_position_set(data, EWL_POSITION_LEFT);

        else if (w == button_pright)
                ewl_border_label_position_set(data, EWL_POSITION_RIGHT);

        else if (w == button_ptop)
                ewl_border_label_position_set(data, EWL_POSITION_TOP);

        else if (w == button_pbottom)
                ewl_border_label_position_set(data, EWL_POSITION_BOTTOM);
}

static void
checkbutton_cb(Ewl_Widget *w, void *ev __UNUSED__, void *data)
{
        if (ewl_checkbutton_is_checked(EWL_CHECKBUTTON(w)))
        {
                ewl_widget_disable(EWL_WIDGET(data));
                ewl_button_label_set(EWL_BUTTON(w), "Enable");
        }
        else if (!ewl_checkbutton_is_checked(EWL_CHECKBUTTON(w)))
        {
                ewl_widget_enable(EWL_WIDGET(data));
                ewl_button_label_set(EWL_BUTTON(w), "Disable");
        }
}

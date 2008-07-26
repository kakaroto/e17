/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "Ewl_Test.h"
#include "ewl_test_private.h"
#include "ewl_border.h"
#include "ewl_label.h"
#include "ewl_radiobutton.h"
#include "ewl_separator.h"

#include <stdio.h>
#include <string.h>

static Ewl_Widget *button_aleft, *button_acenter;
static Ewl_Widget *button_aright, *button_atop, *button_abottom;
static Ewl_Widget *button_pleft, *button_pright, *button_ptop, *button_pbottom;

static int create_test(Ewl_Container *box);
static void border_change_alignment(Ewl_Widget *w, void *ev, void *data);
static void border_change_position(Ewl_Widget *w, void *ev, void *data);
static void checkbutton_cb(Ewl_Widget *w, void *ev, void *data);

/* unit tests */
static int constructor_test(char *buf, int len);
static int label_set_get_test(char *buf, int len);
static int label_position_set_get_test(char *buf, int len);
static int label_alignment_set_get_test(char *buf, int len);

static Ewl_Unit_Test border_unit_tests[] = {
                {"constructor", constructor_test, NULL, -1, 0},
                {"Border label set/get", label_set_get_test, NULL, -1, 0},
                {"Border label position set/get", label_position_set_get_test, NULL, -1, 0},
                {"Border label alignment set/get", label_alignment_set_get_test, NULL, -1, 0},
                {NULL, NULL, NULL, -1, 0}
        };

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

static int
constructor_test(char *buf, int len)
{
        Ewl_Widget *b;
        int ret = 0;

        b = ewl_border_new();

        if (!EWL_BORDER_IS(b))
        {
                LOG_FAILURE(buf, len, "returned border widget  is not of the"
                               " type border");
                goto DONE;
        }
        if (!!strcmp(ewl_widget_appearance_get(b), EWL_BORDER_TYPE))
        {
                LOG_FAILURE(buf, len, "border has wrong appearance");
                goto DONE;
        }
        if (!ewl_border_label_position_get(EWL_BORDER(b)) == EWL_POSITION_TOP)
        {
                LOG_FAILURE(buf, len, "the label position of the border "
                                "is wrong");
                goto DONE;
        }
        if (ewl_widget_focusable_get(b))
        {
                LOG_FAILURE(buf, len, "border widget is focusable");
                goto DONE;
        }

        ret = 1;
DONE:
        ewl_widget_destroy(b);

        return ret;
}

static int label_set_get_test(char *buf, int len)
{
        Ewl_Widget *border;
        int ret = 0;

        border = ewl_border_new();
        if (ewl_border_label_get(EWL_BORDER(border)))
                LOG_FAILURE(buf, len, "default border label set");
        else {
                const char *label;

                ewl_border_label_set(EWL_BORDER(border), "label value");

                label = ewl_border_label_get(EWL_BORDER(border));
                if (label && !strcmp(label, "label value"))
                        ret = 1;
                else
                        LOG_FAILURE(buf, len, "border label doesn't match");
        }

        ewl_widget_destroy(border);

        return ret;
}

static int label_position_set_get_test(char *buf, int len)
{
        Ewl_Widget *border;
        unsigned int pos;
        int ret = 0;

        border = ewl_border_new();
        pos = ewl_border_label_position_get(EWL_BORDER(border));
        if (pos != EWL_POSITION_TOP) {
                LOG_FAILURE(buf, len, "default border label position wrong");
                goto POSITION_ERROR;
        }

        ewl_border_label_position_set(EWL_BORDER(border), EWL_POSITION_LEFT);
        pos = ewl_border_label_position_get(EWL_BORDER(border));
        if (pos != EWL_POSITION_LEFT) {
                LOG_FAILURE(buf, len, "border label position not left");
                goto POSITION_ERROR;
        }

        ewl_border_label_position_set(EWL_BORDER(border), EWL_POSITION_RIGHT);
        pos = ewl_border_label_position_get(EWL_BORDER(border));
        if (pos != EWL_POSITION_RIGHT) {
                LOG_FAILURE(buf, len, "border label position not right");
                goto POSITION_ERROR;
        }

        ewl_border_label_position_set(EWL_BORDER(border), EWL_POSITION_BOTTOM);
        pos = ewl_border_label_position_get(EWL_BORDER(border));
        if (pos != EWL_POSITION_BOTTOM) {
                LOG_FAILURE(buf, len, "border label position not bottom");
                goto POSITION_ERROR;
        }

        ewl_border_label_position_set(EWL_BORDER(border), EWL_POSITION_TOP);
        pos = ewl_border_label_position_get(EWL_BORDER(border));
        if (pos != EWL_POSITION_TOP) {
                LOG_FAILURE(buf, len, "border label position not top");
                goto POSITION_ERROR;
        }

        ret = 1;

POSITION_ERROR:
        ewl_widget_destroy(border);

        return ret;
}

static int label_alignment_set_get_test(char *buf, int len)
{
        Ewl_Widget *border;
        int align;
        int req_align;
        int ret = 0;

        border = ewl_border_new();
        align = ewl_border_label_alignment_get(EWL_BORDER(border));
        if (align != EWL_FLAG_ALIGN_LEFT) {
                LOG_FAILURE(buf, len, "default border label alignment wrong");
                goto POSITION_ERROR;
        }

        req_align = (EWL_FLAG_ALIGN_LEFT | EWL_FLAG_ALIGN_RIGHT |
                                EWL_FLAG_ALIGN_BOTTOM | EWL_FLAG_ALIGN_TOP |
                                EWL_FLAG_ALIGN_CENTER);

        while (req_align >= 0) {

                ewl_border_label_alignment_set(EWL_BORDER(border), req_align);
                align = ewl_border_label_alignment_get(EWL_BORDER(border));
                if (align != req_align) {
                        LOG_FAILURE(buf, len,
                                        "border label alignment %x does not "
                                        "match requested %x", align, req_align);
                        goto POSITION_ERROR;
                }

                --req_align;
        }

        ewl_border_label_alignment_set(EWL_BORDER(border),
                        EWL_FLAG_ALIGN_RIGHT);
        align = ewl_border_label_alignment_get(EWL_BORDER(border));
        if (align != EWL_FLAG_ALIGN_RIGHT) {
                LOG_FAILURE(buf, len, "border label alignment not right");
                goto POSITION_ERROR;
        }

        ewl_border_label_alignment_set(EWL_BORDER(border),
                        EWL_FLAG_ALIGN_BOTTOM);
        align = ewl_border_label_alignment_get(EWL_BORDER(border));
        if (align != EWL_FLAG_ALIGN_BOTTOM) {
                LOG_FAILURE(buf, len, "border label alignment not bottom");
                goto POSITION_ERROR;
        }

        ewl_border_label_alignment_set(EWL_BORDER(border),
                        EWL_FLAG_ALIGN_TOP);
        align = ewl_border_label_alignment_get(EWL_BORDER(border));
        if (align != EWL_FLAG_ALIGN_TOP) {
                LOG_FAILURE(buf, len, "border label alignment not top");
                goto POSITION_ERROR;
        }

        ret = 1;

POSITION_ERROR:
        ewl_widget_destroy(border);

        return ret;
}

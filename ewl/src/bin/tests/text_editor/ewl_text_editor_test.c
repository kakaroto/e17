/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "Ewl_Test.h"
#include "ewl_test_private.h"
#include "ewl_combo.h"
#include "ewl_entry.h"
#include "ewl_icon.h"
#include "ewl_icon_theme.h"
#include "ewl_label.h"
#include "ewl_scrollpane.h"
#include "ewl_spacer.h"
#include "ewl_spinner.h"
#include "ewl_text_trigger.h"
#include "ewl_toolbar.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct
{
        char *name;
        void (*cb)(Ewl_Widget *w, void *ev, void *data);
} Ewl_Text_Editor_Control;

static char *txt = "I'm not a lumberjack, or a fur trader,\n"
                "I don't live in an igloo,\n"
                "Or eat blubber,\n"
                "Or own a dogsled,\n"
                "And I don't know Jimmy, Sally, or Suzie from Canada,"
                "although I'm certain they're really really nice,\n"
                "I have a Prime Minister, not a President,\n"
                "I speak English and French, not American,\n"
                "And I pronounce it about, not aboot,\n"
                "I can proudly sew my country's flag on my backpack,\n"
                "I believe in peacekeeping, not policing,\n"
                "diversity, not assimilation,\n"
                "and that the Beaver is a truly proud and nobel animal,\n"
                "a touque is a hat,\n"
                "a chesterfield is a couch,\n"
                "and it is pronounced Zed, not Zee, Zed,\n"
                "Canada is the 2nd largest land mass,\n"
                "the first nation of hockey,\n"
                "and the best part of North America,\n"
                "My name is Joe,\n"
                "and I AM CANADIAN!!!!!";

static int create_test(Ewl_Container *box);

static void ete_style_apply(Ewl_Text *t, Ewl_Text_Style s);
static void ete_align_apply(unsigned int align);
static void ete_font_apply(char *font);

static void ete_cb_styles_changed(Ewl_Widget *w, void *ev, void *data);
static void ete_cb_strikethrough(Ewl_Widget *w, void *ev, void *data);
static void ete_cb_underline(Ewl_Widget *w, void *ev, void *data);
static void ete_cb_justify_left(Ewl_Widget *w, void *ev, void *data);
static void ete_cb_justify_center(Ewl_Widget *w, void *ev, void *data);
static void ete_cb_justify_right(Ewl_Widget *w, void *ev, void *data);
static void ete_cb_font_size(Ewl_Widget *w, void *ev, void *data);
static void ete_cb_bold(Ewl_Widget *w, void *ev, void *data);
static void ete_cb_italic(Ewl_Widget *w, void *ev, void *data);

static void ete_cb_fetch(Ewl_Widget *w, void *ev, void *data);
static void ete_cb_set(Ewl_Widget *w, void *ev, void *data);
static void ete_cb_load(Ewl_Widget *w, void *ev, void *data);
static void ete_cb_clear(Ewl_Widget *w, void *ev, void *data);

void
test_info(Ewl_Test *test)
{
        test->name = "Text Editor";
        test->tip = "Defines a class for multi-line text layout\n"
                        "and formatting.";
        test->filename = __FILE__;
        test->func = create_test;
        test->type = EWL_TEST_TYPE_ADVANCED;
}

static int
create_test(Ewl_Container *box)
{
        Ewl_Widget *hbox, *scroll, *o;

        hbox = ewl_htoolbar_new();
        ewl_container_child_append(EWL_CONTAINER(box), hbox);
        ewl_toolbar_icon_part_hide(EWL_TOOLBAR(hbox), EWL_ICON_PART_LABEL);
        ewl_widget_show(hbox);

        {
                int i;
                Ewl_Model *model;
                Ewl_View *view;
                Ecore_List *styles;

                struct
                {
                        char *icon;
                        char *tooltip;
                        void (*cb)(Ewl_Widget *w, void *ev, void *data);
                } format[] = {
                        {EWL_ICON_FORMAT_JUSTIFY_LEFT, "Left align",
                                                        ete_cb_justify_left},
                        {EWL_ICON_FORMAT_JUSTIFY_CENTER, "Center",
                                                        ete_cb_justify_center},
                        {EWL_ICON_FORMAT_JUSTIFY_RIGHT, "Right align",
                                                        ete_cb_justify_right},
                        {EWL_ICON_FORMAT_TEXT_BOLD, "Bold", ete_cb_bold},
                        {EWL_ICON_FORMAT_TEXT_ITALIC, "Italic", ete_cb_italic},
                        {EWL_ICON_FORMAT_TEXT_UNDERLINE, "Underline",
                                                        ete_cb_underline},
                        {EWL_ICON_FORMAT_TEXT_STRIKETHROUGH, "Strikethrough",
                                                        ete_cb_strikethrough},
                        {NULL, NULL, NULL}
                };

                styles = ecore_list_new();
                ecore_list_append(styles, "None");
                ecore_list_append(styles, "Shadow");
                ecore_list_append(styles, "Soft Shadow");
                ecore_list_append(styles, "Far Shadow");
                ecore_list_append(styles, "Glow");
                ecore_list_append(styles, "Outline");

                for (i = 0; format[i].icon != NULL; i++)
                {
                        const char *path;

                        path = ewl_icon_theme_icon_path_get(format[i].icon,
                                                        EWL_ICON_SIZE_SMALL);

                        o = ewl_icon_simple_new();
                        ewl_icon_image_set(EWL_ICON(o), path,
                                                        format[i].icon);
                        ewl_icon_label_set(EWL_ICON(o), format[i].tooltip);
                        ewl_container_child_append(EWL_CONTAINER(hbox), o);
                        ewl_object_fill_policy_set(EWL_OBJECT(o),
                                                        EWL_FLAG_FILL_VFILL);
                        ewl_callback_append(o, EWL_CALLBACK_CLICKED,
                                                        format[i].cb, NULL);
                        ewl_attach_tooltip_text_set(o, format[i].tooltip);
                        ewl_widget_show(o);
                }

                o = ewl_spacer_new();
                ewl_container_child_append(EWL_CONTAINER(hbox), o);
                ewl_widget_show(o);

                o = ewl_label_new();
                ewl_container_child_append(EWL_CONTAINER(hbox), o);
                ewl_object_fill_policy_set(EWL_OBJECT(o), EWL_FLAG_FILL_NONE);
                ewl_object_alignment_set(EWL_OBJECT(o), EWL_FLAG_ALIGN_LEFT);
                ewl_label_text_set(EWL_LABEL(o), "Font Style");
                ewl_widget_show(o);

                model = ewl_model_ecore_list_instance();
                view = ewl_label_view_get();
                o = ewl_combo_new();
                ewl_mvc_model_set(EWL_MVC(o), model);
                ewl_mvc_view_set(EWL_MVC(o), view);
                ewl_mvc_data_set(EWL_MVC(o), styles);
                ewl_mvc_selected_set(EWL_MVC(o), NULL,
                                ewl_mvc_data_get(EWL_MVC(o)), 0, 0);
                ewl_container_child_append(EWL_CONTAINER(hbox), o);
                ewl_callback_append(o, EWL_CALLBACK_VALUE_CHANGED,
                                                ete_cb_styles_changed, NULL);
                ewl_object_fill_policy_set(EWL_OBJECT(o), EWL_FLAG_FILL_HFILL);
                ewl_widget_show(o);

                o = ewl_label_new();
                ewl_container_child_append(EWL_CONTAINER(hbox), o);
                ewl_object_fill_policy_set(EWL_OBJECT(o), EWL_FLAG_FILL_NONE);
                ewl_object_alignment_set(EWL_OBJECT(o), EWL_FLAG_ALIGN_LEFT);
                ewl_label_text_set(EWL_LABEL(o), "Font Size");
                ewl_widget_show(o);

                o = ewl_spinner_new();
                ewl_container_child_append(EWL_CONTAINER(hbox), o);
                ewl_object_fill_policy_set(EWL_OBJECT(o), EWL_FLAG_FILL_NONE);
                ewl_range_value_set(EWL_RANGE(o), 12);
                ewl_range_minimum_value_set(EWL_RANGE(o), 8);
                ewl_range_maximum_value_set(EWL_RANGE(o), 72);
                ewl_range_step_set(EWL_RANGE(o), 1);
                ewl_spinner_digits_set(EWL_SPINNER(o), 0);
                ewl_callback_append(o, EWL_CALLBACK_VALUE_CHANGED,
                                        ete_cb_font_size, NULL);
                ewl_widget_show(o);
        }

        scroll = ewl_scrollpane_new();
        ewl_scrollport_hscrollbar_flag_set(EWL_SCROLLPORT(scroll),
                                        EWL_SCROLLPORT_FLAG_ALWAYS_HIDDEN);
        ewl_container_child_append(EWL_CONTAINER(box), scroll);
        ewl_widget_name_set(scroll, "scrollpane");
        ewl_widget_show(scroll);

        o = ewl_entry_new();
        ewl_object_fill_policy_set(EWL_OBJECT(o), EWL_FLAG_FILL_FILL);
        ewl_container_child_append(EWL_CONTAINER(scroll), o);
        ewl_widget_name_set(o, "entry");
        ewl_entry_multiline_set(EWL_ENTRY(o), TRUE);
        ewl_text_wrap_set(EWL_TEXT(o), EWL_TEXT_WRAP_WORD);
        ewl_text_font_size_set(EWL_TEXT(o), 12);

        ewl_text_underline_color_set(EWL_TEXT(o), 0, 0, 0, 255);
        ewl_text_double_underline_color_set(EWL_TEXT(o), 50, 50, 50, 255);
        ewl_text_shadow_color_set(EWL_TEXT(o), 128, 128, 128, 128);
        ewl_text_outline_color_set(EWL_TEXT(o), 200, 200, 200, 200);
        ewl_text_strikethrough_color_set(EWL_TEXT(o), 0, 0, 0, 255);
        ewl_text_glow_color_set(EWL_TEXT(o), 0, 255, 0, 128);
        ewl_widget_show(o);

        hbox = ewl_hbox_new();
        ewl_container_child_append(EWL_CONTAINER(box), hbox);
        ewl_object_fill_policy_set(EWL_OBJECT(hbox), EWL_FLAG_FILL_NONE);
        ewl_widget_show(hbox);

        {
                int i;
                Ewl_Text_Editor_Control btns [] = {
                                {"Fetch", ete_cb_fetch},
                                {"Set",   ete_cb_set},
                                {"Load",  ete_cb_load},
                                {"Clear", ete_cb_clear},
                                {NULL, NULL}
                        };

                for (i = 0; btns[i].name != NULL; i++)
                {
                        o = ewl_button_new();
                        ewl_button_label_set(EWL_BUTTON(o), btns[i].name);
                        ewl_container_child_append(EWL_CONTAINER(hbox), o);
                        ewl_object_fill_policy_set(EWL_OBJECT(o),
                                        EWL_FLAG_FILL_NONE);
                        ewl_callback_append(o, EWL_CALLBACK_CLICKED, btns[i].cb, NULL);
                        ewl_widget_show(o);
                }

        }

        return 1;
}

static void
ete_style_apply(Ewl_Text *t, Ewl_Text_Style s)
{
        unsigned int cursor_pos;
        unsigned int styles;

        cursor_pos = ewl_text_cursor_position_get(t);
        styles = ewl_text_styles_get(t, cursor_pos);

        if (styles & s)
                styles &= ~s;
        else
                styles |= s;

        if (ewl_text_has_selection(t))
        {
                Ewl_Text_Trigger *selection;

                selection = EWL_TEXT_TRIGGER(ewl_text_selection_get(t));
                ewl_text_cursor_position_set(t,
                                ewl_text_trigger_start_pos_get(selection));

                ewl_text_styles_apply(t, styles,
                                ewl_text_trigger_length_get(selection));

                ewl_text_cursor_position_set(t, cursor_pos);
        }
        else
                ewl_text_styles_set(t, styles);
}

static void
ete_cb_styles_changed(Ewl_Widget *w, void *ev __UNUSED__,
                                        void *data __UNUSED__)
{
        Ewl_Widget *entry;
        Ewl_Selection_Idx *idx;
        char *str;

        idx = ewl_mvc_selected_get(EWL_MVC(w));
        str = ecore_list_index_goto(ewl_mvc_data_get(EWL_MVC(w)), idx->row);

        entry = ewl_widget_name_find("entry");
        if (!strcmp(str, "None"))
                ete_style_apply(EWL_TEXT(entry), EWL_TEXT_STYLE_NONE);

        else if (!strcmp(str, "Shadow"))
                ete_style_apply(EWL_TEXT(entry), EWL_TEXT_STYLE_SHADOW);

        else if (!strcmp(str, "Soft Shadow"))
                ete_style_apply(EWL_TEXT(entry), EWL_TEXT_STYLE_SOFT_SHADOW);

        else if (!strcmp(str, "Far Shadow"))
                ete_style_apply(EWL_TEXT(entry), EWL_TEXT_STYLE_FAR_SHADOW);

        else if (!strcmp(str, "Glow"))
                ete_style_apply(EWL_TEXT(entry), EWL_TEXT_STYLE_GLOW);

        else if (!strcmp(str, "Outline"))
                ete_style_apply(EWL_TEXT(entry), EWL_TEXT_STYLE_OUTLINE);


}

static void
ete_cb_strikethrough(Ewl_Widget *w __UNUSED__, void *ev __UNUSED__,
                                                void *data __UNUSED__)
{
        Ewl_Widget *entry;

        entry = ewl_widget_name_find("entry");
        ete_style_apply(EWL_TEXT(entry), EWL_TEXT_STYLE_STRIKETHROUGH);
}

static void
ete_cb_underline(Ewl_Widget *w __UNUSED__, void *ev __UNUSED__,
                                                void *data __UNUSED__)
{
        Ewl_Widget *entry;

        entry = ewl_widget_name_find("entry");
        ete_style_apply(EWL_TEXT(entry), EWL_TEXT_STYLE_UNDERLINE);
}

static void
ete_align_apply(unsigned int align)
{
        Ewl_Widget *w;
        Ewl_Text *t;

        w = ewl_widget_name_find("entry");
        t = EWL_TEXT(w);

        if (ewl_text_has_selection(t))
        {
                Ewl_Text_Trigger *selection;
                unsigned int cursor_pos;

                cursor_pos = ewl_text_cursor_position_get(t);

                selection = EWL_TEXT_TRIGGER(ewl_text_selection_get(t));
                ewl_text_cursor_position_set(t,
                                ewl_text_trigger_start_pos_get(selection));

                ewl_text_align_apply(t, align,
                                ewl_text_trigger_length_get(selection));

                ewl_text_cursor_position_set(t, cursor_pos);
        }
        else
                ewl_text_align_set(t, align);
}

static void
ete_cb_justify_left(Ewl_Widget *w __UNUSED__, void *ev __UNUSED__,
                                                void *data __UNUSED__)
{
        ete_align_apply(EWL_FLAG_ALIGN_LEFT);
}

static void
ete_cb_justify_center(Ewl_Widget *w __UNUSED__, void *ev __UNUSED__,
                                                void *data __UNUSED__)
{
        ete_align_apply(EWL_FLAG_ALIGN_CENTER);
}

static void
ete_cb_justify_right(Ewl_Widget *w __UNUSED__, void *ev __UNUSED__,
                                                void *data __UNUSED__)
{
        ete_align_apply(EWL_FLAG_ALIGN_RIGHT);
}

static void
ete_font_apply(char *font)
{
        Ewl_Widget *w;
        Ewl_Text *t;
        char *of;

        w = ewl_widget_name_find("entry");
        t = EWL_TEXT(w);

        if (ewl_text_has_selection(EWL_TEXT(t)))
                of = ewl_text_font_get(EWL_TEXT(t),
                                ewl_text_trigger_start_pos_get(
                                        EWL_TEXT_TRIGGER(ewl_text_selection_get(EWL_TEXT(t)))));
        else
                of = ewl_text_font_get(EWL_TEXT(t),
                                ewl_text_cursor_position_get(EWL_TEXT(t)));

        if (!strcmp(of, "ewl/default/bold-italic"))
        {
                if (!strcmp(font, "ewl/default/bold"))
                        font = "ewl/default/italic";
                else if (!strcmp(font, "ewl/default/italic"))
                        font = "ewl/default/bold";
        }
        else if ((!strcmp(of, "ewl/default/italic") && !strcmp(font, "ewl/default/bold"))
                        || ((!strcmp(of, "ewl/default/bold") && (!strcmp(font, "ewl/default/italic")))))
                font = "ewl/default/bold-italic";

        else if (!strcmp(of, font))
                font = "ewl/default";

        if (ewl_text_has_selection(t))
        {
                Ewl_Text_Trigger *selection;
                unsigned int cursor_pos;

                cursor_pos = ewl_text_cursor_position_get(t);

                selection = EWL_TEXT_TRIGGER(ewl_text_selection_get(t));
                ewl_text_cursor_position_set(t,
                                ewl_text_trigger_start_pos_get(selection));

                ewl_text_font_apply(t, font,
                                ewl_text_trigger_length_get(selection));

                ewl_text_cursor_position_set(t, cursor_pos);
        }
        else
                ewl_text_font_set(t, font);
}

static void
ete_cb_bold(Ewl_Widget *w __UNUSED__, void *ev __UNUSED__,
                                        void *data __UNUSED__)
{
        ete_font_apply("ewl/default/bold");
}

static void
ete_cb_italic(Ewl_Widget *w __UNUSED__, void *ev __UNUSED__,
                                        void *data __UNUSED__)
{
        ete_font_apply("ewl/default/italic");
}

static void
ete_cb_font_size(Ewl_Widget *w __UNUSED__, void *ev __UNUSED__,
                                                void *data __UNUSED__)
{
        Ewl_Widget *entry;

        entry = ewl_widget_name_find("entry");
        if (ewl_text_has_selection(EWL_TEXT(entry)))
        {
                unsigned int cursor_pos;
                Ewl_Text_Trigger *selection;

                cursor_pos = ewl_text_cursor_position_get(EWL_TEXT(entry));
                selection = EWL_TEXT_TRIGGER(ewl_text_selection_get(
                                        EWL_TEXT(entry)));
                ewl_text_cursor_position_set(EWL_TEXT(entry),
                                        ewl_text_trigger_start_pos_get(selection));
                ewl_text_font_size_apply(EWL_TEXT(entry),
                                        ewl_range_value_get(EWL_RANGE(w)),
                                        ewl_text_trigger_length_get(selection));
                ewl_text_cursor_position_set(EWL_TEXT(entry), cursor_pos);
        }
        else
                ewl_text_font_size_set(EWL_TEXT(entry),
                                        ewl_range_value_get(EWL_RANGE(w)));
}

static void
ete_cb_fetch(Ewl_Widget *w __UNUSED__, void *ev __UNUSED__,
                                        void *data __UNUSED__)
{
        Ewl_Widget *entry;
        char *txt;

        entry = ewl_widget_name_find("entry");
        txt = ewl_text_text_get(EWL_TEXT(entry));

        if (txt) {
                printf("%s\n", txt);
                free(txt);
        }
}

static void
ete_cb_set(Ewl_Widget *w __UNUSED__, void *ev __UNUSED__,
                                        void *data __UNUSED__)
{
        Ewl_Widget *entry;

        entry = ewl_widget_name_find("entry");
        ewl_text_text_set(EWL_TEXT(entry), txt);
}

static void
ete_cb_load(Ewl_Widget *w __UNUSED__, void *ev __UNUSED__,
                                        void *data __UNUSED__)
{
        printf("I do nothing yet ...\n");
}

static void
ete_cb_clear(Ewl_Widget *w __UNUSED__, void *ev __UNUSED__,
                                        void *data __UNUSED__)
{
        Ewl_Widget *entry, *scroll;

        entry = ewl_widget_name_find("entry");
        ewl_text_clear(EWL_TEXT(entry));

        scroll = ewl_widget_name_find("scrollpane");
        ewl_widget_configure(scroll);
}


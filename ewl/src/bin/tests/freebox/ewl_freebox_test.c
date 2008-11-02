/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "Ewl_Test.h"
#include "ewl_test_private.h"
#include "ewl_border.h"
#include "ewl_button.h"
#include "ewl_freebox.h"
#include "ewl_icon.h"
#include "ewl_scrollpane.h"

#include <stdlib.h>
#include <string.h>
#include <time.h>

static int create_test(Ewl_Container *box);
static void ewl_freebox_cb_icon_change(Ewl_Widget *w, void *ev, void *data);
static int ewl_freebox_cb_compare(Ewl_Widget *a, Ewl_Widget *b);
static void ewl_freebox_cb_add(Ewl_Widget *w, void *ev, void *data);
static void ewl_freebox_cb_clear(Ewl_Widget *w, void *ev, void *data);

static Ewl_Widget *sort_fb;

typedef struct Freebox_Test Freebox_Test;

struct Freebox_Test
{
        const char *name;
        const Ewl_Freebox_Layout_Type type;
        Ewl_Freebox_Comparator compare;
};

extern Ewl_Unit_Test freebox_unit_tests[];

void
test_info(Ewl_Test *test)
{
        test->name = "Freebox";
        test->tip = "The freebox is used for a more free layout system\n"
                                        "then the other containers",
        test->filename = __FILE__;
        test->func = create_test;
        test->type = EWL_TEST_TYPE_CONTAINER;
        test->unit_tests = freebox_unit_tests;
}

static int
create_test(Ewl_Container *box)
{
        int i;
        const Freebox_Test fbtests[] = {
                {
                        "Manual Placement",
                        EWL_FREEBOX_LAYOUT_MANUAL,
                        NULL
                       },
                {
                        "Auto Placement",
                        EWL_FREEBOX_LAYOUT_AUTO,
                        NULL
                },
                {
                        "Comparator Placement (by name)",
                        EWL_FREEBOX_LAYOUT_COMPARATOR,
                        ewl_freebox_cb_compare
                },
                { NULL, EWL_FREEBOX_LAYOUT_AUTO, NULL }
        };

        srand(time(NULL));

        ewl_object_fill_policy_set(EWL_OBJECT(box), EWL_FLAG_FILL_ALL);

        for (i = 0; fbtests[i].name != NULL; i++) {
                Ewl_Widget *border, *hbox, *fb, *pane, *o;

                border  = ewl_border_new();
                ewl_border_label_set(EWL_BORDER(border),
                                (char *)fbtests[i].name);
                ewl_object_fill_policy_set(EWL_OBJECT(border), EWL_FLAG_FILL_FILL);
                ewl_container_child_append(EWL_CONTAINER(box), border);
                ewl_widget_show(border);

                /* nest the freebox in a scrollpane */
                pane = ewl_scrollpane_new();
                ewl_container_child_append(EWL_CONTAINER(border), pane);
                ewl_widget_show(pane);

                /* create a freebox of the type for this test */
                fb = ewl_freebox_new();
                ewl_freebox_layout_type_set(EWL_FREEBOX(fb), fbtests[i].type);
                if (fbtests[i].compare) {
                        ewl_freebox_comparator_set(EWL_FREEBOX(fb),
                                        fbtests[i].compare);
                        sort_fb = fb;
                }
                ewl_container_child_append(EWL_CONTAINER(pane), fb);
                ewl_widget_show(fb);

                /* pack controls for the freebox */
                hbox = ewl_hbox_new();
                ewl_container_child_append(EWL_CONTAINER(border), hbox);
                ewl_object_fill_policy_set(EWL_OBJECT(hbox),
                                EWL_FLAG_FILL_NONE);
                ewl_object_alignment_set(EWL_OBJECT(hbox), EWL_FLAG_ALIGN_TOP);
                ewl_widget_show(hbox);

                o = ewl_button_new();
                ewl_button_label_set(EWL_BUTTON(o), "Add items");
                ewl_object_fill_policy_set(EWL_OBJECT(o), EWL_FLAG_FILL_NONE);
                ewl_object_alignment_set(EWL_OBJECT(o), EWL_FLAG_ALIGN_TOP);
                ewl_container_child_append(EWL_CONTAINER(hbox), o);
                ewl_callback_append(o, EWL_CALLBACK_CLICKED,
                                        ewl_freebox_cb_add, fb);
                ewl_widget_show(o);

                o = ewl_button_new();
                ewl_button_label_set(EWL_BUTTON(o), "Clear items");
                ewl_object_fill_policy_set(EWL_OBJECT(o), EWL_FLAG_FILL_NONE);
                ewl_object_alignment_set(EWL_OBJECT(o), EWL_FLAG_ALIGN_TOP);
                ewl_container_child_append(EWL_CONTAINER(hbox), o);
                ewl_callback_append(o, EWL_CALLBACK_CLICKED,
                                        ewl_freebox_cb_clear, fb);
                ewl_widget_show(o);
        }

        return 1;
}

static void
ewl_freebox_cb_icon_change(Ewl_Widget *w __UNUSED__, void *ev __UNUSED__,
                                                                void *data)
{
        Ewl_Freebox *fb;

        fb = data;
        ewl_freebox_resort(EWL_FREEBOX(fb));
}

static int
ewl_freebox_cb_compare(Ewl_Widget *a, Ewl_Widget *b)
{
        const char *a_txt, *b_txt;

        a_txt = ewl_icon_label_get(EWL_ICON(a));
        b_txt = ewl_icon_label_get(EWL_ICON(b));

        return strcmp(a_txt, b_txt);
}

static void
ewl_freebox_cb_add(Ewl_Widget *w __UNUSED__, void *ev __UNUSED__,
                                                void *data)
{
        Ewl_Freebox *fb;
        Ewl_Widget *i;
        int t;

        struct
        {
                char *name;
                char *path;
        } files[] = {
                {"Draw", PACKAGE_DATA_DIR "/ewl/images/Draw.png"},
                {"End", PACKAGE_DATA_DIR "/ewl/images/End.png"},
                {"Card", PACKAGE_DATA_DIR "/ewl/images/NewBCard.png"},
                {"Open", PACKAGE_DATA_DIR "/ewl/images/Open.png"},
                {"Package", PACKAGE_DATA_DIR "/ewl/images/Package.png"},
                {"World", PACKAGE_DATA_DIR "/ewl/images/World.png"},
                {NULL, NULL}
        };

        fb = data;

        for (t = 0; files[t].name != NULL; t++)
        {
                long width, height;

                width = (rand() % 30) + 30;
                height = (rand() % 30) + 30;

                i = ewl_icon_new();
                ewl_container_child_append(EWL_CONTAINER(fb), i);
                ewl_icon_label_set(EWL_ICON(i), files[t].name);
                ewl_icon_image_set(EWL_ICON(i), files[t].path, NULL);
                ewl_object_fill_policy_set(EWL_OBJECT(i),
                                                EWL_FLAG_FILL_FILL);
                ewl_object_minimum_size_set(EWL_OBJECT(i), (int)width,
                                                        (int)height);
                if (fb == EWL_FREEBOX(sort_fb))
                {
                        ewl_icon_editable_set(EWL_ICON(i), TRUE);
                        ewl_callback_append(i, EWL_CALLBACK_VALUE_CHANGED,
                                        ewl_freebox_cb_icon_change, fb);
                }
                ewl_widget_show(i);
        }
}

static void
ewl_freebox_cb_clear(Ewl_Widget *w __UNUSED__, void *ev __UNUSED__,
                                                void *data)
{
        Ewl_Freebox *fb;

        fb = data;

        ewl_container_reset(EWL_CONTAINER(fb));
}

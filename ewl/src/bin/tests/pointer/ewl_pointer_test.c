/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "Ewl_Test.h"
#include "ewl_test_private.h"
#include "ewl_button.h"
#include "ewl_entry.h"

#include <stdio.h>
#include <stdlib.h>

static int create_test(Ewl_Container *win);

void
test_info(Ewl_Test *test)
{
        test->name = "Pointer";
        test->tip = "Creates a custom cursor from a buffer engine window.";
        test->filename = __FILE__;
        test->func = create_test;
        test->type = EWL_TEST_TYPE_ADVANCED;
}

static int
create_test(Ewl_Container *box)
{
        Ewl_Widget *pointer_win;
        Ewl_Widget *o;
        int pos = 0;

        pointer_win = ewl_cursor_new();
        ewl_widget_appearance_set(pointer_win, "dndcursor");
        ewl_widget_show(pointer_win);

        o = ewl_entry_new();
        ewl_attach_mouse_argb_cursor_set(o, pointer_win);
        ewl_text_text_set(EWL_TEXT(o), "Expect to see a button cursor\n"
                       "over this entry and a normal cursor\n"
                       "over other parts of the window");
        ewl_entry_editable_set(EWL_ENTRY(o), FALSE);
        ewl_object_fill_policy_set(EWL_OBJECT(o), EWL_FLAG_FILL_NONE);
        ewl_container_child_append(EWL_CONTAINER(box), o);
        ewl_widget_show(o);

        pos = EWL_MOUSE_CURSOR_SB_UP_ARROW;

        o = ewl_button_new();
        ewl_attach_mouse_cursor_set(o, pos);
        ewl_button_label_set(EWL_BUTTON(o), "Something different here");
        ewl_object_fill_policy_set(EWL_OBJECT(o), EWL_FLAG_FILL_NONE);
        ewl_container_child_append(EWL_CONTAINER(box), o);
        ewl_widget_show(o);

        return 1;
}

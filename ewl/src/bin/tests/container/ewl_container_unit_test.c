/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "Ewl_Test.h"
#include "ewl_test_private.h"
#include "ewl_button.h"
#include "ewl_macros.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* unit tests */
static int test_constructor(char *buf, int len);
static int test_parents_switch(char *buf, int len);
static int test_child_append(char *buf, int len);
static int test_child_prepend(char *buf, int len);
static int test_child_index_get(char *buf, int len);
static int test_child_index_internal_get(char *buf, int len);
static int test_child_count_internal_get(char *buf, int len);
static int test_child_at_get(char *buf, int len);
static int test_reset(char *buf, int len);
//static int test_largest_sum_prefer(char *buf, int len);
static int test_redirect_set_get(char *buf, int len);


Ewl_Unit_Test container_unit_tests[] = {
                {"constructor", test_constructor, NULL, -1, 0},
                {"switch parents", test_parents_switch, NULL, -1, 0},
                {"Child append", test_child_append, NULL, -1, 0},
                {"Child prepend", test_child_prepend, NULL, -1, 0},
                {"Child index get", test_child_index_get, NULL, -1, 0},
                {"Child internal index get", test_child_index_internal_get, NULL, -1, 0},
                {"Child internal child count", test_child_count_internal_get, NULL, -1, 0},
                {"Child at get", test_child_at_get, NULL, -1, 0},
                {"Reset", test_reset, NULL, -1, 0},
                //{"Largest/sum prefer", test_largest_sum_prefer, NULL, -1, 0},
                {"Redirect set get", test_redirect_set_get, NULL, -1, 0},
                {NULL, NULL, NULL, -1, 0}
        };

static int
test_constructor(char *buf, int len)
{
        Ewl_Widget *c;
        int ret = 0;

        c = ewl_container_new();

        if (!EWL_CONTAINER_IS(c))
        {
                LOG_FAILURE(buf, len, "returned widget is not of the type"
                                " " EWL_CONTAINER_TYPE);
                goto DONE;
        }
        if (!ewl_widget_recursive_get(c))
        {
                LOG_FAILURE(buf, len, "container is not recrusive");
                goto DONE;
        }

        ret = 1;
DONE:
        ewl_widget_destroy(c);

        return ret;
}

static int
test_parents_switch(char *buf, int len)
{
        Ewl_Widget *p1, *p2, *w;

        p1 = ewl_vbox_new();
        ewl_widget_show(p1);

        w = ewl_button_new();
        ewl_container_child_append(EWL_CONTAINER(p1), w);
        ewl_widget_show(w);

        if (!(w->parent == p1))
        {
                snprintf(buf, len, "initial parent incorrect (%p vs %p)", w->parent, p1);
                return FALSE;
        }

        p2 = ewl_vbox_new();
        ewl_widget_show(p2);

        ewl_container_child_append(EWL_CONTAINER(p2), w);
        if (!(w->parent == p2))
        {
                snprintf(buf, len, "reparent incorrect (%p vs %p)", w->parent, p2);
                return FALSE;
        }

        return TRUE;
}

static int 
test_child_append(char *buf, int len)
{
        Ewl_Widget *c, *w;
        int i, ret = 1;
        Ecore_List *list;

        c = ewl_container_new();
        list = ecore_list_new();
        
        for (i = 0; i < 10; i++)
        {
                w = ewl_widget_new();
                ewl_container_child_append(EWL_CONTAINER(c), w);
                ecore_list_append(list, w);
        }

        ecore_list_first_goto(list);
        ewl_container_child_iterate_begin(EWL_CONTAINER(c));
        while ((w = ewl_container_child_next(EWL_CONTAINER(c))))
        {
                if (w != ecore_list_next(list))
                {
                        ret = 0;
                        LOG_FAILURE(buf, len, "The order of the appended widets"
                                        " and of the presend widgets differ");
                        break;
                }
        }

        ewl_widget_destroy(c);
        ecore_list_destroy(list);

        return ret;
}

static int 
test_child_prepend(char *buf, int len)
{
        Ewl_Widget *c, *w;
        int i, ret = 1;
        Ecore_List *list;

        c = ewl_container_new();
        list = ecore_list_new();
        
        for (i = 0; i < 10; i++)
        {
                w = ewl_widget_new();
                ewl_container_child_prepend(EWL_CONTAINER(c), w);
                ecore_list_prepend(list, w);
        }

        ecore_list_first_goto(list);
        ewl_container_child_iterate_begin(EWL_CONTAINER(c));
        while ((w = ewl_container_child_next(EWL_CONTAINER(c))))
        {
                if (w != ecore_list_next(list))
                {
                        ret = 0;
                        LOG_FAILURE(buf, len, "The order of the prepended widets"
                                        " and of the presend widgets differ");
                        break;
                }
        }

        ewl_widget_destroy(c);
        ecore_list_destroy(list);

        return ret;
}

//static int test_child_insert(char *buf, int len);
//static int test_child_internal_insert(char *buf, int len);
static int
test_child_index_get(char *buf, int len)
{
        Ewl_Widget *c, *w;
        int i, ret = 1;
        Ecore_List *list;

        c = ewl_container_new();
        list = ecore_list_new();
        
        for (i = 0; i < 30; i++)
        {
                w = ewl_widget_new();

                if ((i % 2))
                        ewl_widget_internal_set(w, TRUE);
                else
                        ecore_list_prepend(list, w);
                ewl_container_child_prepend(EWL_CONTAINER(c), w);
        }

        i = 0;
        ecore_list_first_goto(list);
        while ((w = ecore_list_next(list)))
        {
                int index = ewl_container_child_index_get(EWL_CONTAINER(c), w);

                if (i != index)
                {
                        ret = 0;
                        LOG_FAILURE(buf, len, "Returned Index %i is incorrect,"
                                        " it should be %i", index, i);
                        break;
                }
                i++;
        }

        ewl_widget_destroy(c);
        ecore_list_destroy(list);

        return ret;
}

static int
test_child_index_internal_get(char *buf, int len)
{
        Ewl_Widget *c, *w;
        int i, ret = 1;
        Ecore_List *list;

        c = ewl_container_new();
        list = ecore_list_new();
        
        for (i = 0; i < 30; i++)
        {
                w = ewl_widget_new();

                if ((i % 2))
                        ewl_widget_internal_set(w, TRUE);
                ecore_list_prepend(list, w);
                ewl_container_child_prepend(EWL_CONTAINER(c), w);
        }

        i = 0;
        ecore_list_first_goto(list);
        while ((w = ecore_list_next(list)))
        {
                int index = ewl_container_child_index_internal_get(
                                EWL_CONTAINER(c), w);

                if (i != index)
                {
                        ret = 0;
                        LOG_FAILURE(buf, len, "Returned Index %i is incorrect,"
                                        " it should be %i", index, i);
                        break;
                }
                i++;
        }

        ewl_widget_destroy(c);
        ecore_list_destroy(list);

        return ret;
}

//static int test_child_iterate(char *buf, int len);
static int
test_child_count_internal_get(char *buf, int len)
{
        Ewl_Widget *c, *w;
        int i, ret = 1;
        int not_internal = 0;

        c = ewl_container_new();
        
        for (i = 0; i < 30; i++)
        {
                w = ewl_widget_new();

                if ((i % 2))
                        ewl_widget_internal_set(w, TRUE);
                else
                        not_internal++;
                ewl_container_child_prepend(EWL_CONTAINER(c), w);
        }

        if (i != ewl_container_child_count_internal_get(EWL_CONTAINER(c)))
        {
                ret = 0;
                LOG_FAILURE(buf, len, "Returned internal child count %i is "
                                "incorrect, it should be %i", 
                                ewl_container_child_count_internal_get(
                                                        EWL_CONTAINER(c)),
                                i);
                goto DONE;
        }
        if (not_internal != ewl_container_child_count_get(EWL_CONTAINER(c)))
        {
                ret = 0;
                LOG_FAILURE(buf, len, "Returned child count %i is incorrect,"
                                " it should be %i", 
                                ewl_container_child_count_get(EWL_CONTAINER(c)),
                                not_internal);
                goto DONE;
        }

DONE:
        ewl_widget_destroy(c);

        return ret;
}

static int 
test_child_at_get(char *buf, int len)
{
        Ewl_Widget *c;
        Ewl_Widget *w1, *w2, *w3, *w4;
        Ewl_Widget *found;
        int ret = 0;

        c = ewl_container_new();
        ewl_object_size_request(EWL_OBJECT(c), 100, 100);
        ewl_widget_show(c);

        /* We place now the widget that it looks like that:
         *  +----+----+
         *  | w1 | w2 |
         *  +------+--+
         *  | w3 | w4 |
         *  +----+----+
         */
        w1 = ewl_widget_new();
        ewl_container_child_append(EWL_CONTAINER(c), w1);
        ewl_object_place(EWL_OBJECT(w1), 0, 0, 50, 50);
        ewl_widget_show(w1);

        w2 = ewl_widget_new();
        ewl_container_child_append(EWL_CONTAINER(c), w2);
        ewl_object_place(EWL_OBJECT(w2), 50, 0, 50, 50);
        ewl_widget_show(w2);

        w3 = ewl_widget_new();
        ewl_container_child_append(EWL_CONTAINER(c), w3);
        ewl_object_place(EWL_OBJECT(w3), 0, 50, 50, 50);

        w4 = ewl_widget_new();
        ewl_container_child_append(EWL_CONTAINER(c), w4);
        ewl_object_place(EWL_OBJECT(w4), 50, 50, 50, 50);
        ewl_widget_show(w4);

        found = ewl_container_child_at_get(EWL_CONTAINER(c), 25, 25);
        if (found != w1)
        {
                LOG_FAILURE(buf, len, "Do not find widget 1");
                goto DONE;
        }
        found = ewl_container_child_at_get(EWL_CONTAINER(c), 75, 25);
        if (found != w2)
        {
                LOG_FAILURE(buf, len, "Do not find widget 2");
                goto DONE;
        }
        found = ewl_container_child_at_get(EWL_CONTAINER(c), 25, 75);
        /* we shouldn't find it because it is not shown */
        if (found == w3)
        {
                LOG_FAILURE(buf, len, "We found widget 3");
                goto DONE;
        }
        found = ewl_container_child_at_get(EWL_CONTAINER(c), 75, 75);
        if (found != w4)
        {
                LOG_FAILURE(buf, len, "Do not find widget 4");
                goto DONE;
        }
        ret = 1;
DONE:
        ewl_widget_destroy(c);
        return ret;

}

static int
test_reset(char *buf, int len)
{
        Ewl_Widget *c, *w;
        int i, ret = 1;

        c = ewl_container_new();
        
        for (i = 0; i < 30; i++)
        {
                w = ewl_widget_new();
                ewl_container_child_prepend(EWL_CONTAINER(c), w);
        }

        ewl_container_reset(EWL_CONTAINER(c));
        if (ewl_container_child_count_get(EWL_CONTAINER(c)) != 0)
        {
                LOG_FAILURE(buf, len, "The container is not emty after reset");
                ret = 0;
        }

        ewl_widget_destroy(c);

        return ret;
}

/* largest and sum prefer works only for realized widgets */
#if 0
static int
test_largest_sum_prefer(char *buf, int len)
{
        int i = 0;
        int ret = 0;
        struct {
                int width;
                int height;
                unsigned char visible;
        } sizes[] = {
                {20, 30, 1},
                { 1, 63, 0},
                {25, 25, 1},
                {21, 36, 1},
                {45, 45, 1},
                {65, 85, 0},
                {48, 52, 1},
                { 0,  0, 0}
        };

        const int sum_w = 159;
        const int sum_h = 188;
        const int largest_w = 48;
        const int largest_h = 52;

        Ewl_Widget *c;

        c = ewl_container_new();
        ewl_object_fill_policy_set(EWL_OBJECT(c), EWL_FLAG_FILL_FILL);
        ewl_widget_show(c);

        for (i = 0; sizes[i].width != 0; i++)
        {
                Ewl_Widget *w;

                w = ewl_widget_new();
                ewl_object_preferred_inner_size_set(EWL_OBJECT(w), 
                                                        sizes[i].width,
                                                        sizes[i].height);
                ewl_container_child_append(EWL_CONTAINER(c), w);

                if (sizes[i].visible)
                        ewl_widget_show(w);
        }

        ewl_container_sum_prefer(EWL_CONTAINER(c), EWL_ORIENTATION_HORIZONTAL);
        if (ewl_object_preferred_w_get(EWL_OBJECT(c)) != sum_w)
        {
                LOG_FAILURE(buf, len, "sum of preferred width is %d, but it "
                                "should be %d", ewl_object_preferred_w_get(
                                                        EWL_OBJECT(c)), sum_w);
                goto DONE;
        }

        ewl_container_sum_prefer(EWL_CONTAINER(c), EWL_ORIENTATION_VERTICAL);
        if (ewl_object_preferred_h_get(EWL_OBJECT(c)) != sum_h)
        {
                LOG_FAILURE(buf, len, "sum of preferred height is %d, but it "
                                "should be %d", ewl_object_preferred_h_get(
                                                        EWL_OBJECT(c)), sum_h);
                goto DONE;
        }

        ewl_container_largest_prefer(EWL_CONTAINER(c), 
                                        EWL_ORIENTATION_HORIZONTAL);
        if (ewl_object_preferred_w_get(EWL_OBJECT(c)) != largest_w)
        {
                LOG_FAILURE(buf, len, "largest width is %d, but it "
                                "should be %d", ewl_object_preferred_w_get(
                                                        EWL_OBJECT(c)),
                                largest_w);
                goto DONE;
        }

        ewl_container_largest_prefer(EWL_CONTAINER(c), 
                                        EWL_ORIENTATION_VERTICAL);
        if (ewl_object_preferred_h_get(EWL_OBJECT(c)) != largest_h)
        {
                LOG_FAILURE(buf, len, "largest height is %d, but it "
                                "should be %d", ewl_object_preferred_h_get(
                                                        EWL_OBJECT(c)),
                                largest_h);
                goto DONE;
        }

        ret = 1;
DONE:
        ewl_widget_destroy(c);
        return ret;
}
#endif

static int test_redirect_set_get(char *buf, int len)
{
        Ewl_Widget *c1, *c2, *w;
        int ret = 0;

        c1 = ewl_container_new();
        c2 = ewl_container_new();
        ewl_container_child_append(EWL_CONTAINER(c1), c2);

        ewl_container_redirect_set(EWL_CONTAINER(c1), EWL_CONTAINER(c2));

        if (ewl_container_redirect_get(EWL_CONTAINER(c1)) != EWL_CONTAINER(c2))
        {
                LOG_FAILURE(buf, len, "returned redirect container differs "
                                "from the set one");
                goto DONE;
        }

        w = ewl_widget_new();
        ewl_container_child_append(EWL_CONTAINER(c1), w);

        /* this widget needs to be in the container c2 now */
        ewl_container_child_iterate_begin(EWL_CONTAINER(c2));
        if (ewl_container_child_next(EWL_CONTAINER(c2)) != w)
        {
                LOG_FAILURE(buf, len, "added child is not part of the redirect "
                                "container");
                goto DONE;
        }

        ret = 1;
DONE:
        ewl_widget_destroy(c1);
        return ret;
}


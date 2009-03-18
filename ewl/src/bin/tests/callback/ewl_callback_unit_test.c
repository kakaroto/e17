/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include <limits.h>
#include "Ewl_Test.h"
#include "ewl_test_private.h"
#include "ewl_button.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static int test_id_append(char *buf, int len);
static int test_id_prepend(char *buf, int len);
static int test_id_insert_after(char *buf, int len);
static int test_id_shared(char *buf, int len);
static int test_id_unique(char *buf, int len);
static int test_del_call(char *buf, int len);
static int test_id_del_call(char *buf, int len);
static int test_type_del_call(char *buf, int len);
static int test_data_del_call(char *buf, int len);
static int test_empty_del_call(char *buf, int len);
static int test_clear_call(char *buf, int len);
static int test_append_call(char *buf, int len);
static int test_prepend_call(char *buf, int len);
static int test_notify_call(char *buf, int len);
static int test_intercept_call(char *buf, int len);
static int test_append_in_chain_call(char *buf, int len);
static int test_prepend_in_chain_call(char *buf, int len);
static int test_insert_after_in_chain_call(char *buf, int len);
static int test_insert_before_in_chain_call(char *buf, int len);
static int test_delete_after_in_chain_call(char *buf, int len);
static int test_delete_before_in_chain_call(char *buf, int len);
static int test_delete_nothing_in_chain_call(char *buf, int len);
static int test_custom_callback_add(char *buf, int len);
static int test_custom_callback_unique(char *buf, int len);
static int test_custom_callback_append(char *buf, int len);
static int test_custom_callback_prepend(char *buf, int len);
static int test_custom_callback_insert_after(char *buf, int len);
static int test_custom_callback_del_single(char *buf, int len);
static int test_custom_callback_del_multiple(char *buf, int len);
static int test_custom_callback_del_type_single(char *buf, int len);
static int test_custom_callback_del_type_multiple(char *buf, int len);
static int test_custom_callback_clear(char *buf, int len);
static int test_custom_callback_call_custom(char *buf, int len);

/*
 * Callbacks for manipulating the tests.
 */
static void base_callback(Ewl_Widget *w, void *event, void *data);
static void custom_callback(Ewl_Widget *w, void *event, void *data);
static void differing_callback(Ewl_Widget *w, void *event, void *data);
static void append_callback(Ewl_Widget *w, void *event, void *data);
static void prepend_callback(Ewl_Widget *w, void *event, void *data);
static void insert_after_callback(Ewl_Widget *w, void *event, void *data);
static void insert_before_callback(Ewl_Widget *w, void *event, void *data);
static void delete_callback(Ewl_Widget *w, void *event, void *data);

Ewl_Unit_Test callback_unit_tests[] = {
                {"append/get id", test_id_append, NULL, -1, 0},
                {"prepend/get id", test_id_prepend, NULL, -1, 0},
                {"insert after/get id", test_id_insert_after, NULL, -1, 0},
                {"shared id", test_id_shared, NULL, -1, 0},
                {"unique id", test_id_unique, NULL, -1, 0},
                {"del/call", test_del_call, NULL, -1, 0},
                {"del_id/call", test_id_del_call, NULL, -1, 0},
                {"del_data/call", test_data_del_call, NULL, -1, 0},
                {"del_type/call", test_type_del_call, NULL, -1, 0},
                {"del from empty", test_empty_del_call, NULL, -1, 0},
                {"clear/call", test_clear_call, NULL, -1, 0},
                {"append/call", test_append_call, NULL, -1, 0},
                {"prepend/call", test_prepend_call, NULL, -1, 0},
                {"parent notify", test_notify_call, NULL, -1, 0},
                {"parent intercept", test_intercept_call, NULL, -1, 0},
                {"append during call", test_append_in_chain_call, NULL, -1, 0},
                {"prepend during call", test_prepend_in_chain_call, NULL, -1, 0},
                {"insert after during call", test_insert_after_in_chain_call, NULL, -1, 0},
                {"insert before during call", test_insert_before_in_chain_call, NULL, -1, 0},
                {"delete after during call", test_delete_after_in_chain_call, NULL, -1, 0},
                {"delete before during call", test_delete_before_in_chain_call, NULL, -1, 0},
                {"delete nothing during call", test_delete_nothing_in_chain_call, NULL, -1, 0},
                {"create custom callback", test_custom_callback_add, NULL, -1, 0},
                {"unique custom callback", test_custom_callback_unique, NULL, -1, 0},
                {"append custom callback", test_custom_callback_append, NULL, -1, 0},
                {"prepend custom callback", test_custom_callback_prepend, NULL, -1, 0},
                {"insert after custom callback", test_custom_callback_insert_after, NULL, -1, 0},
                {"delete single custom callback", test_custom_callback_del_single, NULL, -1, 0},
                {"delete custom callback of multiple", test_custom_callback_del_multiple, NULL, -1, 0},
                {"delete custom callback type", test_custom_callback_del_type_single, NULL, -1, 0},
                {"delete custom callback type of multiple", test_custom_callback_del_type_multiple, NULL, -1, 0},
                {"clear with custom callback", test_custom_callback_clear, NULL, -1, 0},
                {"call custom from within custom", test_custom_callback_call_custom, NULL, -1, 0},
                {NULL, NULL, NULL, -1, 0}
        };

static unsigned int CALLBACK_CUSTOM_TYPE = 0;
static unsigned int CALLBACK_CUSTOM_TYPE2 = 0;

/*
 * Append a callback and verify that the returned id is valid.
 */
static int
test_id_append(char *buf, int len)
{
        Ewl_Widget *w;
        int id;
        int ret = 0;

        w = ewl_widget_new();
        id = ewl_callback_append(w, EWL_CALLBACK_CONFIGURE, base_callback,
                        NULL);

        if (id)
                ret = 1;
        else
                LOG_FAILURE(buf, len, "invalid callback id returned");

        ewl_widget_destroy(w);

        return ret;
}

/*
 * Prepend a callback and verify that the returned id is valid.
 */
static int
test_id_prepend(char *buf, int len)
{
        Ewl_Widget *w;
        int id;
        int ret = 0;

        w = ewl_widget_new();
        id = ewl_callback_prepend(w, EWL_CALLBACK_CONFIGURE, base_callback,
                        NULL);

        if (id)
                ret = 1;
        else
                LOG_FAILURE(buf, len, "invalid callback id returned");

        ewl_widget_destroy(w);

        return ret;
}

/*
 * Insert a callback and verify that the returned id is valid.
 */
static int
test_id_insert_after(char *buf, int len)
{
        Ewl_Widget *w;
        int id;
        int ret = 0;

        w = ewl_widget_new();
        ewl_callback_prepend(w, EWL_CALLBACK_CONFIGURE, base_callback,
                        NULL);
        id = ewl_callback_insert_after(w, EWL_CALLBACK_CONFIGURE, base_callback,
                        w, base_callback, NULL);

        if (id)
                ret = 1;
        else
                LOG_FAILURE(buf, len, "invalid callback id returned");

        ewl_widget_destroy(w);

        return ret;
}

/*
 * Append a duplicate callback and verify that the id's match.
 */
static int
test_id_shared(char *buf, int len)
{
        Ewl_Widget *w;
        int id, id2;
        int ret = 0;

        w = ewl_widget_new();
        id = ewl_callback_append(w, EWL_CALLBACK_CONFIGURE, base_callback,
                        NULL);
        id2 = ewl_callback_append(w, EWL_CALLBACK_REALIZE, base_callback,
                        NULL);

        if (id == id2)
                ret = 1;
        else
                LOG_FAILURE(buf, len, "callback id's don't match");

        ewl_widget_destroy(w);

        return ret;
}

/*
 * Append a callbacks that vary in the function and data, and verify that the
 * id's differ.
 */
static int
test_id_unique(char *buf, int len)
{
        Ewl_Widget *w;
        int id, id2;
        int ret = 0;

        w = ewl_widget_new();
        id = ewl_callback_append(w, EWL_CALLBACK_CONFIGURE, base_callback,
                        NULL);
        id2 = ewl_callback_append(w, EWL_CALLBACK_CONFIGURE, base_callback,
                        w);

        if (id != id2) {
                id = ewl_callback_append(w, EWL_CALLBACK_CONFIGURE,
                                differing_callback, w);
                if (id != id2)
                        ret = 1;
                else
                        LOG_FAILURE(buf, len, "callback with different functions"
                                       " id's match");
        }
        else
                LOG_FAILURE(buf, len, "callback with different data id's match");

        ewl_widget_destroy(w);

        return ret;
}

/*
 * Prepend a callback and verify that removing it prevents it from being called.
 */
static int
test_del_call(char *buf, int len)
{
        Ewl_Widget *w;
        int ret = 0;

        w = ewl_widget_new();
        ewl_callback_prepend(w, EWL_CALLBACK_CONFIGURE, base_callback, NULL);
        ewl_callback_del(w, EWL_CALLBACK_CONFIGURE, base_callback);
        ewl_callback_call(w, EWL_CALLBACK_CONFIGURE);

        if ((long)ewl_widget_data_get(w, w) != 1)
                ret = 1;
        else
                LOG_FAILURE(buf, len, "del failed to remove callback");

        ewl_widget_destroy(w);

        return ret;
}

/*
 * Prepend a callback and verify that removing it prevents it from being called.
 */
static int
test_id_del_call(char *buf, int len)
{
        Ewl_Widget *w;
        int id;
        int ret = 0;

        w = ewl_widget_new();
        id = ewl_callback_prepend(w, EWL_CALLBACK_CONFIGURE, base_callback,
                        NULL);
        ewl_callback_del_cb_id(w, EWL_CALLBACK_CONFIGURE, id);
        ewl_callback_call(w, EWL_CALLBACK_CONFIGURE);

        if ((long)ewl_widget_data_get(w, w) != 1)
                ret = 1;
        else
                LOG_FAILURE(buf, len, "del failed to remove callback");

        ewl_widget_destroy(w);

        return ret;
}

/*
 * Prepend a callback and verify that removing it prevents it from being called.
 */
static int
test_data_del_call(char *buf, int len)
{
        Ewl_Widget *w;
        int ret = 0;

        w = ewl_widget_new();
        ewl_callback_prepend(w, EWL_CALLBACK_CONFIGURE, base_callback, w);
        ewl_callback_del_with_data(w, EWL_CALLBACK_CONFIGURE, base_callback, w);
        ewl_callback_call(w, EWL_CALLBACK_CONFIGURE);

        if ((long)ewl_widget_data_get(w, w) != 1) {

                /*
                 * Check that the data actually matches.
                 */
                ewl_callback_prepend(w, EWL_CALLBACK_CONFIGURE, base_callback,
                                w);
                ewl_callback_del_with_data(w, EWL_CALLBACK_CONFIGURE,
                                base_callback, NULL);
                ewl_callback_call(w, EWL_CALLBACK_CONFIGURE);

                if ((long)ewl_widget_data_get(w, w) == 1)
                        ret = 1;
                else
                        LOG_FAILURE(buf, len, "del_data removed callback");
        }
        else
                LOG_FAILURE(buf, len, "del_data failed to remove callback");

        ewl_widget_destroy(w);

        return ret;
}

/*
 * Prepend a callback and verify that clearing the chain prevents it from being
 * called.
 */
static int
test_type_del_call(char *buf, int len)
{
        Ewl_Widget *w;
        int ret = 0;

        w = ewl_widget_new();
        ewl_callback_prepend(w, EWL_CALLBACK_CONFIGURE, base_callback, NULL);
        ewl_callback_del_type(w, EWL_CALLBACK_CONFIGURE);
        ewl_callback_call(w, EWL_CALLBACK_CONFIGURE);

        if ((long)ewl_widget_data_get(w, w) != 1)
                ret = 1;
        else
                LOG_FAILURE(buf, len, "del_type failed to remove callback");

        ewl_widget_destroy(w);

        return ret;
}

/*
 * Clear a callback chain and verify deleting a non-existent function succeeds.
 */
static int
test_empty_del_call(char *buf __UNUSED__, int len __UNUSED__)
{
        Ewl_Widget *w;
        int ret = 0;

        w = ewl_widget_new();
        ewl_callback_del_type(w, EWL_CALLBACK_CONFIGURE);
        ewl_callback_del(w, EWL_CALLBACK_CONFIGURE, base_callback);
        ewl_callback_call(w, EWL_CALLBACK_CONFIGURE);

        /*
         * If we reach here, no segv occurred and we passed.
         */
        ret = 1;

        ewl_widget_destroy(w);

        return ret;
}

/*
 * Prepend a callback and verify that clearing the chain prevents it from being
 * called.
 */
static int
test_clear_call(char *buf, int len)
{
        Ewl_Widget *w;
        int ret = 0;

        w = ewl_widget_new();
        ewl_callback_prepend(w, EWL_CALLBACK_CONFIGURE, base_callback, NULL);
        ewl_callback_clear(w);
        ewl_callback_call(w, EWL_CALLBACK_CONFIGURE);

        if ((long)ewl_widget_data_get(w, w) != 1)
                ret = 1;
        else
                LOG_FAILURE(buf, len, "clear failed to remove callback");

        ewl_widget_destroy(w);

        return ret;
}

/*
 * Append a callback and verify that calling the chain triggers the callback.
 */
static int
test_append_call(char *buf, int len)
{
        Ewl_Widget *w;
        int ret = 0;

        w = ewl_widget_new();
        ewl_callback_del_type(w, EWL_CALLBACK_CONFIGURE);
        ewl_callback_append(w, EWL_CALLBACK_CONFIGURE, base_callback, NULL);
        ewl_callback_call(w, EWL_CALLBACK_CONFIGURE);

        if ((long)ewl_widget_data_get(w, w) == 1)
                ret = 1;
        else
                LOG_FAILURE(buf, len, "callback function not called");

        ewl_widget_destroy(w);

        return ret;
}

/*
 * Prepend a callback and verify that calling the chain triggers the callback.
 */
static int
test_prepend_call(char *buf, int len)
{
        Ewl_Widget *w;
        int ret = 0;

        w = ewl_widget_new();
        ewl_callback_del_type(w, EWL_CALLBACK_CONFIGURE);
        ewl_callback_prepend(w, EWL_CALLBACK_CONFIGURE, base_callback, NULL);
        ewl_callback_call(w, EWL_CALLBACK_CONFIGURE);

        if ((long)ewl_widget_data_get(w, w) == 1)
                ret = 1;
        else
                LOG_FAILURE(buf, len, "callback function not called");

        ewl_widget_destroy(w);

        return ret;
}

/*
 * Add a callback, set notify on parent, and call to check if parent notified.
 */
static int
test_notify_call(char *buf, int len)
{
        Ewl_Widget *parent;
        Ewl_Widget *w;
        int ret = 0;

        parent = ewl_cell_new();
        ewl_callback_del_type(parent, EWL_CALLBACK_CONFIGURE);
        ewl_container_callback_notify(EWL_CONTAINER(parent),
                        EWL_CALLBACK_CONFIGURE);

        w = ewl_widget_new();
        ewl_container_child_append(EWL_CONTAINER(parent), w);

        /*
         * Add parent callback
         */
        ewl_callback_prepend(parent, EWL_CALLBACK_CONFIGURE, base_callback,
                        NULL);

        /*
         * Add child callbacks.
         */
        ewl_callback_del_type(w, EWL_CALLBACK_CONFIGURE);
        ewl_callback_prepend(w, EWL_CALLBACK_CONFIGURE, differing_callback,
                        NULL);
        ewl_callback_call(w, EWL_CALLBACK_CONFIGURE);

        if ((long)ewl_widget_data_get(parent, parent) == 1)
                if ((long)ewl_widget_data_get(w, w) == 2)
                        ret = 1;
                else
                        LOG_FAILURE(buf, len, "callback function not called");
        else
                LOG_FAILURE(buf, len, "notify function not called");

        ewl_widget_destroy(w);
        ewl_widget_destroy(parent);

        return ret;
}

/*
 * Add a callback, set intercept on parent, and call to check if intercepted.
 */
static int
test_intercept_call(char *buf, int len)
{
        Ewl_Widget *parent;
        Ewl_Widget *w;
        int ret = 0;

        parent = ewl_cell_new();
        ewl_callback_del_type(parent, EWL_CALLBACK_CONFIGURE);
        ewl_container_callback_intercept(EWL_CONTAINER(parent),
                        EWL_CALLBACK_CONFIGURE);

        w = ewl_widget_new();
        ewl_container_child_append(EWL_CONTAINER(parent), w);

        /*
         * Add parent callback
         */
        ewl_callback_prepend(parent, EWL_CALLBACK_CONFIGURE, base_callback,
                        NULL);

        /*
         * Add child callbacks.
         */
        ewl_callback_del_type(w, EWL_CALLBACK_CONFIGURE);
        ewl_callback_prepend(w, EWL_CALLBACK_CONFIGURE, differing_callback,
                        NULL);
        ewl_callback_call(w, EWL_CALLBACK_CONFIGURE);

        if ((long)ewl_widget_data_get(parent, parent) == 1) {
                if ((long)ewl_widget_data_get(w, w) == 2)
                        LOG_FAILURE(buf, len, "callback function called");
                else
                        ret = 1;
        }
        else
                LOG_FAILURE(buf, len, "intercept function not called");

        ewl_widget_destroy(w);
        ewl_widget_destroy(parent);

        return ret;
}

/*
 * Append a callback while in the callback chain and verify that calling the
 * chain triggers the callback.
 */
static int
test_append_in_chain_call(char *buf, int len)
{
        Ewl_Widget *w;
        int ret = 0;

        w = ewl_widget_new();
        ewl_callback_del_type(w, EWL_CALLBACK_CONFIGURE);
        ewl_callback_append(w, EWL_CALLBACK_CONFIGURE, append_callback, NULL);
        ewl_callback_call(w, EWL_CALLBACK_CONFIGURE);

        if ((long)ewl_widget_data_get(w, w) == 1)
                ret = 1;
        else
                LOG_FAILURE(buf, len, "callback function not called");

        ewl_widget_destroy(w);

        return ret;
}

/*
 * Prepend a callback while in the callback chain and verify that calling the
 * chain does not trigger the callback.
 */
static int
test_prepend_in_chain_call(char *buf, int len)
{
        Ewl_Widget *w;
        int ret = 0;

        w = ewl_widget_new();
        ewl_callback_del_type(w, EWL_CALLBACK_CONFIGURE);
        ewl_callback_prepend(w, EWL_CALLBACK_CONFIGURE, prepend_callback, NULL);
        ewl_callback_call(w, EWL_CALLBACK_CONFIGURE);

        if ((long)ewl_widget_data_get(w, w) != 1)
                ret = 1;
        else
                LOG_FAILURE(buf, len, "callback function called");

        ewl_widget_destroy(w);

        return ret;
}

/*
 * Insert a callback after the current one, while in the callback chain and
 * verify that calling the chain triggers the callback.
 */
static int
test_insert_after_in_chain_call(char *buf, int len)
{
        Ewl_Widget *w;
        int ret = 0;

        w = ewl_widget_new();
        ewl_callback_del_type(w, EWL_CALLBACK_CONFIGURE);
        ewl_callback_prepend(w, EWL_CALLBACK_CONFIGURE, insert_after_callback,
                        NULL);
        ewl_callback_prepend(w, EWL_CALLBACK_CONFIGURE, differing_callback,
                        NULL);
        ewl_callback_call(w, EWL_CALLBACK_CONFIGURE);

        if ((long)ewl_widget_data_get(w, w) == 1)
                ret = 1;
        else
                LOG_FAILURE(buf, len, "callback function not called");

        ewl_widget_destroy(w);

        return ret;
}

/*
 * Insert a callback before the current one, while in the callback chain and
 * verify that calling the chain does not trigger the callback.
 */
static int
test_insert_before_in_chain_call(char *buf, int len)
{
        Ewl_Widget *w;
        int ret = 0;

        w = ewl_widget_new();
        ewl_callback_del_type(w, EWL_CALLBACK_CONFIGURE);
        ewl_callback_prepend(w, EWL_CALLBACK_CONFIGURE, insert_before_callback,
                        NULL);
        ewl_callback_prepend(w, EWL_CALLBACK_CONFIGURE, differing_callback,
                        NULL);
        ewl_callback_call(w, EWL_CALLBACK_CONFIGURE);

        if ((long)ewl_widget_data_get(w, w) == 2)
                ret = 1;
        else
                LOG_FAILURE(buf, len, "callback function called");

        ewl_widget_destroy(w);

        return ret;
}

/*
 * Delete a callback before the current one, while in the callback chain and
 * verify that calling the chain continues properly.
 */
static int
test_delete_before_in_chain_call(char *buf, int len)
{
        Ewl_Widget *w;
        int ret = 0;

        w = ewl_widget_new();
        ewl_callback_del_type(w, EWL_CALLBACK_CONFIGURE);
        ewl_callback_prepend(w, EWL_CALLBACK_CONFIGURE, differing_callback,
                        NULL);
        ewl_callback_prepend(w, EWL_CALLBACK_CONFIGURE, delete_callback,
                        NULL);
        ewl_callback_prepend(w, EWL_CALLBACK_CONFIGURE, base_callback,
                        NULL);
        ewl_callback_call(w, EWL_CALLBACK_CONFIGURE);

        if ((long)ewl_widget_data_get(w, w) == 2)
                ret = 1;
        else
                LOG_FAILURE(buf, len, "callback function not called");

        ewl_widget_destroy(w);

        return ret;
}

/*
 * Delete a callback after the current one, while in the callback chain and
 * verify that calling the chain does not call the removed callback.
 */
static int
test_delete_after_in_chain_call(char *buf, int len)
{
        Ewl_Widget *w;
        int ret = 0;

        w = ewl_widget_new();
        ewl_callback_del_type(w, EWL_CALLBACK_CONFIGURE);
        ewl_callback_prepend(w, EWL_CALLBACK_CONFIGURE, base_callback,
                        NULL);
        ewl_callback_prepend(w, EWL_CALLBACK_CONFIGURE, delete_callback,
                        NULL);
        ewl_callback_prepend(w, EWL_CALLBACK_CONFIGURE, differing_callback,
                        NULL);
        ewl_callback_call(w, EWL_CALLBACK_CONFIGURE);

        if ((long)ewl_widget_data_get(w, w) == 2)
                ret = 1;
        else
                LOG_FAILURE(buf, len, "callback function called");

        ewl_widget_destroy(w);

        return ret;
}

/*
 * Delete a non-existent callback, while in the callback chain and
 * verify that calling the chain does not modify anything
 */
static int
test_delete_nothing_in_chain_call(char *buf, int len)
{
        Ewl_Widget *w;
        int ret = 0;

        w = ewl_widget_new();
        ewl_callback_del_type(w, EWL_CALLBACK_CONFIGURE);
        ewl_callback_prepend(w, EWL_CALLBACK_CONFIGURE, delete_callback,
                        NULL);
        ewl_callback_append(w, EWL_CALLBACK_CONFIGURE, differing_callback,
                        NULL);
        ewl_callback_call(w, EWL_CALLBACK_CONFIGURE);

        if ((long)ewl_widget_data_get(w, w) == 2)
                ret = 1;
        else
                LOG_FAILURE(buf, len, "callback function not called");

        ewl_widget_destroy(w);

        return ret;
}

/*
 * Verify creating a custom callback type.
 */
static int
test_custom_callback_add(char *buf, int len)
{
        int ret = 0;

        CALLBACK_CUSTOM_TYPE = ewl_callback_type_add();
        if (CALLBACK_CUSTOM_TYPE > EWL_CALLBACK_MAX)
                ret = 1;
        else
                LOG_FAILURE(buf, len, "callback type registration failed");

        return ret;
}

/*
 * Verify creating a custom callback type gets unique value.
 */
static int
test_custom_callback_unique(char *buf, int len)
{
        int ret = 0;

        CALLBACK_CUSTOM_TYPE2 = ewl_callback_type_add();
        if (CALLBACK_CUSTOM_TYPE != CALLBACK_CUSTOM_TYPE2)
                ret = 1;
        else
                LOG_FAILURE(buf, len, "callback type not unique");

        return ret;
}

/*
 * Test appending and calling a callback of a custom type.
 */
static int
test_custom_callback_append(char *buf, int len)
{
        Ewl_Widget *w;
        int ret = 0;

        w = ewl_widget_new();
        ewl_callback_append(w, CALLBACK_CUSTOM_TYPE, base_callback, NULL);
        ewl_callback_call(w, CALLBACK_CUSTOM_TYPE);

        if ((long)ewl_widget_data_get(w, w) == 1)
                ret = 1;
        else
                LOG_FAILURE(buf, len, "callback function not called");

        ewl_widget_destroy(w);

        return ret;
}

/*
 * Test prepending and calling a callback of a custom type.
 */
static int
test_custom_callback_prepend(char *buf, int len)
{
        Ewl_Widget *w;
        int ret = 0;

        w = ewl_widget_new();
        ewl_callback_prepend(w, CALLBACK_CUSTOM_TYPE, base_callback, NULL);
        ewl_callback_call(w, CALLBACK_CUSTOM_TYPE);

        if ((long)ewl_widget_data_get(w, w) == 1)
                ret = 1;
        else
                LOG_FAILURE(buf, len, "callback function not called");

        ewl_widget_destroy(w);

        return ret;
}

/*
 * Test inserting and calling a callback of a custom type.
 */
static int
test_custom_callback_insert_after(char *buf, int len)
{
        Ewl_Widget *w;
        int ret = 0;

        w = ewl_widget_new();
        ewl_callback_prepend(w, CALLBACK_CUSTOM_TYPE, base_callback, NULL);
        ewl_callback_insert_after(w, CALLBACK_CUSTOM_TYPE, differing_callback,
                        NULL, base_callback, NULL);
        ewl_callback_call(w, CALLBACK_CUSTOM_TYPE);

        if ((long)ewl_widget_data_get(w, w) == 2)
                ret = 1;
        else
                LOG_FAILURE(buf, len, "callback function not called");

        ewl_widget_destroy(w);

        return ret;
}

/*
 * Test deleting and calling a single callback of a custom type.
 */
static int
test_custom_callback_del_single(char *buf, int len)
{
        Ewl_Widget *w;
        int ret = 0;

        w = ewl_widget_new();
        ewl_callback_append(w, CALLBACK_CUSTOM_TYPE, base_callback, NULL);
        ewl_callback_del(w, CALLBACK_CUSTOM_TYPE, base_callback);
        ewl_callback_call(w, CALLBACK_CUSTOM_TYPE);

        if ((long)ewl_widget_data_get(w, w) == 0)
                ret = 1;
        else
                LOG_FAILURE(buf, len, "callback function called");

        ewl_widget_destroy(w);

        return ret;
}

/*
 * Test deleting and calling a single callback of a custom type with other
 * callbacks of the same type present.
 */
static int
test_custom_callback_del_multiple(char *buf, int len)
{
        Ewl_Widget *w;
        int ret = 0;

        w = ewl_widget_new();
        ewl_callback_append(w, CALLBACK_CUSTOM_TYPE, differing_callback, NULL);
        ewl_callback_append(w, CALLBACK_CUSTOM_TYPE, base_callback, NULL);
        ewl_callback_del(w, CALLBACK_CUSTOM_TYPE, base_callback);
        ewl_callback_call(w, CALLBACK_CUSTOM_TYPE);

        if ((long)ewl_widget_data_get(w, w) == 2) {
                ewl_callback_append(w, CALLBACK_CUSTOM_TYPE, base_callback,
                                NULL);
                ewl_callback_del(w, CALLBACK_CUSTOM_TYPE, differing_callback);
                ewl_callback_call(w, CALLBACK_CUSTOM_TYPE);

                if ((long)ewl_widget_data_get(w, w) == 1)
                        ret = 1;
                else
                        LOG_FAILURE(buf, len, "first callback function called");
        }
        else
                LOG_FAILURE(buf, len, "second callback function called");

        ewl_widget_destroy(w);

        return ret;
}

/*
 * Test deleting and calling a single callback type of a custom type.
 */
static int
test_custom_callback_del_type_single(char *buf, int len)
{
        Ewl_Widget *w;
        int ret = 0;

        w = ewl_widget_new();
        ewl_callback_append(w, CALLBACK_CUSTOM_TYPE, base_callback, NULL);
        ewl_callback_del_type(w, CALLBACK_CUSTOM_TYPE);
        ewl_callback_call(w, CALLBACK_CUSTOM_TYPE);

        if ((long)ewl_widget_data_get(w, w) == 0)
                ret = 1;
        else
                LOG_FAILURE(buf, len, "callback function called");

        ewl_widget_destroy(w);

        return ret;
}

/*
 * Test deleting and calling a single callback type of with multiple custom
 * types.
 */
static int
test_custom_callback_del_type_multiple(char *buf, int len)
{
        Ewl_Widget *w;
        int ret = 0;

        w = ewl_widget_new();
        ewl_callback_append(w, CALLBACK_CUSTOM_TYPE, base_callback, NULL);
        ewl_callback_append(w, CALLBACK_CUSTOM_TYPE2, differing_callback, NULL);
        ewl_callback_del_type(w, CALLBACK_CUSTOM_TYPE);
        ewl_callback_call(w, CALLBACK_CUSTOM_TYPE2);

        if ((long)ewl_widget_data_get(w, w) == 2) {
                ewl_callback_call(w, CALLBACK_CUSTOM_TYPE);
                if ((long)ewl_widget_data_get(w, w) == 2)
                        ret = 1;
                else
                        LOG_FAILURE(buf, len, "callback function called");
        }
        else
                LOG_FAILURE(buf, len, "callback function not called");

        ewl_widget_destroy(w);

        return ret;
}

/*
 * Test clearing callbacks and calling a single callback type of a custom type.
 */
static int
test_custom_callback_clear(char *buf, int len)
{
        Ewl_Widget *w;
        int ret = 0;

        w = ewl_widget_new();
        ewl_callback_append(w, CALLBACK_CUSTOM_TYPE, base_callback, NULL);
        ewl_callback_clear(w);
        ewl_callback_call(w, CALLBACK_CUSTOM_TYPE);

        if ((long)ewl_widget_data_get(w, w) == 0)
                ret = 1;
        else
                LOG_FAILURE(buf, len, "callback function called");

        ewl_widget_destroy(w);

        return ret;
}

/*
 * Test calling a custom callback from within a custom callback.  Will create valgrind errors.
 */
static int
test_custom_callback_call_custom(char *buf, int len)
{
        Ewl_Widget *w;
        int ret = 0;

        w = ewl_widget_new();
        ewl_callback_append(w, CALLBACK_CUSTOM_TYPE, custom_callback, NULL);
        ewl_callback_append(w, CALLBACK_CUSTOM_TYPE2, base_callback, NULL);
        ewl_callback_call(w, CALLBACK_CUSTOM_TYPE);

        if ((long)ewl_widget_data_get(w, w) == 1)
                ret = 1;
        else
                LOG_FAILURE(buf, len, "second custom callback not called");

        ewl_widget_destroy(w);

        return ret;
}

static void
custom_callback(Ewl_Widget *w, void *event __UNUSED__, void *data __UNUSED__)
{
        ewl_callback_call(w, CALLBACK_CUSTOM_TYPE2);
        return;
}

static void
base_callback(Ewl_Widget *w, void *event __UNUSED__, void *data __UNUSED__)
{
        ewl_widget_data_set(w, w, (void *)(long)1);
        return;
}

static void
differing_callback(Ewl_Widget *w, void *event __UNUSED__, void *data __UNUSED__)
{
        ewl_widget_data_set(w, w, (void *)(long)2);
        return;
}

static void
append_callback(Ewl_Widget *w, void *event __UNUSED__, void *data __UNUSED__)
{
        ewl_callback_append(w, EWL_CALLBACK_CONFIGURE, base_callback, NULL);
        return;
}

static void
prepend_callback(Ewl_Widget *w, void *event __UNUSED__, void *data __UNUSED__)
{
        ewl_callback_prepend(w, EWL_CALLBACK_CONFIGURE, base_callback, NULL);
        return;
}

static void
insert_after_callback(Ewl_Widget *w, void *event __UNUSED__,
                        void *data __UNUSED__)
{
        ewl_callback_insert_after(w, EWL_CALLBACK_CONFIGURE, base_callback,
                        NULL, insert_after_callback, NULL);
        return;
}

static void
insert_before_callback(Ewl_Widget *w, void *event __UNUSED__,
                                void *data __UNUSED__)
{
        ewl_callback_insert_after(w, EWL_CALLBACK_CONFIGURE, base_callback,
                        NULL, differing_callback, NULL);
        return;
}

static void
delete_callback(Ewl_Widget *w, void *event __UNUSED__, void *data __UNUSED__)
{
        ewl_callback_del(w, EWL_CALLBACK_CONFIGURE, base_callback);
        return;
}

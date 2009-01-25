/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "Ewl_Test.h"
#include "ewl_test_private.h"
#include "ewl_config.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define RAND_RANGE(from, to) ((from) + rand() / RAND_MAX * ((to) - (from)))

static int test_string_get_set(char *buf, int len);
static int test_int_get_set(char *buf, int len);
static int test_float_get_set(char *buf, int len);
static int test_color_get_set(char *buf, int len);

static int test_string_overwrite(char *buf, int len);
static int test_int_overwrite(char *buf, int len);
static int test_float_overwrite(char *buf, int len);
static int test_color_overwrite(char *buf, int len);

static int test_remove_key(char *buf, int len);
static int test_keys_remove(char *buf, int len);
static int test_keys_get(char *buf, int len);

/*
 * This set of tests is targeted at config
 */

Ewl_Unit_Test config_unit_tests[] = {
                {"string get/set", test_string_get_set, NULL, -1, 0},
                {"int get/set", test_int_get_set, NULL, -1, 0},
                {"float get/set", test_float_get_set, NULL, -1, 0},
                {"color get/set", test_color_get_set, NULL, -1, 0},
                {"string overwrite", test_string_overwrite, NULL, -1, 0},
                {"int overwrite", test_int_overwrite, NULL, -1, 0},
                {"float overwrite", test_float_overwrite, NULL, -1, 0},
                {"color overwrite", test_color_overwrite, NULL, -1, 0},
                {"remove key", test_remove_key, NULL, -1, 0},
                {"remove keys get", test_keys_remove, NULL, -1, 0},
                {"test keys get", test_keys_get, NULL, -1, 0},
                {NULL, NULL, NULL, -1, 0}
        };

/*
 * Set a string to a new config and retrieve it again
 */
static int
test_string_get_set(char *buf, int len)
{
        Ewl_Config *conf;
        const char *string = "The sun is shinning.";
        const char *value;
        int ret = 0;

        conf = ewl_config_new("unit test");

        ewl_config_string_set(conf, "weather", string, EWL_STATE_TRANSIENT);

        /* first try to get a string that cannot exist */
        value = ewl_config_string_get(conf, "climate");
        if (value) {
                LOG_FAILURE(buf, len, "config finds an string for"
                                        "non-existing key");
                goto CLEANUP;
        }

        /* now try to get the weather report */
        value = ewl_config_string_get(conf, "weather");
        if (!value) {
                LOG_FAILURE(buf, len, "config does not find the string we set");
                goto CLEANUP;
        }

        if (strcmp(value, string)) {
                LOG_FAILURE(buf, len, "config returned a different string");
                goto CLEANUP;
        }

        /* the config must not save our address */
        if (value == string) {
                LOG_FAILURE(buf, len, "config returned the address of the set "
                                        "string");
                goto CLEANUP;
        }

        /* everything went fine */
        ret = 1;

CLEANUP:
        ewl_config_destroy(conf);

        return ret;
}

/*
 * Set a int  to a new config and retrieve it again
 */
static int
test_int_get_set(char *buf, int len)
{
        Ewl_Config *conf;
        int number = 1423;
        int value;
        int ret = 0;

        conf = ewl_config_new("unit test");

        ewl_config_int_set(conf, "number", number, EWL_STATE_TRANSIENT);

        /* first try to get a value that cannot exist */
        value = ewl_config_int_get(conf, "letter");

        /* on error it should return 0 */
        if (value != 0) {
                LOG_FAILURE(buf, len, "config returns a number unequal zero for"
                                        "non-existing key");
                goto CLEANUP;
        }

        /* now try to get the set number */
        value = ewl_config_int_get(conf, "number");
        if (value != number) {
                LOG_FAILURE(buf, len, "config returns wrong number");
                goto CLEANUP;
        }

        /* everything went fine */
        ret = 1;

CLEANUP:
        ewl_config_destroy(conf);

        return ret;
}

/*
 * Set a float to a new config and retrieve it again
 */
static int
test_float_get_set(char *buf, int len)
{
        Ewl_Config *conf;
        int number = 2.99792E8;
        int value;
        int ret = 0;

        conf = ewl_config_new("unit test");

        ewl_config_float_set(conf, "velocity", number, EWL_STATE_TRANSIENT);

        /* first try to get a value that cannot exist */
        value = ewl_config_float_get(conf, "speed");

        /* on error it should return 0.0, according to the docs */
        if (value != 0.0) {
                LOG_FAILURE(buf, len, "config returns a number unequal zero for"
                                        "non-existing key");
                goto CLEANUP;
        }

        /* now try to get the set velocity */
        value = ewl_config_float_get(conf, "velocity");
        if (value != number) {
                LOG_FAILURE(buf, len, "config returns wrong number");
                goto CLEANUP;
        }

        /* everything went fine */
        ret = 1;

CLEANUP:
        ewl_config_destroy(conf);

        return ret;
}

/*
 * Set a color to a new config and retrieve it again
 */
static int
test_color_get_set(char *buf, int len)
{
        Ewl_Config *conf;
        int r, b, g, a;
        int ret = 0;

        conf = ewl_config_new("unit test");

        ewl_config_color_set(conf, "bg_color", 0, 100, 200, 255,
                                                        EWL_STATE_TRANSIENT);

        /* first try to get a value that cannot exist */
        ewl_config_color_get(conf, "fg_color", &r, &g, &b, &a);

        /* on error it should return 0 */
        if (r != 0 || g != 0 || b != 0 || a != 0) {
                LOG_FAILURE(buf, len, "config returns not (0,0,0,0) for "
                                        "non-existing key");
                goto CLEANUP;
        }

        /* now try to get the set number */
        ewl_config_color_get(conf, "bg_color", &r, &g, &b, &a);
        if (r != 0 || g != 100 || b != 200 || a != 255) {
                LOG_FAILURE(buf, len, "config returns wrong color");
                goto CLEANUP;
        }

        /* everything went fine */
        ret = 1;

CLEANUP:
        ewl_config_destroy(conf);

        return ret;
}

/*
 * Try to overwrite a string
 */
static int
test_string_overwrite(char *buf, int len)
{
        int ret = 1;
        Ewl_Config *conf;
        const char *strings[] = {
                "First Value",
                "Second Value",
                "Yet another",
                "The last one",
                NULL
        };
        const char **string = strings;

        conf = ewl_config_new("unit test");

        while (*string == NULL) {
                const char *value;

                ewl_config_string_set(conf, "test key", *string,
                                                EWL_STATE_TRANSIENT);
                value = ewl_config_string_get(conf, "test key");

                if (strcmp(value, *string)) {
                        LOG_FAILURE(buf, len, "The returned string is different"
                                        " from the set string");
                        ret = 0;
                        break;
                }
                string++;
        }

        ewl_config_destroy(conf);

        return ret;
}

/*
 * Try to overwrite a int
 */
static int
test_int_overwrite(char *buf, int len)
{
        int ret = 1;
        Ewl_Config *conf;
        int i;

        conf = ewl_config_new("unit test");

        for (i = 0; i < 12; i++) {
                int v = RAND_RANGE(-1000, 1000);
                int v_r;

                ewl_config_int_set(conf, "test key", v, EWL_STATE_TRANSIENT);
                v_r = ewl_config_int_get(conf, "test key");

                if (v_r != v) {
                        LOG_FAILURE(buf, len, "The returned int is different"
                                        " from the set int");
                        ret = 0;
                        break;
                }
        }

        ewl_config_destroy(conf);

        return ret;
}

/*
 * Try to overwrite a float
 */
static int
test_float_overwrite(char *buf, int len)
{
        int ret = 1;
        Ewl_Config *conf;
        int i;

        conf = ewl_config_new("unit test");

        for (i = 0; i < 12; i++) {
                float v = rand();
                float v_r;

                ewl_config_float_set(conf, "test key", v, EWL_STATE_TRANSIENT);
                v_r = ewl_config_float_get(conf, "test key");

                if (v_r != v) {
                        LOG_FAILURE(buf, len, "The returned float is different"
                                        " from the set float");
                        ret = 0;
                        break;
                }
        }

        ewl_config_destroy(conf);

        return ret;
}

/*
 * Try to overwrite a color
 */
static int
test_color_overwrite(char *buf, int len)
{
        int ret = 1;
        Ewl_Config *conf;
        int i;

        conf = ewl_config_new("unit test");

        for (i = 0; i < 12; i++) {
                int r = RAND_RANGE(0, 255);
                int g = RAND_RANGE(0, 255);
                int b = RAND_RANGE(0, 255);
                int a = RAND_RANGE(0, 255);
                int r_r, g_r, b_r, a_r;

                ewl_config_color_set(conf, "test key", r, g, b, a,
                                                        EWL_STATE_TRANSIENT);
                ewl_config_color_get(conf, "test key", &r_r, &g_r, &b_r, &a_r);

                if (r_r != r || g_r != g || b_r != b || a_r != a) {
                        LOG_FAILURE(buf, len, "The returned color is different"
                                        " from the set color");
                        ret = 0;
                        break;
                }
        }

        ewl_config_destroy(conf);

        return ret;
}

/*
 * Remove items from the hashes
 *
 * Note, this test case accesses widget internals. Don't do this in your
 * code. It's just the best way to do the tests. Again, don't do what I'm
 * doing.
 */
static int
test_remove_key(char *buf, int len)
{
        int ret = 1;
        Ewl_Config *cfg;

        cfg = ewl_config_new("unit test");

        cfg->data.system = ecore_hash_new(ecore_str_hash, ecore_str_compare);
        ecore_hash_free_key_cb_set(cfg->data.system, free);
        ecore_hash_free_value_cb_set(cfg->data.system, free);
        ecore_hash_set(cfg->data.system, strdup("/test/key"), strdup("value"));
        ecore_hash_set(cfg->data.system, strdup("/system/remove"), strdup("value"));
        ecore_hash_set(cfg->data.system, strdup("/user/remove"), strdup("value"));
        ecore_hash_set(cfg->data.system, strdup("/instance/remove"), strdup("value"));

        cfg->data.user = ecore_hash_new(ecore_str_hash, ecore_str_compare);
        ecore_hash_free_key_cb_set(cfg->data.user, free);
        ecore_hash_free_value_cb_set(cfg->data.user, free);
        ecore_hash_set(cfg->data.user, strdup("/test/key"), strdup("value"));
        ecore_hash_set(cfg->data.user, strdup("/system/remove"), strdup("value"));
        ecore_hash_set(cfg->data.user, strdup("/user/remove"), strdup("value"));
        ecore_hash_set(cfg->data.user, strdup("/instance/remove"), strdup("value"));

        cfg->data.instance = ecore_hash_new(ecore_str_hash, ecore_str_compare);
        ecore_hash_free_key_cb_set(cfg->data.instance, free);
        ecore_hash_free_value_cb_set(cfg->data.instance, free);
        ecore_hash_set(cfg->data.instance, strdup("/test/key"), strdup("value"));
        ecore_hash_set(cfg->data.instance, strdup("/system/remove"), strdup("value"));
        ecore_hash_set(cfg->data.instance, strdup("/user/remove"), strdup("value"));
        ecore_hash_set(cfg->data.instance, strdup("/instance/remove"), strdup("value"));

        ewl_config_key_remove(cfg, "/test/key");
        if (ecore_hash_get(cfg->data.system, "/test/key") != NULL)
        {
                LOG_FAILURE(buf, len, "System hash contains key after key remove");
                ret = 0;
                goto EXIT;
        }
        else if (ecore_hash_get(cfg->data.user, "/test/key") != NULL)
        {
                LOG_FAILURE(buf, len, "User hash contains key after key remove");
                ret = 0;
                goto EXIT;
        }
        else if (ecore_hash_get(cfg->data.instance, "/test/key") != NULL)
        {
                LOG_FAILURE(buf, len, "Instance hash contains key after key remove");
                ret = 0;
                goto EXIT;
        }

        ewl_config_system_key_remove(cfg, "/system/remove");
        if (ecore_hash_get(cfg->data.system, "/system/remove") != NULL)
        {
                LOG_FAILURE(buf, len, "System hash contains key after system key remove");
                ret = 0;
                goto EXIT;
        }
        else if (ecore_hash_get(cfg->data.user, "/system/remove") == NULL)
        {
                LOG_FAILURE(buf, len, "User hash missing key after system key remove");
                ret = 0;
                goto EXIT;
        }
        else if (ecore_hash_get(cfg->data.instance, "/system/remove") == NULL)
        {
                LOG_FAILURE(buf, len, "Instance hash missing key after system key remove");
                ret = 0;
                goto EXIT;
        }

        ewl_config_user_key_remove(cfg, "/user/remove");
        if (ecore_hash_get(cfg->data.system, "/user/remove") == NULL)
        {
                LOG_FAILURE(buf, len, "System hash missing key after user key remove");
                ret = 0;
                goto EXIT;
        }
        else if (ecore_hash_get(cfg->data.user, "/user/remove") != NULL)
        {
                LOG_FAILURE(buf, len, "User hash contains key after user key remove");
                ret = 0;
                goto EXIT;
        }
        else if (ecore_hash_get(cfg->data.instance, "/user/remove") == NULL)
        {
                LOG_FAILURE(buf, len, "Instance hash missing key after user key remove");
                ret = 0;
                goto EXIT;
        }

        ewl_config_instance_key_remove(cfg, "/instance/remove");
        if (ecore_hash_get(cfg->data.system, "/instance/remove") == NULL)
        {
                LOG_FAILURE(buf, len, "System hash missing key after instance key remove");
                ret = 0;
                goto EXIT;
        }
        else if (ecore_hash_get(cfg->data.user, "/instance/remove") == NULL)
        {
                LOG_FAILURE(buf, len, "User hash missing key after instance key remove");
                ret = 0;
                goto EXIT;
        }
        else if (ecore_hash_get(cfg->data.instance, "/instance/remove") != NULL)
        {
                LOG_FAILURE(buf, len, "Instance hash contains key after instance key remove");
                ret = 0;
                goto EXIT;
        }

EXIT:
        ewl_config_destroy(cfg);
        return ret;
}

static int
test_key_pressent(char *buf, int len, Ecore_Hash *hash, const char *key, const char *hash_name)
{
        if (ecore_hash_get(hash, key))
        {
                LOG_FAILURE(buf, len, "%s hash contains key %s after instance key remove", hash_name, key);
                return 0;
        }
        return 1;
}

static Ecore_Hash*
test_make_hash()
{
        Ecore_Hash *hash = ecore_hash_new(ecore_str_hash, ecore_str_compare);
        ecore_hash_free_key_cb_set(hash, free);
        ecore_hash_free_value_cb_set(hash, free);
        ecore_hash_set(hash, strdup("/test/key"), strdup("value"));
        ecore_hash_set(hash, strdup("/system/remove"), strdup("value"));
        ecore_hash_set(hash, strdup("/system/remove1"), strdup("value"));
        ecore_hash_set(hash, strdup("/system/remove2"), strdup("value"));
        ecore_hash_set(hash, strdup("/system/remove5"), strdup("value"));
        ecore_hash_set(hash, strdup("/user/remove"), strdup("value"));
        ecore_hash_set(hash, strdup("/instance/remove"), strdup("value"));

        return hash;
}

static int
test_keys_remove(char *buf, int len)
{
#define TEST_KEY_SYSTEM(key) if(ret) ret = test_key_pressent(buf, len, cfg->data.system, key, "System")
#define TEST_KEY_USER(key) if(ret) ret = test_key_pressent(buf, len, cfg->data.user, key, "User")
#define TEST_KEY_INSTANCE(key) if(ret) ret = test_key_pressent(buf, len, cfg->data.instance, key, "Instance")

        int ret = 1;
        Ewl_Config *cfg;

        cfg = ewl_config_new("unit test");
        cfg->data.system = test_make_hash();
        cfg->data.user = test_make_hash();
        cfg->data.instance = test_make_hash();

        ewl_config_keys_remove(cfg, "/system/remove");

        TEST_KEY_SYSTEM("/system/remove");
        TEST_KEY_SYSTEM("/system/remove1");
        TEST_KEY_SYSTEM("/system/remove2");
        TEST_KEY_SYSTEM("/system/remove5");

        TEST_KEY_USER("/system/remove");
        TEST_KEY_USER("/system/remove1");
        TEST_KEY_USER("/system/remove2");
        TEST_KEY_USER("/system/remove5");

        TEST_KEY_INSTANCE("/system/remove");
        TEST_KEY_INSTANCE("/system/remove1");
        TEST_KEY_INSTANCE("/system/remove2");
        TEST_KEY_INSTANCE("/system/remove5");

        ewl_config_destroy(cfg);

#undef TEST_KEY_SYSTEM
#undef TEST_KEY_USER
#undef TEST_KEY_INSTANCE

        return ret;
}

static int
test_current_key(char *buf, int len, Ecore_List *list, const char *key)
{
        const char *str;

        str = ecore_list_next(list);
        if (str)
        {
                printf("DEBUG: test key: %s; found key: %s\n", key, str);
                if (!strcmp(str, key))
                        return 1;
                else
                {
                        LOG_FAILURE(buf, len, "Returned wrong key: %s not %s", str, key);
                        return 0;
                }
        }
        else
        {
                LOG_FAILURE(buf, len, "Missing key: %s", key);
                return 0;
        }
}

static int
test_keys_get(char *buf, int len)
{
#define TEST(key)   \
    if(ret) ret = test_current_key(buf, len, keys_list, key);

        int ret = 1;
        Ewl_Config *cfg;
        Ecore_List *keys_list;

        cfg = ewl_config_new("unit test");
        cfg->data.system = test_make_hash();
        cfg->data.user = test_make_hash();
        cfg->data.instance = test_make_hash();

        keys_list = ewl_config_keys_get(cfg, "/system/remo");

        if (keys_list)
        {
                TEST("/system/remove");
                TEST("/system/remove1");
                TEST("/system/remove2");
                TEST("/system/remove5");
        }
        else
        {
                LOG_FAILURE(buf, len, "The list with keys is empty");
                ret = 0;
        }

        ewl_config_destroy(cfg);
#undef TEST
        return ret;
}

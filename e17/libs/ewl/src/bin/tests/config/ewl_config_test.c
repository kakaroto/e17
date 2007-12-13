/* vim: set sw=8 ts=8 sts=8 noexpandtab: */
#include "Ewl_Test.h"
#include "ewl_test_private.h"
#include "ewl_config.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static int string_get_set(char *buf, int len);
static int int_get_set(char *buf, int len);
static int float_get_set(char *buf, int len);
static int color_get_set(char *buf, int len);
/*
 * This set of tests is targeted at config
 */

static Ewl_Unit_Test config_unit_tests[] = {
		{"string get/set", string_get_set, NULL, -1, 0},
		{"int get/set", int_get_set, NULL, -1, 0},
		{"float get/set", float_get_set, NULL, -1, 0},
		{"color get/set", color_get_set, NULL, -1, 0},
		{NULL, NULL, NULL, -1, 0}
	};

void
test_info(Ewl_Test *test)
{
	test->name = "Config";
	test->tip = "The config system.";
	test->filename = __FILE__;
	test->type = EWL_TEST_TYPE_MISC;
	test->unit_tests = config_unit_tests;
}

/*
 * Set a string to a new config and retrieve it again
 */
static int
string_get_set(char *buf, int len)
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
int_get_set(char *buf, int len)
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
float_get_set(char *buf, int len)
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
color_get_set(char *buf, int len)
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


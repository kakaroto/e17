/* vim: set sw=8 ts=8 sts=8 noexpandtab: */
#include "Ewl_Test.h"
#include "ewl_test_private.h"
#include "ewl_config.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define RAND_RANGE(from, to) ((from) + rand() / RAND_MAX * ((to) - (from)))

static int string_get_set(char *buf, int len);
static int int_get_set(char *buf, int len);
static int float_get_set(char *buf, int len);
static int color_get_set(char *buf, int len);

static int string_overwrite(char *buf, int len);
static int int_overwrite(char *buf, int len);
static int float_overwrite(char *buf, int len);
static int color_overwrite(char *buf, int len);


/*
 * This set of tests is targeted at config
 */

static Ewl_Unit_Test config_unit_tests[] = {
		{"string get/set", string_get_set, NULL, -1, 0},
		{"int get/set", int_get_set, NULL, -1, 0},
		{"float get/set", float_get_set, NULL, -1, 0},
		{"color get/set", color_get_set, NULL, -1, 0},
		{"string overwrite", string_overwrite, NULL, -1, 0},
		{"int overwrite", int_overwrite, NULL, -1, 0},
		{"float overwrite", float_overwrite, NULL, -1, 0},
		{"color overwrite", color_overwrite, NULL, -1, 0},
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

/*
 * Try to overwrite a string
 */
static int
string_overwrite(char *buf, int len)
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
int_overwrite(char *buf, int len)
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
float_overwrite(char *buf, int len)
{
	int ret = 1;
	Ewl_Config *conf;
	int i;

	conf = ewl_config_new("unit test");

	for (i = 0; i < 12; i++) {
		float v = rand();
		float v_r;

		ewl_config_int_set(conf, "test key", v, EWL_STATE_TRANSIENT);
		v_r = ewl_config_int_get(conf, "test key");

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
color_overwrite(char *buf, int len)
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


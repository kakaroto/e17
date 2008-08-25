/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "Ewl_Test.h"
#include "ewl_text_fmt.h"
#include "ewl_text_trigger.h"
#include "ewl_test_private.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static int test_text_set_get(char *buf, int len);
static int test_text_set_get_null(char *buf, int len);
static int test_valid_utf8_text_set_get(char *buf, int len);
static int test_invalid_utf8_text_set_get(char *buf, int len);

Ewl_Unit_Test text_unit_tests[] = {
                {"text set/get", test_text_set_get, NULL, -1, 0},
                {"valid UTF-8 text set/get", test_valid_utf8_text_set_get, NULL, -1, 0},
                {"invalid UTF-8 text set/get", test_invalid_utf8_text_set_get, NULL, -1, 0},
                {"null text set/get", test_text_set_get_null, NULL, -1, 0},
                {NULL, NULL, NULL, -1, 0}
        };

static int
test_text_set_get(char *buf, int len)
{
        Ewl_Widget *o;
        char *t;
        int ret = 0;

        o = ewl_text_new();
        ewl_text_text_set(EWL_TEXT(o), "This is the test text.");
        t = ewl_text_text_get(EWL_TEXT(o));

        if (strcmp(t, "This is the test text."))
                LOG_FAILURE(buf, len, "text_get did not match text_set.");
        else
                ret = 1;

        return ret;
}

static int
test_text_set_get_null(char *buf, int len)
{
        Ewl_Widget *o;
        char *t;
        int ret = 0;

        o = ewl_text_new();
        ewl_text_text_set(EWL_TEXT(o), "");
        t = ewl_text_text_get(EWL_TEXT(o));

        if (t)
                LOG_FAILURE(buf, len, "text_get did not return null.");
        else
                ret = 1;

        return ret;
}

static int
test_valid_utf8_text_set_get(char *buf, int len)
{
        Ewl_Widget *t;
        char text[] = "This a valid UTF-8 string, although it contains "
                        "at the moment only 7-bit ascii code.\n"
                        " This will be changed later!";
        const char * returned_text;
        unsigned char *utext;

        utext = (unsigned char *)text;

        t = ewl_text_new();
        ewl_widget_show(t);

        /*
         * append the US-ASCII string
         */
        ewl_text_text_set(EWL_TEXT(t), text);
        returned_text = ewl_text_text_get(EWL_TEXT(t));

        if (strcmp(text, returned_text)) {
                LOG_FAILURE(buf, len, "Incorrect UTF-8 validation during"
                                "setting a US-ASCII string");
                return FALSE;
        }

        ewl_text_clear(EWL_TEXT(t));

        /* insert the umlaut a with two dots */
        utext[2] = 0xC3;
        utext[3] = 0xA4;
        /* insert the euro sign */
        utext[10] = 0xE2;
        utext[11] = 0x82;
        utext[12] = 0xAC;
        /*
         * set a valid UTF-8 string
         */
        ewl_text_text_set(EWL_TEXT(t), text);
        returned_text = ewl_text_text_get(EWL_TEXT(t));

        if (strcmp(text, returned_text)) {
                LOG_FAILURE(buf, len, "Incorrect UTF-8 validation during"
                                "setting a valid UTF-8 string");
                return FALSE;
        }
        return TRUE;
}


static int
test_invalid_utf8_text_set_get(char *buf, int len)
{
        Ewl_Widget *t;
        char text[] = "This a valid UTF-8 string, although it contains "
                        "at the moment only 7-bit ascii code.\n"
                        " This will be changed later!";
        const char *returned_text;
        unsigned char *utext;

        utext = (unsigned char *)text;

        t = ewl_text_new();
        ewl_widget_show(t);

        /* insert some invalid bytes */
        utext[2] = 254;
        utext[4] = 0xA4;

        /* insert the euro sign without the 3rd byte */
        utext[10] = 0xE2;
        utext[11] = 0x82;

        /* set a valid UTF-8 string */
        ewl_text_text_set(EWL_TEXT(t), text);
        returned_text = ewl_text_text_get(EWL_TEXT(t));

        if (!returned_text)
                LOG_FAILURE(buf, len, "ewl_text_text_get() returned a NULL pointer");

        else if (!strcmp(text, returned_text)
                || returned_text[2] < 0
                || returned_text[4] < 0
                || returned_text[10] < 0
                || returned_text[11] < 0)
        {
                LOG_FAILURE(buf, len, "Incorrect UTF-8 validation during"
                                "setting a invalid UTF-8 string");
                return FALSE;
        }
        return TRUE;
}


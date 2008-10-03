/*
 * enna.c
 * Copyright (C) Nicolas Aguirre 2006,2007,2008 <aguirre.nicolas@gmail.com>
 *
 * enna.c is free software copyrighted by Nicolas Aguirre.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name ``Nicolas Aguirre'' nor the name of any other
 *    contributor may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * enna.c IS PROVIDED BY Nicolas Aguirre ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL Nicolas Aguirre OR ANY OTHER CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#define _GNU_SOURCE
#include <getopt.h>

#include "enna.h"
#include "enna_inc.h"

/* Global Variable Enna *enna*/
Enna *enna;

static char *conffile = NULL;
static char *theme_name = NULL;
static int run_fullscreen = 0;
static int run_gl = 0;

/* Callbacks */

/* Functions */
static void _create_gui(void);

/* Calbacks */
static int _event_bg_key_down_cb(void *data, int type, void *event)
{
    Enna *enna;
    enna_key_t key;

    key = enna_get_key(event);

    /*if (key == ENNA_KEY_UNKNOWN)
     return 0;*/

    enna = (Enna *) data;
    if (!enna)
        return 0;

    if (key == ENNA_KEY_QUIT)
        ecore_main_loop_quit();

    if (enna_mainmenu_visible(enna->o_mainmenu))
    {
        switch (key)
        {
            case ENNA_KEY_MENU:
            {
                enna_content_show();
                enna_mainmenu_hide(enna->o_mainmenu);
                edje_object_signal_emit(enna->o_edje, "mainmenu,hide", "enna");
                break;
            }
            case ENNA_KEY_RIGHT:
            {
                enna_mainmenu_select_next(enna->o_mainmenu);
                break;
            }
            case ENNA_KEY_LEFT:
            {
                enna_mainmenu_select_prev(enna->o_mainmenu);
                break;
            }
            case ENNA_KEY_OK:
            case ENNA_KEY_SPACE:
            {
                enna_mainmenu_activate_nth(enna->o_mainmenu,
                        enna_mainmenu_selected_get(enna->o_mainmenu));
                break;
            }
            default:
                break;
        }
    }
    else
    {
        switch (key)
        {
            case ENNA_KEY_MENU:
            {
                enna_content_hide();
                enna_mainmenu_show(enna->o_mainmenu);
                //edje_object_signal_emit(enna->o_edje, "mainmenu,show", "enna");
                //edje_object_signal_emit(enna->o_edje, "module,hide", "enna");
                break;
            }
            default:
                enna_activity_event(
                        enna_mainmenu_selected_activity_get(enna->o_mainmenu),
                        event);
                break;
        }
    }
    return 0;
}
static void _resize_viewport_cb(Ecore_Evas * ee)
{
    Evas_Coord w, h, x, y;

    if (!enna->ee)
        return;

    evas_output_viewport_get(enna->evas, &x, &y, &w, &h);
    evas_object_resize(enna->o_edje, w, h);
    evas_object_move(enna->o_edje, x, y);
    ecore_evas_resize(enna->ee, w, h);
}

/* Functions */

static int _enna_init(int run_gl)
{
    char tmp[PATH_MAX];

    enna->lvl = ENNA_MSG_INFO;
    enna->home = enna_util_user_home_get();

    ecore_init();
    ecore_file_init();
    ecore_evas_init();
    edje_init();
    enna_module_init();

    sprintf(tmp, "%s/.enna", enna->home);

    if (!ecore_file_exists(tmp))
        ecore_file_mkdir(tmp);

    sprintf(tmp, "%s/.enna/covers", enna->home);
    if (!ecore_file_exists(tmp))
        ecore_file_mkdir(tmp);

    if (enna_config->verbosity)
    {
        if (!strcmp(enna_config->verbosity, "none"))
            enna->lvl = ENNA_MSG_NONE;
        else if (!strcmp(enna_config->verbosity, "event"))
            enna->lvl = ENNA_MSG_EVENT;
        else if (!strcmp(enna_config->verbosity, "info"))
            enna->lvl = ENNA_MSG_INFO;
        else if (!strcmp(enna_config->verbosity, "warning"))
            enna->lvl = ENNA_MSG_WARNING;
        else if (!strcmp(enna_config->verbosity, "error"))
            enna->lvl = ENNA_MSG_ERROR;
        else if (!strcmp(enna_config->verbosity, "critical"))
            enna->lvl = ENNA_MSG_CRITICAL;
    }

    if (!strcmp(enna_config->engine, "gl")
            && ecore_evas_engine_type_supported_get(ECORE_EVAS_ENGINE_OPENGL_X11))
    {
        enna_log(ENNA_MSG_INFO, NULL, "Load GL engine");
        enna->ee = ecore_evas_gl_x11_new(NULL, 0, 0, 0, 64, 64);
        if (enna->ee)
            enna->ee_winid = ecore_evas_gl_x11_window_get(enna->ee);
    }
    else if (!strcmp(enna_config->engine, "xrender")
            && ecore_evas_engine_type_supported_get(ECORE_EVAS_ENGINE_XRENDER_X11))
    {
        enna_log(ENNA_MSG_INFO, NULL, "Load XRENDER engine");
        enna->ee = ecore_evas_xrender_x11_new(NULL, 0, 0, 0, 64, 64);
        if (enna->ee)
            enna->ee_winid = ecore_evas_xrender_x11_window_get(enna->ee);
    }
    else if (!strcmp(enna_config->engine, "x11_16")
            && ecore_evas_engine_type_supported_get(ECORE_EVAS_ENGINE_SOFTWARE_16_X11))
    {
        enna_log(ENNA_MSG_INFO, NULL, "Load X11_16 engine");
        enna->ee = ecore_evas_software_x11_16_new(NULL, 0, 0, 0, 64, 64);
        if (enna->ee)
            enna->ee_winid = ecore_evas_software_x11_16_window_get(enna->ee);
    }
    else if (!strcmp(enna_config->engine, "x11")
            && ecore_evas_engine_type_supported_get(ECORE_EVAS_ENGINE_SOFTWARE_X11))
    {
        enna_log(ENNA_MSG_INFO, NULL, "Load X11 engine");
        enna->ee = ecore_evas_software_x11_new(NULL, 0, 0, 0, 64, 64);
        if (enna->ee)
            enna->ee_winid = ecore_evas_software_x11_window_get(enna->ee);
    }
    else if (ecore_evas_engine_type_supported_get(ECORE_EVAS_ENGINE_SOFTWARE_X11))
    {
        enna_log(
                ENNA_MSG_WARNING,
                NULL,
                "Specified \'%s\' engine not found, use X11 software default engine",
                enna_config->engine);
        enna->ee = ecore_evas_software_x11_new(NULL, 0, 0, 0, 64, 64);
        if (enna->ee)
            enna->ee_winid = ecore_evas_software_x11_window_get(enna->ee);
    }
    else
    {
        enna_log(ENNA_MSG_CRITICAL, NULL,
                "Can not create Ecore Evas with %s engine!",
                enna_config->engine);
        return 0;
    }

    if (!enna->ee)
    {
        enna_log(ENNA_MSG_CRITICAL, NULL, "Can not Initialize Ecore Evas !");
        return 0;
    }

    enna->use_covers = enna_config->use_covers;
    enna->use_snapshots = enna_config->use_snapshots;

    ecore_evas_fullscreen_set(enna->ee, enna_config->fullscreen
            | run_fullscreen);

    ecore_evas_title_set(enna->ee, "enna HTPC");
    ecore_evas_name_class_set(enna->ee, "enna", "enna");
    ecore_evas_borderless_set(enna->ee, 0);
    ecore_evas_shaped_set(enna->ee, 1);
    enna->evas = ecore_evas_get(enna->ee);

    evas_data_attach_set(enna->evas, enna);

    _create_gui();
    ecore_evas_show(enna->ee);
    enna_input_init();
    return 1;
}

static void _create_gui()
{
    Evas_Object *o;
    Evas_Coord w, h;
    Enna_Module *em;

    o = edje_object_add(enna->evas);
    edje_object_file_set(o, enna_config_theme_get(), "enna");
    edje_object_size_min_get(o, &w, &h);
    evas_object_resize(o, w, h);
    evas_object_move(o, 0, 0);
    evas_object_show(o);
    ecore_evas_resize(enna->ee, w, h);
    enna->o_edje = o;

    /* Create Background Object */
    o = enna_background_add(enna->evas);
    edje_object_part_swallow(enna->o_edje, "enna.swallow.background", o);
    enna->o_background = o;
    /* Create Mainmenu Object */
    o = enna_mainmenu_add(enna->evas);
    edje_object_part_swallow(enna->o_edje, "enna.swallow.mainmenu", o);
    enna->o_mainmenu = o;

    edje_object_signal_emit(enna->o_edje, "mainmenu,show", "enna");
    evas_object_focus_set(enna->o_edje, 1);

    /*evas_object_event_callback_add(enna->o_edje,
     EVAS_CALLBACK_KEY_DOWN,
     _event_bg_key_down_cb, enna);*/

    ecore_event_handler_add(ECORE_X_EVENT_KEY_DOWN, _event_bg_key_down_cb, enna);

    ecore_evas_callback_resize_set(enna->ee, _resize_viewport_cb);
    /* Create Content Object */
    o = enna_content_add(enna->evas);
    edje_object_part_swallow(enna->o_edje, "enna.swallow.module", o);
    enna->o_content = o;

    /* Create Modules */
    em = enna_module_open("polling", enna->evas);
    enna_module_enable(em);
    em = enna_module_open("music", enna->evas);
    enna_module_enable(em);
    em = enna_module_open("video", enna->evas);
    enna_module_enable(em);
    em = enna_module_open("photo", enna->evas);
    enna_module_enable(em);
    em = enna_module_open("localfiles", enna->evas);
    enna_module_enable(em);
#ifdef BUILD_LMS_MODULE
    em = enna_module_open("lms", enna->evas);
    enna_module_enable(em);
#endif
    em = enna_module_open("shoutcast", enna->evas);
    enna_module_enable(em);
    em = enna_module_open("netstreams", enna->evas);
    enna_module_enable(em);
    /* Load mainmenu items */

    enna_activity_init("music");
    enna_activity_init("video");
    enna_activity_init("photo");

    enna_mainmenu_load_from_activities(enna->o_mainmenu);
    enna_mainmenu_select_nth(enna->o_mainmenu, 0);

    enna_mainmenu_show(enna->o_mainmenu);
    ecore_evas_show(enna->ee);

    /* Initialize and load mediaplayer modules */
    enna_mediaplayer_init();

}

static void _enna_shutdown()
{

    enna_activity_del("music");
    enna_activity_del("video");
    enna_activity_del("photo");
    enna_input_shutdown();
    enna_config_shutdown();
    enna_module_shutdown();
    enna_mediaplayer_shutdown();
    evas_object_del(enna->o_background);
    evas_object_del(enna->o_edje);
    evas_object_del(enna->o_mainmenu);
    edje_shutdown();
    ecore_evas_shutdown();
    ecore_file_shutdown();
    ecore_shutdown();
    ENNA_FREE(enna->home);
    ENNA_FREE(enna);
}

static void usage(char *binname)
{
    printf("Enna MediaCenter\n");
    printf(" Usage: %s [options ...]\n", binname);
    printf(" Available Options:\n");
    printf("  -c, (--config):  Specify configuration file to be used.\n");
    printf("  -f, (--fs):      Force Fullscreen mode.\n");
    printf("  -h, (--help):    Display this help.\n");
    printf("  -t, (--theme):   Specify theme name to be used.\n");
    printf("  -V, (--version): Display Enna version number.\n");
    exit(0);
}

static int parse_command_line(int argc, char **argv)
{
    int c, index;
    char short_options[] = "Vhfgc:t:b:";
    struct option long_options [] =
    {
    { "help", no_argument, 0, 'h' },
    { "version", no_argument, 0, 'V' },
    { "fs", no_argument, 0, 'f' },
    { "config", required_argument, 0, 'c' },
    { "theme", required_argument, 0, 't' },
    { 0, 0, 0, 0 } };

    /* command line argument processing */
    while (1)
    {
        c = getopt_long(argc, argv, short_options, long_options, &index);

        if (c == EOF)
            break;

        switch (c)
        {
            case 0:
                /* opt = long_options[index].name; */
                break;

            case '?':
            case 'h':
                usage(argv[0]);
                return -1;

            case 'V':
                break;

            case 'f':
                run_fullscreen = 1;
                break;

            case 'c':
                conffile = strdup(optarg);
                break;

            case 't':
                theme_name = strdup(optarg);
                break;

            default:
                usage(argv[0]);
                return -1;
        }
    }

    return 0;
}

int main(int arc, char **arv)
{
    if (parse_command_line(arc, arv) < 0)
        return 0;

    /* Must be called first */
    enna_config_init();

    enna = calloc(1, sizeof(Enna));

    if (!_enna_init(run_gl))
        return 0;

    ecore_main_loop_begin();

    _enna_shutdown();
    enna_log(ENNA_MSG_INFO, NULL, "Bye Bye !");

    return 0;
}

/* vim: set sw=4 ts=4 sts=4 expandtab: */
#include <Ecore.h>
#include <Ecore_Evas.h>
#include <Ecore_File.h>
#include <Edje.h>
#include <Ewl.h>
#include "Eli_App.h"
#include <stdlib.h>
#include <stdio.h>
#include "eli_statistics.h"
#include "eli_hiscore.h"
#include "ewl_help_reader.h"

static int sig_exit_cb(void * data, int ev_type, void * ev);

int main(int argc, char ** argv)
{
    Eli_App * eap;
    
    eap = NULL;

    /* Initialize NLS */
#ifdef ENABLE_NLS
    setlocale(LC_MESSAGES, "");
    setlocale(LC_MONETARY, "");
    setlocale(LC_CTYPE, "");
    bindtextdomain(PACKAGE, PACKAGE_LOCALE_DIR);
    bind_textdomain_codeset(PACKAGE, "UTF-8");
    textdomain(PACKAGE);
#endif
    /* Initialize Ecore */
    if (!ecore_init()) {
        fprintf(stderr, _("Elitaire Error: Cannot init Ecore\n"));
        return 1;
    }
    if (!ecore_file_init()) {
        fprintf(stderr, _("Elitaire Error: Cannot init Ecore File\n"));
        return 1;
    }
    /* Initialize Edje */
    if (!edje_init()) {
        fprintf(stderr, _("Elitaire Error: Cannot init Edje\n"));
        return 1;
    }
    /* Initialize Ecore_Config */
    if (ecore_config_init("elitaire") != ECORE_CONFIG_ERR_SUCC) {
        fprintf(stderr, _("Elitaire Error: Cannot init Ecore Config\n"));
        return 1;
    }
    /* Initialize Ewl */
    if (!ewl_init(&argc, argv)) {
        fprintf(stderr, _("Elitaire Error: Cannot init Ewl\n"));
        return 1;
    }
    /* Init Highscore and Statistics */
    eli_highscore_init("elitaire");
    eli_statistics_init("elitaire");
    help_reader_init(PACKAGE_DATA_DIR"/doc/");

    /* Init the main app */
    eap = eli_app_new();
    eli_app_init(eap, "edje");
    eli_app_gui_make(eap);

    ecore_event_handler_add(ECORE_EVENT_SIGNAL_EXIT, sig_exit_cb, eap);
    /* ***Main Loop*** */
    ecore_main_loop_begin();

    eli_app_del(eap);
    /* shutdown all */
    help_reader_shutdown();
    eli_statistics_shutdown();
    eli_highscore_shutdown();
    ewl_shutdown();
    //eli_app_gui_del(eap);
    ecore_config_shutdown();
    ecore_file_shutdown();
    edje_shutdown();

    ecore_evas_shutdown();

    ecore_shutdown();
    return 0;
}

static int sig_exit_cb(void * data, int ev_type, void * ev)
{
    ecore_main_loop_quit();
    return 1;
}

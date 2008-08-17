/* vim: set sw=4 ts=4 sts=4 expandtab: */
#include "Eli_App.h"
#include <Ewl.h>
#include "ewl_dialogs.h"

void eli_frontend_ewl_dialogs_init(Eli_App * eap)
{
    eap->frontend->win_config_open = ewl_frontend_dialog_config_open;
    eap->frontend->win_hiscore_open = ewl_frontend_dialog_highscore_open;
    eap->frontend->win_hiscore_add = ewl_frontend_dialog_highscore_add;
    eap->frontend->win_about_open = ewl_frontend_dialog_about_open;
}


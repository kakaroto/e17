#ifndef _EWL_THEME_H_
#define _EWL_THEME_H_ 1


#include "includes.h"
#include "debug.h"
#include "ll.h"
#include "util.h"
#include "ewldb.h"
#include "image.h"
#include "event.h"
#include "widget.h"
#include "translators.h"

/* from state.h */
extern EwlLL *ewl_state_get_path_list();
extern char  *ewl_state_get_application_name();
extern char  *ewl_state_get_theme();

char         *ewl_theme_find_db(char *name);   /* searches the given paht */
char         *ewl_theme_find_file(char *name); /* same, but for media */

char         *ewl_theme_get_string(char *key);
EwlBool       ewl_theme_get_int(char *key, int *val);
EwlBool       ewl_theme_get_float(char *key, float *val);

EwlImage     *ewl_theme_get_image(char *key);
EwlImLayer   *ewl_theme_get_imlayer(char *key);

void          ewl_widget_get_theme(EwlWidget *wid, char *key);

#endif /*_EWL_THEME_H_*/

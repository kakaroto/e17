#ifndef _THEME_H_
#define _THEME_H_


#include "includes.h"
#include "list.h"
#include "util.h"
#include "widget.h"
#include "ewldb.h"

/* from api.h */
extern EwlList *ewl_get_path_list();
extern char  *ewl_get_application_name();
extern char  *ewl_get_theme();

char         *ewl_theme_find_db(char *name);   /* searches the given paht */
char         *ewl_theme_find_file(char *name); /* same, but for media */

char         *ewl_theme_get_string(char *key);
char          ewl_theme_get_int(char *key, int *val);
char          ewl_theme_get_float(char *key, float *val);

/*EwlImage     *ewl_theme_get_image(char *key);*/

void          ewl_widget_get_theme(EwlWidget *widget, char *key);

#endif /* _THEME_H_ */

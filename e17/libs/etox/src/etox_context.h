#ifndef _ETOX_CONTEXT_H
#define _ETOX_CONTEXT_H

/*
 * Context management functions
 */
Etox_Context *etox_context_save(Etox *et);
int etox_context_load(Etox *et, Etox_Context *context);
int etox_context_free(Etox_Context *context);

/*
 * Color management functions
 */
int etox_context_get_color(Etox *et, int *r, int *g, int *b, int *a);
int etox_context_set_color(Etox *et, int r, int g, int b, int a);
int etox_context_set_color_db(Etox *et, char *name);

/*
 * Callback management functions
 */
int etox_context_clear_callbacks(Etox *et);
int etox_context_set_callback_list(Etox *et, Ewd_List *list);
int etox_context_add_callback(Etox *et, int type, Etox_Cb_Func func, void *data);
int etox_context_del_callback(Etox *et, int index);

/*
 * Font managment functions
 */
char *etox_context_get_font(Etox *et, int *size);
int etox_context_set_font(Etox *et, char *fontname, int size);
int etox_context_set_font_size(Etox *et, int size);

/*
 * Style management functions
 */
char *etox_context_get_style(Etox *et);
int etox_context_set_style(Etox *et, char *stylename);

/*
 * Alignment management functions
 */
int etox_context_get_align(Etox *et);
int etox_context_set_align(Etox *et, int align);

#endif

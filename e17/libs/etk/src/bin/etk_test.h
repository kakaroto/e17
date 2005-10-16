#ifndef _ETK_TEST_H_
#define _ETK_TEST_H_

#include "Etk.h"

/* Gettext */
#include <libintl.h>
#define _(String) gettext (String)
#define gettext_noop(String) String
#define N_(String) gettext_noop (String)

typedef struct _Etk_Test_Set Etk_Test_Set;
struct _Etk_Test_Set
{
	char *name;
	void (*func)(void *data);
};

void etk_test_button_window_create(void *data);
void etk_test_entry_window_create(void *data);
void etk_test_table_window_create(void *data);
void etk_test_image_window_create(void *data);
void etk_test_scale_window_create(void *data);
void etk_test_colorpicker_window_create(void *data);
void etk_test_canvas_window_create(void *data);
void etk_test_tree_window_create(void *data);

#endif

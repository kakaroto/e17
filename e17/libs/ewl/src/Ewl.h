#include <Ebits.h>
#include <Ecore.h>
#include <Edb.h>
#include <Evas.h>
#define LIBRARY_NAME "EWL"
#include <Etox.h>
#include <Ewd.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#ifndef PATH_LEN
#define PATH_LEN 4096
#endif

#ifndef EWL_USER_DIR
#define EWL_USER_DIR ".e/ewl"
#endif

enum _ewl_callback_type {
    EWL_CALLBACK_REALIZE,
    EWL_CALLBACK_SHOW,
    EWL_CALLBACK_HIDE,
    EWL_CALLBACK_DESTROY,
    EWL_CALLBACK_CONFIGURE,
	EWL_CALLBACK_KEY_DOWN,
	EWL_CALLBACK_KEY_UP,
	EWL_CALLBACK_MOUSE_DOWN,
	EWL_CALLBACK_MOUSE_UP,
	EWL_CALLBACK_MOUSE_MOVE,
	EWL_CALLBACK_FOCUS_IN,
	EWL_CALLBACK_FOCUS_OUT,
	EWL_CALLBACK_SELECT,
	EWL_CALLBACK_UNSELECT,
	EWL_CALLBACK_CLICKED,
	EWL_CALLBACK_RELEASED,
	EWL_CALLBACK_HILITED,
    EWL_CALLBACK_MAX
};

typedef enum _ewl_callback_type Ewl_Callback_Type;

typedef struct _ewl_container Ewl_Container;

#include <ewl_enums.h>

#include <ewl_object.h>
#include <ewl_widget.h>
#include <ewl_container.h>

#include <ewl_callback.h>
#include <ewl_events.h>

#include <ewl_macros.h>
#include <ewl_misc.h>

#include <ewl_box.h>
#include <ewl_table.h>

#include <ewl_prefs.h>
#include <ewl_theme.h>

#include <ewl_button.h>
#include <ewl_window.h>
#include <ewl_text.h>
#include <ewl_entry.h>

/* Playing around */
#include <ewl_fx.h>

#include <ewl_seeker.h>
#include <ewl_spinner.h>
